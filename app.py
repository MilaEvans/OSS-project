from flask import Flask, request, render_template, session
import subprocess
import os
import re

app = Flask(__name__)
app.secret_key = 'very_secret_key'
app.config['UPLOAD_FOLDER'] = 'static/uploads'
os.makedirs(app.config['UPLOAD_FOLDER'], exist_ok=True)

# C++ 실행파일 경로 (빌드 후 MixBot으로 이름 붙였다고 가정)
CPP_EXEC = os.path.join(os.path.dirname(__file__),'mbti_project', 'MixBot')

VALID_MBTI = {
    "INTJ", "INTP", "ENTJ", "ENTP",
    "INFJ", "INFP", "ENFJ", "ENFP",
    "ISTJ", "ISFJ", "ESTJ", "ESFJ",
    "ISTP", "ISFP", "ESTP", "ESFP"
}

INTEREST_KEYWORDS = {
    "운동", "예술", "음악", "봉사", "토론", "IT", "창업", "문학",
    "댄스", "게임", "여행", "사진", "영상", "미술", "연극", "요리"
}

# 서버 시작 시 유사어 사전 로드
similar_words = {}
with open("similar_words.txt", "r", encoding="utf-8") as f:
    for line in f:
        line = line.strip()
        if not line:
            continue
        parts = line.split()
        if len(parts) == 2:
            synonym, keyword = parts
            similar_words[synonym] = keyword

def extract_mbti_and_interest(text):
    txt_upper = text.upper()
    mbti_found = next((mbti for mbti in VALID_MBTI if mbti in txt_upper), None)

    lowered = text.lower()
    # 유사어 적용 (예: "농구" -> "운동")
    for syn, kw in similar_words.items():
        if syn in lowered:
            lowered = lowered.replace(syn, kw)

    interest_found = next((kw for kw in INTEREST_KEYWORDS if kw.lower() in lowered), None)
    return mbti_found, interest_found

@app.route("/", methods=["GET"])
def index():
    session.clear()
    session["stage"] = 0
    session["base_filters"] = []  # MBTI/관심사 + 추가 필터 저장
    return render_template("chat.html", history=[("bot", "MBTI나 관심사를 입력해주세요!")])

@app.route("/chat", methods=["POST"])
def chat():
    session.setdefault("history", [])
    user_input = request.form.get("keyword", "").strip()
    bot_response = ""

    try:
        # “인기”만 입력된 경우 (stage 0일 때만)
        if session.get("stage", 0) == 0 and re.search(r"\b인기\b", user_input) and len(user_input.split()) == 1:
            result = subprocess.run(
                [CPP_EXEC, "인기"],
                text=True, capture_output=True,
                encoding='utf-8', errors='replace'
            )
            if result.returncode != 0:
                bot_response = f"[오류] {result.stderr.strip() or '원인 불명'}"
            else:
                bot_response = result.stdout.strip()
            session["history"].append(("user", user_input))
            session["history"].append(("bot", bot_response))
            return render_template("chat.html", history=session["history"])

        stage = session.get("stage", 0)

        # ─── 단계 0: MBTI 또는 관심사 입력 대기 ────────────────────────────────
        if stage == 0:
            mbti, interest = extract_mbti_and_interest(user_input)
            if mbti or interest:
                chosen = mbti if mbti else interest
                session["base_filters"] = [chosen]
                session["stage"] = 1
                bot_response = (
                    f"좋습니다! '{chosen}'을(를) 기반으로 추가 필터를 입력해주세요.\n"
                    "예) 시간대: 오전/오후/저녁, 회비: 무료/유료, 요일: 월요일 등\n"
                    "더 추가 없이 최종 추천을 원하시면 '끝'이라고 입력해 주세요."
                )
            else:
                bot_response = "MBTI(예: INFP) 또는 관심사(예: 운동, 음악)를 입력해주세요."
            session["history"].append(("user", user_input))
            session["history"].append(("bot", bot_response))
            return render_template("chat.html", history=session["history"])

        # ─── 단계 1: 추가 필터 입력 ─────────────────────────────────────────
        if stage == 1:
            # “끝” 입력 시 → 지금까지 누적된 필터로 최종 추천
            if user_input == "끝":
                combined = " ".join(session["base_filters"])
                result = subprocess.run(
                    [CPP_EXEC] + combined.split(),
                    text=True, capture_output=True,
                    encoding='utf-8', errors='replace'
                )
                if result.returncode != 0:
                    bot_response = f"[오류] {result.stderr.strip() or '원인 불명'}"
                else:
                    bot_response = result.stdout.strip()
                # 결과를 보여준 뒤 상태 초기화
                session["stage"] = 0
                session["base_filters"] = []
                session["history"].append(("user", user_input))
                session["history"].append(("bot", bot_response))
                return render_template("chat.html", history=session["history"])

            # (유사어 적용)
            lower = user_input.lower()
            for syn, kw in similar_words.items():
                if syn in lower:
                    user_input = user_input.replace(syn, kw)

            # 지금 상태에서 마지막 필터를 추가하기 전에 기록
            session["base_filters"].append(user_input)
            combined = " ".join(session["base_filters"])
            # C++ 호출
            result = subprocess.run(
                [CPP_EXEC] + combined.split(),
                text=True, capture_output=True,
                encoding='utf-8', errors='replace'
            )

            if result.returncode != 0:
                bot_response = f"[오류] {result.stderr.strip() or '원인 불명'}"
                # (이 에러는 일반적으로 없겠지만, C++ 호출 자체 실패 시)
            else:
                output = result.stdout.strip()
                # C++에서 “’<필터어>’에 해당되는 동아리가 없습니다. 다시 입력해주세요.” 메시지를 보냈다면
                if output.find("해당되는 동아리가 없습니다") != -1:
                    # C++가 출력한 오류 메시지를 그대로 사용자에게 전달
                    bot_response = output
                    # 마지막에 append했던 필터어만 제거
                    session["base_filters"].pop()
                    # stage는 여전히 1 (추가 필터를 다시 받음)
                else:
                    # “- 동아리이름” 줄만 골라서 목록으로 만듦
                    lines = [ln.strip() for ln in output.splitlines() if ln.startswith("- ")]
                    clubs = [ln[2:] for ln in lines]

                    if len(clubs) > 1:
                        bot_response = (
                            f"현재 조건으로 {len(clubs)}개의 동아리가 있습니다:\n"
                            + "\n".join(f"- {c}" for c in clubs)
                            + "\n필터를 더 추가하시거나, '끝'이라고 입력하여 최종 추천을 받아보세요."
                        )
                        # stage=1 그대로 유지 → 다음 필터를 입력받음
                    elif len(clubs) == 1:
                        bot_response = (
                            f"조건에 맞는 동아리가 하나 발견되었습니다:\n- {clubs[0]}\n추천을 마칩니다."
                        )
                        # 결과가 하나만 남았으므로, 자동으로 stage=0으로 리셋
                        session["stage"] = 0
                        session["base_filters"] = []
                    else:
                        # newResult empty이면서 prevResult도 empty인 상황
                        bot_response = (
                            "죄송합니다. 조건에 맞는 동아리가 없습니다.\n"
                            "필터를 수정하거나, 다른 MBTI/관심사를 입력해주세요."
                        )
                        session["stage"] = 0
                        session["base_filters"] = []

            session["history"].append(("user", user_input))
            session["history"].append(("bot", bot_response))
            return render_template("chat.html", history=session["history"])

        # ─── 예외 상황 ─────────────────────────────────────────
        bot_response = "알 수 없는 상태입니다. MBTI나 관심사를 입력해주세요."
        session["stage"] = 0
        session["base_filters"] = []
        session["history"].append(("user", user_input))
        session["history"].append(("bot", bot_response))
        return render_template("chat.html", history=session["history"])

    except Exception as e:
        bot_response = f"앗! 오류가 발생했어요: {str(e)}"
        session["history"].append(("user", user_input))
        session["history"].append(("bot", bot_response))
        return render_template("chat.html", history=session["history"])

@app.route("/clear", methods=["POST"])
def clear():
    session.clear()
    session["stage"] = 0
    session["base_filters"] = []
    return render_template("chat.html", history=[("bot", "MBTI나 관심사를 입력해주세요!")])

from flask import Flask, request, render_template, session
import subprocess
import os
import re

app = Flask(__name__)
app.secret_key = 'very_secret_key'

# C++ 실행파일 경로 (mbti_project 디렉터리 내 MixBot)
CPP_EXEC = os.path.join(os.path.dirname(__file__), 'mbti_project', 'MixBot')

VALID_MBTI = {
    "INTJ", "INTP", "ENTJ", "ENTP",
    "INFJ", "INFP", "ENFJ", "ENFP",
    "ISTJ", "ISFJ", "ESTJ", "ESFJ",
    "ISTP", "ISFP", "ESTP", "ESFP"
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

def extract_mbti(text):
    txt_upper = text.upper()
    return next((mbti for mbti in VALID_MBTI if mbti in txt_upper), None)

@app.route("/", methods=["GET"])
def index():
    session.clear()
    session["stage"] = 0
    session["mbti"] = None
    session["filters"] = []
    return render_template("chat.html", history=[("bot", "MBTI만 입력해주세요! 예: INFP")])

@app.route("/chat", methods=["POST"])
def chat():
    session.setdefault("history", [])
    user_input = request.form.get("keyword", "").strip()
    bot_response = ""

    try:
        stage = session.get("stage", 0)

        # “인기”만 입력된 경우 (stage 0에서만)
        if stage == 0 and re.fullmatch(r"인기( 동아리|순위)?", user_input):
            result = subprocess.run(
                [CPP_EXEC, "인기"],
                text=True, capture_output=True,
                encoding='utf-8', errors='replace'
            )
            bot_response = result.stdout.strip() if result.returncode == 0 else f"[오류] {result.stderr.strip()}"
            session["history"].append(("user", user_input))
            session["history"].append(("bot", bot_response))
            return render_template("chat.html", history=session["history"])

        # ─── 단계 0: MBTI 입력 대기 ────────────────────────────────────
        if stage == 0:
            mbti = extract_mbti(user_input)
            if mbti:
                session["mbti"] = mbti
                session["filters"] = []
                session["stage"] = 1
                bot_response = (
                    f"좋습니다! ‘{mbti}’을(를) 기반으로 추가 필터를 입력해주세요.\n"
                    "예) 시간대(오전/오후/저녁), 회비(무료/유료), 요일(월요일…일요일), 형태(온라인/오프라인).\n"
                    "모든 필터 입력 후 최종 추천을 원하시면 ‘끝’이라고 입력해주세요."
                )
            else:
                bot_response = "MBTI만 입력해주세요! 예: INFP"

            session["history"].append(("user", user_input))
            session["history"].append(("bot", bot_response))
            return render_template("chat.html", history=session["history"])

        # ─── 단계 1: 추가 필터 입력 중 ─────────────────────────────────
        if stage == 1:
            # “끝” 입력 시 → 누적된 필터 + MBTI + 카운트 플래그로 최종 호출
            if user_input == "끝":
                cmd = [CPP_EXEC, session["mbti"]] + session["filters"] + ["__count__"]
                result = subprocess.run(
                    cmd,
                    text=True, capture_output=True,
                    encoding='utf-8', errors='replace'
                )
                bot_response = result.stdout.strip() if result.returncode == 0 else f"[오류] {result.stderr.strip()}"

                # 상태 초기화
                session["stage"] = 0
                session["mbti"] = None
                session["filters"] = []
                session["history"].append(("user", user_input))
                session["history"].append(("bot", bot_response))
                return render_template("chat.html", history=session["history"])

            # 1) 유사어 적용 (예: "농구" → "운동")
            lower = user_input.lower()
            for syn, kw in similar_words.items():
                if syn in lower:
                    user_input = user_input.replace(syn, kw)

            # 2) 새 필터 추가
            session["filters"].append(user_input)

            # 3) C++ 호출 (사전보기 모드, 카운트 플래그 미포함)
            cmd = [CPP_EXEC, session["mbti"]] + session["filters"]
            result = subprocess.run(
                cmd,
                text=True, capture_output=True,
                encoding='utf-8', errors='replace'
            )

            if result.returncode != 0:
                # 오류 발생 → 직전 필터는 무시하고 재시도
                session["filters"].pop()
                bot_response = f"[오류] {result.stderr.strip() or '원인 불명'}"
            else:
                output = result.stdout.strip()

                # “해당되는 동아리가 없습니다” 메시지 검사
                if "해당되는 동아리가 없습니다" in output:
                    session["filters"].pop()
                    bot_response = (
                        f"‘{user_input}’에 해당되는 동아리가 없습니다.\n"
                        "다른 필터를 입력해주세요 (예: 오전/오후/유료/토요일/온라인)."
                    )
                    session["history"].append(("user", user_input))
                    session["history"].append(("bot", bot_response))
                    return render_template("chat.html", history=session["history"])

                # “‘- 동아리이름’” 줄만 골라 리스트 생성
                lines = [ln.strip() for ln in output.splitlines() if ln.startswith("- ")]
                clubs = [ln[2:] for ln in lines]

                if len(clubs) > 1:
                    bot_response = (
                        f"현재 조건으로 {len(clubs)}개의 동아리가 있습니다:\n"
                        + "\n".join(f"- {c}" for c in clubs)
                        + "\n필터를 더 입력하시거나, ‘끝’이라고 입력하여 최종 추천을 받아보세요."
                    )
                elif len(clubs) == 1:
                    bot_response = (
                        f"현재 조건에 맞는 동아리가 하나 남았습니다:\n- {clubs[0]}\n"
                        "더 좁히려면 필터를 추가 입력해주세요.\n"
                        "최종 추천을 원하시면 ‘끝’이라고 입력하세요."
                    )
                else:
                    bot_response = (
                        "죄송합니다. 조건에 맞는 동아리가 없습니다.\n"
                        "처음부터 MBTI를 다시 입력해주세요."
                    )
                    session["stage"] = 0
                    session["mbti"] = None
                    session["filters"] = []

            session["history"].append(("user", user_input))
            session["history"].append(("bot", bot_response))
            return render_template("chat.html", history=session["history"])

        # ─── 예외 상황 ─────────────────────────────────────────
        bot_response = "알 수 없는 상태입니다. MBTI만 입력해주세요."
        session["stage"] = 0
        session["mbti"] = None
        session["filters"] = []
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
    session["mbti"] = None
    session["filters"] = []
    return render_template("chat.html", history=[("bot", "MBTI만 입력해주세요! 예: INFP")])


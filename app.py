from flask import Flask, request, render_template, session
import subprocess
import os
import re

app = Flask(__name__)
app.secret_key = 'very_secret_key'

CPP_EXEC = os.path.join(os.path.dirname(__file__), 'mbti_project', 'MixBot')

VALID_MBTI = {
    "INTJ", "INTP", "ENTJ", "ENTP",
    "INFJ", "INFP", "ENFJ", "ENFP",
    "ISTJ", "ISFJ", "ESTJ", "ESFJ",
    "ISTP", "ISFP", "ESTP", "ESFP"
}

<<<<<<< HEAD
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
=======
INTEREST_KEYWORDS = {
    "운동":   ["운동", "축구", "농구", "헬스", "배드민턴", "러닝", "야구", "탁구"],
    "예술":   ["사진", "그림", "예술", "드로잉", "디자인", "페인팅"],
    "음악":   ["노래", "음악", "연주", "작곡", "기타", "피아노"],
    "봉사":   ["봉사", "기부", "도움", "자원봉사"],
    "토론":   ["토론", "토의", "의견", "논쟁", "스피치", "연설"],
    "IT":     ["코딩", "프로그래밍", "개발", "해킹", "컴퓨터", "AI", "인공지능"],
    "창업":   ["창업", "비즈니스", "마케팅", "기획", "스타트업"],
    "문학":   ["책", "소설", "시", "글쓰기", "에세이"],
}

def extract_mbti_and_interest(text):
    t = text.lower()
    t = re.sub(r"[은는이가요의을를]", " ", t)

    mbti_found = None
    for mbti in VALID_MBTI:
        if mbti.lower() in t:
            mbti_found = mbti
            break

    interest_found = None
    for category, kws in INTEREST_KEYWORDS.items():
        if any(kw in t for kw in kws):
            interest_found = category
            break

    return mbti_found, interest_found

@app.route("/", methods=["GET"])
def index():
    session.setdefault("history", [])
    return render_template("chat.html", history=session["history"])
>>>>>>> 250c004f92e537b703fd135608ad0474225a9c64

@app.route("/chat", methods=["POST"])
def chat():
    session.setdefault("history", [])
    user_input = request.form.get("keyword", "").strip()
    bot_response = ""

    try:
<<<<<<< HEAD
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
=======
        if uploaded_file and uploaded_file.filename:
            filename = secure_filename(uploaded_file.filename)
            saved_path = os.path.join(app.config['UPLOAD_FOLDER'], filename)
            uploaded_file.save(saved_path)
            bot_response = f"이미지 '{filename}'이 업로드되었습니다. (분석 기능 없음)"
            image_path = f"/static/uploads/{filename}"

        elif user_input:
            mbti, interest = extract_mbti_and_interest(user_input)

            if mbti:
                exe_path = os.path.join(os.path.dirname(__file__), 'mbti_project', 'MixedMbti.exe')
>>>>>>> 250c004f92e537b703fd135608ad0474225a9c64
                result = subprocess.run(
                    cmd,
                    text=True, capture_output=True,
                    encoding='utf-8', errors='replace'
                )
<<<<<<< HEAD
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
=======
                if result.returncode != 0:
                    bot_response = f"[MBTI 분석 실패] {result.stderr.strip() or '원인 불명'}"
                else:
                    bot_response = result.stdout.strip() or "[MBTI 분석 결과 없음]"
                image_path = "/static/sports.jpg"
            else:
                bot_response = ""

            if interest:
                if interest == "운동":
                    bot_response += "\n운동 관련 추천: 체대 동아리, 풋살 동아리, 농구 동아리"
                elif interest == "예술":
                    bot_response += "\n예술 관련 추천: 사진 동아리, 디자인 동아리"
                elif interest == "음악":
                    bot_response += "\n음악 관련 추천: 밴드 동아리, 작곡 동아리"
                elif interest == "봉사":
                    bot_response += "\n봉사 관련 추천: 봉사 동아리, 나눔 동아리"
                elif interest == "토론":
                    bot_response += "\n토론 관련 추천: 토론 동아리, 스피치 동아리"
                elif interest == "IT":
                    bot_response += "\n개발 관련 추천: 프로그래밍 동아리, 해킹 동아리"
                elif interest == "창업":
                    bot_response += "\n창업 관련 추천: 창업 동아리, 스타트업 동아리"
                elif interest == "문학":
                    bot_response += "\n문학 관련 추천: 문학 동아리, 창작 동아리"

                try:
                    interest_exe = os.path.join(os.path.dirname(__file__), "interest", "interest_recommender")
                    interest_result = subprocess.run(
                        [interest_exe],
                        input=user_input,
                        text=True,
                        capture_output=True,
                        encoding="utf-8",
                        errors="replace"
                    )
                    if interest_result.returncode == 0 and interest_result.stdout.strip():
                        bot_response += "\n\n[추천 결과]\n" + interest_result.stdout.strip()
                    else:
                        bot_response += "\n\n[추천 결과 없음 또는 오류]"
                except Exception as e:
                    bot_response += f"\n\n[추천 오류] {str(e)}"
            else:
                if not mbti:
                    bot_response = "지원되지 않는 키워드입니다. 예: infp, 운동, 예술, 음악, IT, 봉사, 토론, 창업, 문학"

        else:
            bot_response = "텍스트 또는 이미지를 입력해주세요."

    except Exception as e:
        bot_response = f"오류 발생: {str(e)}"

    if user_input:
>>>>>>> 250c004f92e537b703fd135608ad0474225a9c64
        session["history"].append(("user", user_input))
        session["history"].append(("bot", bot_response))
        return render_template("chat.html", history=session["history"])

@app.route("/clear", methods=["POST"])
def clear():
<<<<<<< HEAD
    session.clear()
    session["stage"] = 0
    session["mbti"] = None
    session["filters"] = []
    return render_template("chat.html", history=[("bot", "MBTI만 입력해주세요! 예: INFP")])
=======
    session.pop("history", None)
    return render_template("chat.html", history=[])
##파일 이름 미정
@app.route("/other")
def other_page():
    return render_template("other.html")
>>>>>>> 250c004f92e537b703fd135608ad0474225a9c64

if __name__ == "__main__":
    app.run(debug=True)

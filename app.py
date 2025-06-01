from flask import Flask, request, render_template, session
import subprocess
import os
import re
from werkzeug.utils import secure_filename

app = Flask(__name__, template_folder=os.path.join(os.path.dirname(__file__), 'template'))
app.secret_key = 'very_secret_key'
app.config['UPLOAD_FOLDER'] = 'static/uploads'
os.makedirs(app.config['UPLOAD_FOLDER'], exist_ok=True)

VALID_MBTI = {
    "INTJ", "INTP", "ENTJ", "ENTP",
    "INFJ", "INFP", "ENFJ", "ENFP",
    "ISTJ", "ISFJ", "ESTJ", "ESFJ",
    "ISTP", "ISFP", "ESTP", "ESFP"
}

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

    mbti_found = next((mbti for mbti in VALID_MBTI if mbti.lower() in t), None)
    interest_found = next((category for category, kws in INTEREST_KEYWORDS.items() if any(kw in t for kw in kws)), None)

    return mbti_found, interest_found

def is_meaningless(text):
    return text.strip() in {"?", "안녕", "ㅎ", "ㅎㅎ", "ㅋㅋ", "ㅠㅠ", "..."}

@app.route("/", methods=["GET"])
def index():
    session.setdefault("history", [])
    return render_template("chat.html", history=session["history"])

@app.route("/chat", methods=["POST"])
def chat():
    session.setdefault("history", [])

    user_input = request.form.get("keyword", "").strip()
    uploaded_file = request.files.get("image")
    image_path = None
    bot_response = ""

    try:
        if uploaded_file and uploaded_file.filename:
            filename = secure_filename(uploaded_file.filename)
            saved_path = os.path.join(app.config['UPLOAD_FOLDER'], filename)
            uploaded_file.save(saved_path)
            bot_response = f"이미지 '{filename}'이 업로드되었습니다. (분석 기능 없음)"
            image_path = f"/static/uploads/{filename}"

        elif user_input:
            mbti, interest = extract_mbti_and_interest(user_input)
            mbti_response = ""
            interest_response = ""

            if mbti:
                exe_path = os.path.join(os.path.dirname(__file__), 'mbti_project', 'MixedMbti')
                result = subprocess.run(
                    [exe_path, mbti], text=True, capture_output=True, encoding='utf-8', errors='replace'
                )
                if result.returncode != 0:
                    mbti_response = f"[MBTI 분석 실패] {result.stderr.strip() or '원인 불명'}"
                else:
                    mbti_response = result.stdout.strip() or "[MBTI 분석 결과 없음]"
                image_path = "/static/sports.jpg"

            if interest:
                interest_map = {
                    "운동": "운동 좋아하시는군요! 체대, 풋살, 농구 동아리를 추천드려요.",
                    "예술": "예술 감성 좋으시네요~ 사진, 디자인 동아리는 어떠세요?",
                    "음악": "음악을 좋아하신다니, 밴드나 작곡 동아리 추천드려요!",
                    "봉사": "따뜻한 마음을 가진 분이시군요! 봉사나 나눔 동아리는 어때요?",
                    "토론": "생각 나누기를 좋아하시나요? 토론, 스피치 동아리를 추천해요!",
                    "IT": "개발에 관심이 있다면, 프로그래밍이나 해킹 동아리를 추천할게요!",
                    "창업": "창업에 열정이 있다면, 창업, 스타트업 동아리와 잘 맞을 거예요!",
                    "문학": "문학적 감성이 풍부하시네요~ 문학, 창작 동아리를 추천드려요!"
                }
                interest_response = interest_map.get(interest, "")

            if not mbti and not interest:
                if is_meaningless(user_input):
                    bot_response = "이전에 알려주신 정보에 대한 추천은 이미 드렸어요. 다른 관심사나 궁금한 게 있으신가요?"
                else:
                    bot_response = "음... 제가 이해하기 어려운 표현이었어요. 예: infp, 운동, 예술 같은 키워드를 사용해 주세요!"
            else:
                bot_response = mbti_response
                if mbti_response and interest_response:
                    bot_response += " "
                bot_response += interest_response

            # 중복 응답 방지
            if session.get("last_response") == bot_response:
                bot_response = "이미 추천을 드렸던 내용이에요. 다른 질문이 있으신가요?"
            else:
                session["last_response"] = bot_response

        else:
            bot_response = "텍스트 또는 이미지를 입력해주세요."

    except Exception as e:
        bot_response = f"앗! 오류가 발생했어요: {str(e)}"

    if user_input:
        session["history"].append(("user", user_input))
    session["history"].append(("bot", bot_response))
    session.modified = True

    return render_template("chat.html", history=session["history"], image_path=image_path)

@app.route("/clear", methods=["POST"])
def clear():
    session.pop("history", None)
    session.pop("last_response", None)
    return render_template("chat.html", history=[])

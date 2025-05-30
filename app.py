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
    "운동": ["운동", "축구", "농구", "헬스", "배드민턴", "러닝", "야구", "탁구"],
    "예술": ["사진", "그림", "예술", "드로잉", "디자인", "페인팅"],
    "음악": ["노래", "음악", "연주", "작곡", "기타", "피아노"],
    "봉사": ["봉사", "기부", "도움", "자원봉사"],
    "토론": ["토론", "토의", "의견", "논쟁", "스피치", "연설"],
    "IT": ["코딩", "프로그래밍", "개발", "해킹", "컴퓨터", "AI", "인공지능"],
    "창업": ["창업", "비즈니스", "마케팅", "기획", "스타트업"],
    "문학": ["책", "소설", "시", "글쓰기", "에세이"],
}

def extract_mbti_and_interest(text):
    text = text.lower()

    # 조사 제거를 위한 간단 전처리
    text = re.sub(r"[은는이가요의을를]", " ", text)

    mbti_found = None
    interest_found = None

    # MBTI 판별
    for mbti in VALID_MBTI:
        if mbti.lower() in text:
            mbti_found = mbti
            break

    # 키워드 분석
    if any(word in text for word in ["운동", "축구", "농구"]):
        interest_found = "운동"
    elif any(word in text for word in ["사진", "그림", "예술"]):
        interest_found = "사진"

    return mbti_found, interest_found


@app.route("/", methods=["GET"])
def index():
    if "history" not in session:
        session["history"] = []
    return render_template("chat.html", history=session["history"])

@app.route("/chat", methods=["POST"])
def chat():
    if "history" not in session:
        session["history"] = []

    user_input = request.form.get("keyword", "").strip()
    uploaded_file = request.files.get("image")
    image_path = None
    bot_response = ""

    try:
        if uploaded_file and uploaded_file.filename != "":
            filename = secure_filename(uploaded_file.filename)
            saved_path = os.path.join(app.config['UPLOAD_FOLDER'], filename)
            uploaded_file.save(saved_path)
            bot_response = f"이미지 '{filename}'이 업로드되었습니다. (분석 기능 없음)"
            image_path = f"/static/uploads/{filename}"

        elif user_input:
            mbti, interest = extract_mbti_and_interest(user_input)

            if mbti:
                exe_path = os.path.join(os.path.dirname(__file__), 'mbti_project', 'MixedMbti.exe')
                result = subprocess.run(
                    [exe_path, mbti],
                    text=True,
                    capture_output=True,
                    encoding='utf-8',
                    errors='replace'
                )

                if result.returncode != 0:
                    bot_response = f"[MBTI 분석 실패] {result.stderr.strip() if result.stderr else '원인 불명'}"
                else:
                    bot_response = result.stdout.strip() or "[MBTI 분석 결과 없음]"
                image_path = "/static/sports.jpg"
            else:
                bot_response = ""

           if interest:
                   if interest == "운동":
                       bot_response += "\n운동 관련 추천: 체대 동아리, 풋살 동아리, 농구 동아리"
                       image_path = "/static/sports.jpg"
                 elif interest == "예술":
                       bot_response += "\n예술 관련 추천: 사진 동아리, 디자인 동아리"
                       image_path = "/static/photo.jpg"
                 elif interest == "음악":
                       bot_response += "\n음악 관련 추천: 밴드 동아리, 작곡 동아리"
                       image_path = "/static/music.jpg"
                 elif interest == "IT":
                       bot_response += "\n개발 관련 추천: 프로그래밍 동아리, 해킹 동아리"
                       image_path = "/static/it.jpg"
    # 추가적으로 다른 카테고리도 같은 방식으로 확장 가능

                 else:
                       bot_response = "텍스트 또는 이미지를 입력해주세요."

    except Exception as e:
        bot_response = f"오류 발생: {str(e)}"

    if user_input:
        session["history"].append(("user", user_input))
    session["history"].append(("bot", bot_response))
    session.modified = True

    return render_template("chat.html", history=session["history"], image_path=image_path)

@app.route("/clear", methods=["POST"])
def clear():
    session.pop("history", None)
    return render_template("chat.html", history=[])


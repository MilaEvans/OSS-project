from flask import Flask, request, render_template, session
import subprocess
import os
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

def extract_mbti_and_interest(text):
    words = text.upper().split()
    mbti = next((w for w in words if w in VALID_MBTI), None)
    interest = " ".join([w for w in text.split() if w.upper() != mbti]) if mbti else text
    return mbti, interest.strip()

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
                if "운동" in interest or "농구" in interest:
                    bot_response += "\n운동 관련 추천: 체대 동아리, 농구 동아리"
                    image_path = "/static/sports.jpg"
                elif "사진" in interest or "그림" in interest:
                    bot_response += "\n예술 관련 추천: 사진 동아리, 미술 동아리"
                    image_path = "/static/photo.jpg"
                elif not mbti:
                    bot_response = f"키워드 '{interest}'에 대한 추천만 제공됩니다.\n예: 운동, 사진"

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


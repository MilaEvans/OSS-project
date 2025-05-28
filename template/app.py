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
            upper_input = user_input.upper()

            if upper_input in VALID_MBTI:
                exe_path = os.path.join(os.path.dirname(__file__), 'mbti_project', 'MixedMbti.exe')
                result = subprocess.run(
                    [exe_path, upper_input],
                    text=True,
                    capture_output=True,
                    encoding='utf-8',  
                    errors='replace'
                )

                if result.returncode != 0:
                    bot_response = f"[실패] 오류 발생: {result.stderr.strip() if result.stderr else '실패 원인 불명'}"
                else:
                    bot_response = result.stdout.strip() if result.stdout else "[경고] 출력 없음"
                image_path = "/static/sports.jpg"

            elif "운동" in user_input:
                bot_response = "운동 관련 추천: 체대 동아리, 풋살 동아리"
                image_path = "/static/sports.jpg"

            elif "사진" in user_input:
                bot_response = "예술 감성 추천: 사진 동아리, 그림 동아리"
                image_path = "/static/photo.jpg"

            else:
                bot_response = "지원되지 않는 키워드입니다. 예: intj, 운동, 사진"
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

if __name__ == "__main__":
    print("✅ Flask 서버 시작 중... http://127.0.0.1:5000")
    app.run(debug=True)

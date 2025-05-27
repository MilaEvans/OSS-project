from flask import Flask, request, render_template
import subprocess
import os
from werkzeug.utils import secure_filename

app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = 'static/uploads'
os.makedirs(app.config['UPLOAD_FOLDER'], exist_ok=True)

@app.route("/", methods=["GET"])
def index():
    return render_template("chat.html")

@app.route("/chat", methods=["POST"])
def chat():
    user_input = request.form.get("keyword", "").strip()
    uploaded_file = request.files.get("image")
    image_path = None
    bot_response = ""

    try:
        if uploaded_file and uploaded_file.filename != "":
            # 이미지 업로드 처리
            filename = secure_filename(uploaded_file.filename)
            saved_path = os.path.join(app.config['UPLOAD_FOLDER'], filename)
            uploaded_file.save(saved_path)

            # 분석 로직: 추후 DeepFace 또는 C++ 연동 가능
            bot_response = f"이미지 '{filename}'을 업로드했습니다. (분석 결과를 여기에 표시할 수 있습니다)"
            image_path = f"/static/uploads/{filename}"

        elif user_input:
            if "mbti" in user_input.lower():
                result = subprocess.run(
                    ['./mbti_project/MixedMbti.exe'],
                    input=user_input,
                    text=True,
                    capture_output=True,
                    encoding='cp949'
                )
                bot_response = result.stdout.strip()
                image_path = "/static/sports.jpg"

            elif "사진" in user_input or "이미지" in user_input:
                result = subprocess.run(
                    ['./Image extraction/image_extraction.exe'],
                    input=user_input,
                    text=True,
                    capture_output=True,
                    encoding='cp949'
                )
                bot_response = result.stdout.strip()
                image_path = "/static/photo.jpg"

            else:
                bot_response = "지원하지 않는 키워드입니다. 'mbti' 또는 '사진'을 포함해주세요."
        else:
            bot_response = "텍스트 또는 이미지를 입력해주세요."

    except Exception as e:
        bot_response = f"오류 발생: {str(e)}"

    return render_template("chat.html", user_input=user_input, bot_response=bot_response, image_path=image_path)

if __name__ == "__main__":
    app.run(debug=True)

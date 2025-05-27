from flask import Flask, request, render_template
import subprocess
import os
from werkzeug.utils import secure_filename
from deepface_club_recommender import recommend_club_by_face

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
            filename = secure_filename(uploaded_file.filename)
            saved_path = os.path.join(app.config['UPLOAD_FOLDER'], filename)
            uploaded_file.save(saved_path)

            bot_response = recommend_club_by_face(saved_path)
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

            #아래부분은 현재 MBTI외의 기능이 없기 때문에 일시적으로 MBTI외의 반응을 보기위해 추가해놓은 것임
            elif "운동" in user_input:
                bot_response = "운동 관련 추천: 체대 동아리, 풋살 동아리"
                image_path = "/static/sports.jpg"

            elif "사진" in user_input:
                bot_response = "예술 감성 추천: 사진 동아리, 그림 동아리"
                image_path = "/static/photo.jpg"

            else:
                bot_response = "지원되지 않는 키워드입니다. 예: mbti, 운동, 사진"
        else:
            bot_response = "텍스트 또는 이미지를 입력해주세요."

    except Exception as e:
        bot_response = f"오류 발생: {str(e)}"

    return render_template("chat.html", user_input=user_input, bot_response=bot_response, image_path=image_path)

if __name__ == "__main__":
    app.run(debug=True)

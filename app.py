
from flask import Flask, request, render_template
import subprocess
import os

app = Flask(__name__)

@app.route("/")
def index():
    return render_template("chat.html")

@app.route("/chat", methods=["POST"])
def chat():
    user_input = request.form["keyword"]
    image_path = "/static/default.jpg"
    
    if "운동" in user_input:
        bot_response = "운동 관련 추천 동아리는 '체육회', '풋살 동아리' 등이 있어요! ⚽"
        image_path = "/static/default.jpg"
    elif "사진" in user_input or "이미지" in user_input:
        bot_response = "당신은 예술적인 감성이 뛰어나군요! 사진 동아리를 추천합니다. 📸"
        image_path = "/static/default.jpg"
    else:
        try:
            result = subprocess.run(
                ['./mbti_project/MixedMbti.exe'],
                input=user_input,
                text=True,
                capture_output=True
            )
            bot_response = result.stdout.strip() or "추천 결과가 없습니다."
        except Exception as e:
            bot_response = f"추천 처리 중 오류 발생: {str(e)}"

    return render_template("chat.html", user_input=user_input, bot_response=bot_response, image_path=image_path)

if __name__ == "__main__":
    app.run(debug=True)

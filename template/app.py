from flask import Flask, request, render_template
import subprocess

app = Flask(__name__)

@app.route("/", methods=["GET"])
def index():
    return render_template("chat.html")

@app.route("/chat", methods=["POST"])
def chat():
    user_input = request.form.get("keyword", "").strip()
    bot_response = ""
    image_path = None

    try:
        if user_input and "mbti" in user_input.lower():
            result = subprocess.run(
                ['./mbti_project/MixedMbti.exe'],
                input=user_input,
                text=True,
                capture_output=True,
                encoding='cp949'
            )
            bot_response = result.stdout.strip()
            image_path = "/static/sports.jpg"
        else:
            bot_response = "지원하는 키워드는 'mbti'입니다."

    except Exception as e:
        bot_response = f"오류 발생: {str(e)}"

    return render_template("chat.html", user_input=user_input, bot_response=bot_response, image_path=image_path)

if __name__ == "__main__":
    app.run(debug=True)

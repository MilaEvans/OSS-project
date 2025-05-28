from flask import Flask, request, jsonify, session, render_template
import subprocess

app = Flask(__name__)
app.secret_key = "my-secret-key"

MBTI_EXE_PATH = "./mbti_bot"

valid_mbtis = {
    "INTJ", "INTP", "ENTJ", "ENTP",
    "INFJ", "INFP", "ENFJ", "ENFP",
    "ISTJ", "ISFJ", "ESTJ", "ESFJ",
    "ISTP", "ISFP", "ESTP", "ESFP"
}

@app.route("/")
def index():
    return render_template("chat.html")

@app.route("/chat", methods=["POST"])
def chat():
    user_input = request.form.get("user_input", "").strip()
    user_input_upper = user_input.upper()
    context = session.get("context", {})
    response = ""

    if "mbti" not in context:
        if len(user_input_upper) == 4 and user_input_upper in valid_mbtis:
            try:
                result = subprocess.run(
                    [MBTI_EXE_PATH, user_input_upper],
                    capture_output=True,
                    text=True,
                    timeout=3
                )
                if result.returncode == 0:
                    context["mbti"] = user_input_upper
                    response = result.stdout.strip() + "\n어떤 활동이나 관심 키워드를 가지고 계신가요?"
                else:
                    response = "MBTI 분석 실패"
            except Exception as e:
                response = f"C++ 실행 오류: {str(e)}"
        else:
            response = "MBTI를 먼저 정확히 입력해주세요. 예: infp"
    elif "interest" not in context:
        context["interest"] = user_input
        response = f"관심 키워드 '{user_input}'에 대해 동아리를 탐색 중입니다... (예: 농구 → 스포츠 동아리!)"
    else:
        response = f"입력하신 MBTI는 {context['mbti']}, 관심사는 {context['interest']}입니다.\n새로운 질문을 하시려면 새로고침해주세요."

    session["context"] = context
    return jsonify({"response": response})

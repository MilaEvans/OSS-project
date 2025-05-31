from flask import Flask, request, render_template, session
import subprocess
import os
import re
from werkzeug.utils import secure_filename

app = Flask(
    __name__,
    template_folder=os.path.join(os.path.dirname(__file__), 'template')  
)
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
    """
    텍스트에서 MBTI(대문자)와 관심사 카테고리를 추출합니다.
    """
    
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
                exe_path = os.path.join(
                    os.path.dirname(__file__),
                    'mbti_project',
                    'MixedMbti.exe'
                )
                result = subprocess.run(
                    [exe_path, mbti],
                    text=True,
                    capture_output=True,
                    encoding='utf-8',
                    errors='replace'
                )
                if result.returncode != 0:
                    mbti_response = f"[MBTI 분석 실패] {result.stderr.strip() or '원인 불명'}"
                else:
                    mbti_response = result.stdout.strip() or "[MBTI 분석 결과 없음]"

                image_path = "/static/sports.jpg"

            if interest:
                interest_map = {
                    "운동": "운동 관련 추천: 체대 동아리, 풋살 동아리, 농구 동아리",
                    "예술": "예술 관련 추천: 사진 동아리, 디자인 동아리",
                    "음악": "음악 관련 추천: 밴드 동아리, 작곡 동아리",
                    "봉사": "봉사 관련 추천: 봉사 동아리, 나눔 동아리",
                    "토론": "토론 관련 추천: 토론 동아리, 스피치 동아리",
                    "IT": "개발 관련 추천: 프로그래밍 동아리, 해킹 동아리",
                    "창업": "창업 관련 추천: 창업 동아리, 스타트업 동아리",
                    "문학": "문학 관련 추천: 문학 동아리, 창작 동아리"
                }
                interest_response = interest_map.get(interest, "")

            if not mbti and not interest:
                bot_response = "지원되지 않는 키워드입니다. 예: infp, 운동, 예술, 음악, IT, 봉사, 토론, 창업, 문학"
            else:
                
                bot_response = mbti_response
                if mbti_response and interest_response:
                    bot_response += "\n"
                bot_response += interest_response

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

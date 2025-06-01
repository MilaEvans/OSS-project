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

@app.route('/', methods=['GET', 'POST'])
def index():
    user_input = ""
    bot_response = ""

    if request.method == 'POST':
        user_input = request.form.get('keyword', '')

        try:
            exe_path = os.path.join('interest', 'interest_recommender')
            result = subprocess.run(
                [exe_path],
                input=user_input,
                text=True,
                capture_output=True
            )

            bot_response = result.stdout.strip()
        except Exception as e:
            bot_response = f"오류 발생: {str(e)}"

    return render_template('chat.html', user_input=user_input, bot_response=bot_response)

if __name__ == '__main__':
    app.run(debug=True)


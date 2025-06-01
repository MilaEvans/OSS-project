# app.py
from flask import Flask, request, render_template, session
import subprocess
import os

app = Flask(__name__, template_folder=os.path.join(os.path.dirname(__file__), 'template'))
app.secret_key = 'very_secret_key'

# (1) 업로드 폴더 설정 및 생성
app.config['UPLOAD_FOLDER'] = 'static/uploads'
os.makedirs(app.config['UPLOAD_FOLDER'], exist_ok=True)

# (2) C++ 실행 파일 경로
# 실제 빌드된 MixedMbti 바이너리(위에서 수정한)를 지정하세요
MIXED_MBTI_EXEC = os.path.join(os.path.dirname(__file__), 'mbti_project', 'MixedMbti')

# (3) 세션 히스토리 초기화
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
        # (4) 이미지 업로드 처리 (별도 분석 기능 없음)
        if uploaded_file and uploaded_file.filename:
            filename = uploaded_file.filename
            saved_path = os.path.join(app.config['UPLOAD_FOLDER'], filename)
            uploaded_file.save(saved_path)
            bot_response = f"이미지 '{filename}'이 업로드되었습니다. (분석 기능 없음)"
            image_path = f"/static/uploads/{filename}"

        # (5) 텍스트 입력이 있을 때만 C++ 실행 파일 호출
        elif user_input:
            # subprocess로 C++ 실행, CLI 인자에 user_input 전체를 넘김
            result = subprocess.run(
                [MIXED_MBTI_EXEC] + user_input.split(),
                text=True,
                capture_output=True,
                encoding='utf-8',
                errors='replace'
            )
            if result.returncode != 0:
                bot_response = f"[오류 발생] {result.stderr.strip() or '원인 불명'}"
            else:
                bot_response = result.stdout.strip()

        else:
            bot_response = "텍스트 또는 이미지를 입력해주세요."

    except Exception as e:
        bot_response = f"앗! 오류가 발생했어요: {str(e)}"

    # (6) 세션 히스토리에 추가
    if user_input:
        session["history"].append(("user", user_input))
    session["history"].append(("bot", bot_response))
    session.modified = True

    return render_template("chat.html", history=session["history"], image_path=image_path)

@app.route("/clear", methods=["POST"])
def clear():
    session.pop("history", None)
    return render_template("chat.html", history=[])


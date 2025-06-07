from flask import Flask, request, render_template, session, redirect, url_for
from werkzeug.utils import secure_filename
import subprocess
import os
import re

app = Flask(__name__)
app.secret_key = 'very_secret_key'

# 이미지 업로드 폴더 설정
app.config['UPLOAD_FOLDER'] = 'static/uploads'
os.makedirs(app.config['UPLOAD_FOLDER'], exist_ok=True)

@app.template_filter('tag_class')
def tag_class_filter(value):
    return {
        '운동': 'badge-sports',
        '음악': 'badge-music',
        '기술': 'badge-tech',
        '봉사': 'badge-service',
        '기타': 'badge-default'
    }.get(value, 'badge-default')

CPP_EXEC = os.path.join(os.path.dirname(__file__), 'mbti_project', 'MixBot')

VALID_MBTI = {
    "INTJ", "INTP", "ENTJ", "ENTP",
    "INFJ", "INFP", "ENFJ", "ENFP",
    "ISTJ", "ISFJ", "ESTJ", "ESFJ",
    "ISTP", "ISFP", "ESTP", "ESFP"
}

similar_words = {}
with open("similar_words.txt", "r", encoding="utf-8") as f:
    for line in f:
        line = line.strip()
        if not line:
            continue
        parts = line.split()
        if len(parts) == 2:
            synonym, keyword = parts
            similar_words[synonym] = keyword

def extract_mbti(text):
    txt_upper = text.upper()
    return next((mbti for mbti in VALID_MBTI if mbti in txt_upper), None)

@app.route("/", methods=["GET"])
def index():
    return render_template("chat.html", history=[("bot", "안녕하세요! 😊\n당신의 MBTI를 입력해주시면, 어울리는 동아리를 추천해드릴게요.\n예: INFP")])

@app.route("/chat", methods=["GET", "POST"])
def chat():
    session.setdefault("history", [])
    user_input = request.form.get("keyword", "").strip()
    bot_response = ""

    try:
        stage = session.get("stage", 0)

        if request.method == "GET":
            return render_template("chat.html", history=session["history"])

        if stage == 0 and re.fullmatch(r"인기( 동아리|순위)?", user_input):
            result = subprocess.run(
                [CPP_EXEC, "인기"],
                text=True, capture_output=True,
                encoding='utf-8', errors='replace'
            )
            bot_response = "앗, 아직 MBTI를 입력하지 않으셨어요 😅 예: ENFP 처럼 입력해주세요!"
            session["history"].append(("user", user_input))
            session["history"].append(("bot", bot_response))
            return render_template("chat.html", history=session["history"])

        if stage == 0:
            mbti = extract_mbti(user_input)
            if mbti:
                session["mbti"] = mbti
                session["filters"] = []
                session["stage"] = 1
                bot_response = (
                        f"{mbti} 타입이시군요! 😊\n"
                        "좋아요, 취향에 맞는 동아리를 찾기 위해 몇 가지를 여쭤볼게요.\n\n"
                        "🕒 선호하는 시간대가 있나요? (예: 오전, 오후, 저녁)\n"
                        "💰 회비는 어떻게 되면 좋을까요? (무료 / 유료)\n"
                        "📅 가능한 요일이 있으신가요? (월~일 중 선택)\n"
                        "🌐 활동 형태는요? (온라인 / 오프라인)\n\n"
                        "모두 입력하셨다면 ‘끝’이라고 입력해 주세요!"
                )
            else:
                bot_response = "MBTI만 입력해주세요! 예: INFP"
            session["history"].append(("user", user_input))
            session["history"].append(("bot", bot_response))
            return render_template("chat.html", history=session["history"])

        if stage == 1:
            if user_input == "끝":
                cmd = [CPP_EXEC, session["mbti"]] + session["filters"] + ["__count__"]
                result = subprocess.run(
                    cmd,
                    text=True, capture_output=True,
                    encoding='utf-8', errors='replace'
                )
                bot_response = result.stdout.strip() if result.returncode == 0 else f"[오류] {result.stderr.strip()}"
                session["stage"] = 0
                session["mbti"] = None
                session["filters"] = []
                session["history"].append(("user", user_input))
                session["history"].append(("bot", bot_response))
                return render_template("chat.html", history=session["history"])

            lower = user_input.lower()
            for syn, kw in similar_words.items():
                if syn in lower:
                    user_input = user_input.replace(syn, kw)

            session["filters"].append(user_input)
            cmd = [CPP_EXEC, session["mbti"]] + session["filters"]
            result = subprocess.run(
                cmd,
                text=True, capture_output=True,
                encoding='utf-8', errors='replace'
            )
            if result.returncode != 0:
                session["filters"].pop()
                bot_response = f"[오류] {result.stderr.strip() or '원인 불명'}"
            else:
                output = result.stdout.strip()
                if "해당되는 동아리가 없습니다" in output:
                    session["filters"].pop()
                    bot_response = (
                        f"‘{user_input}’에 해당되는 동아리가 없습니다.\n"
                        "다른 필터를 입력해주세요 (예: 오전/오후/유료/토요일/온라인)."
                    )
                    session["history"].append(("user", user_input))
                    session["history"].append(("bot", bot_response))
                    return render_template("chat.html", history=session["history"])

                lines = [ln.strip() for ln in output.splitlines() if ln.startswith("- ")]
                clubs = [ln[2:] for ln in lines]

                if len(clubs) > 1:
                    bot_response = (
                        f"현재 조건으로 {len(clubs)}개의 동아리가 있습니다:\n"
                        + "\n".join(f"- {c}" for c in clubs)
                        + "\n필터를 더 입력하시거나, ‘끝’이라고 입력하여 최종 추천을 받아보세요."
                    )
                elif len(clubs) == 1:
                    bot_response = (
                        f"현재 조건에 맞는 동아리가 하나 남았습니다:\n- {clubs[0]}\n"
                        "더 좁히려면 필터를 추가 입력해주세요.\n"
                        "최종 추천을 원하시면 ‘끝’이라고 입력하세요."
                    )
                else:
                    bot_response = (
                        "죄송합니다. 조건에 맞는 동아리가 없습니다.\n"
                        "처음부터 MBTI를 다시 입력해주세요."
                    )
                    session["stage"] = 0
                    session["mbti"] = None
                    session["filters"] = []

            session["history"].append(("user", user_input))
            session["history"].append(("bot", bot_response))
            return render_template("chat.html", history=session["history"])

        bot_response = "알 수 없는 상태입니다. MBTI만 입력해주세요."
        session["stage"] = 0
        session["mbti"] = None
        session["filters"] = []
        session["history"].append(("user", user_input))
        session["history"].append(("bot", bot_response))
        return render_template("chat.html", history=session["history"])

    except Exception as e:
        bot_response = f"앗! 오류가 발생했어요: {str(e)}"
        session["history"].append(("user", user_input))
        session["history"].append(("bot", bot_response))
        return render_template("chat.html", history=session["history"])

@app.route("/clear", methods=["POST"])
def clear():
    session.clear()
    session["stage"] = 0
    session["mbti"] = None
    session["filters"] = []
    return render_template("chat.html", history=[("bot", "MBTI만 입력해주세요! 예: INFP")])

@app.route("/result")
def result():
    uploaded_image = session.get("uploaded_image")
    return render_template("result.html", uploaded_image=uploaded_image)

@app.route("/club/<club_id>")
def club_detail(club_id):
    return render_template("club_detail.html", club_id=club_id)

@app.route("/apply", methods=["GET", "POST"])
def apply_form():
    if request.method == "POST":
        return redirect(url_for("apply_success"))
    return render_template("apply_form.html")

@app.route("/apply/success")
def apply_success():
    return render_template("apply_success.html")

@app.route("/index", methods=["GET", "POST"])
def index_page():
    if request.method == "POST":
        file = request.files.get("image")
        if file:
            filename = secure_filename(file.filename)
            upload_path = os.path.join(app.config['UPLOAD_FOLDER'], filename)
            file.save(upload_path)

            session["uploaded_image"] = url_for('static', filename='uploads/' + filename)
            return redirect(url_for("result"))
        else:
            return render_template("index.html", error="이미지를 업로드해주세요.")
    return render_template("index.html")

if __name__ == "__main__":
    app.run(debug=True)

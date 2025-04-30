from flask import Flask, request, render_template
import subprocess

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('chat.html', user_input=None, bot_response=None)

@app.route('/chat', methods=['POST'])
def chat():
    user_input = request.form['keyword']

    # C++ 추천 로직 실행
    result = subprocess.run(
        ['./()'],  # 윈도우면 './().exe'
        input=user_input,
        text=True,
        capture_output=True
    )

    bot_response = result.stdout.strip()
    return render_template('chat.html', user_input=user_input, bot_response=bot_response)

if __name__ == '__main__':
    app.run(debug=True)

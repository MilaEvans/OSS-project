import os
import subprocess
from flask import Flask, request, render_template
from PIL import Image
import pytesseract

app = Flask(__name__)
UPLOAD_FOLDER = 'uploads'
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

# Tesseract 경로 (본인 환경에 맞게 수정)
pytesseract.pytesseract.tesseract_cmd = r'C:\Program Files\Tesseract-OCR\tesseract.exe'

# 동아리 정보 딕셔너리
clubs_info = {
    "코딩": "파이썬과 웹 개발을 배우는 소모임입니다.",
    "Robot Club": "자율주행 로봇을 직접 설계하고 구현하는 동아리입니다.",
    "AI Club": "머신러닝과 인공지능을 연구하며 대회에 참가합니다.",
    "Electric Vehicle Club": "전기차 및 자율주행 시스템을 개발하는 동아리입니다.",
    "Computer Vision Club": "영상처리와 객체 인식을 연구하는 동아리입니다."
}

# clean 함수 정의
def clean(text):
    return ''.join(c.lower() for c in text if not c.isspace())

@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'POST':
        file = request.files['poster']
        if not file:
            return "No file uploaded"

        filepath = os.path.join(UPLOAD_FOLDER, file.filename)
        file.save(filepath)

        ocr_text = pytesseract.image_to_string(Image.open(filepath), lang='kor+eng')

        textfile_path = os.path.join(UPLOAD_FOLDER, 'ocr_input.txt')
        with open(textfile_path, 'w', encoding='cp949') as f:
            f.write(ocr_text)

        cpp_exe_path = r'C:\mytest\doseong\recommender1.exe'

        try:
            result = subprocess.run(
                [cpp_exe_path, textfile_path],
                capture_output=True,
                text=True,
                encoding='cp949',
                timeout=5
            )
            output = result.stdout
        except Exception as e:
            output = f"Error running C++ program: {str(e)}"

        os.remove(filepath)
        os.remove(textfile_path)

        # 하이퍼링크 삽입
        output_lines = output.strip().split('\n')
        linked_output = []
        for line in output_lines:
            for club in clubs_info:
                if club in line:
                    line = line.replace(club, f'<a href="/club/{club}">{club}</a>')
            linked_output.append(line)

        return render_template('result.html',
                               ocr_text=ocr_text,
                               output='\n'.join(linked_output),
                               clubs=list(clubs_info.items()))

    return render_template('index.html')

@app.route('/club/<club_name>')
def club_detail(club_name):
    desc = clubs_info.get(club_name, "정보가 없습니다.")
    return render_template('club_detail.html', club_name=club_name, description=desc)

if __name__ == '__main__':
    app.run(debug=True)

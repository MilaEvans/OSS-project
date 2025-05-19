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

        cpp_exe_path = r'C:\mytest\doseong\recommender1.exe'  # C++ 실행파일 경로

        try:
            # C++ 프로그램 실행, 결과 캡처
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

        # 동아리 정보 (결과 출력용)
        clubs = [
            ("코딩", "코딩!!!!"),
            ("Robot Club", "자율주행 로봇 개발"),
            ("AI Club", "자율주행 인공지능 알고리즘 연구"),
            ("Electric Vehicle Club", "전기차 및 자율주행차 연구"),
            ("Computer Vision Club", "컴퓨터 비전과 영상 처리 연구")
        ]

        return render_template('result.html',
                               ocr_text=ocr_text,
                               output=output,
                               clubs=clubs)

    return render_template('index.html')


if __name__ == '__main__':
    app.run(debug=True)

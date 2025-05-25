import os
import subprocess
from flask import Flask, request, render_template
from PIL import Image
import pytesseract

app = Flask(__name__)
UPLOAD_FOLDER = 'uploads'
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

# Tesseract 경로 (환경에 맞게 수정)
pytesseract.pytesseract.tesseract_cmd = r'C:\Program Files\Tesseract-OCR\tesseract.exe'

# 동아리 정보
clubs_info = {
    "COSMIC": "컴퓨터공학과 동아리",
    "CaTs": "컴퓨터공학과 동아리",
    "CERT": "컴퓨터공학과 동아리",
}

@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'POST':
        file = request.files.get('poster')
        if not file:
            return "No file uploaded", 400

        filepath = os.path.join(UPLOAD_FOLDER, file.filename)
        file.save(filepath)

        # OCR로 텍스트 추출
        ocr_text = pytesseract.image_to_string(Image.open(filepath), lang='kor+eng')

        # OCR 텍스트 저장
        textfile_path = os.path.join(UPLOAD_FOLDER, 'ocr_input.txt')
        with open(textfile_path, 'w', encoding='cp949') as f:
            f.write(ocr_text)

        # C++ 실행
        cpp_exe_path = r'C:\mytest\doseong\recommender1.exe'
        try:
            proc = subprocess.Popen(
                [cpp_exe_path, textfile_path],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                encoding='cp949'
            )
            output, errors = proc.communicate(timeout=5)
            if errors:
                print("C++ 프로그램 에러:", errors)
        except subprocess.TimeoutExpired:
            proc.kill()
            output, errors = proc.communicate()
            output = f"Timeout expired. Process killed.\n{output}"
        except Exception as e:
            output = f"Error running C++ program: {str(e)}"

        # 임시 파일 삭제
        os.remove(filepath)
        os.remove(textfile_path)

        # 결과 파싱
        recommended_clubs = []
        for line in output.strip().split('\n'):
            if ':' in line:
                name, _ = line.split(':', 1)
                name = name.strip()
                if name in clubs_info:
                    icon_map = {
                        "코딩": "laptop-code",
                        "Robot Club": "robot",
                        "AI Club": "cpu",
                        "Electric Vehicle Club": "battery-half",
                        "Computer Vision Club": "camera-video"
                    }
                    recommended_clubs.append({
                        "name": name,
                        "description": clubs_info[name],
                        "icon": icon_map.get(name, "star"),
                        "tags": ["추천"]
                    })

        return render_template('result.html',
                               ocr_text=ocr_text,
                               recommended_clubs=recommended_clubs)

    return render_template('index.html')

@app.route('/club/<club_name>')
def club_detail(club_name):
    desc = clubs_info.get(club_name, "정보가 없습니다.")
    return render_template('club_detail.html', club_name=club_name, description=desc)

if __name__ == '__main__':
    app.run(debug=True)

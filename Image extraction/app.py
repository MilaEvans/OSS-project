import os
import subprocess
from flask import Flask, request, render_template
from PIL import Image
import pytesseract

app = Flask(__name__)
UPLOAD_FOLDER = 'uploads'
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

# Tesseract 경로 (본인 환경에 맞게 수정하세요)
pytesseract.pytesseract.tesseract_cmd = r'C:\Program Files\Tesseract-OCR\tesseract.exe'

# 동아리 정보 딕셔너리 (이름 : 설명)
clubs_info = {
    "코딩": "파이썬과 웹 개발을 배우는 소모임입니다.",
    "Robot Club": "자율주행 로봇을 직접 설계하고 구현하는 동아리입니다.",
    "AI Club": "머신러닝과 인공지능을 연구하며 대회에 참가합니다.",
    "Electric Vehicle Club": "전기차 및 자율주행 시스템을 개발하는 동아리입니다.",
    "Computer Vision Club": "영상처리와 객체 인식을 연구하는 동아리입니다."
}

@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'POST':
        file = request.files.get('poster')
        if not file:
            return "No file uploaded", 400

        filepath = os.path.join(UPLOAD_FOLDER, file.filename)
        file.save(filepath)

        # OCR로 이미지에서 텍스트 추출
        ocr_text = pytesseract.image_to_string(Image.open(filepath), lang='kor+eng')

        # OCR 텍스트를 임시 파일로 저장 (C++ 프로그램 입력용)
        textfile_path = os.path.join(UPLOAD_FOLDER, 'ocr_input.txt')
        with open(textfile_path, 'w', encoding='cp949') as f:
            f.write(ocr_text)

        # C++ 실행 파일 경로 (환경에 맞게 수정)
        cpp_exe_path = r'C:\mytest\doseong\recommender1.exe'

        try:
            # C++ 프로그램 실행해서 키워드 매칭 결과 받기
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

        # 임시 파일 삭제
        os.remove(filepath)
        os.remove(textfile_path)

        # C++ 출력 결과를 파싱해서 리스트로 변환
        recommended_clubs = []
        for line in output.strip().split('\n'):
            if ':' in line:
                name, _ = line.split(':', 1)
                name = name.strip()
                if name in clubs_info:
                    # 아이콘 예시를 동아리별로 지정 (원하는 아이콘으로 변경 가능)
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
                        "tags": ["추천"]  # 필요하면 태그 추가
                    })

        return render_template('result.html',
                               ocr_text=ocr_text,
                               recommended_clubs=recommended_clubs)

    # GET 요청 시 파일 업로드 폼 렌더링
    return render_template('index.html')

@app.route('/club/<club_name>')
def club_detail(club_name):
    desc = clubs_info.get(club_name, "정보가 없습니다.")
    return render_template('club_detail.html', club_name=club_name, description=desc)

if __name__ == '__main__':
    app.run(debug=True)

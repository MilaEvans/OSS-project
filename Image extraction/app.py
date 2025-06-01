import os
import subprocess
from flask import Flask, request, render_template, redirect, url_for, send_from_directory
from PIL import Image
import pytesseract
import datetime
from markupsafe import Markup

app = Flask(__name__)

UPLOAD_FOLDER = 'uploads'
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

@app.template_filter('nl2br')
def nl2br_filter(s):
    if s is None:
        return ''
    return Markup(s.replace('\n', '<br>\n'))

pytesseract.pytesseract.tesseract_cmd = r'C:\Program Files\Tesseract-OCR\tesseract.exe'

clubs_info = {
    # 코딩
    "코딩하는친구들": {
        "description": "코딩 동아리",
        "introduction": "코딩하는친구들은 ",
        "schedule": [
            "5-7 매주 수요일 오후 6시 ~ 8시",
        ],
        "category": "프로그래밍",
        "members": 30,
        "tags": ["오전", "회비 필요", "월요일", "온라인"],
    },
    "밤샘코딩": {
        "description": "코딩 동아리",
        "introduction": "밤샘코딩은",
        "schedule": [
            "5-6 매주 화요일 오후 7시 ~ 9시",
        ],
        "category": "게임 개발",
        "members": 20,
    },
    "코드수다방": {
        "description": "코딩 동아리",
        "introduction": "코드수다방은 ",
        "schedule": [
            "5-9 매주 금요일 오후 5시 ~ 7시",
        ],
        "category": "보안",
        "members": 15,
    },
    "오픈소스동네": {
        "description": "코딩 동아리",
        "introduction": "오픈소스동네 동아리는 ",
        "schedule": [
            "5-9 매주 금요일 오후 5시 ~ 7시",
        ],
        "category": "보안",
        "members": 15,
    },
    "디버깅모임": {
        "description": "코딩 동아리",
        "introduction": "디버깅모임 동아리는 ",
        "schedule": [
            "5-9 매주 금요일 오후 5시 ~ 7시",
        ],
        "category": "보안",
        "members": 15,
    },

    # 운동
    
    "땀흘리기": {
        "description": "운동 동아리",
        "introduction": "땀흘리기 동아리는",
        "schedule": [
            "5-6 매주 화요일 오후 7시 ~ 9시",
        ],
        "category": "헬스",
        "members": 20,
    },
    "운동하자": {
        "description": "운동 동아리",
        "introduction": "땀흘리기 동아리는",
        "schedule": [
            "5-6 매주 화요일 오후 7시 ~ 9시",
        ],
        "category": "헬스",
        "members": 20,
    },
    "뛰뛰빵빵": {
        "description": "운동 동아리",
        "introduction": "땀흘리기 동아리는",
        "schedule": [
            "5-6 매주 화요일 오후 7시 ~ 9시",
        ],
        "category": "헬스",
        "members": 20,
    },
    "런닝메이트": {
        "description": "운동 동아리",
        "introduction": "땀흘리기 동아리는",
        "schedule": [
            "5-6 매주 화요일 오후 7시 ~ 9시",
        ],
        "category": "헬스",
        "members": 20,
    },
    "주말농구": {
        "description": "운동 동아리",
        "introduction": "땀흘리기 동아리는",
        "schedule": [
            "5-6 매주 화요일 오후 7시 ~ 9시",
        ],
        "category": "헬스",
        "members": 20,
    },

    # 댄스
    
    "춤추는우리": {
        "description": "운동 동아리",
        "introduction": "땀흘리기 동아리는",
        "schedule": [
            "5-6 매주 화요일 오후 7시 ~ 9시",
        ],
        "category": "헬스",
        "members": 20,
    },
    "리듬따라": {
        "description": "운동 동아리",
        "introduction": "땀흘리기 동아리는",
        "schedule": [
            "5-6 매주 화요일 오후 7시 ~ 9시",
        ],
        "category": "헬스",
        "members": 20,
    },
    "뽐내기댄스": {
        "description": "운동 동아리",
        "introduction": "땀흘리기 동아리는",
        "schedule": [
            "5-6 매주 화요일 오후 7시 ~ 9시",
        ],
        "category": "헬스",
        "members": 20,
    },
    "비트속으로": {
        "description": "운동 동아리",
        "introduction": "땀흘리기 동아리는",
        "schedule": [
            "5-6 매주 화요일 오후 7시 ~ 9시",
        ],
        "category": "헬스",
        "members": 20,
    },
    "즐거운댄스": {
        "description": "운동 동아리",
        "introduction": "땀흘리기 동아리는",
        "schedule": [
            "5-6 매주 화요일 오후 7시 ~ 9시",
        ],
        "category": "헬스",
        "members": 20,
    },

                    
}

@app.route('/uploads/<filename>')
def uploaded_file(filename):
    return send_from_directory(UPLOAD_FOLDER, filename)

@app.route('/', methods=['GET', 'POST'])
def index():
    categories = sorted(set(c['category'] for c in clubs_info.values()))
    selected_category = request.args.get('category', '전체')

    if selected_category == '전체':
        filtered_clubs = clubs_info
    else:
        filtered_clubs = {k:v for k,v in clubs_info.items() if v['category'] == selected_category}

    if request.method == 'POST':
        file = request.files.get('poster')
        if not file or file.filename == '':
            return "No file uploaded", 400

        filename = file.filename
        filepath = os.path.join(UPLOAD_FOLDER, filename)
        file.save(filepath)

        ocr_text = pytesseract.image_to_string(Image.open(filepath), lang='kor+eng')

        if len(ocr_text.strip()) < 10:
            os.remove(filepath)
            return "포스터 이미지에서 텍스트를 인식하지 못했습니다.", 400

        textfile_path = os.path.join(UPLOAD_FOLDER, 'ocr_input.txt')
        with open(textfile_path, 'w', encoding='cp949') as f:
            f.write(ocr_text)

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

        os.remove(textfile_path)

        recommended_clubs = []
        for line in output.strip().split('\n'):
            if ':' in line:
                name, _ = line.split(':', 1)
                name = name.strip()
                if name in clubs_info:
                    icon_map = {
                        "COSMIC": "laptop-code",
                        "CaTs": "gamepad",
                        "CERT": "shield-alt"
                    }
                    recommended_clubs.append({
                        "name": name,
                        "description": clubs_info[name]["description"],
                        "icon": icon_map.get(name, "star"),
                        "tags": clubs_info[name].get("tags", [])
                    })

        return render_template('result.html',
                               ocr_text=ocr_text,
                               recommended_clubs=recommended_clubs,
                               uploaded_filename=filename)

    return render_template('index.html',
                           clubs=filtered_clubs,
                           categories=categories,
                           selected_category=selected_category)

@app.route('/club/<club_name>')
def club_detail(club_name):
    club = clubs_info.get(club_name)
    if not club:
        return render_template('club_detail.html',
                               club_name=club_name,
                               description="정보가 없습니다.",
                               introduction="소개 정보가 없습니다.",
                               schedule=[],
                               members=0,
                               category="",
                               datetime=datetime)

    return render_template('club_detail.html',
                           club_name=club_name,
                           description=club["description"],
                           introduction=club["introduction"],
                           schedule=club.get("schedule", []),
                           members=club.get("members", 0),
                           category=club.get("category", ""),
                           datetime=datetime)

if __name__ == '__main__':
    app.run(debug=True)

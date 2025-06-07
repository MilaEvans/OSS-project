import os
import subprocess
from flask import Flask, request, render_template, send_from_directory
from PIL import Image
import pytesseract
from markupsafe import Markup

app = Flask(__name__)

UPLOAD_FOLDER = 'uploads'
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

@app.template_filter('nl2br')
def nl2br_filter(s):
    if s is None:
        return ''
    return Markup(s.replace('\n', '<br>\n'))

# 새로 추가: 태그별 CSS 클래스 필터
@app.template_filter('tag_class')
def tag_class_filter(tag):
    mapping = {
        "저녁": "tag-evening",
        "회비 없음": "tag-free",
        "수요일": "tag-wednesday",
        "온라인": "tag-online",
        "오후": "tag-afternoon",
        "일요일": "tag-sunday",
        "토요일": "tag-saturday",
        "회비 있음": "tag-paid",
        "금요일": "tag-friday",
        "오프라인": "tag-offline",
    }
    return mapping.get(tag, "tag-default")

# tesseract 경로 설정 (Windows 예시)
pytesseract.pytesseract.tesseract_cmd = r'C:\Program Files\Tesseract-OCR\tesseract.exe'

clubs_info = {
"코딩하는친구들": {
    "description": "웹 개발 동아리",
    "introduction": "웹 개발에 관심 있는 학생들이 모여 다양한 프로젝트를 함께 진행하는 코딩하는친구들입니다.",
    "schedule": ["매주 수요일 저녁 6시부터 8시까지 온라인으로 진행됩니다."],
    "category": "웹",
    "members": 30,
    "tags": ["저녁", "회비 없음", "수요일", "온라인"]
},
"밤샘코딩": {
    "description": "게임 개발 동아리",
    "introduction": "게임 개발에 열정을 가진 멤버들이 모여 매주 수요일 오후에 온라인으로 함께 코딩하는 밤샘코딩입니다.",
    "schedule": ["매주 수요일 오후 7시부터 9시까지 온라인에서 진행됩니다."],
    "category": "게임 개발",
    "members": 20,
    "tags": ["오후", "회비 없음", "수요일", "온라인"]
},
"코드수다방": {
    "description": "C++ 프로그래밍 동아리",
    "introduction": "C++ 언어와 보안 관련 주제를 다루며 매주 일요일 오전에 온라인으로 모이는 코드수다방입니다.",
    "schedule": ["매주 일요일 오전 9시부터 11시까지 온라인에서 진행됩니다."],
    "category": "C++",
    "members": 15,
    "tags": ["오전", "회비 없음", "일요일", "온라인"]
},
"오픈소스동네": {
    "description": "오픈소스 기여 동아리",
    "introduction": "오픈소스 프로젝트에 기여하며 함께 성장하는 오픈소스동네입니다.",
    "schedule": ["매주 토요일 오전 9시부터 11시까지 온라인에서 활동합니다."],
    "category": "오픈소스",
    "members": 15,
    "tags": ["오전", "회비 있음", "토요일", "온라인"]
},
"디버깅모임": {
    "description": "오프라인 디버깅 동아리",
    "introduction": "프로그램의 오류를 함께 찾아내고 해결하는 오프라인 디버깅모임입니다.",
    "schedule": ["매주 금요일 오후 5시부터 7시까지 오프라인에서 진행됩니다."],
    "category": "디버깅",
    "members": 15,
    "tags": ["오후", "회비 있음", "금요일", "오프라인"]
},



        # 운동
    
"땀흘리기": {
    "description": "토요일 오프라인 헬스 운동 동아리",
    "introduction": "땀흘리기 동아리는 매주 토요일 저녁에 오프라인에서 함께 운동하며 건강을 증진하는 헬스 동아리입니다.",
    "schedule": [
        "매주 토요일 저녁 7시 ~ 9시",
    ],
    "tags": ["저녁", "회비 없음", "토요일", "오프라인"],
    "category": "헬스",
    "members": 20,
},
"운동하자": {
    "description": "화요일 저녁 온라인 탁구 동아리",
    "introduction": "운동하자는 화요일 저녁에 온라인으로 모여 탁구 관련 훈련과 대회를 즐기는 동아리입니다.",
    "schedule": [
        "매주 화요일 저녁 7시 ~ 9시",
    ],
    "tags": ["저녁", "회비 있음", "화요일", "오프라인"],
    "category": "탁구",
    "members": 20,
},
"뛰뛰빵빵": {
    "description": "화요일 오전 온라인 탁구 동아리",
    "introduction": "뛰뛰빵빵은 화요일 오전에 온라인으로 모여 탁구 실력 향상을 위한 연습과 토론을 진행하는 동아리입니다.",
    "schedule": [
        "매주 화요일 오전 7시 ~ 9시",
    ],
    "tags": ["오전", "회비 없음", "화요일", "오프라인"],
    "category": "탁구",
    "members": 20,
},
"런닝메이트": {
    "description": "월요일 저녁 온라인 러닝 동아리",
    "introduction": "런닝메이트는 월요일 저녁에 온라인으로 함께 러닝 계획을 세우고 운동을 진행하는 러닝 동아리입니다.",
    "schedule": [
        "매주 월요일 저녁 7시 ~ 9시",
    ],
    "tags": ["저녁", "회비 있음", "월요일", "오프라인"],
    "category": "러닝",
    "members": 20,
},
"주말농구": {
    "description": "수요일 저녁 오프라인 농구 동아리",
    "introduction": "주말농구는 수요일 저녁에 오프라인으로 모여 농구 경기를 즐기고 실력을 키우는 농구 동아리입니다.",
    "schedule": [
        "매주 수요일 저녁 7시 ~ 9시",
    ],
    "tags": ["저녁", "회비 있음", "수요일", "오프라인"],
    "category": "농구",
    "members": 20,
},


    # 댄스
    
 "춤추는우리": {
    "description": "K-POP 댄스 동아리",
    "introduction": "춤추는우리 동아리는 K-POP 댄스를 사랑하는 사람들이 모여 다양한 아이돌 댄스와 퍼포먼스를 연습하는 동아리입니다.",
    "schedule": [
        "5-6 매주 금요일 오전 10시 ~ 12시",
    ],
    "tags": ["오전", "회비 있음", "금요일", "온라인"],
    "category": "K-POP",
    "members": 20,
},
"리듬따라": {
    "description": "힙합 댄스 동아리",
    "introduction": "리듬따라 동아리는 힙합과 스트리트 댄스를 중심으로 자유로운 표현과 팀워크를 즐기는 모임입니다.",
    "schedule": [
        "5-6 매주 토요일 오전 10시 ~ 12시",
    ],
    "tags": ["오전", "회비 있음", "토요일", "온라인"],
    "category": "힙합",
    "members": 20,
},
"뽐내기댄스": {
    "description": "스트릿 댄스 동아리",
    "introduction": "뽐내기댄스 동아리는 스트릿 댄스와 다양한 퍼포먼스를 통해 자신만의 개성을 표현하는 동아리입니다.",
    "schedule": [
        "5-6 매주 금요일 저녁 7시 ~ 9시",
    ],
    "tags": ["저녁", "회비 있음", "금요일", "온라인"],
    "category": "스트릿",
    "members": 20,
},
"비트속으로": {
    "description": "락킹 댄스 동아리",
    "introduction": "비트속으로 동아리는 락킹 댄스를 즐기며 리듬과 몸의 움직임을 조화롭게 맞추는 동아리입니다.",
    "schedule": [
        "5-6 매주 일요일 오전 10시 ~ 12시",
    ],
    "tags": ["오전", "회비 있음", "일요일", "오프라인"],
    "category": "락킹",
    "members": 20,
},
"즐거운댄스": {
    "description": "탱고 댄스 동아리",
    "introduction": "즐거운댄스 동아리는 아르헨티나 탱고를 중심으로 우아하고 열정적인 춤을 배우는 동아리입니다.",
    "schedule": [
        "5-6 매주 수요일 저녁 7시 ~ 9시",
    ],
    "tags": ["저녁", "회비 없음", "수요일", "오프라인"],
    "category": "탱고",
    "members": 20,
},


    # 게임

    "게임하러모여": {
  "description": "게임 동아리",
  "introduction": "함께 모여 다양한 게임을 즐기는 동아리입니다.",
  "schedule": ["5-6 매주 월요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 있음", "월요일", "오프라인"],
  "category": "게임",
  "members": 20
},

"보드게임존": {
  "description": "게임 동아리",
  "introduction": "보드게임을 중심으로 한 커뮤니티 모임입니다.",
  "schedule": ["5-6 매주 수요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 없음", "수요일", "오프라인"],
  "category": "게임",
  "members": 18
},

"게임친구들": {
  "description": "게임 동아리",
  "introduction": "게임을 좋아하는 친구들이 모여 자유롭게 즐깁니다.",
  "schedule": ["5-6 매주 화요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 있음", "화요일", "오프라인"],
  "category": "게임",
  "members": 22
},

"온라인모험단": {
  "description": "게임 동아리",
  "introduction": "온라인 게임 중심의 모험가들을 위한 동아리입니다.",
  "schedule": ["5-6 매주 토요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 있음", "토요일", "오프라인"],
  "category": "게임",
  "members": 25
},

"게임톡": {
  "description": "게임 동아리",
  "introduction": "게임 이야기를 함께 나누고 소통하는 동아리입니다.",
  "schedule": ["5-6 매주 화요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 있음", "화요일", "오프라인"],
  "category": "게임",
  "members": 19
},
    # 여행

"여행가자": {
  "description": "여행 동아리",
  "introduction": "여행을 좋아하는 사람들의 아침 모임입니다.",
  "schedule": ["5-6 매주 금요일 오전 9시 ~ 11시"],
  "tags": ["오전", "회비 없음", "금요일", "온라인"],
  "category": "여행",
  "members": 16
},

"떠나볼까": {
  "description": "여행 동아리",
  "introduction": "도심 속을 떠나 여유를 찾는 오프라인 여행 동아리.",
  "schedule": ["5-6 매주 월요일 오후 2시 ~ 4시"],
  "tags": ["오후", "회비 없음", "월요일", "오프라인"],
  "category": "여행",
  "members": 21
},

"길따라걷기": {
  "description": "여행 동아리",
  "introduction": "산책과 트레킹 중심의 소규모 여행 모임입니다.",
  "schedule": ["5-6 매주 월요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 있음", "월요일", "온라인"],
  "category": "여행",
  "members": 14
},

"캠핑가족": {
  "description": "여행 동아리",
  "introduction": "캠핑을 사랑하는 사람들이 모이는 동아리입니다.",
  "schedule": ["5-6 매주 화요일 오전 9시 ~ 11시"],
  "tags": ["오전", "회비 있음", "화요일", "오프라인"],
  "category": "여행",
  "members": 17
},

"바람따라": {
  "description": "여행 동아리",
  "introduction": "자유로운 여행을 즐기는 모임입니다.",
  "schedule": ["5-6 매주 화요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 없음", "화요일", "오프라인"],
  "category": "여행",
  "members": 23
},

    # 음악

"음악좋아": {
  "description": "음악 동아리",
  "introduction": "음악을 좋아하는 사람들의 모임입니다.",
  "schedule": ["5-6 매주 화요일 오후 3시 ~ 5시"],
  "tags": ["오후", "회비 없음", "화요일", "온라인"],
  "category": "음악",
  "members": 20
},

"노래방친구들": {
  "description": "음악 동아리",
  "introduction": "노래방에서 노래 부르는 걸 좋아하는 사람들 모임.",
  "schedule": ["5-6 매주 화요일 오전 10시 ~ 12시"],
  "tags": ["오전", "회비 있음", "화요일", "오프라인"],
  "category": "음악",
  "members": 18
},

"멜로디모임": {
  "description": "음악 동아리",
  "introduction": "함께 멜로디를 만들어보는 창작 음악 동아리입니다.",
  "schedule": ["5-6 매주 토요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 없음", "토요일", "오프라인"],
  "category": "음악",
  "members": 22
},

"악기연주회": {
  "description": "음악 동아리",
  "introduction": "악기 연주를 함께하는 모임입니다.",
  "schedule": ["5-6 매주 일요일 오후 2시 ~ 4시"],
  "tags": ["오후", "회비 없음", "일요일", "온라인"],
  "category": "음악",
  "members": 15
},

"소리모아": {
  "description": "음악 동아리",
  "introduction": "다양한 장르의 음악을 함께 듣고 나누는 모임입니다.",
  "schedule": ["5-6 매주 월요일 오후 3시 ~ 5시"],
  "tags": ["오후", "회비 없음", "월요일", "온라인"],
  "category": "음악",
  "members": 19
},

    # 자기개발

"함께성장": {
  "description": "자기개발 동아리",
  "introduction": "서로의 성장을 응원하며 함께 배우는 모임입니다.",
  "schedule": ["5-6 매주 월요일 오전 9시 ~ 11시"],
  "tags": ["오전", "회비 없음", "월요일", "온라인"],
  "category": "자기개발",
  "members": 15
},

"작은도전": {
  "description": "자기개발 동아리",
  "introduction": "작은 목표를 함께 도전하고 성취하는 동아리입니다.",
  "schedule": ["5-6 매주 일요일 오후 2시 ~ 4시"],
  "tags": ["오후", "회비 있음", "일요일", "온라인"],
  "category": "자기개발",
  "members": 18
},

"꿈꾸는우리": {
  "description": "자기개발 동아리",
  "introduction": "꿈을 향해 함께 나아가는 아침 모임입니다.",
  "schedule": ["5-6 매주 금요일 오전 9시 ~ 11시"],
  "tags": ["오전", "회비 없음", "금요일", "온라인"],
  "category": "자기개발",
  "members": 12
},

"스킬업": {
  "description": "자기개발 동아리",
  "introduction": "다양한 기술과 지식을 공유하는 온라인 학습 동아리입니다.",
  "schedule": ["5-6 매주 목요일 오전 9시 ~ 11시"],
  "tags": ["오전", "회비 없음", "목요일", "온라인"],
  "category": "자기개발",
  "members": 20
},

"자기관리": {
  "description": "자기개발 동아리",
  "introduction": "습관 형성과 자기관리에 집중하는 모임입니다.",
  "schedule": ["5-6 매주 일요일 오후 3시 ~ 5시"],
  "tags": ["오후", "회비 있음", "일요일", "오프라인"],
  "category": "자기개발",
  "members": 17
},

    # 독서

"책읽는시간": {
  "description": "독서 동아리",
  "introduction": "함께 책을 읽고 생각을 나누는 시간입니다.",
  "schedule": ["5-6 매주 금요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 있음", "금요일", "온라인"],
  "category": "독서",
  "members": 14
},

"책모임": {
  "description": "독서 동아리",
  "introduction": "책을 함께 읽고 토론하는 독서 커뮤니티입니다.",
  "schedule": ["5-6 매주 금요일 오후 3시 ~ 5시"],
  "tags": ["오후", "회비 있음", "금요일", "온라인"],
  "category": "독서",
  "members": 16
},

"한줄토크": {
  "description": "독서 동아리",
  "introduction": "책 속 한 줄로 이야기 나누는 오프라인 모임입니다.",
  "schedule": ["5-6 매주 토요일 오후 2시 ~ 4시"],
  "tags": ["오후", "회비 있음", "토요일", "오프라인"],
  "category": "독서",
  "members": 18
},

"읽고싶은책": {
  "description": "독서 동아리",
  "introduction": "회원들이 추천한 책을 함께 읽고 소통합니다.",
  "schedule": ["5-6 매주 수요일 오후 3시 ~ 5시"],
  "tags": ["오후", "회비 있음", "수요일", "오프라인"],
  "category": "독서",
  "members": 20
},

"독서친구": {
  "description": "독서 동아리",
  "introduction": "책을 좋아하는 사람들의 온라인 친목 모임입니다.",
  "schedule": ["5-6 매주 토요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 없음", "토요일", "온라인"],
  "category": "독서",
  "members": 13
},

    # 봉사

"도움의손길": {
  "description": "봉사 동아리",
  "introduction": "도움이 필요한 이웃을 위해 함께 봉사합니다.",
  "schedule": ["5-6 매주 월요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 없음", "월요일", "온라인"],
  "category": "봉사",
  "members": 19
},

"함께나누기": {
  "description": "봉사 동아리",
  "introduction": "작은 정성을 함께 나누는 따뜻한 모임입니다.",
  "schedule": ["5-6 매주 금요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 없음", "금요일", "온라인"],
  "category": "봉사",
  "members": 15
},

"따뜻한마음": {
  "description": "봉사 동아리",
  "introduction": "마음을 나누는 다양한 봉사활동을 합니다.",
  "schedule": ["5-6 매주 금요일 오후 3시 ~ 5시"],
  "tags": ["오후", "회비 있음", "금요일", "온라인"],
  "category": "봉사",
  "members": 17
},

"봉사모임": {
  "description": "봉사 동아리",
  "introduction": "정기적인 활동으로 지역사회에 기여하는 동아리입니다.",
  "schedule": ["5-6 매주 화요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 있음", "화요일", "온라인"],
  "category": "봉사",
  "members": 22
},

"기쁨나누기": {
  "description": "봉사 동아리",
  "introduction": "소소한 행복을 함께 나누는 오프라인 봉사활동 모임입니다.",
  "schedule": ["5-6 매주 화요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 없음", "화요일", "오프라인"],
  "category": "봉사",
  "members": 20
},

    # 사진

"찰칵찰칵": {
  "description": "사진 동아리",
  "introduction": "토요일 아침, 사진 찍으며 감성을 나눠요.",
  "schedule": ["5-6 매주 토요일 오전 10시 ~ 12시"],
  "tags": ["오전", "회비 없음", "토요일", "온라인"],
  "category": "사진",
  "members": 14
},

"사진찍기좋아": {
  "description": "사진 동아리",
  "introduction": "사진이 좋아 모인 사람들의 오프라인 모임입니다.",
  "schedule": ["5-6 매주 일요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 있음", "일요일", "오프라인"],
  "category": "사진",
  "members": 18
},

"풍경사진": {
  "description": "사진 동아리",
  "introduction": "풍경 촬영과 사진 공유 중심의 온라인 모임입니다.",
  "schedule": ["5-6 매주 화요일 오후 2시 ~ 4시"],
  "tags": ["오후", "회비 있음", "화요일", "온라인"],
  "category": "사진",
  "members": 16
},

"우리사진관": {
  "description": "사진 동아리",
  "introduction": "사진으로 일상을 기록하는 수요일 아침 모임입니다.",
  "schedule": ["5-6 매주 수요일 오전 9시 ~ 11시"],
  "tags": ["오전", "회비 없음", "수요일", "온라인"],
  "category": "사진",
  "members": 13
},

"사진산책": {
  "description": "사진 동아리",
  "introduction": "산책하며 사진 찍고 함께 공유하는 즐거운 시간!",
  "schedule": ["5-6 매주 목요일 오후 3시 ~ 5시"],
  "tags": ["오후", "회비 없음", "목요일", "온라인"],
  "category": "사진",
  "members": 17
},

    # 영상

"영상찍자": {
  "description": "영상 동아리",
  "introduction": "직접 촬영하며 영상 콘텐츠를 만들어보는 모임입니다.",
  "schedule": ["5-6 매주 목요일 오후 2시 ~ 4시"],
  "tags": ["오후", "회비 있음", "목요일", "온라인"],
  "category": "영상",
  "members": 15
},

"영상편집모임": {
  "description": "영상 동아리",
  "introduction": "편집 기술을 함께 배우고 나누는 시간입니다.",
  "schedule": ["5-6 매주 목요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 없음", "목요일", "온라인"],
  "category": "영상",
  "members": 18
},

"필름러버": {
  "description": "영상 동아리",
  "introduction": "필름 감성 가득한 영상 제작을 즐기는 모임입니다.",
  "schedule": ["5-6 매주 목요일 오전 10시 ~ 12시"],
  "tags": ["오전", "회비 있음", "목요일", "온라인"],
  "category": "영상",
  "members": 12
},

"우리영상": {
  "description": "영상 동아리",
  "introduction": "영상으로 일상을 기록하는 수요일 저녁 모임입니다.",
  "schedule": ["5-6 매주 수요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 있음", "수요일", "온라인"],
  "category": "영상",
  "members": 20
},

"유튜브친구들": {
  "description": "영상 동아리",
  "introduction": "유튜브 콘텐츠를 함께 기획하고 제작하는 모임입니다.",
  "schedule": ["5-6 매주 일요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 없음", "일요일", "온라인"],
  "category": "영상",
  "members": 19
},


    # 미술

"그림그리기": {
  "description": "미술 동아리",
  "introduction": "일요일 저녁, 그림으로 마음을 표현해봐요.",
  "schedule": ["5-6 매주 일요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 있음", "일요일", "온라인"],
  "category": "미술",
  "members": 16
},

"낙서모임": {
  "description": "미술 동아리",
  "introduction": "편하게 낙서하며 창의력 키우는 오프라인 모임입니다.",
  "schedule": ["5-6 매주 일요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 없음", "일요일", "오프라인"],
  "category": "미술",
  "members": 14
},

"아트타임": {
  "description": "미술 동아리",
  "introduction": "일요일 아침에 그림과 함께 여유를 즐겨요.",
  "schedule": ["5-6 매주 일요일 오전 10시 ~ 12시"],
  "tags": ["오전", "회비 없음", "일요일", "오프라인"],
  "category": "미술",
  "members": 13
},

"색칠놀이": {
  "description": "미술 동아리",
  "introduction": "색칠과 채색을 중심으로 그림 그리는 즐거움을 느껴요.",
  "schedule": ["5-6 매주 수요일 저녁 7시 ~ 9시"],
  "tags": ["저녁", "회비 있음", "수요일", "온라인"],
  "category": "미술",
  "members": 15
},

"손그림모임": {
  "description": "미술 동아리",
  "introduction": "디지털이 아닌 손그림에 집중하는 미술 모임입니다.",
  "schedule": ["5-6 매주 목요일 오전 9시 ~ 11시"],
  "tags": ["오전", "회비 없음", "목요일", "온라인"],
  "category": "미술",
  "members": 12
},


    # 연극

"연극놀이": {
  "description": "연극 동아리",
  "introduction": "일요일 저녁, 온라인으로 연극을 즐기는 모임입니다.",
  "schedule": ["5-6 매주 일요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 있음", "일요일", "온라인"],
  "category": "연극",
  "members": 15
},

"무대연습": {
  "description": "연극 동아리",
  "introduction": "수요일 오전, 무대 연습 중심의 온라인 모임입니다.",
  "schedule": ["5-6 매주 수요일 오전 10시 ~ 12시"],
  "tags": ["오전", "회비 있음", "수요일", "온라인"],
  "category": "연극",
  "members": 12
},

"연기하는우리": {
  "description": "연극 동아리",
  "introduction": "월요일 오후, 연기 실력을 함께 키우는 모임입니다.",
  "schedule": ["5-6 매주 월요일 오후 2시 ~ 4시"],
  "tags": ["오후", "회비 있음", "월요일", "온라인"],
  "category": "연극",
  "members": 18
},

"즉흥연기": {
  "description": "연극 동아리",
  "introduction": "월요일 저녁, 오프라인에서 즉흥 연기를 즐기는 모임입니다.",
  "schedule": ["5-6 매주 월요일 오후 7시 ~ 9시"],
  "tags": ["저녁", "회비 있음", "월요일", "오프라인"],
  "category": "연극",
  "members": 16
},

"연극모임": {
  "description": "연극 동아리",
  "introduction": "일요일 오후, 오프라인으로 연극을 만드는 모임입니다.",
  "schedule": ["5-6 매주 일요일 오후 2시 ~ 4시"],
  "tags": ["오후", "회비 있음", "일요일", "오프라인"],
  "category": "연극",
  "members": 14
},

    # 요리

"요리해요": {
  "description": "요리 동아리",
  "introduction": "수요일 오후, 온라인으로 요리법을 공유하는 모임입니다.",
  "schedule": ["5-6 매주 수요일 오후 2시 ~ 4시"],
  "tags": ["오후", "회비 있음", "수요일", "온라인"],
  "category": "요리",
  "members": 20
},

"간식만들기": {
  "description": "요리 동아리",
  "introduction": "월요일 오전, 오프라인으로 간식을 만드는 모임입니다.",
  "schedule": ["5-6 매주 월요일 오전 10시 ~ 12시"],
  "tags": ["오전", "회비 있음", "월요일", "오프라인"],
  "category": "요리",
  "members": 18
},

"베이킹클럽": {
  "description": "요리 동아리",
  "introduction": "화요일 오후, 오프라인에서 베이킹을 배우고 나누는 모임입니다.",
  "schedule": ["5-6 매주 화요일 오후 2시 ~ 4시"],
  "tags": ["오후", "회비 있음", "화요일", "오프라인"],
  "category": "요리",
  "members": 17
},

"맛있는시간": {
  "description": "요리 동아리",
  "introduction": "토요일 오전, 온라인에서 요리법을 나누는 즐거운 모임입니다.",
  "schedule": ["5-6 매주 토요일 오전 10시 ~ 12시"],
  "tags": ["오전", "회비 있음", "토요일", "온라인"],
  "category": "요리",
  "members": 16
},

"요리친구들": {
  "description": "요리 동아리",
  "introduction": "목요일 오전, 오프라인에서 함께 요리하는 모임입니다.",
  "schedule": ["5-6 매주 목요일 오전 10시 ~ 12시"],
  "tags": ["오전", "회비 있음", "목요일", "오프라인"],
  "category": "요리",
  "members": 15
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
        filtered_clubs = {k: v for k, v in clubs_info.items() if v['category'] == selected_category}

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
            output, _ = proc.communicate()
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
                    recommended_clubs.append({
                        "name": name,
                        "description": clubs_info[name]["description"],
                        "icon": "star",
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
                               category="")

    return render_template('club_detail.html',
                           club_name=club_name,
                           description=club["description"],
                           introduction=club["introduction"],
                           schedule=club.get("schedule", []),
                           members=club.get("members", 0),
                           category=club.get("category", ""))

@app.route('/apply/<club_name>', methods=['GET', 'POST'])
def apply_club(club_name):
    if request.method == 'POST':
        name = request.form.get('name')
        email = request.form.get('email')
        motivation = request.form.get('motivation')

        # 신청 처리 로직 (DB나 이메일 연동 가능)
        print(f"[신청] 이름: {name}, 이메일: {email}, 동기: {motivation}, 동아리: {club_name}")

        return render_template('apply_success.html', club_name=club_name, name=name)

    return render_template('apply_form.html', club_name=club_name)

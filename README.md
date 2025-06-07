#충북대학교 동아리 추천

충북대학교 오픈소스기초프로젝트 LOC팀에서 진행하는 프로젝트: 충북대학교 동아리 추천 WEB을 제작하였습니다.

@문제점
현재 충북대학교 동아리는 매우 많지만, 그에 대한 자세한 정보를 찾기는 쉽지않다. 

@구현된 기능
- MBIT와 성향에 따른 동아리 추천을 해준다.
- 이미지를 올리면 분석을 통해 그에 맞는 동아리를 추천해준다.
- 추천 받은 동아리에 신청을 할 수 있다. <<= 기능미구현

@설치방법 

pip install flask

pip install pillow

pip install tesseract

pip install python

pip install g++

pip install pytesseract

@실행 방법
** 윈도우 10인 경우, app.py 실행 전에 필요
- $rm ./mbti_project/MixBot
- $g++ -std=c++17 ./mbti_project/MixBot.cpp -o ./mbti_project/MixBot
1. app.py 파일 실행
2. Flask를 통해 만들어진 주소 접속
3. 사이트 실행
![image](https://github.com/user-attachments/assets/79a89b8e-a43d-4a82-8816-6f6a48be4e28)
사진 상단의 기능 일부 미구현


MIT License

Copyright (c) Club Recommending Chat Bot

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 


@개발자 항목
MilaEvans(팀장, 한민서)
useropea(이정준) dlwjdwns1220@naver.com
HappyThings2(김도성) doseong0213@naver.com

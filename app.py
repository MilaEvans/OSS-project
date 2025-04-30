from flask import Flask, request, render_template
import subprocess

app = Flask(__name__)

def get_response_from_cpp(keyword): #사용자가 입력한 키워드(운동, 프로그래밍 등)
    try: #에러 발생해도, 프로그램 실행하도록 예외 처리
        process = subprocess.Popen(
            ["./().exe"],  # c++로 만든 .exe 파일 실행, () 부분은 .exe 파일명
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

        output, error = process.communicate(input=keyword)
        return output.strip() if output else "추천 결과를 가져오지 못했습니다." # 결과 있다면, 공백 제거 후 반환. 없으면 else
    except Exception as e: 
        return f"오류 발생: {str(e)}" # 실행 중 어떤 오류가 나도 서버 멈추지 않고, 텍스트로 오류 메시지 보여줌

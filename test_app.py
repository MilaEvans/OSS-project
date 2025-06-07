import unittest
from app import app
from flask import session
from unittest.mock import patch

class AppFullTest(unittest.TestCase):
    def setUp(self):
        self.client = app.test_client()
        self.client.testing = True

    def test_index_and_clear(self):
        self.client.get('/')
        response = self.client.post('/clear')
        self.assertEqual(response.status_code, 200)
        self.assertIn("MBTI만 입력해주세요".encode(), response.data)

    def test_chat_invalid_mbti(self):
        self.client.get('/')
        response = self.client.post('/chat', data={'keyword': '나는동아리추천'})
        self.assertIn("MBTI만 입력해주세요".encode(), response.data)

    @patch("app.subprocess.run")
    def test_chat_popular(self, mock_run):
        mock_run.return_value.returncode = 0
        mock_run.return_value.stdout = "- 인기 동아리1\n- 인기 동아리2"

        self.client.get('/')
        response = self.client.post('/chat', data={'keyword': '인기'})
        self.assertIn("인기 동아리".encode(), response.data)

    @patch("app.subprocess.run")
    def test_chat_mbti_filter_end(self, mock_run):
        mock_run.return_value.returncode = 0
        mock_run.return_value.stdout = "- 동아리1\n- 동아리2"

        self.client.get('/')
        self.client.post('/chat', data={'keyword': 'INFP'})
        self.client.post('/chat', data={'keyword': '오전'})
        response = self.client.post('/chat', data={'keyword': '끝'})
        self.assertIn("동아리".encode(), response.data)

    @patch("app.subprocess.run")
    def test_chat_synonym_filter(self, mock_run):
        mock_run.return_value.returncode = 0
        mock_run.return_value.stdout = "- 음악 동아리"

        self.client.get('/')
        self.client.post('/chat', data={'keyword': 'INFP'})
        response = self.client.post('/chat', data={'keyword': '밴드'})
        self.assertIn("동아리".encode(), response.data)

    @patch("app.subprocess.run")
    def test_chat_cpp_error(self, mock_run):
        mock_run.return_value.returncode = 1
        mock_run.return_value.stderr = "C++ 오류 발생"

        self.client.get('/')
        self.client.post('/chat', data={'keyword': 'INFP'})
        response = self.client.post('/chat', data={'keyword': '오전'})
        self.assertIn("오류".encode(), response.data)

    @patch("app.subprocess.run")
    def test_chat_no_club_found(self, mock_run):
        mock_run.return_value.returncode = 0
        mock_run.return_value.stdout = "해당되는 동아리가 없습니다"

        self.client.get('/')
        self.client.post('/chat', data={'keyword': 'INFP'})
        response = self.client.post('/chat', data={'keyword': '토요일'})
        self.assertIn("해당되는 동아리가 없습니다".encode(), response.data)

    @patch("app.subprocess.run", side_effect=Exception("강제 에러"))
    def test_chat_exception_handling(self, _):
        self.client.get('/')
        self.client.post('/chat', data={'keyword': 'INFP'})
        response = self.client.post('/chat', data={'keyword': '오전'})
        self.assertIn("오류가 발생했어요".encode(), response.data)

    def test_other_routes(self):
        self.assertEqual(self.client.get("/result").status_code, 200)
        self.assertEqual(self.client.get("/club/1").status_code, 200)
        self.assertEqual(self.client.get("/apply").status_code, 200)
        self.assertEqual(self.client.post("/apply").status_code, 302)
        self.assertEqual(self.client.get("/apply/success").status_code, 200)

    # ✅ 수정된: /chat GET 핸들러만 테스트 (문구 없음)
    def test_chat_get_session_empty(self):
        response = self.client.get('/chat')
        self.assertEqual(response.status_code, 200)

    @patch("app.subprocess.run")
    def test_chat_single_club(self, mock_run):
        mock_run.return_value.returncode = 0
        mock_run.return_value.stdout = "- 유일 동아리"

        self.client.get('/')
        self.client.post('/chat', data={'keyword': 'INFP'})
        response = self.client.post('/chat', data={'keyword': '오전'})
        self.assertIn("하나 남았습니다".encode(), response.data)

    @patch("app.subprocess.run")
    def test_chat_filter_error_handling(self, mock_run):
        mock_run.return_value.returncode = 1
        mock_run.return_value.stderr = "실패"

        self.client.get('/')
        self.client.post('/chat', data={'keyword': 'INFP'})
        response = self.client.post('/chat', data={'keyword': '오후'})
        self.assertIn("오류".encode(), response.data)

    @patch("app.subprocess.run")
    def test_chat_invalid_format_output(self, mock_run):
        mock_run.return_value.returncode = 0
        mock_run.return_value.stdout = "클럽 리스트인데 - 기호 없음\n정상 파싱 불가"

        self.client.get('/')
        self.client.post('/chat', data={'keyword': 'INFP'})
        response = self.client.post('/chat', data={'keyword': '말도안됨'})
        self.assertIn("MBTI를 다시 입력해주세요".encode(), response.data)


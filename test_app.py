import unittest
import image
from app import app
import io
from PIL import Image
import base64
import tempfile

class TestApp(unittest.TestCase):
    def setUp(self):
        self.app = app.test_client()
        self.app.testing = True

    def test_index_route(self):
        response = self.app.get('/')
        self.assertEqual(response.status_code, 200)
        self.assertIn("동아리 추천 챗봇", response.data.decode("utf-8"))

    def test_club_detail_route(self):
        response = self.app.get('/club/코딩하는친구들')
        self.assertEqual(response.status_code, 200)
        self.assertIn("코딩하는친구들", response.data.decode("utf-8"))

class TestAppRoutes(unittest.TestCase):
    def setUp(self):
        self.app = app.test_client()
        self.app.testing = True

    def test_get_index(self):
        response = self.app.get('/index')
        self.assertEqual(response.status_code, 200)

    def test_post_index_no_file(self):
        response = self.app.post('/index', data={})
        self.assertIn(response.status_code, [200, 302])

    def test_chat_post_empty(self):
        response = self.app.post('/chat', json={"message": ""})
        self.assertEqual(response.status_code, 200)
        if response.is_json:
            data = response.get_json()
            self.assertIn("response", data)

    def test_chat_post_keyword(self):
        response = self.app.post('/chat', json={"message": "음악"})
        self.assertEqual(response.status_code, 200)
        if response.is_json:
            data = response.get_json()
            self.assertIn("response", data)

    def test_chat_post_mbti(self):
        response = self.app.post('/chat', json={"message": "infp"})
        self.assertEqual(response.status_code, 200)
        if response.is_json:
            data = response.get_json()
            self.assertIn("response", data)

    def test_chat_post_image_base64(self):
        img = Image.new('RGB', (10, 10), color='white')
        buffer = io.BytesIO()
        img.save(buffer, format='PNG')
        img_bytes = buffer.getvalue()
        img_base64 = base64.b64encode(img_bytes).decode('utf-8')

        response = self.app.post('/chat', json={"image": img_base64})
        self.assertEqual(response.status_code, 200)
        if response.is_json:
            data = response.get_json()
            self.assertIn("response", data)

class TestImageModule(unittest.TestCase):
    def test_clubs_info_exists(self):
        self.assertIsInstance(image.clubs_info, dict)
        self.assertIn("코딩하는친구들", image.clubs_info)

    def test_club_data_fields(self):
        club = image.clubs_info.get("코딩하는친구들", {})
        self.assertIn("description", club)
        self.assertIn("tags", club)
        self.assertIn("schedule", club)

    def test_tag_class_filter(self):
        self.assertEqual(image.tag_class_filter("오프라인"), "tag-offline")
        self.assertEqual(image.tag_class_filter("없는태그"), "tag-default")

    def test_nl2br_filter(self):
        input_text = "첫 줄\n두 번째 줄"
        expected = "첫 줄<br>\n두 번째 줄"
        self.assertEqual(str(image.nl2br_filter(input_text)), expected)

class TestAdditionalAppRoutes(unittest.TestCase):
    def setUp(self):
        self.app = app.test_client()
        self.app.testing = True

    def test_result_without_file(self):
        response = self.app.get('/result')
        self.assertIn(response.status_code, [200, 302])

    def test_apply_post(self):
        response = self.app.post('/apply', data={
            'name': '테스터',
            'email': 'tester@example.com',
            'motivation': '열정있어요',
            'club': '코딩하는친구들'
        })
        self.assertIn(response.status_code, [200, 302])
        if response.status_code == 200:
            self.assertIn("신청이 완료되었습니다", response.data.decode("utf-8"))

if __name__ == '__main__':
    unittest.main()

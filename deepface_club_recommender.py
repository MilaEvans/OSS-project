from deepface import DeepFace
import os

# 1. 감정 → 성향 태그 매핑
def map_emotion_to_traits(dominant_emotion):
    emotion_to_traits = {
        "happy": "사교적 / 긍정적",
        "neutral": "차분함 / 안정적",
        "sad": "감성적 / 내향적",
        "angry": "주도적 / 적극적",
        "surprise": "모험적 / 창의적",
        "fear": "신중함 / 계획적",
        "disgust": "비판적 / 독립적"
    }
    return emotion_to_traits.get(dominant_emotion, "일반적")

# 2. 성향 → 동아리 추천
def recommend_club(trait):
    trait_to_club = {
        "사교적 / 긍정적": ["홍보 동아리", "봉사 동아리", "MC/방송 동아리"],
        "차분함 / 안정적": ["서적/독서 동아리", "보드게임 동아리"],
        "감성적 / 내향적": ["문학 동아리", "미술 동아리"],
        "주도적 / 적극적": ["창업 동아리", "학술 동아리", "기획 동아리"],
        "모험적 / 창의적": ["댄스 동아리", "영상/사진 동아리", "연극 동아리"],
        "신중함 / 계획적": ["기록 동아리", "연구회", "프로그래밍 동아리"],
        "비판적 / 독립적": ["토론 동아리", "철학 동아리", "정치/사회 동아리"],
        "일반적": ["학생회", "편집 동아리"]
    }
    return trait_to_club.get(trait, ["동아리 없음"])

# 3. 사진 분석 및 추천 수행
def analyze_photo_and_recommend(image_path):
    try:
        result = DeepFace.analyze(img_path=image_path, actions=['emotion'], enforce_detection=False)
        dominant_emotion = result[0]["dominant_emotion"]
        print(f"감정 분석 결과: {dominant_emotion}")

        trait = map_emotion_to_traits(dominant_emotion)
        print(f"성향 추정: {trait}")

        clubs = recommend_club(trait)
        print(f"추천 동아리: {', '.join(clubs)}")

    except Exception as e:
        print(f"오류 발생: {str(e)}")

# 4. 실행 예시
if __name__ == "__main__":
    image_path = "your_image.jpg"  # 사용자 사진 경로
    if os.path.exists(image_path):
        analyze_photo_and_recommend(image_path)
    else:
        print("이미지 파일을 찾을 수 없습니다.")

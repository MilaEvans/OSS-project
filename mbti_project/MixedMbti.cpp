#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <algorithm>

// ─── (1) MBTI 분석을 위한 클래스·데이터 ───────────────────────────────
class MBTIInfo {
public:
    std::string personality;
    std::vector<std::string> clubTypes;  // 실제 동아리 이름
    std::vector<std::string> hobbies;

    MBTIInfo(const std::string& p,
             const std::vector<std::string>& c,
             const std::vector<std::string>& h)
        : personality(p), clubTypes(c), hobbies(h) {}

    void printInfo(const std::string& mbti) const {
        std::cout << "[MBTI 분석 결과: " << mbti << "]\n";
        std::cout << "당신은 " << personality << " 타입이시군요!\n";

        if (!clubTypes.empty()) {
            std::cout << "이런 성향이라면 다음 동아리들을 추천드려요:\n";
            for (const auto& club : clubTypes) {
                std::cout << "- " << club << "\n";
            }
        }

        if (!hobbies.empty()) {
            std::cout << "취미로는 ";
            for (size_t i = 0; i < hobbies.size(); ++i) {
                std::cout << hobbies[i];
                if (i < hobbies.size() - 1) std::cout << ", ";
            }
            std::cout << " 등을 추천드립니다.\n";
        }
    }
};

// 실제 동아리 이름 맵: 카테고리 → 동아리 이름들
static const std::map<std::string, std::vector<std::string>> categoryClubs = {
    { "코딩",     { "코딩하는친구들", "밤샘코딩", "코드수다방", "오픈소스동네", "디버깅모임" } },
    { "운동",     { "땀흘리기", "운동하자", "뛰뛰빵빵", "런닝메이트", "주말농구" } },
    { "댄스",     { "춤추는우리", "리듬따라", "뽐내기댄스", "비트속으로", "즐거운댄스" } },
    { "게임",     { "게임하러모여", "보드게임존", "게임친구들", "온라인모험단", "게임톡" } },
    { "여행",     { "여행가자", "떠나볼까", "길따라걷기", "캠핑가족", "바람따라" } },
    { "음악",     { "음악좋아", "노래방친구들", "멜로디모임", "악기연주회", "소리모아" } },
    { "자기개발", { "함께성장", "작은도전", "꿈꾸는우리", "스킬업", "자기관리" } },
    { "독서",     { "책읽는시간", "책모임", "한줄토크", "읽고싶은책", "독서친구" } },
    { "봉사",     { "도움의손길", "함께나누기", "따뜻한마음", "봉사모임", "기쁨나누기" } },
    { "사진",     { "찰칵찰칱", "사진찍기좋아", "풍경사진", "우리사진관", "사진산책" } },
    { "영상",     { "영상찍자", "영상편집모임", "필름러버", "우리영상", "유튜브친구들" } },
    { "미술",     { "그림그리기", "낙서모임", "아트타임", "색칠놀이", "손그림모임" } },
    { "연극",     { "연극놀이", "무대연습", "연기하는우리", "즉흥연기", "연극모임" } },
    { "요리",     { "요리해요", "간식만들기", "베이킹클럽", "맛있는시간", "요리친구들" } }
};

// ─── (2) MBTI별 성향 및 관련 동아리 매핑 ───────────────────────────────
// MBTI마다 위 categoryClubs에서 몇 개를 골라 실제 연관된 동아리 이름을 배정
static const std::map<std::string, std::tuple<
    std::string,                   // 성향 설명
    std::vector<std::string>,      // 추천 동아리(실제 이름들)
    std::vector<std::string>       // 추천 취미
>> mbtiData = {
    { "INTJ", { 
        "논리적이고 전략적인 성향",
        { "코딩하는친구들", "오픈소스동네", "디버깅모임" },  // 코딩 카테고리
        { "독서", "퍼즐" }
    }},
    { "INTP", {
        "탐구적이고 분석적인 성향",
        { "밤샘코딩", "코드수다방", "게임하러모여" },  // 코딩·게임
        { "과학", "퍼즐" }
    }},
    { "ENTJ", {
        "계획적이고 리더십 강한 성향",
        { "함께성장", "꿈꾸는우리", "스킬업" },       // 자기개발
        { "토론", "멘토링" }
    }},
    { "ENTP", {
        "아이디어 넘치는 토론가",
        { "토론놀이", "연극모임", "즉흥연기" },       // 연극·토론
        { "즉흥 연설", "토론" }
    }},
    { "INFJ", {
        "이상주의적 조력자",
        { "따뜻한마음", "봉사모임", "함께나누기" },   // 봉사
        { "글쓰기", "상담" }
    }},
    { "INFP", {
        "감성적이고 가치 지향적",
        { "음악좋아", "소리모아", "사진찍기좋아" },   // 음악·사진
        { "글쓰기", "음악 감상" }
    }},
    { "ENFJ", {
        "지도력 있고 배려심 많은 성향",
        { "봉사모임", "도움의손길", "파티 기획" },  // 봉사·행사
        { "멘토링", "공연 기획" }
    }},
    { "ENFP", {
        "창의적이고 열정적인 성향",
        { "멜로디모임", "악기연주회", "영상찍자" },   // 음악·영상
        { "여행", "사진" }
    }},
    { "ISTJ", {
        "책임감 강하고 실용적인 성향",
        { "책읽는시간", "책모임", "한줄토크" },      // 독서 관련
        { "가계부 정리", "통계" }
    }},
    { "ISFJ", {
        "헌신적이고 온화한 성향",
        { "베이킹클럽", "요리해요", "봉사모임" },     // 요리·봉사
        { "베이킹", "독서" }
    }},
    { "ESTJ", {
        "조직적이고 체계적인 성향",
        { "행정 동아리", "리더십 동아리", "스킬업" }, // 조직/자기개발
        { "플래너 정리", "스케줄 관리" }
    }},
    { "ESFJ", {
        "사교적이고 배려 깊은 성향",
        { "환영회 동아리", "학교 행사 기획", "파티 기획" }, // 행사
        { "파티 기획", "친목 활동" }
    }},
    { "ISTP", {
        "실용적이고 논리적인 성향",
        { "기계 동아리", "DIY 동아리", "런닝메이트" },   // 운동·DIY
        { "조립", "자동차" }
    }},
    { "ISFP", {
        "조용하고 예술적인 성향",
        { "그림그리기", "사진찍기좋아", "음악좋아" }, // 미술·사진·음악
        { "작곡", "그림" }
    }},
    { "ESTP", {
        "모험을 즐기고 활동적인 성향",
        { "주말농구", "런닝메이트", "즐거운댄스" }, // 운동·댄스
        { "스포츠", "댄스" }
    }},
    { "ESFP", {
        "외향적이고 낙천적인 성향",
        { "연극놀이", "공연 동아리", "노래방친구들" }, // 연극·음악
        { "노래", "연기" }
    }}
};

static const std::set<std::string> validMBTIs = {
    "INTJ", "INTP", "ENTJ", "ENTP",
    "INFJ", "INFP", "ENFJ", "ENFP",
    "ISTJ", "ISFJ", "ESTJ", "ESFJ",
    "ISTP", "ISFP", "ESTP", "ESFP"
};

std::string toUpperCase(const std::string& str) {
    std::string upperStr = str;
    std::transform(
        upperStr.begin(), upperStr.end(),
        upperStr.begin(), ::toupper
    );
    return upperStr;
}

// MBTI 분석: rawInput에 포함된 validMBTIs 중 하나가 있으면 처리
int processMbti(const std::string& rawInput) {
    std::string upper = toUpperCase(rawInput);
    for (const auto& mbti : validMBTIs) {
        if (upper.find(mbti) != std::string::npos) {
            auto it = mbtiData.find(mbti);
            if (it != mbtiData.end()) {
                const auto& [personality, clubs, hobbies] = it->second;
                MBTIInfo info(personality, clubs, hobbies);
                info.printInfo(mbti);
                return 0;
            }
        }
    }
    std::cout << "잘못된 MBTI입니다. 예: INFP, INTJ 같은 네 글자를 입력해주세요.\n";
    return 1;
}

// ─── (3) 관심사 키워드 기반 추천 ───────────────────────────────
// 키워드 → 실제 동아리 이름 목록
static const std::map<std::string, std::vector<std::string>> interestKeywords = {
    { "운동",     categoryClubs.at("운동") },
    { "예술",     categoryClubs.at("미술") },
    { "음악",     categoryClubs.at("음악") },
    { "봉사",     categoryClubs.at("봉사") },
    { "토론",     { "토론놀이", "연극모임", "즉흥연기" } },
    { "IT",       categoryClubs.at("코딩") },
    { "창업",     categoryClubs.at("자기개발") },
    { "문학",     categoryClubs.at("독서") },
    { "댄스",     categoryClubs.at("댄스") },
    { "게임",     categoryClubs.at("게임") },
    { "여행",     categoryClubs.at("여행") },
    { "사진",     categoryClubs.at("사진") },
    { "영상",     categoryClubs.at("영상") },
    { "미술",     categoryClubs.at("미술") },
    { "연극",     categoryClubs.at("연극") },
    { "요리",     categoryClubs.at("요리") }
};

// 입력 문자열에 매칭된 모든 카테고리의 동아리를 모아 출력
// 반환값: true(추천 결과 출력됨), false(키워드가 전혀 매칭되지 않음)
bool processInterest(const std::string& rawInput) {
    std::string lowered = rawInput;
    std::set<std::string> matchedClubs;

    for (const auto& pair : interestKeywords) {
        const std::string& category = pair.first;
        if (lowered.find(category) != std::string::npos) {
            for (const auto& club : pair.second) {
                matchedClubs.insert(club);
            }
        }
    }
    if (!matchedClubs.empty()) {
        std::cout << "[관심사 기반 추천]\n";
        for (const auto& club : matchedClubs) {
            std::cout << "- " << club << "\n";
        }
        return true;
    }
    return false;
}

// ─── (4) 최종 처리: MBTI 우선 → 관심사(복수 가능) → 그 외 안내 ─────────────────────
int main(int argc, char* argv[]) {
    std::string userInput;
    if (argc >= 2) {
        userInput = argv[1];
        for (int i = 2; i < argc; ++i) {
            userInput += " ";
            userInput += argv[i];
        }
    }

    // 1) MBTI 검사
    if (processMbti(userInput) == 0) {
        return 0;
    }

    // 2) 관심사 키워드 검사(복수 추천 지원)
    if (processInterest(userInput)) {
        return 0;
    }

    // 3) 그 외
    std::cout << "음... 제가 이해하기 어려운 표현이었어요. 예: INFP, 운동, 음악 같은 키워드를 사용해 주세요!\n";
    return 0;
}

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <algorithm>

// ===== MBTI 클래스 기반 추천 =====
class MBTIInfo {
protected:
    std::string personality;
    std::vector<std::string> clubTypes;
    std::vector<std::string> hobbies;

public:
    virtual ~MBTIInfo() = default;

    void printInfo(const std::string& mbti) const {
        std::cout << "\n[MBTI 분석 결과: " << mbti << "]\n";
        std::cout << "성격 성향: " << personality << "\n";

        std::cout << "추천 동아리: ";
        for (const auto& club : clubTypes) std::cout << club << ", ";
        std::cout << "\n";

        std::cout << "추천 취미 키워드: ";
        for (const auto& hobby : hobbies) std::cout << hobby << ", ";
        std::cout << "\n";
    }
};

// ===== 16가지 MBTI 각각 정의 =====
std::unique_ptr<MBTIInfo> createMBTIInfo(const std::string& mbti) {
    class CustomMBTI : public MBTIInfo {
    public:
        CustomMBTI(const std::string& p, const std::vector<std::string>& c, const std::vector<std::string>& h) {
            personality = p;
            clubTypes = c;
            hobbies = h;
        }
    };

    const std::map<std::string, std::tuple<std::string, std::vector<std::string>, std::vector<std::string>>> mbtiData = {
        { "INTJ", { "논리적이고 전략적인 성향", { "개발 동아리", "창업 동아리" }, { "코딩", "독서" } } },
        { "INTP", { "탐구적이고 분석적인 성향", { "연구 동아리", "프로그래밍 동아리" }, { "퍼즐", "과학" } } },
        { "ENTJ", { "계획적이고 리더십 강한 성향", { "경영 동아리", "창업 동아리" }, { "리더십 게임", "토론" } } },
        { "ENTP", { "아이디어 넘치는 토론가", { "발명 동아리", "토론 동아리" }, { "즉흥 연설", "토론" } } },
        { "INFJ", { "이상주의적 조력자", { "봉사 동아리", "문학 동아리" }, { "글쓰기", "상담" } } },
        { "INFP", { "감성적이고 가치 지향적", { "예술 동아리", "시사 동아리" }, { "글쓰기", "음악 감상" } } },
        { "ENFJ", { "지도력 있고 배려심 많은 성향", { "봉사 동아리", "교육 동아리" }, { "멘토링", "공연 기획" } } },
        { "ENFP", { "창의적이고 열정적인 성향", { "예술 동아리", "연극 동아리" }, { "사진", "여행" } } },
        { "ISTJ", { "책임감 강하고 실용적인 성향", { "기록 동아리", "통계 동아리" }, { "자료 수집", "가계부 정리" } } },
        { "ISFJ", { "헌신적이고 온화한 성향", { "봉사 동아리", "요리 동아리" }, { "베이킹", "독서" } } },
        { "ESTJ", { "조직적이고 체계적인 성향", { "행정 동아리", "리더십 동아리" }, { "플래너 정리", "스케줄 관리" } } },
        { "ESFJ", { "사교적이고 배려 깊은 성향", { "환영회 동아리", "학교 행사 기획" }, { "친목 활동", "파티 기획" } } },
        { "ISTP", { "실용적이고 논리적인 성향", { "기계 동아리", "DIY 동아리" }, { "조립", "자동차" } } },
        { "ISFP", { "조용하고 예술적인 성향", { "사진 동아리", "음악 동아리" }, { "작곡", "그림" } } },
        { "ESTP", { "모험을 즐기고 활동적인 성향", { "스포츠 동아리", "댄스 동아리" }, { "스포츠", "댄스" } } },
        { "ESFP", { "외향적이고 낙천적인 성향", { "공연 동아리", "이벤트 동아리" }, { "노래", "연기" } } }
    };

    auto it = mbtiData.find(mbti);
    if (it != mbtiData.end()) {
        return std::make_unique<CustomMBTI>(
            std::get<0>(it->second),
            std::get<1>(it->second),
            std::get<2>(it->second)
        );
    }

    return nullptr; // 사실 이 라인 도달하지 않음(입력 유효성 검사 때문에)
}

// ===== 유효 MBTI 검사 =====
const std::set<std::string> validMBTIs = {
    "INTJ", "INTP", "ENTJ", "ENTP",
    "INFJ", "INFP", "ENFJ", "ENFP",
    "ISTJ", "ISFJ", "ESTJ", "ESFJ",
    "ISTP", "ISFP", "ESTP", "ESFP"
};

std::string toUpperCase(const std::string& str) {
    std::string upperStr = str;
    std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
    return upperStr;
}

std::string getValidMBTI() {
    std::string input;
    while (true) {
        std::cout << "당신의 MBTI를 입력하세요 (예: INTJ): ";
        std::getline(std::cin, input);
        input = toUpperCase(input);
        if (validMBTIs.count(input)) return input;
        std::cout << "잘못된 MBTI입니다. 다시 입력해주세요.\n";
    }
}

// ===== main 함수 =====
int main() {
    std::cout << "=== 동아리 추천 챗봇 ===\n";
    std::string mbti = getValidMBTI();
    auto info = createMBTIInfo(mbti);
    info->printInfo(mbti);

    return 0;
}

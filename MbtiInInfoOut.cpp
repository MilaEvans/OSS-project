#include <iostream>
#include <string>
#include <vector>
#include <memory> 


class MBTIInfo {
protected:
    std::string personality;
    std::vector<std::string> clubTypes;
    std::vector<std::string> hobbies;

public:
    virtual ~MBTIInfo() = default; 

    virtual void printInfo(const std::string& mbti) const {
        std::cout << "\n[MBTI 분석 결과: " << mbti << "]\n";
        std::cout << "성격 성향: " << personality << "\n";

        std::cout << "추천 동아리 유형: ";
        for (const auto& club : clubTypes) std::cout << club << ", ";
        std::cout << "\n";

        std::cout << "추천 취미 키워드: ";
        for (const auto& hobby : hobbies) std::cout << hobby << ", ";
        std::cout << "\n";
    }
};

class ENFP : public MBTIInfo {
public:
    ENFP() {
        personality = "열정적이고 창의적인 성향";
        clubTypes = { "예술 동아리", "연극 동아리", "홍보/기획 동아리" };
        hobbies = { "사진 촬영", "여행", "즉흥적인 활동" };
    }
};

class INTJ : public MBTIInfo {
public:
    INTJ() {
        personality = "논리적이고 전략적인 성향";
        clubTypes = { "개발 동아리", "스터디 그룹", "창업 동아리" };
        hobbies = { "코딩", "독서", "문제 해결" };
    }
};

int main() {
    std::string mbti;
    std::cout << "MBTI를 입력하세요 (예: ENFP): ";
    std::cin >> mbti;

    for (auto& c : mbti) c = toupper(c);

    std::unique_ptr<MBTIInfo> info;

    if (mbti == "ENFP") {
        info = std::make_unique<ENFP>();
    }
    else if (mbti == "INTJ") {
        info = std::make_unique<INTJ>();
    }
    else {
        std::cout << "아직 지원하지 않는 MBTI입니다.\n";
        return 0;
    }

    info->printInfo(mbti);
    return 0;
}

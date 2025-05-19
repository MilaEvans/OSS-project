#include <iostream>
#include <string>
#include <algorithm>
#include <set>
//실무 코드에서는 using namespace std;를 사용하지 않는 것이 원칙
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

std::string getMBTI() {
    std::string input;

    while (true) {
        std::cout << "당신의 MBTI를 입력하세요 (예: INTJ): ";
        std::cin >> input;

        input = toUpperCase(input);

        if (validMBTIs.count(input)) {
            std::cout << "당신의 MBTI: " << input << std::endl;
            return input;
        }
        else {
            std::cout << "잘못된 MBTI입니다. 다시 입력해주세요.\n";
        }
    }
}

int main() {
    std::string mbti = getMBTI();
    return 0;
}

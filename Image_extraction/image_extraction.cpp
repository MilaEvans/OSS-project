#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

// clean 함수: 공백 제거 + 소문자 변환
std::string clean(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (!isspace(c)) {
            result += std::tolower(c);
        }
    }
    return result;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "No input text file provided.\n";
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream infile(filename);
    if (!infile) {
        std::cout << "Failed to open input file.\n";
        return 1;
    }

    std::string input((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    infile.close();

    input = clean(input);

    // 각 동아리마다 여러 개의 키워드(관련 단어) 등록
    struct Club {
        std::string name;
        std::string description;
        std::vector<std::string> keywords;
    };

    std::vector<Club> clubs = {
        {"코딩", "코딩!!!!", {"코딩", "프로그래밍", "html", "css", "javascript", "python", "웹"}},
        {"Robot Club", "자율주행 로봇 개발", {"로봇", "자율주행", "자동차", "기계"}},
        {"AI Club", "자율주행 인공지능 알고리즘 연구", {"인공지능", "ai", "머신러닝", "딥러닝", "알고리즘"}},
        {"Electric Vehicle Club", "전기차 및 자율주행차 연구", {"전기차", "전기", "배터리", "자율주행"}},
        {"Computer Vision Club", "컴퓨터 비전과 영상 처리 연구", {"컴퓨터비전", "영상처리", "opencv", "이미지", "영상"}}
    };

    std::vector<std::string> matches;

    for (const auto& club : clubs) {
        for (const auto& kw : club.keywords) {
            if (input.find(clean(kw)) != std::string::npos) {
                matches.push_back(club.name + ": " + club.description);
                break;  // 키워드 하나라도 매칭되면 해당 동아리 추가하고 다음 동아리 검사
            }
        }
    }

    if (matches.empty()) {
        std::cout << "No matching clubs found.\n";
    }
    else {
        for (const auto& m : matches) {
            std::cout << m << "\n";
        }
    }

    return 0;
}

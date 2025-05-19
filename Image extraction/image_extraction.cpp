#include <iostream>
#include <fstream> 
#include <string>
#include <vector>
#include <algorithm>

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

    input = clean(input); // 입력 문자열 공백 제거

    std::vector<std::pair<std::string, std::string>> clubs = {
        {"코딩", "코딩!!!!"},
        {"Robot Club", "자율주행 로봇 개발"},
        {"AI Club", "자율주행 인공지능 알고리즘 연구"},
        {"Electric Vehicle Club", "전기차 및 자율주행차 연구"},
        {"Computer Vision Club", "컴퓨터 비전과 영상 처리 연구"}
    };

    std::vector<std::string> matches;
    for (auto& club : clubs) {
        std::string keyword_clean = clean(club.first);
        std::string desc_clean = clean(club.second);

        if (input.find(keyword_clean) != std::string::npos ||
            input.find(desc_clean) != std::string::npos) {
            matches.push_back(club.first + ": " + club.second);
        }
    }

    if (matches.empty()) {
        std::cout << "No matching clubs found.\n";
    }
    else {
        for (auto& m : matches) {
            std::cout << m << "\n";
        }
    }

    return 0;
}


}

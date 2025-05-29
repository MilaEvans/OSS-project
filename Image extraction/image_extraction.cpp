#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

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

    struct Club {
        std::vector<std::string> names;
        std::string description;
        std::vector<std::string> keywords;
    };

    std::vector<Club> clubs = {
        {{"COSMIC", "CaTs", "CERT"}, "컴퓨터공학과 동아리", {"코딩", "프로그래밍", "html", "css", "javascript", "python", "웹"}},
    };

    std::vector<std::string> matches;

    for (const auto& club : clubs) {
        for (const auto& kw : club.keywords) {
            if (input.find(clean(kw)) != std::string::npos) {
                for (const auto& name : club.names) {
                    matches.push_back(name + ": " + club.description); // 모든 이름 출력
                }
                break; // 중복 방지를 위해 다음 클럽으로
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

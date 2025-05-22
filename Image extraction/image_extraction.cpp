#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

// clean �Լ� ����: ���� ���� + �ҹ��� ��ȯ
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

    input = clean(input); // �Է� ���ڿ� ���� ����

    std::vector<std::pair<std::string, std::string>> clubs = {
        {"�ڵ�", "�ڵ�!!!!"},
        {"Robot Club", "�������� �κ� ����"},
        {"AI Club", "�������� �ΰ����� �˰��� ����"},
        {"Electric Vehicle Club", "������ �� ���������� ����"},
        {"Computer Vision Club", "��ǻ�� ������ ���� ó�� ����"}
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



#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

// clean �Լ�: ���� ���� + �ҹ��� ��ȯ
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

    // �� ���Ƹ����� ���� ���� Ű����(���� �ܾ�) ���
    struct Club {
        std::string name;
        std::string description;
        std::vector<std::string> keywords;
    };

    std::vector<Club> clubs = {
        {"�ڵ�", "�ڵ�!!!!", {"�ڵ�", "���α׷���", "html", "css", "javascript", "python", "��"}},
        {"Robot Club", "�������� �κ� ����", {"�κ�", "��������", "�ڵ���", "���"}},
        {"AI Club", "�������� �ΰ����� �˰��� ����", {"�ΰ�����", "ai", "�ӽŷ���", "������", "�˰���"}},
        {"Electric Vehicle Club", "������ �� ���������� ����", {"������", "����", "���͸�", "��������"}},
        {"Computer Vision Club", "��ǻ�� ������ ���� ó�� ����", {"��ǻ�ͺ���", "����ó��", "opencv", "�̹���", "����"}}
    };

    std::vector<std::string> matches;

    for (const auto& club : clubs) {
        for (const auto& kw : club.keywords) {
            if (input.find(clean(kw)) != std::string::npos) {
                matches.push_back(club.name + ": " + club.description);
                break;  // Ű���� �ϳ��� ��Ī�Ǹ� �ش� ���Ƹ� �߰��ϰ� ���� ���Ƹ� �˻�
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

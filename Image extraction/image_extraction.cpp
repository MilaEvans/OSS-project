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
        {{"�ڵ��ϴ�ģ����", "����ڵ�", "�ڵ���ٹ�", "���¼ҽ�����", "��������"}, "�ڵ� ���� ���Ƹ�", {"�ڵ�", "���α׷���", "html", "css", "javascript", "python", "��"}},
        {{"���긮��", "�����", "�ٶٻ���", "���׸���Ʈ", "�ָ���"}, "� ���� ���Ƹ�", {"�"}},
        {{"���ߴ¿츮", "�������", "�˳����", "��Ʈ������", "��ſ��"}, "�� ���� ���Ƹ�", {"��"}},
        {{"�����Ϸ���", "���������", "����ģ����", "�¶��θ����", "������"}, "���� ���� ���Ƹ�", {"����"}},
        {{"���డ��", "��������", "�����ȱ�", "ķ�ΰ���", "�ٶ�����"}, "���� ���� ���Ƹ�", {"����"}},
        {{"��������", "�뷡��ģ����", "��ε����", "�Ǳ⿬��ȸ", "�Ҹ����"}, "���� ���� ���Ƹ�", {"����"}},
        {{"�Բ�����", "��������", "�޲ٴ¿츮", "��ų��", "�ڱ����"}, "�ڱⰳ�� ���� ���Ƹ�", {"�ڱⰳ��"}},
        {{"å�д½ð�", "å����", "������ũ", "�а����å", "����ģ��"}, "���� ���� ���Ƹ�", {"����"}},
        {{"�����Ǽձ�", "�Բ�������", "�����Ѹ���", "�������", "��ݳ�����"}, "���� ���� ���Ƹ�", {"����"}},
        {{"��Ĭ��Ĭ", "�����������", "ǳ�����", "�츮������", "������å"}, "���� ���� ���Ƹ�", {"����"}},
        {{"��������", "������������", "�ʸ�����", "�츮����", "��Ʃ��ģ����"}, "���� ���� ���Ƹ�", {"����"}},
        {{"�׸��׸���", "��������", "��ƮŸ��", "��ĥ����", "�ձ׸�����"}, "�̼� ���� ���Ƹ�", {"�̼�"}},
        {{"���س���", "���뿬��", "�����ϴ¿츮", "���￬��", "���ظ���"}, "���� ���� ���Ƹ�", {"����"}},
        {{"�丮�ؿ�", "���ĸ����", "����ŷŬ��", "���ִ½ð�", "�丮ģ����"}, "�丮 ���� ���Ƹ�", {"�丮"}}
    };

    std::vector<std::string> matches;

    for (const auto& club : clubs) {
        for (const auto& kw : club.keywords) {
            if (input.find(clean(kw)) != std::string::npos) {
                for (const auto& name : club.names) {
                    matches.push_back(name + ": " + club.description); // ��� �̸� ���
                }
                break; // �ߺ� ������ ���� ���� Ŭ������
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

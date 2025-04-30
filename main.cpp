#include <iostream>
#include <string>
#include <map>
#include <vector>

bool isKeywordMatch(const std::string& qq, const std::string& tokenFirst, const std::string& tokenSecond, const std::string& keyword)
{
	if (qq.find(tokenFirst) != std::string::npos)
	{
		return tokenSecond == keyword;
	}
	return false;
}
int main()
{
	bool matched = false;
	bool matched1 = false;

	std::map<std::string, std::vector<std::string>> keyword;
	std::map<std::string, std::string> danceKeyword;

	keyword["�ڵ�"] = { "�˰��� ���Ƹ�", "�� ���� ���Ƹ�", "��Ŀ�� ���Ƹ�" };
	keyword["��"] = { "K-POP�� ���Ƹ�, ��Ʈ�� �� ���Ƹ�, ��ŷ �� ���Ƹ�" };

	danceKeyword["��"] = "��";

	std::cout << "ê��: �Ʒ��� ���� Ű���带 �Է��غ�����!" << "\n";
	std::cout << "1. MBTI  2. ���� ����  3. ���" << "\n\n";

	std::string ss; // string �̸� ����
	std::string qq; // string �̸� ����

	std::getline(std::cin, ss);
	if (ss == "���� ����" || ss == "��������")
	{
		std::cout << "���� ���� �ִ� ������ ���� �����Ӱ� �Է����ּ���!" << "\n";
		std::getline(std::cin, qq);
		std::cout << "\n";

		for (const auto& token : keyword)
		{
			std::string keyword = token.first;

			if (qq.find(keyword) != std::string::npos)
			{
				matched = true;
			}
			else
			{
				for (const auto& token1 : danceKeyword)
				{
					if (isKeywordMatch(qq, token1.first, token1.second, keyword))
					{
						matched = true;
						break;
					}
				}
			}
			if (matched)
			{
				matched1 = true;
				std::cout << "��õ ���Ƹ� (" << keyword << " ����): ";
				for (size_t i = 0; i < token.second.size(); i++)
				{
					std::cout << token.second[i];
					if (i != token.second.size() - 1)
					{
						std::cout << ", ";
					}
				}
				std::cout << "\n";
				matched = false;
			}
		}
		if (!matched1)
		{
			std::cout << "���õ� ��õ ���Ƹ��� ã�� �� �����ϴ�.\n";
		}
	}
	//if (ss == "MBTI")
	//{
	//	std::cout << "����� MBTI ������ �Է����ּ���!" << "\n";
	//	std::getline(std::cin, qq);
	//	std::cout << "�Է��� MBTI: " << qq << " (qq string ���� Ȯ�� ���)" << "\n";
	//}
	//else if (ss == "���� ����")
	//{
	//	std::cout << "���� ���� �ִ� ������ ���� �����Ӱ� �Է����ּ���!" << "\n";
	//	std::getline(std::cin, qq); std::cout << "\n";

	//	if (qq.find("�ڵ�") != std::string::npos || qq.find("���α׷���") != std::string::npos)
	//	{
	//		std::cout << "��õ ���Ƹ�: �˰��� ���Ƹ�, �� ���� ���Ƹ�, ��Ŀ�� ���Ƹ�" << "\n";
	//		matched = true;

	//		std::cout << "\n�� ���� ��õ�� ���� �Ʒ� �������� �亯���ּ���!\n";

	//		std::string timePref, modePref;

	//		std::cout << "Q1. �ַ� Ȱ�� ������ �ð���� �����ΰ���? (��: ���� ����, �ָ� ��)\n";
	//		std::getline(std::cin, timePref);

	//		std::cout << "Q2. Ȱ�� ����� ��ȣ�ϽŴٸ�? (�¶���, �������� �� �ϳ�)\n";
	//		std::getline(std::cin, modePref);

	//		std::cout << "\n��ſ��� �� �´� ���Ƹ��� �м� ��...\n";

	//		if (timePref.find("����") != std::string::npos && modePref.find("�¶���") != std::string::npos)
	//		{
	//			std::cout << "��õ: ���� ���� �¶��� ��Ŀ�� ���Ƹ�\n";
	//		}
	//		else if (timePref.find("�ָ�") != std::string::npos && modePref.find("��������") != std::string::npos)
	//		{
	//			std::cout << "��õ: �ָ� �������� �ڵ����͵� ���Ƹ�\n";
	//		}
	//		else
	//		{
	//			std::cout << "��õ: �پ��� �ð��뿡 Ȱ���ϴ� ������ ���� ���Ƹ�\n";
	//		}
	//	}

	//	if (qq.find("��") != std::string::npos || qq.find("��") != std::string::npos)
	//	{
	//		std::cout << "��õ ���Ƹ�: ��Ʈ���� ���Ƹ�, K-POP �� ���Ƹ�" << "\n";
	//		matched = true;
	//	}
	//	if (qq.find("����") != std::string::npos || qq.find("�Կ�") != std::string::npos)
	//	{
	//		std::cout << "��õ ���Ƹ�: ��Ĭ, �����θ� ��⿡�� �ƽ���!" << "\n";
	//		matched = true;
	//	}

	//	if (!matched)
	//	{
	//		std::cout << "�˼��ؿ�, �ش� ������ �´� ���Ƹ��� ���� �غ� ���̿���!" << "\n";
	//	}
	//}

	std::cout << ss << " (ss string Ȯ�� ���)";
}

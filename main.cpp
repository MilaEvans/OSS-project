#include <iostream>
#include <string>

int main()
{
	bool matched = false;

	std::cout << "ê��: �Ʒ��� ���� Ű���带 �Է��غ�����!" << "\n";
	std::cout << "1. MBTI" << " " << "2. ���� ����" << " " << "3. ���" << "\n\n";

	std::string ss; // string �̸� ����
	std::getline(std::cin, ss);


	if (ss == "MBTI")
	{
		std::string qq; // string �̸� ����
		std::cout << "����� MBTI ������ �Է����ּ���!" << "\n";
		std::getline(std::cin, qq);
		std::cout << "�Է��� MBTI: " << qq << "(qq string ���� Ȯ�� ���)" << "\n"; // qq string Ȯ��
	}
	else if (ss == "���� ����")
	{
		std::string qq; // string �̸� ����
		std::cout << "���� ���� �ִ� ������ ���� �����Ӱ� �Է����ּ���!" << "\n";
		std::getline(std::cin, qq); std::cout << "\n";

		if (qq.find("�ڵ�") != std::string::npos || qq.find("���α׷���") != std::string::npos)
		{
			std::cout << "��õ ���Ƹ�: �˰��� ���Ƹ�, �� ���� ���Ƹ�, ��Ŀ�� ���Ƹ�" << "\n";
			// ���Ƹ� ��� ��� or �ٸ� Ű���� Ȯ��(ex ���ϴ� �ð���)
			matched = true;
		}
		if (qq.find("��") != std::string::npos || qq.find("��") != std::string::npos)
		{
			std::cout << "��õ ���Ƹ�: ��Ʈ���� ���Ƹ�, K-POP �� ���Ƹ�" << "\n";
			matched = true;
		}
		if (qq.find("����") != std::string::npos || qq.find("�Կ�") != std::string::npos)
		{
			std::cout << "��õ ���Ƹ�: ��Ĭ ���Ƹ�, �����θ� ��⿡�� �ƽ���!" << "\n";
			matched = true;
		}
		if (!matched)
		{
			std::cout << "�˼��ؿ�, �ش� ������ �´� ���Ƹ��� ���� �غ����̿���!" << "\n";
		}
	}
	std::cout << ss << "(ss string Ȯ�� ���)"; // ss string Ȯ��

}
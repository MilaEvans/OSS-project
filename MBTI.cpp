#include "MBTI.h"

#include <iostream>
#include <vector>
#include <string>

void User::addTag(const std::string& tag)
{
	tags.push_back(tag);
}

const std::vector<std::string>& User::getTags() const 
{
	return tags;
}

void User::askMBTI()
{
	std::cout << "���Ƹ� ��õ ê���� ���� �� ȯ���մϴ�!\n\n";
	std::cout << "MBTI�� �Է����ּ��� ��(: INTJ, ENFP): ";
	std::string mbti;
	std::cin >> mbti;

	if (mbti == "ISTP" || mbti == "ESTP")
	{
		addTag("��������");
	}
	else
	{
		std::cout << "�߸��� MBTI �Է��Դϴ�. �ٽ� �Է����ּ���.\n";
	}
}

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
	std::cout << "동아리 추천 챗봇에 오신 걸 환영합니다!\n\n";
	std::cout << "MBTI를 입력해주세요 예(: INTJ, ENFP): ";
	std::string mbti;
	std::cin >> mbti;

	if (mbti == "ISTP" || mbti == "ESTP")
	{
		addTag("모험적인");
	}
	else
	{
		std::cout << "잘못된 MBTI 입력입니다. 다시 입력해주세요.\n";
	}
}

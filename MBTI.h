#ifndef MBTI_H
#define MBTI_H

#include <iostream>
#include <string>
#include <vector>

class User
{
private:
	std::vector<std::string> tags;
public:
	void addTag(const std::string& tag); // tag 추가 함수
	const std::vector<std::string>& getTags() const; // 사용자 태그 반환
	void askMBTI();
	
};
#endif
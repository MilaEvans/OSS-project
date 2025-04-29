#ifndef MBTI_H
#define MBTI_H

#include <iostream>
#include <vector>
#include <string>

class User
{
private:
	std::vector<std::string> tags;
public:
	void addTag(const std::string& tag); // tag 추가 함수
	
};
#endif

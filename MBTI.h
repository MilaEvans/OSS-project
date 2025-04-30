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
	void addTag(const std::string& tag); // tag �߰� �Լ�
	const std::vector<std::string>& getTags() const; // ����� �±� ��ȯ
	void askMBTI();
	
};
#endif
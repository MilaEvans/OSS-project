#ifndef MBTI_H
#define MBTI_H

class User
{
private:
	std::vector<std::string> tags;
public:
	void addTag(const std::string& tag); // tag �߰� �Լ�
	const std::vector<std::string>& getTags() const; // ����� �±� ��ȯ
	void askMBTI()
	{
		std::cout << "���Ƹ� ��õ ê���� ���� �� ȯ���մϴ�!\n\n";
	}
};
#endif
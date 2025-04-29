#ifndef MBTI_H
#define MBTI_H

class User
{
private:
	std::vector<std::string> tags;
public:
	void addTag(const std::string& tag); // tag 추가 함수
	const std::vector<std::string>& getTags() const; // 사용자 태그 반환
	void askMBTI()
	{
		std::cout << "동아리 추천 챗봇에 오신 걸 환영합니다!\n\n";
	}
};
#endif
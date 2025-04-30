#include <iostream>
#include <string>

int main()
{
	bool matched = false;

	std::cout << "챗봇: 아래에 관심 키워드를 입력해보세요!" << "\n";
	std::cout << "1. MBTI" << " " << "2. 관심 주제" << " " << "3. 취미" << "\n\n";

	std::string ss; // string 이름 변경
	std::getline(std::cin, ss);


	if (ss == "MBTI")
	{
		std::string qq; // string 이름 변경
		std::cout << "당신의 MBTI 유형을 입력해주세요!" << "\n";
		std::getline(std::cin, qq);
		std::cout << "입력한 MBTI: " << qq << "(qq string 내용 확인 출력)" << "\n"; // qq string 확인
	}
	else if (ss == "관심 주제")
	{
		std::string qq; // string 이름 변경
		std::cout << "요즘 관심 있는 주제에 대해 자유롭게 입력해주세요!" << "\n";
		std::getline(std::cin, qq); std::cout << "\n";

		if (qq.find("코딩") != std::string::npos || qq.find("프로그래밍") != std::string::npos)
		{
			std::cout << "추천 동아리: 알고리즘 동아리, 앱 개발 동아리, 해커톤 동아리" << "\n";
			// 동아리 목록 출력 or 다른 키워드 확장(ex 원하는 시간대)
			matched = true;
		}
		if (qq.find("댄스") != std::string::npos || qq.find("춤") != std::string::npos)
		{
			std::cout << "추천 동아리: 스트릿댄스 동아리, K-POP 댄스 동아리" << "\n";
			matched = true;
		}
		if (qq.find("사진") != std::string::npos || qq.find("촬영") != std::string::npos)
		{
			std::cout << "추천 동아리: 찰칵 동아리, 눈으로만 담기에는 아쉬워!" << "\n";
			matched = true;
		}
		if (!matched)
		{
			std::cout << "죄송해요, 해당 주제에 맞는 동아리를 아직 준비중이에요!" << "\n";
		}
	}
	std::cout << ss << "(ss string 확인 출력)"; // ss string 확인

}
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>

bool isKeywordMatch(const std::string& input, const std::string& tokenFirst, const std::string& tokenSecond, const std::string& keyword)
{
	if (input.find(tokenFirst) != std::string::npos)
	{
		return tokenSecond == keyword;
	}
	return false;
}

void loadSimilarWords(const std::string& filename, std::map<std::string, std::string>& similarWordsMap)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "유사어 파일을 열 수 없습니다: " << filename << "\n";
		return;
	}

	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string synonym, keyword;
		if (iss >> synonym >> keyword)
		{
			similarWordsMap[synonym] = keyword;
		}
	}
	file.close();
}

int main()
{
	bool isCurrentMatched = false; // 기존 Matched
	bool anyMatchFound = false; // 기존 Matched1

	std::map<std::string, std::vector<std::string>> keyword;
	std::map<std::string, std::string> similarWords; // 유사어 저장용

	keyword["코딩"] = { "COSMIC", "CaTs", "CERT" };

	loadSimilarWords("C:\\Users\\pring\\Desktop\\similar_words.txt", similarWords); // 유사어 파일 로드

	std::cout << "챗봇: 아래에 관심 키워드를 입력해보세요!" << "\n";
	std::cout << "1. MBTI  2. 관심 주제" << "\n\n";

	std::string categoryInput; // 기존 ss
	std::string userInput; // 기존 qq

	std::getline(std::cin, categoryInput);
	if (categoryInput == "관심 주제" || categoryInput == "관심주제")
	{
		std::cout << "요즘 관심 있는 주제에 대해 자유롭게 입력해주세요!" << "\n";
		std::string userInput; // 기존 qq
		std::getline(std::cin, userInput);
		std::cout << "\n";

		for (const auto& token : keyword)
		{
			std::string key = token.first;

			if (userInput.find(key) != std::string::npos)
			{
				isCurrentMatched = true;
			}
			else
			{
				for (const auto& token1 : similarWords)
				{
					if (isKeywordMatch(userInput, token1.first, token1.second, key))
					{
						isCurrentMatched = true;
						break;
					}
				}
			}
			if (isCurrentMatched)
			{
				anyMatchFound = true;
				std::cout << "추천 동아리 (" << key << " 관련): ";
				for (size_t i = 0; i < token.second.size(); i++)
				{
					std::cout << token.second[i];
					if (i != token.second.size() - 1)
					{
						std::cout << ", ";
					}
				}
				std::cout << "\n";
				isCurrentMatched = false;
			}
		}
		if (!anyMatchFound)
		{
			std::cout << "관련된 추천 동아리를 찾을 수 없습니다.\n";
		}
	}
	//if (ss == "MBTI")
	//{
	//	std::cout << "당신의 MBTI 유형을 입력해주세요!" << "\n";
	//	std::getline(std::cin, qq);
	//	std::cout << "입력한 MBTI: " << qq << " (qq string 내용 확인 출력)" << "\n";
	//}
	//else if (ss == "관심 주제")
	//{
	//	std::cout << "요즘 관심 있는 주제에 대해 자유롭게 입력해주세요!" << "\n";
	//	std::getline(std::cin, qq); std::cout << "\n";

	//	if (qq.find("코딩") != std::string::npos || qq.find("프로그래밍") != std::string::npos)
	//	{
	//		std::cout << "추천 동아리: 알고리즘 동아리, 앱 개발 동아리, 해커톤 동아리" << "\n";
	//		matched = true;

	//		std::cout << "\n더 나은 추천을 위해 아래 질문에도 답변해주세요!\n";

	//		std::string timePref, modePref;

	//		std::cout << "Q1. 주로 활동 가능한 시간대는 언제인가요? (예: 평일 저녁, 주말 등)\n";
	//		std::getline(std::cin, timePref);

	//		std::cout << "Q2. 활동 방식을 선호하신다면? (온라인, 오프라인 중 하나)\n";
	//		std::getline(std::cin, modePref);

	//		std::cout << "\n당신에게 잘 맞는 동아리를 분석 중...\n";

	//		if (timePref.find("저녁") != std::string::npos && modePref.find("온라인") != std::string::npos)
	//		{
	//			std::cout << "추천: 평일 저녁 온라인 해커톤 동아리\n";
	//		}
	//		else if (timePref.find("주말") != std::string::npos && modePref.find("오프라인") != std::string::npos)
	//		{
	//			std::cout << "추천: 주말 오프라인 코딩스터디 동아리\n";
	//		}
	//		else
	//		{
	//			std::cout << "추천: 다양한 시간대에 활동하는 유연한 개발 동아리\n";
	//		}
	//	}

	//	if (qq.find("댄스") != std::string::npos || qq.find("춤") != std::string::npos)
	//	{
	//		std::cout << "추천 동아리: 스트릿댄스 동아리, K-POP 댄스 동아리" << "\n";
	//		matched = true;
	//	}
	//	if (qq.find("사진") != std::string::npos || qq.find("촬영") != std::string::npos)
	//	{
	//		std::cout << "추천 동아리: 찰칵, 눈으로만 담기에는 아쉬워!" << "\n";
	//		matched = true;
	//	}

	//	if (!matched)
	//	{
	//		std::cout << "죄송해요, 해당 주제에 맞는 동아리를 아직 준비 중이에요!" << "\n";
	//	}
	//}

	std::cout << ss << " (ss string 확인 출력)";
}

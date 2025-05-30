#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>

// 사용자 입력 함수
std::string getUserInput(const std::string& prompt)
{
	std::cout << prompt;
	std::string input;
	std::getline(std::cin, input);
	return input;
}

// 유사어가 keyword와 매칭되는지 검사
bool isKeywordMatch(const std::string& input, const std::string& tokenFirst, const std::string& tokenSecond, const std::string& keyword)
{
	if (input.find(tokenFirst) != std::string::npos)
	{
		return tokenSecond == keyword;
	}
	return false;
}

// 유사어 파일 로드
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

// 추천 동아리 출력 함수
void printRecommendedClubs(const std::string& key, const std::vector<std::string>& clubs)
{
	std::cout << "추천 동아리 (" << key << " 관련): ";
	for (size_t i = 0; i < clubs.size(); ++i)
	{
		std::cout << clubs[i];
		if (i != clubs.size() - 1)
		{
			std::cout << ", ";
		}
	}
	std::cout << "\n";
}

// 키워드 및 유사어 기반 추천 로직 함수
void processInterestInput(const std::string& userInput,
	const std::map<std::string, std::vector<std::string>>& keywordMap,
	const std::map<std::string, std::string>& similarWords)
{
	bool isCurrentMatched = false;
	bool anyMatchFound = false;

	for (const auto& token : keywordMap)
	{
		const std::string& key = token.first;

		// 1. 직접 키워드 매칭
		if (userInput.find(key) != std::string::npos)
		{
			isCurrentMatched = true;
		}
		else
		{
			// 2. 유사어 매칭
			for (const auto& pair : similarWords)
			{
				if (isKeywordMatch(userInput, pair.first, pair.second, key))
				{
					isCurrentMatched = true;
					break;
				}
			}
		}

		// 출력
		if (isCurrentMatched)
		{
			anyMatchFound = true;
			printRecommendedClubs(key, token.second);
			isCurrentMatched = false;
		}
	}

	if (!anyMatchFound)
	{
		std::cout << "관련된 추천 동아리를 찾을 수 없습니다.\n";
	}
}


int main()
{
	std::map<std::string, std::vector<std::string>> keyword;
	std::map<std::string, std::string> similarWords; // 유사어 저장용

	// 키워드 등록
	keyword["코딩"] = { "COSMIC", "CaTs", "CERT" };

	// 유사어 파일 로드
	loadSimilarWords("C:\\Users\\pring\\Desktop\\similar_words.txt", similarWords); // 유사어 파일 로드

	// 사용자 입력
	std::string categoryInput = getUserInput("챗봇: 아래에 관심 키워드를 입력해보세요!\n1. MBTI  2. 관심 주제\n\n");

	if (categoryInput == "관심 주제" || categoryInput == "관심주제")
	{
		std::string userInput = getUserInput("요즘 관심 있는 주제에 대해 자유롭게 입력해주세요!\n\n");
		processInterestInput(userInput, keyword, similarWords); // 관심 키워드 처리 함수 호출
	}
}

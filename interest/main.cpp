#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>

std::string getUserInput(const std::string& prompt)
{
	std::cout << prompt;
	std::string input;
	std::getline(std::cin, input);
	return input;
}

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

int main()
{
	bool isCurrentMatched = false; // 기존 Matched
	bool anyMatchFound = false; // 기존 Matched1

	std::map<std::string, std::vector<std::string>> keyword;
	std::map<std::string, std::string> similarWords; // 유사어 저장용

	keyword["코딩"] = { "COSMIC", "CaTs", "CERT" };

	loadSimilarWords("C:\\Users\\pring\\Desktop\\similar_words.txt", similarWords); // 유사어 파일 로드

	std::string categoryInput = getUserInput("챗봇: 아래에 관심 키워드를 입력해보세요!\n1. MBTI  2. 관심 주제\n\n");

	if (categoryInput == "관심 주제" || categoryInput == "관심주제")
	{
		std::string userInput = getUserInput("요즘 관심 있는 주제에 대해 자유롭게 입력해주세요!\n\n");

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
				printRecommendedClubs(key, token.second);
				isCurrentMatched = false;
			}
		}
		if (!anyMatchFound)
		{
			std::cout << "관련된 추천 동아리를 찾을 수 없습니다.\n";
		}
	}
}

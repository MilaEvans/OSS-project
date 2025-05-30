#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

// 단어 빈도 저장용
std::map<std::string, int> wordFrequency;

// 인기 동아리 추천 횟수 저장용
std::map<std::string, int> clubRecommendCount;

// 사용자 입력에서 단어 추출 및 빈도 증가 함수
void analyzeUserInput(const std::string& input) {
	std::istringstream iss(input);
	std::string word;
	while (iss >> word) {
		// 전처리 없이 그대로 빈도 증가
		wordFrequency[word]++;
	}
}

// 파일 읽기 함수 추가
void loadRecommendCount(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "추천 횟수 파일을 열 수 없습니다. 새로 생성합니다: " << filename << "\n";
		return;
	}

	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string clubName;
		int count = 0;
		if (iss >> clubName >> count)
		{
			clubRecommendCount[clubName] = count;
		}
	}
	file.close();
}

// 파일 저장 함수 추가
void saveRecommendCount(const std::string& filename)
{
	std::ofstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "추천 횟수 파일을 저장할 수 없습니다: " << filename << "\n";
		return;
	}

	for (const auto& pair : clubRecommendCount)
	{
		file << pair.first << " " << pair.second << "\n";
	}
	file.close();
}



// 잠재 키워드 후보 출력
void printCandidateKeywords(int threshold) {
	std::cout << "\n잠재 키워드 후보 (빈도 " << threshold << " 이상):\n";
	bool found = false;
	for (const auto& pair : wordFrequency) {
		if (pair.second >= threshold) {
			std::cout << pair.first << " (" << pair.second << "회)\n";
			found = true;
		}
	}
	if (!found) {
		std::cout << "(없음)\n";
	}
}

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

// 추천 동아리 출력 및 추천 횟수 누적 함수
void printRecommendedClubs(const std::string& key, const std::vector<std::string>& clubs)
{
	std::cout << "추천 동아리 (" << key << " 관련): ";
	for (size_t i = 0; i < clubs.size(); ++i)
	{
		std::cout << clubs[i];
		clubRecommendCount[clubs[i]]++;
		if (i != clubs.size() - 1)
		{
			std::cout << ", ";
		}
	}
	std::cout << "\n";
}

// 인기 동아리 순위 출력 함수
void printPopularClubs()
{
	if (clubRecommendCount.empty())
	{
		std::cout << "\n(아직 추천된 동아리가 없습니다)\n";
		return;
	}

	std::vector<std::pair<std::string, int>> clubList(clubRecommendCount.begin(), clubRecommendCount.end());

	std::sort(clubList.begin(), clubList.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // 추천 횟수 기준 내림차순 정렬
		});

	std::cout << "\n인기 동아리 순위 \n";
	int rank = 1;
	for (const auto& entry : clubList)
	{
		std::cout << rank++ << "위: " << entry.first << " (" << entry.second << "회 추천됨)\n";
	}
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

	// 추천 횟수 저장 파일명
	const std::string recommendCountFile = "recommend_count.txt";

	// 기존 데이터 로드
	loadRecommendCount(recommendCountFile);

	// 키워드 등록
	keyword["코딩"] = { "COSMIC", "CaTs", "CERT" };
	keyword["스포츠"] = { "H.I.T", "BBP", "setup", "ACE", "SPLIT", "EDGE", "다마스", "lightweight", "북두칠성"};
	keyword["댄스"] = { "SIVA CREW" };
	keyword["게임"] = { "Online Club" };
	keyword["여행"] = { "NAWoo", ""};
	keyword["응원단"] = { "늘해랑" };
	keyword["뮤지컬"] = { "AMUSEMENT" };


	// 유사어 파일 로드
	loadSimilarWords("C:\\Users\\pring\\Desktop\\similar_words.txt", similarWords); // 유사어 파일 로드

	// 사용자 입력
	std::string categoryInput = getUserInput("챗봇: 아래에 관심 키워드를 입력해보세요!\n1. MBTI  2. 관심 주제\n\n");

	if (categoryInput == "관심 주제" || categoryInput == "관심주제")
	{
		while (true)
		{
			std::string userInput = getUserInput("요즘 관심 있는 주제에 대해 자유롭게 입력해주세요!\n\n");
			if (userInput == "exit") break;

			// 3. 단어 빈도 분석
			analyzeUserInput(userInput);

			// 4. 추천 처리
			processInterestInput(userInput, keyword, similarWords); // 관심 키워드 처리 함수 호출

			// 5. 잠재 키워드 후보 출력 (빈도 2 이상 단어)
			printCandidateKeywords(2);

			std::cout << "\n---------------------\n";
		}
	}

	else
	{
		std::cout << "현재 지원하지 않는 카테고리입니다.\n";
	}


	// 인기 동아리 순위 출력
	printPopularClubs();

	// 프로그램 종료 전에 추천 횟수 저장
	saveRecommendCount(recommendCountFile);
}

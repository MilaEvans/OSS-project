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


// 예산/비용 관련 동아리 추천
void recommendClubsByBudget(const std::string& userInput, const std::vector<std::string>& budgetClubs) {
	std::string lowered = userInput;
	std::transform(lowered.begin(), lowered.end(), lowered.begin(), ::tolower);

	if (lowered.find("예산") != std::string::npos || lowered.find("비용") != std::string::npos ||
		lowered.find("싸다") != std::string::npos || lowered.find("무료") != std::string::npos ||
		lowered.find("비싸다") != std::string::npos)
	{
		std::cout << "\n예산을 고려해 가입할 수 있는 저렴한 동아리는 다음과 같아요:\n";
		for (const auto& club : budgetClubs) {
			std::cout << "- " << club << "\n";
			clubRecommendCount[club]++;
		}
		std::cout << "\n";
	}
}

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

// 저녁 시간대에 활동할 수 있는 동아리 추천 함수
void checkEveningActivity(const std::string& userInput, const std::vector<std::string>& eveningClubs)
{
	std::string lowered = userInput;
	std::transform(lowered.begin(), lowered.end(), lowered.begin(), ::tolower);

	// "저녁" 또는 "밤" + "동아리" 또는 "활동" 포함 여부 검사
	if ((lowered.find("저녁") != std::string::npos || lowered.find("밤") != std::string::npos) &&
		(lowered.find("동아리") != std::string::npos || lowered.find("활동") != std::string::npos))
	{
		std::cout << "저녁 시간대에 활동할 수 있는 동아리는 다음과 같아요:\n";
		for (const auto& club : eveningClubs)
		{
			std::cout << "- " << club << "\n";
			clubRecommendCount[club]++;  // 추천 횟수 누적
		}
		std::cout << "\n";
	}
}

// 요일 기반 동아리 추천
void recommendClubsByDay(const std::string& userInput, const std::map<std::string, std::vector<std::string>>& dayClubs) {
	// 한글 요일 리스트
	std::vector<std::string> days = { "월요일", "화요일", "수요일", "목요일", "금요일", "토요일", "일요일" };

	for (const auto& day : days) {
		if (userInput.find(day) != std::string::npos) {
			std::cout << "\n" << day << "에 활동하는 동아리는 다음과 같아요:\n";
			for (const auto& club : dayClubs.at(day)) {
				std::cout << "- " << club << "\n";
				clubRecommendCount[club]++;
			}
			std::cout << "\n";
			return;
		}
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

	std::vector<std::string> eveningClubs = {
	"SIVA CREW",  // 댄스
	"AMUSEMENT",  // 뮤지컬
	"COSMIC",     // 코딩
	"늘해랑"      // 응원단
	};

	std::vector<std::string> budgetClubs = { "CaTs", "NAWoo", "COSMIC", "Online Club" }; // 예산 고려 동아리

	// 요일별 동아리 리스트
	std::map<std::string, std::vector<std::string>> dayClubs = {
		{"월요일", {"COSMIC", "H.I.T"}},
		{"화요일", {"SIVA CREW", "Online Club"}},
		{"수요일", {"AMUSEMENT", "CaTs"}},
		{"목요일", {"NAWoo", "늘해랑"}},
		{"금요일", {"BBP", "setup"}},
		{"토요일", {"EDGE", "SPLIT"}},
		{"일요일", {"북두칠성", "다마스"}}
	};

	// 유사어 파일 로드
	loadSimilarWords("C:\\Users\\pring\\Desktop\\similar_words.txt", similarWords); // 유사어 파일 로드

	// 사용자 입력
	std::cout << "동아리 추천 챗봇에 오신 것을 환영합니다! 종료하려면 'exit'을 입력하세요.\n\n";
	{
		while (true)
		{
			std::string userInput = getUserInput("요즘 관심 있는 주제에 대해 자유롭게 입력해주세요!\n\n");

			if (userInput == "exit") break;
			// 0. 요일 기반 
			recommendClubsByDay(userInput, dayClubs);              
			// 1. 비용 고려
			recommendClubsByBudget(userInput, budgetClubs);
			// 2. 저녁 시간대 동아리 질문 확인 및 추천
			checkEveningActivity(userInput, eveningClubs);

			// 3. 단어 빈도 분석
			analyzeUserInput(userInput);

			// 4. 추천 처리
			processInterestInput(userInput, keyword, similarWords); // 관심 키워드 처리 함수 호출

			// 5. 잠재 키워드 후보 출력 (빈도 2 이상 단어)

			printCandidateKeywords(2);

			std::cout << "\n---------------------\n";
		}
	}


	// 인기 동아리 순위 출력
	printPopularClubs();

	// 프로그램 종료 전에 추천 횟수 저장
	saveRecommendCount(recommendCountFile);
}

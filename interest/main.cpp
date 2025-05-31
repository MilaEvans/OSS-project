#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>  // std::hash

const std::vector<std::string> timeSlots = { "오전", "오후", "저녁" };
const std::vector<std::string> days = { "월요일", "화요일", "수요일", "목요일", "금요일", "토요일", "일요일" };

struct ClubInfo {
	bool hasFee;
	std::string time;
	std::string day;
};

ClubInfo generateFixedInfo(const std::string& clubName) {
	std::hash<std::string> hasher;
	size_t hash = hasher(clubName);

	bool hasFee = (hash % 2 == 0);
	size_t timeIdx = days.size();
	size_t dayIdx = (hash / (2 * timeSlots.size())) % days.size();

	return ClubInfo{ hasFee, timeSlots[timeIdx], days[dayIdx] };
}

// 단어 빈도 저장용
std::map<std::string, int> wordFrequency;

// 인기 동아리 추천 횟수 저장용
std::map<std::string, int> clubRecommendCount;


// 예산/비용 관련 동아리 추천
void recommendClubsByBudget(const std::string& userInput, const std::vector<std::string>& allClubs) {
	std::string lowered = userInput;
	std::transform(lowered.begin(), lowered.end(), lowered.begin(), ::tolower);

	bool wantFree = (
		lowered.find("무료") != std::string::npos ||
		lowered.find("저렴") != std::string::npos ||
		lowered.find("싸다") != std::string::npos ||
		lowered.find("비용 없다") != std::string::npos ||
		lowered.find("공짜") != std::string::npos
		);

	bool wantPaid = (
		lowered.find("비싸다") != std::string::npos ||
		lowered.find("유료") != std::string::npos ||
		lowered.find("비용 있다") != std::string::npos ||
		lowered.find("고급") != std::string::npos
		);

	if (!wantFree && !wantPaid) return;

	std::vector<std::string> result;

	for (const auto& club : allClubs) {
		ClubInfo info = generateFixedInfo(club);

		if (wantFree && !info.hasFee) {
			result.push_back(club);
		}
		else if (wantPaid && info.hasFee) {
			result.push_back(club);
		}
	}

	if (!result.empty()) {
		if (wantFree) {
			std::cout << "\n비용이 들지 않는 동아리는 다음과 같아요:\n";
		}
		else if (wantPaid) {
			std::cout << "\n비용이 드는 동아리는 다음과 같아요:\n";
		}
		for (const auto& club : result) {
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


// 오전 시간대에 활동할 수 있는 동아리 추천 함수
void checkMorningActivity(const std::string& userInput, const std::vector<std::string>& morningClubs)
{
	std::string lowered = userInput;
	std::transform(lowered.begin(), lowered.end(), lowered.begin(), ::tolower);

	if ((lowered.find("오전") != std::string::npos || lowered.find("아침") != std::string::npos) &&
		(lowered.find("동아리") != std::string::npos || lowered.find("활동") != std::string::npos))
	{
		std::cout << "오전 시간대에 활동할 수 있는 동아리는 다음과 같아요:\n";
		for (const auto& club : morningClubs)
		{
			std::cout << "- " << club << "\n";
			clubRecommendCount[club]++;
		}
		std::cout << "\n";
	}
}

// 오후 시간대에 활동할 수 있는 동아리 추천 함수
void checkAfternoonActivity(const std::string& userInput, const std::vector<std::string>& afternoonClubs)
{
	std::string lowered = userInput;
	std::transform(lowered.begin(), lowered.end(), lowered.begin(), ::tolower);

	if ((lowered.find("오후") != std::string::npos || lowered.find("점심") != std::string::npos) &&
		(lowered.find("동아리") != std::string::npos || lowered.find("활동") != std::string::npos))
	{
		std::cout << "오후 시간대에 활동할 수 있는 동아리는 다음과 같아요:\n";
		for (const auto& club : afternoonClubs)
		{
			std::cout << "- " << club << "\n";
			clubRecommendCount[club]++;
		}
		std::cout << "\n";
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
	std::map<std::string, std::vector<std::string>> dayClubs;
	// 추천 횟수 저장 파일명
	const std::string recommendCountFile = "recommend_count.txt";

	// 기존 데이터 로드
	loadRecommendCount(recommendCountFile);

	// 키워드 등록
	keyword["코딩"] = { "코딩하는친구들", "밤샘코딩", "코드수다방", "오픈소스동네", "디버깅모임" };
	keyword["운동"] = { "땀흘리기", "운동하자", "뛰뛰빵빵", "런닝메이트", "주말농구" }; 
	keyword["댄스"] = { "춤추는우리", "리듬따라", "뽐내기댄스", "비트속으로", "즐거운댄스" }; 
	keyword["게임"] = { "게임하러모여", "보드게임존", "게임친구들", "온라인모험단", "게임톡" };
	keyword["여행"] = { "여행가자", "떠나볼까", "길따라걷기", "캠핑가족", "바람따라" };
	keyword["음악"] = { "음악좋아", "노래방친구들", "멜로디모임", "악기연주회", "소리모아" };
	keyword["자기개발"] = { "함께성장", "작은도전", "꿈꾸는우리", "스킬업", "자기관리" };
	keyword["독서"] = { "책읽는시간", "책모임", "한줄토크", "읽고싶은책", "독서친구" };
	keyword["봉사"] = { "도움의손길", "함께나누기", "따뜻한마음", "봉사모임", "기쁨나누기" }; 
	keyword["사진"] = { "찰칵찰칵", "사진찍기좋아", "풍경사진", "우리사진관", "사진산책" }; 
	keyword["영상"] = { "영상찍자", "영상편집모임", "필름러버", "우리영상", "유튜브친구들" };
	keyword["미술"] = { "그림그리기", "낙서모임", "아트타임", "색칠놀이", "손그림모임" };
	keyword["연극"] = { "연극놀이", "무대연습", "연기하는우리", "즉흥연기", "연극모임" };
	keyword["요리"] = { "요리해요", "간식만들기", "베이킹클럽", "맛있는시간", "요리친구들" }; 

	std::vector<std::string> budgetClubs, morningClubs, afternoonClubs, eveningClubs, allClubs;

	for (const auto& pair : keyword) {
		for (const auto& club : pair.second) {
			allClubs.push_back(club);
			ClubInfo info = generateFixedInfo(club);
			if (!info.hasFee) budgetClubs.push_back(club);
			if (info.time == "오전") morningClubs.push_back(club);
			else if (info.time == "오후") afternoonClubs.push_back(club);
			else if (info.time == "저녁") eveningClubs.push_back(club);
			dayClubs[info.day].push_back(club);
		}
	}



	// 유사어 파일 로드
	loadSimilarWords("similar_words.txt", similarWords); // 유사어 파일 로드

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
			recommendClubsByBudget(userInput, allClubs);

			// 2. 저녁 시간대 동아리 질문 확인 및 추천
			checkEveningActivity(userInput, eveningClubs);
			checkMorningActivity(userInput, morningClubs);
			checkAfternoonActivity(userInput, afternoonClubs);


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

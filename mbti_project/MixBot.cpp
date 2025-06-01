// MixBot.cpp
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <tuple>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <regex>
#include <functional>

// ─── (1) 동아리 확장 정보 구조체 정의 ─────────────────────────────────────────
struct ExtendedClubInfo {
    std::string name;
    bool isFree;                       // true: 무료, false: 유료
    std::vector<std::string> days;     // 예: {"월요일", "수요일"}
    std::vector<std::string> times;    // 예: {"오전", "저녁"}
    std::vector<std::string> mbtiTags; // 예: {"INTJ", "ENTP"} (선택사항)
    std::string location;              // "오프라인" 또는 "온라인"
};

// ─── (2) CSV 파일에서 동아리 정보 로드 ─────────────────────────────────────────
static std::map<std::string, ExtendedClubInfo> clubData;

void loadClubData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "club_data 파일을 열 수 없습니다: " << filename << "\n";
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string token;
        ExtendedClubInfo info;

        // 1) 이름
        std::getline(ss, token, ',');
        info.name = token;

        // 2) 회비 여부 (1=무료, 0=유료)
        std::getline(ss, token, ',');
        info.isFree = (token == "1");

        // 3) 요일들 (세미콜론 구분)
        std::getline(ss, token, ',');
        if (!token.empty()) {
            std::stringstream ds(token);
            std::string d;
            while (std::getline(ds, d, ';')) {
                info.days.push_back(d);
            }
        }

        // 4) 시간대들 (세미콜론 구분)
        std::getline(ss, token, ',');
        if (!token.empty()) {
            std::stringstream ts(token);
            std::string t;
            while (std::getline(ts, t, ';')) {
                info.times.push_back(t);
            }
        }

        // 5) 형태 (오프라인/온라인)
        std::getline(ss, token, ',');
        info.location = token;

        // 6) MBTI 태그들 (세미콜론 구분, 선택적)
        std::getline(ss, token);
        if (!token.empty()) {
            std::stringstream ms(token);
            std::string m;
            while (std::getline(ms, m, ';')) {
                info.mbtiTags.push_back(m);
            }
        }

        clubData[info.name] = info;
    }
    file.close();
}

// ─── (3) 유사어 로드 ───────────────────────────────────────────────────────────
static std::map<std::string, std::string> similarWords;

void loadSimilarWords(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "유사어 파일을 열 수 없습니다: " << filename << "\n";
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string synonym, keyword;
        if (iss >> synonym >> keyword) {
            similarWords[synonym] = keyword;
        }
    }
    file.close();
}

// ─── (4) 추천 횟수 로드/저장 ───────────────────────────────────────────────────
static std::map<std::string, int> clubRecommendCount;

void loadRecommendCount(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        // 파일 없으면 신규 생성 예정
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string clubName;
        int cnt;
        if (iss >> clubName >> cnt) {
            clubRecommendCount[clubName] = cnt;
        }
    }
    file.close();
}

void saveRecommendCount(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "recommend_count.txt 저장 실패\n";
        return;
    }
    for (const auto& pair : clubRecommendCount) {
        file << pair.first << " " << pair.second << "\n";
    }
    file.close();
}

// ─── (5) MBTI 분석을 위한 기존 코드 ───────────────────────────────────────────
class MBTIInfoBasic {
public:
    std::string personality;
    std::vector<std::string> clubTypes;
    std::vector<std::string> hobbies;

    MBTIInfoBasic(const std::string& p,
                  const std::vector<std::string>& c,
                  const std::vector<std::string>& h)
        : personality(p), clubTypes(c), hobbies(h) {}

    void printInfo(const std::string& mbti) const {
        std::cout << "[MBTI 분석 결과: " << mbti << "]\n";
        std::cout << "당신은 " << personality << " 타입이시군요!\n";
        if (!clubTypes.empty()) {
            std::cout << "이런 성향이라면 다음 동아리들을 추천드려요:\n";
            for (const auto& club : clubTypes) {
                std::cout << "- " << club << "\n";
            }
        }
        if (!hobbies.empty()) {
            std::cout << "취미로는 ";
            for (size_t i = 0; i < hobbies.size(); ++i) {
                std::cout << hobbies[i];
                if (i < hobbies.size() - 1) std::cout << ", ";
            }
            std::cout << " 등을 추천드립니다.\n";
        }
    }
};

static const std::map<std::string, std::tuple<
    std::string,
    std::vector<std::string>,
    std::vector<std::string>
>> mbtiData = {
    { "INTJ", { "논리적이고 전략적인 성향",
                { "코딩하는친구들", "오픈소스동네", "디버깅모임" },
                { "독서", "퍼즐" } } },
    { "INTP", { "탐구적이고 분석적인 성향",
                { "밤샘코딩", "코드수다방", "게임하러모여" },
                { "과학", "퍼즐" } } },
    { "ENTJ", { "계획적이고 리더십 강한 성향",
                { "함께성장", "꿈꾸는우리", "스킬업" },
                { "토론", "멘토링" } } },
    { "ENTP", { "아이디어 넘치는 토론가",
                { "토론놀이", "연극모임", "즉흥연기" },
                { "즉흥 연설", "토론" } } },
    { "INFJ", { "이상주의적 조력자",
                { "따뜻한마음", "봉사모임", "함께나누기" },
                { "글쓰기", "상담" } } },
    { "INFP", { "감성적이고 가치 지향적",
                { "음악좋아", "소리모아", "사진찍기좋아" },
                { "글쓰기", "음악 감상" } } },
    { "ENFJ", { "지도력 있고 배려심 많은 성향",
                { "봉사모임", "도움의손길", "파티 기획" },
                { "멘토링", "공연 기획" } } },
    { "ENFP", { "창의적이고 열정적인 성향",
                { "멜로디모임", "악기연주회", "영상찍자" },
                { "여행", "사진" } } },
    { "ISTJ", { "책임감 강하고 실용적인 성향",
                { "책읽는시간", "책모임", "한줄토크" },
                { "가계부 정리", "통계" } } },
    { "ISFJ", { "헌신적이고 온화한 성향",
                { "베이킹클럽", "요리해요", "봉사모임" },
                { "베이킹", "독서" } } },
    { "ESTJ", { "조직적이고 체계적인 성향",
                { "행정 동아리", "리더십 동아리", "스킬업" },
                { "플래너 정리", "스케줄 관리" } } },
    { "ESFJ", { "사교적이고 배려 깊은 성향",
                { "환영회 동아리", "학교 행사 기획", "파티 기획" },
                { "파티 기획", "친목 활동" } } },
    { "ISTP", { "실용적이고 논리적인 성향",
                { "기계 동아리", "DIY 동아리", "런닝메이트" },
                { "조립", "자동차" } } },
    { "ISFP", { "조용하고 예술적인 성향",
                { "그림그리기", "사진찍기좋아", "음악좋아" },
                { "작곡", "그림" } } },
    { "ESTP", { "모험을 즐기고 활동적인 성향",
                { "주말농구", "런닝메이트", "즐거운댄스" },
                { "스포츠", "댄스" } } },
    { "ESFP", { "외향적이고 낙천적인 성향",
                { "연극놀이", "공연 동아리", "노래방친구들" },
                { "노래", "연기" } } }
};

static const std::set<std::string> validMBTIs = {
    "INTJ","INTP","ENTJ","ENTP",
    "INFJ","INFP","ENFJ","ENFP",
    "ISTJ","ISFJ","ESTJ","ESFJ",
    "ISTP","ISFP","ESTP","ESFP"
};

std::string toUpperCase(const std::string& str) {
    std::string upperStr = str;
    std::transform(
        upperStr.begin(), upperStr.end(),
        upperStr.begin(), ::toupper
    );
    return upperStr;
}

// ─── (6) 단순 관심사 키워드 기반 추천 ─────────────────────────────────────────
static const std::map<std::string, std::vector<std::string>> interestKeywords = {
    { "운동",     {"땀흘리기","운동하자","뛰뛰빵빵","런닝메이트","주말농구"} },
    { "예술",     {"그림그리기","낙서모임","아트타임","색칠놀이","손그림모임"} },
    { "음악",     {"음악좋아","노래방친구들","멜로디모임","악기연주회","소리모아"} },
    { "봉사",     {"도움의손길","함께나누기","따뜻한마음","봉사모임","기쁨나누기"} },
    { "토론",     {"토론놀이","연극모임","즉흥연기"} },
    { "IT",       {"코딩하는친구들","밤샘코딩","코드수다방","오픈소스동네","디버깅모임"} },
    { "창업",     {"함께성장","작은도전","꿈꾸는우리","스킬업","자기관리"} },
    { "문학",     {"책읽는시간","책모임","한줄토크","읽고싶은책","독서친구"} },
    { "댄스",     {"춤추는우리","리듬따라","뽐내기댄스","비트속으로","즐거운댄스"} },
    { "게임",     {"게임하러모여","보드게임존","게임친구들","온라인모험단","게임톡"} },
    { "여행",     {"여행가자","떠나볼까","길따라걷기","캠핑가족","바람따라"} },
    { "사진",     {"찰칵찰칱","사진찍기좋아","풍경사진","우리사진관","사진산책"} },
    { "영상",     {"영상찍자","영상편집모임","필름러버","우리영상","유튜브친구들"} },
    { "미술",     {"그림그리기","낙서모임","아트타임","색칠놀이","손그림모임"} },
    { "연극",     {"연극놀이","무대연습","연기하는우리","즉흥연기","연극모임"} },
    { "요리",     {"요리해요","간식만들기","베이킹클럽","맛있는시간","요리친구들"} }
};

// ─── (7) RecommendEngine 관련 데이터 및 함수 ───────────────────────────────────────

// 시간대, 요일 상수
static const std::vector<std::string> timeSlots = { "오전", "오후", "저녁" };
static const std::vector<std::string> days = {
    "월요일","화요일","수요일","목요일","금요일","토요일","일요일"
};

// (A) rawInput 한 번에 받아서 필터링한 결과만 리턴하는 헬퍼 함수
std::vector<std::string> filterClubs(const std::string& rawInput,
                                     const std::map<std::string, std::vector<std::string>>& keywordMap,
                                     const std::map<std::string, std::string>& similarWords,
                                     const std::map<std::string, std::vector<std::string>>& dayClubs,
                                     const std::vector<std::string>& allClubs,
                                     const std::vector<std::string>& offlineClubs,
                                     const std::vector<std::string>& onlineClubs,
                                     const std::vector<std::string>& morningClubs,
                                     const std::vector<std::string>& afternoonClubs,
                                     const std::vector<std::string>& eveningClubs) 
{
    // (1) 유사어 치환 & 소문자화
    std::string inputLower = rawInput;
    std::transform(inputLower.begin(), inputLower.end(), inputLower.begin(), ::tolower);
    for (const auto& pair : similarWords) {
        if (inputLower.find(pair.first) != std::string::npos) {
            inputLower = std::regex_replace(inputLower, std::regex(pair.first), pair.second);
        }
    }

    // (2) 필터 변수들
    bool wantFree = false, wantPaid = false;
    bool wantOffline = false, wantOnline = false;
    std::string selectedDay = "", selectedTime = "";
    std::vector<std::string> matchedKeywords;
    std::string upper = toUpperCase(rawInput);
    bool mbtiMentionFound = false;

    // (3) MBTI 태그 검사
    for (const auto& mbti : validMBTIs) {
        if (upper.find(mbti) != std::string::npos) {
            mbtiMentionFound = true;
            break;
        }
    }

    // (4) 요일 검사
    for (const auto& day : days) {
        if (inputLower.find(day) != std::string::npos) {
            selectedDay = day;
            break;
        }
    }

    // (5) 시간대 검사
    if (inputLower.find("오전") != std::string::npos || inputLower.find("아침") != std::string::npos) {
        selectedTime = "오전";
    }
    else if (inputLower.find("오후") != std::string::npos || inputLower.find("점심") != std::string::npos) {
        selectedTime = "오후";
    }
    else if (inputLower.find("저녁") != std::string::npos || inputLower.find("밤") != std::string::npos) {
        selectedTime = "저녁";
    }

    // (6) 회비 검사
    if (inputLower.find("무료") != std::string::npos ||
        inputLower.find("저렴") != std::string::npos ||
        inputLower.find("싸다") != std::string::npos ||
        inputLower.find("비용 없다") != std::string::npos ||
        inputLower.find("공짜") != std::string::npos) {
        wantFree = true;
    }
    if (inputLower.find("비싸다") != std::string::npos ||
        inputLower.find("유료") != std::string::npos ||
        inputLower.find("비용 있다") != std::string::npos ||
        inputLower.find("고급") != std::string::npos) {
        wantPaid = true;
    }

    // (7) 형태(온/오프라인) 검사
    if (inputLower.find("오프라인") != std::string::npos) {
        wantOffline = true;
    }
    if (inputLower.find("온라인") != std::string::npos) {
        wantOnline = true;
    }

    // (8) 관심사 키워드 매칭
    for (const auto& token : keywordMap) {
        const std::string& key = token.first;
        if (inputLower.find(key) != std::string::npos) {
            matchedKeywords.push_back(key);
        }
    }

    // (9) 실제 필터링 수행
    std::vector<std::string> filteredClubs;
    for (const auto& club : allClubs) {
        const auto& info = clubData.at(club);

        // (1) 관심 키워드 매칭 : matchedKeywords 비어있지 않으면 반드시 한 개 이상 포함
        if (!matchedKeywords.empty()) {
            bool keywordMatch = false;
            for (const auto& kw : matchedKeywords) {
                const auto& clubsForKw = keywordMap.at(kw);
                if (std::find(clubsForKw.begin(), clubsForKw.end(), club) != clubsForKw.end()) {
                    keywordMatch = true;
                    break;
                }
            }
            if (!keywordMatch) continue;
        }

        // (2) 요일 체크
        if (!selectedDay.empty()) {
            if (std::find(info.days.begin(), info.days.end(), selectedDay) == info.days.end()) {
                continue;
            }
        }

        // (3) 시간대 체크
        if (!selectedTime.empty()) {
            if (std::find(info.times.begin(), info.times.end(), selectedTime) == info.times.end()) {
                continue;
            }
        }

        // (4) 회비 체크
        if (wantFree && !info.isFree) continue;
        if (wantPaid && info.isFree) continue;

        // (5) 형태 체크
        if (wantOffline && info.location != "오프라인") continue;
        if (wantOnline && info.location != "온라인") continue;

        // (6) MBTI 태그 체크
        if (mbtiMentionFound) {
            bool mbtiMatched = false;
            for (const auto& tag : info.mbtiTags) {
                if (upper.find(tag) != std::string::npos) {
                    mbtiMatched = true;
                    break;
                }
            }
            if (!mbtiMatched) continue;
        }

        // 최종 통과
        filteredClubs.push_back(club);
    }

    return filteredClubs;
}

// ─── (10) “인기 동아리” 처리 (전체 or 컨텍스트별) ─────────────────────────────────
std::string readContextualPopular(const std::string& context) {
    if (!std::ifstream("recommend_count.txt")) {
        return "아직 추천된 정보가 없습니다.";
    }
    std::map<std::string, int> counts;
    std::ifstream in("recommend_count.txt");
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string clubName;
        int cnt;
        if (iss >> clubName >> cnt) {
            counts[clubName] = cnt;
        }
    }
    in.close();

    std::vector<std::pair<std::string,int>> filtered;
    bool isMBTI = (validMBTIs.count(context) > 0);
    if (isMBTI) {
        // MBTI 태그 기반 필터
        for (const auto& pair : counts) {
            const auto& clubName = pair.first;
            const auto& cnt = pair.second;
            const auto& tags = clubData[clubName].mbtiTags;
            if (std::find(tags.begin(), tags.end(), context) != tags.end()) {
                filtered.emplace_back(clubName, cnt);
            }
        }
    } else {
        // 관심사 기반
        auto it = interestKeywords.find(context);
        if (it != interestKeywords.end()) {
            const auto& clubsForKw = it->second;
            for (const auto& clubName : clubsForKw) {
                if (counts.count(clubName)) {
                    filtered.emplace_back(clubName, counts[clubName]);
                }
            }
        }
        for (const auto& pair : similarWords) {
            if (pair.second == context) {
                auto it2 = interestKeywords.find(pair.second);
                if (it2 != interestKeywords.end()) {
                    for (const auto& clubName : it2->second) {
                        if (counts.count(clubName)) {
                            filtered.emplace_back(clubName, counts[clubName]);
                        }
                    }
                }
                break;
            }
        }
    }

    if (filtered.empty()) {
        return "해당 컨텍스트에 맞춘 인기 동아리 정보가 없습니다.";
    }
    std::sort(filtered.begin(), filtered.end(),
              [](auto& a, auto& b){ return a.second > b.second; });

    std::ostringstream oss;
    oss << "[\"" << context << "\" 관련 인기 동아리 순위]\n";
    int rank = 1;
    for (const auto& entry : filtered) {
        oss << rank++ << "위: " << entry.first << " (" << entry.second << "회 추천됨)\n";
    }
    return oss.str();
}

// ─── (11) 최종 main ─────────────────────────────────────────────
int main(int argc, char* argv[]) {
    loadClubData("club_data.csv");
    loadSimilarWords("similar_words.txt");
    loadRecommendCount("recommend_count.txt");

    // 인자가 없으면 안내문 출력
    if (argc < 2) {
        std::cout << "필터를 입력해주세요.\n";
        return 0;
    }

    // argv[1..n] 을 한 문자열로 합치기
    std::string userInput = argv[1];
    for (int i = 2; i < argc; ++i) {
        userInput += " ";
        userInput += argv[i];
    }

    // “인기”만 입력된 경우
    if (argc == 2 && (userInput == "인기" || userInput == "인기 동아리" || userInput == "인기순위")) {
        // 전체 인기
        std::map<std::string,int> counts;
        std::ifstream in("recommend_count.txt");
        if (!in.is_open()) {
            std::cout << "아직 추천된 정보가 없습니다.\n";
            return 0;
        }
        std::string line;
        while (std::getline(in, line)) {
            std::istringstream iss(line);
            std::string clubName;
            int cnt;
            if (iss >> clubName >> cnt) {
                counts[clubName] = cnt;
            }
        }
        if (counts.empty()) {
            std::cout << "아직 인기 동아리 정보가 없습니다.\n";
            return 0;
        }
        std::vector<std::pair<std::string,int>> sorted;
        for (auto& p : counts) sorted.push_back(p);
        std::sort(sorted.begin(), sorted.end(), [](auto& a, auto& b){ return a.second > b.second; });
        std::cout << "[전체 인기 동아리 순위]\n";
        int rank = 1;
        for (auto& entry : sorted) {
            std::cout << rank++ << "위: " << entry.first << " (" << entry.second << "회 추천됨)\n";
        }
        return 0;
    }

    // “<컨텍스트> 인기” 형태 (예: “운동 인기”, “INFP 인기”)
    // 첫 번째 토큰을 context로 보고, 뒤에 “인기”만 있으면 해당 context 인기 출력
    if (argc == 3 && (std::string(argv[2]) == "인기" || std::string(argv[2]) == "인기동아리" || std::string(argv[2]) == "인기순위")) {
        std::string context = argv[1];
        std::cout << readContextualPopular(context);
        return 0;
    }

    // 그 외: 다중 필터링 (MBTI or 관심사 → 추가 필터 순서)
    // 만약 인자가 하나만 왔는데 “INFP”나 “운동” 같은 형태라면 → filterClubs 결과 출력
    // 만약 인자가 2개 이상이라면 “이전 누적” vs “새 누적” 비교
    std::vector<std::string> allClubs, offlineClubs, onlineClubs;
    std::vector<std::string> morningClubs, afternoonClubs, eveningClubs;
    std::map<std::string, std::vector<std::string>> dayClubs;
    std::map<std::string, std::vector<std::string>> keywordMap;
    for (const auto& pair : interestKeywords) {
        keywordMap[pair.first] = pair.second;
    }
    for (const auto& pair : clubData) {
        const auto& info = pair.second;
        allClubs.push_back(info.name);
        for (const auto& d : info.days) dayClubs[d].push_back(info.name);
        for (const auto& t : info.times) {
            if (t == "오전") morningClubs.push_back(info.name);
            else if (t == "오후") afternoonClubs.push_back(info.name);
            else if (t == "저녁") eveningClubs.push_back(info.name);
        }
        if (info.location == "오프라인") offlineClubs.push_back(info.name);
        else onlineClubs.push_back(info.name);
    }

    // argv[1..n]을 토큰별로 분리한 벡터
    std::vector<std::string> tokens;
    for (int i = 1; i < argc; ++i) {
        tokens.push_back(argv[i]);
    }

    // === (1) 인자가 하나만 왔을 때 ===
    if (tokens.size() == 1) {
        // interestKeywords나 MBTI 태그 중 하나일 수 있다
        std::string single = tokens[0];
        std::vector<std::string> result = filterClubs(
            single,
            keywordMap, similarWords,
            dayClubs, allClubs,
            offlineClubs, onlineClubs,
            morningClubs, afternoonClubs, eveningClubs
        );
        if (result.empty()) {
            std::cout << "'" << single << "'에 해당되는 동아리가 없습니다. 다시 입력해주세요.\n";
            return 0;
        }
        // 결과 출력 + 카운트 저장
        std::cout << "\n[종합 추천 결과]\n";
        for (const auto& club : result) {
            std::cout << "- " << club << "\n";
            clubRecommendCount[club]++;
        }
        std::cout << "\n";
        saveRecommendCount("recommend_count.txt");
        return 0;
    }

    // === (2) 인자가 2개 이상 (누적 필터) ===
    // (A) prevInput = argv[1..n-1]
    std::string prevInput = tokens[0];
    for (size_t i = 1; i < tokens.size()-1; ++i) {
        prevInput += " " + tokens[i];
    }
    // (B) newInput = argv[1..n]
    std::string newInput = prevInput + " " + tokens.back();
    std::string lastFilter = tokens.back();

    // (C) prevFilter 결과, newFilter 결과
    std::vector<std::string> prevResult = filterClubs(
        prevInput,
        keywordMap, similarWords,
        dayClubs, allClubs,
        offlineClubs, onlineClubs,
        morningClubs, afternoonClubs, eveningClubs
    );
    std::vector<std::string> newResult = filterClubs(
        newInput,
        keywordMap, similarWords,
        dayClubs, allClubs,
        offlineClubs, onlineClubs,
        morningClubs, afternoonClubs, eveningClubs
    );

    // (D) “이전에는 후보가 있었는데, 새 필터를 추가하니 후보가 0개가 된 경우” → 오류 메시지
    if (!prevResult.empty() && newResult.empty()) {
        std::cout << "'" << lastFilter << "'에 해당되는 동아리가 없습니다. 다시 입력해주세요.\n";
        return 0;
    }

    // (E) 그 외: newResult 결과를 출력 + 카운트 저장
    if (newResult.empty()) {
        // (이전결과도 비어있었다면) → “모든 후보가 없을 때”
        std::cout << "조건에 맞는 동아리가 없습니다. 다시 입력해주세요.\n";
        return 0;
    }

    std::cout << "\n[종합 추천 결과]\n";
    for (const auto& club : newResult) {
        std::cout << "- " << club << "\n";
        clubRecommendCount[club]++;
    }
    std::cout << "\n";
    saveRecommendCount("recommend_count.txt");
    return 0;
}

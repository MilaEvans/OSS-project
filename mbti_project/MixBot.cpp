#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
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
    std::vector<std::string> mbtiTags; // 예: {"INFP", "ENFP"} 등
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
    if (!file.is_open()) return;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string name; int cnt;
        if (iss >> name >> cnt) {
            clubRecommendCount[name] = cnt;
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
    for (auto& kv : clubRecommendCount) {
        file << kv.first << " " << kv.second << "\n";
    }
    file.close();
}

// ─── (5) MBTI 데이터와 Interest 키워드 ───────────────────────────────────────────
static const std::set<std::string> validMBTIs = {
    "INTJ","INTP","ENTJ","ENTP",
    "INFJ","INFP","ENFJ","ENFP",
    "ISTJ","ISFJ","ESTJ","ESFJ",
    "ISTP","ISFP","ESTP","ESFP"
};
static const std::map<std::string, std::vector<std::string>> interestKeywords = {
    { "운동", {"땀흘리기","운동하자","뛰뛰빵빵","런닝메이트","주말농구"} },
    { "예술", {"그림그리기","낙서모임","아트타임","색칠놀이","손그림모임"} },
    { "음악", {"음악좋아","노래방친구들","멜로디모임","악기연주회","소리모아"} },
    { "봉사", {"도움의손길","함께나누기","따뜻한마음","봉사모임","기쁨나누기"} },
    { "토론", {"토론놀이","연극모임","즉흥연기"} },
    { "IT", {"코딩하는친구들","밤샘코딩","코드수다방","오픈소스동네","디버깅모임"} },
    { "창업", {"함께성장","작은도전","꿈꾸는우리","스킬업","자기관리"} },
    { "문학", {"책읽는시간","책모임","한줄토크","읽고싶은책","독서친구"} },
    { "댄스", {"춤추는우리","리듬따라","뽐내기댄스","비트속으로","즐거운댄스"} },
    { "게임", {"게임하러모여","보드게임존","게임친구들","온라인모험단","게임톡"} },
    { "여행", {"여행가자","떠나볼까","길따라걷기","캠핑가족","바람따라"} },
    { "사진", {"찰칵찰칱","사진찍기좋아","풍경사진","우리사진관","사진산책"} },
    { "영상", {"영상찍자","영상편집모임","필름러버","우리영상","유튜브친구들"} },
    { "미술", {"그림그리기","낙서모임","아트타임","색칠놀이","손그림모임"} },
    { "연극", {"연극놀이","무대연습","연기하는우리","즉흥연기","연극모임"} },
    { "요리", {"요리해요","간식만들기","베이킹클럽","맛있는시간","요리친구들"} }
};

// ─── (6) 필터링 함수: “rawInput” 문자열을 받아, CSV+유사어+키워드 기반으로 동아리 목록 반환 ─────
static const std::vector<std::string> allDays = {
    "월요일","화요일","수요일","목요일","금요일","토요일","일요일"
};
static const std::vector<std::string> allTimes = { "오전","오후","저녁" };

std::vector<std::string> filterClubs(
        const std::string& rawInput,
        const std::map<std::string, std::vector<std::string>>& keywordMap,
        const std::map<std::string, std::string>& similarWords,
        const std::vector<std::string>& allClubs)
{
    // 1) 유사어 대체 & 소문자화
    std::string inputLower = rawInput;
    std::transform(inputLower.begin(), inputLower.end(), inputLower.begin(), ::tolower);
    for (auto& kv : similarWords) {
        const std::string& syn = kv.first;
        const std::string& kw  = kv.second;
        if (inputLower.find(syn) != std::string::npos) {
            inputLower = std::regex_replace(inputLower, std::regex(syn), kw);
        }
    }

    // 2) 필터 변수 초기화
    bool wantFree = false, wantPaid = false;
    bool wantOffline = false, wantOnline = false;
    std::string selectedDay = "";
    std::string selectedTime = "";
    std::vector<std::string> matchedKeywords; 
    std::string upperRaw = rawInput;
    std::transform(upperRaw.begin(), upperRaw.end(), upperRaw.begin(), ::toupper);

    // 3) MBTI 태그 검사
    bool mbtiMentionFound = false;
    for (auto& m : validMBTIs) {
        if (upperRaw.find(m) != std::string::npos) {
            mbtiMentionFound = true;
            break;
        }
    }

    // 4) 요일 검사
    for (auto& d : allDays) {
        std::string d_lower = d;
        std::transform(d_lower.begin(), d_lower.end(), d_lower.begin(), ::tolower);
        if (inputLower.find(d_lower) != std::string::npos) {
            selectedDay = d;
            break;
        }
    }

    // 5) 시간대 검사
    if (inputLower.find("오전") != std::string::npos || inputLower.find("아침") != std::string::npos) {
        selectedTime = "오전";
    }
    else if (inputLower.find("오후") != std::string::npos || inputLower.find("점심") != std::string::npos) {
        selectedTime = "오후";
    }
    else if (inputLower.find("저녁") != std::string::npos || inputLower.find("밤") != std::string::npos) {
        selectedTime = "저녁";
    }

    // 6) 회비 검사
    if (inputLower.find("무료") != std::string::npos ||
        inputLower.find("저렴") != std::string::npos ||
        inputLower.find("싸다") != std::string::npos ||
        inputLower.find("비용 없다") != std::string::npos ||
        inputLower.find("공짜") != std::string::npos)
    {
        wantFree = true;
    }
    if (inputLower.find("유료") != std::string::npos ||
        inputLower.find("비싸다") != std::string::npos ||
        inputLower.find("비용 있다") != std::string::npos ||
        inputLower.find("고급") != std::string::npos)
    {
        wantPaid = true;
    }

    // 7) 형태(온라인/오프라인) 검사
    if (inputLower.find("오프라인") != std::string::npos) {
        wantOffline = true;
    }
    if (inputLower.find("온라인") != std::string::npos) {
        wantOnline = true;
    }

    // 8) 관심 키워드 매칭
    for (auto& kv : keywordMap) {
        const std::string& key = kv.first;           // 예: "운동", "음악" 등
        std::string key_lower = key;
        std::transform(key_lower.begin(), key_lower.end(), key_lower.begin(), ::tolower);
        if (inputLower.find(key_lower) != std::string::npos) {
            matchedKeywords.push_back(key);
        }
    }

    // 9) 실제 필터링 수행
    std::vector<std::string> filtered;
    for (auto& clubName : allClubs) {
        const auto& info = clubData.at(clubName);

        // (1) 관심 키워드 매칭: matchedKeywords 비어 있지 않으면, 반드시 그 그룹에 속해야 함
        if (!matchedKeywords.empty()) {
            bool ok = false;
            for (auto& key : matchedKeywords) {
                auto& group = keywordMap.at(key);
                if (std::find(group.begin(), group.end(), clubName) != group.end()) {
                    ok = true;
                    break;
                }
            }
            if (!ok) continue;
        }

        // (2) 요일 검사
        if (!selectedDay.empty()) {
            if (std::find(info.days.begin(), info.days.end(), selectedDay) == info.days.end()) {
                continue;
            }
        }

        // (3) 시간대 검사
        if (!selectedTime.empty()) {
            if (std::find(info.times.begin(), info.times.end(), selectedTime) == info.times.end()) {
                continue;
            }
        }

        // (4) 회비 검사
        if (wantFree && !info.isFree) continue;
        if (wantPaid && info.isFree) continue;

        // (5) 형태 검사
        if (wantOffline && info.location != "오프라인") continue;
        if (wantOnline && info.location != "온라인") continue;

        // (6) MBTI 태그 검사
        if (mbtiMentionFound) {
            bool ok = false;
            for (auto& tag : info.mbtiTags) {
                if (upperRaw.find(tag) != std::string::npos) {
                    ok = true;
                    break;
                }
            }
            if (!ok) continue;
        }

        filtered.push_back(clubName);
    }

    return filtered;
}

// ─── (7) “인기 동아리” 처리 (전체 or 컨텍스트별) ─────────────────────────────────
std::string readContextualPopular(const std::string& context) {
    std::ifstream check("recommend_count.txt");
    if (!check.is_open()) {
        return "아직 추천된 정보가 없습니다.";
    }
    check.close();

    std::map<std::string,int> counts;
    std::ifstream in("recommend_count.txt");
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string clubName; int cnt;
        if (iss >> clubName >> cnt) {
            counts[clubName] = cnt;
        }
    }
    in.close();

    std::vector<std::pair<std::string,int>> filtered;
    bool isMBTI = (validMBTIs.count(context) > 0);
    if (isMBTI) {
        for (auto& kv : counts) {
            auto& clubName = kv.first;
            int cnt = kv.second;
            auto& tags = clubData[clubName].mbtiTags;
            if (std::find(tags.begin(), tags.end(), context) != tags.end()) {
                filtered.emplace_back(clubName, cnt);
            }
        }
    } 
    else {
        // 관심사 컨텍스트
        auto it = interestKeywords.find(context);
        if (it != interestKeywords.end()) {
            for (auto& cn : it->second) {
                if (counts.count(cn)) {
                    filtered.emplace_back(cn, counts[cn]);
                }
            }
        }
        // + 유사어 맵핑
        for (auto& kv : similarWords) {
            if (kv.second == context) {
                auto it2 = interestKeywords.find(kv.second);
                if (it2 != interestKeywords.end()) {
                    for (auto& cn : it2->second) {
                        if (counts.count(cn)) {
                            filtered.emplace_back(cn, counts[cn]);
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
    for (auto& entry : filtered) {
        oss << rank++ << "위: " << entry.first << " (" << entry.second << "회 추천됨)\n";
    }
    return oss.str();
}

// ─── (8) 최종 main ─────────────────────────────────────────────
int main(int argc, char* argv[]) {
    loadClubData("club_data.csv");
    loadSimilarWords("similar_words.txt");
    loadRecommendCount("recommend_count.txt");

    if (argc < 2) {
        std::cout << "필터를 입력해주세요.\n";
        return 0;
    }

    // argv[1..]을 tokens 벡터에 복사
    std::vector<std::string> tokens;
    for (int i = 1; i < argc; ++i) {
        tokens.push_back(argv[i]);
    }

    // “인기”만 입력된 경우
    if (tokens.size() == 1 &&
        (tokens[0] == "인기" || tokens[0] == "인기 동아리" || tokens[0] == "인기순위"))
    {
        // 전체 인기 순위
        std::map<std::string,int> counts;
        std::ifstream in("recommend_count.txt");
        if (!in.is_open()) {
            std::cout << "아직 추천된 정보가 없습니다.\n";
            return 0;
        }
        std::string line;
        while (std::getline(in, line)) {
            std::istringstream iss(line);
            std::string clubName; int cnt;
            if (iss >> clubName >> cnt) {
                counts[clubName] = cnt;
            }
        }
        if (counts.empty()) {
            std::cout << "아직 인기 동아리 정보가 없습니다.\n";
            return 0;
        }
        std::vector<std::pair<std::string,int>> sorted;
        for (auto& kv : counts) sorted.emplace_back(kv.first, kv.second);
        std::sort(sorted.begin(), sorted.end(),
                  [](auto& a, auto& b){ return a.second > b.second; });
        std::cout << "[전체 인기 동아리 순위]\n";
        int rank = 1;
        for (auto& entry : sorted) {
            std::cout << rank++ << "위: " << entry.first << " (" << entry.second << "회 추천됨)\n";
        }
        return 0;
    }

    // “<컨텍스트> 인기” 형태
    if (tokens.size() == 2 &&
        (tokens[1] == "인기" || tokens[1] == "인기동아리" || tokens[1] == "인기순위"))
    {
        std::string context = tokens[0];
        std::cout << readContextualPopular(context);
        return 0;
    }

    // ─── 단계별 누적 필터링 ─────────────────────────────────────────────
    // (1) tokens.size()==1: 첫 필터 입력만 있을 때
    if (tokens.size() == 1) {
        std::string only = tokens[0];
        // “MBTI”이거나 “관심사”이든 상관없이, filterClubs로 걸러줌
        std::vector<std::string> allClubs;
        for (auto& kv : clubData) allClubs.push_back(kv.first);

        std::vector<std::string> result = filterClubs(
            only,
            interestKeywords, similarWords,
            allClubs
        );
        if (result.empty()) {
            std::cout << "'" << only << "'에 해당되는 동아리가 없습니다. 다시 입력해주세요.\n";
            return 0;
        }
        std::cout << "\n[단계 1: '" << only << "' 기반 추천]\n";
        for (auto& cname : result) {
            std::cout << "- " << cname << "\n";
            clubRecommendCount[cname]++;
        }
        std::cout << "\n";
        saveRecommendCount("recommend_count.txt");
        return 0;
    }

    // (2) tokens.size() >= 2: “prevInput” = tokens[0..n-2], “lastFilter” = tokens[n-1]
    std::string prevInput = tokens[0];
    for (size_t i = 1; i < tokens.size() - 1; ++i) {
        prevInput += " " + tokens[i];
    }
    std::string lastFilter = tokens.back();
    std::string newInput = prevInput + " " + lastFilter;

    // 모든 동아리 목록 준비
    std::vector<std::string> allClubs;
    for (auto& kv : clubData) allClubs.push_back(kv.first);

    // (A) prevResult = filterClubs(prevInput)
    std::vector<std::string> prevResult =
        filterClubs(prevInput, interestKeywords, similarWords, allClubs);

    // (B) newResult = filterClubs(newInput)
    std::vector<std::string> newResult =
        filterClubs(newInput, interestKeywords, similarWords, allClubs);

    // (C) “이전 결과(prevResult)는 있었는데, 새 필터(newResult)는 0개인 경우”
    if (!prevResult.empty() && newResult.empty()) {
        std::cout << "'" << lastFilter << "'에 해당되는 동아리가 없습니다. 다시 입력해주세요.\n";
        return 0;
    }

    // (D) “newResult도 비어 있고 prevResult도 비어 있었던 경우”
    if (newResult.empty()) {
        std::cout << "조건에 맞는 동아리가 없습니다. 다시 입력해주세요.\n";
        return 0;
    }

    // (E) 정상적으로 좁혀진 결과가 있는 경우
    std::cout << "\n[종합 추천 결과: '" << newInput << "']\n";
    for (auto& cname : newResult) {
        std::cout << "- " << cname << "\n";
        clubRecommendCount[cname]++;
    }
    std::cout << "\n";
    saveRecommendCount("recommend_count.txt");
    return 0;
}

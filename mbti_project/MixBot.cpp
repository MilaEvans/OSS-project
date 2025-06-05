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
#include <functional>

// ─── 동아리 확장 정보 구조체 정의 ─────────────────────────────────────────
struct ExtendedClubInfo {
    std::string name;
    bool isFree;
    std::vector<std::string> days;
    std::vector<std::string> times;
    std::vector<std::string> mbtiTags;
    std::string location;
};

// ─── CSV 파일에서 동아리 정보 로드 ─────────────────────────────────────────
static std::map<std::string, ExtendedClubInfo> clubData;

void loadClubData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string token;
        ExtendedClubInfo info;
        std::getline(ss, token, ',');
        info.name = token;
        std::getline(ss, token, ',');
        info.isFree = (token == "1");
        std::getline(ss, token, ',');
        if (!token.empty()) {
            std::stringstream ds(token);
            std::string d;
            while (std::getline(ds, d, ';')) info.days.push_back(d);
        }
        std::getline(ss, token, ',');
        if (!token.empty()) {
            std::stringstream ts(token);
            std::string t;
            while (std::getline(ts, t, ';')) info.times.push_back(t);
        }
        std::getline(ss, token, ',');
        info.location = token;
        std::getline(ss, token);
        if (!token.empty()) {
            std::stringstream ms(token);
            std::string m;
            while (std::getline(ms, m, ';')) info.mbtiTags.push_back(m);
        }
        clubData[info.name] = info;
    }
    file.close();
}

// ─── 유사어 로드 ───────────────────────────────────────────────────────────
static std::map<std::string, std::string> similarWords;

void loadSimilarWords(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string synonym, keyword;
        if (iss >> synonym >> keyword) similarWords[synonym] = keyword;
    }
    file.close();
}

// ─── 추천 횟수 로드/저장 ───────────────────────────────────────────────────
static std::map<std::string, int> clubRecommendCount;

void loadRecommendCount(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string clubName;
        int cnt;
        if (iss >> clubName >> cnt) clubRecommendCount[clubName] = cnt;
    }
    file.close();
}

void saveRecommendCount(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return;
    for (const auto& pair : clubRecommendCount) {
        file << pair.first << " " << pair.second << "\n";
    }
    file.close();
}

// ─── 상수 정의 ────────────────────────────────────────────────────────────
static const std::vector<std::string> timeSlots = { "오전", "오후", "저녁" };
static const std::vector<std::string> daysOfWeek = {
    "월요일","화요일","수요일","목요일","금요일","토요일","일요일"
};
static const std::set<std::string> validMBTIs = {
    "INTJ","INTP","ENTJ","ENTP",
    "INFJ","INFP","ENFJ","ENFP",
    "ISTJ","ISFJ","ESTJ","ESFJ",
    "ISTP","ISFP","ESTP","ESFP"
};

// ─── 헬퍼 함수: 소문자로 변환 ────────────────────────────────────────────────
std::string toLower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out;
}

// ─── 교집합 필터링 함수 ───────────────────────────────────────────────────
void recommendClubs(const std::vector<std::string>& tokens, bool countMode) {
    const std::string& mbti = tokens[0];
    bool wantFree = false, wantPaid = false;
    bool wantOnline = false, wantOffline = false;
    std::string selectedDay = "", selectedTime = "";

    for (size_t i = 1; i < tokens.size(); ++i) {
        std::string kw = tokens[i], lower = toLower(kw);
        for (auto& p : similarWords) {
            if (lower.find(p.first) != std::string::npos) lower = p.second;
        }
        if (lower == "무료" || lower == "저렴" || lower == "싸다" || lower == "공짜") { wantFree = true; continue; }
        if (lower == "유료" || lower == "비싸다" || lower == "비용 있다" || lower == "고급") { wantPaid = true; continue; }
        if (lower == "온라인") { wantOnline = true; continue; }
        if (lower == "오프라인") { wantOffline = true; continue; }
        for (auto& d : daysOfWeek) {
            if (toLower(d) == lower) { selectedDay = d; break; }
        }
        for (auto& t : timeSlots) {
            if (toLower(t) == lower) { selectedTime = t; break; }
        }
    }

    std::vector<std::string> filteredClubs;
    for (const auto& pair : clubData) {
        const auto& info = pair.second;
        bool mbtiMatched = false;
        for (auto& tag : info.mbtiTags) {
            if (tag == mbti) { mbtiMatched = true; break; }
        }
        if (!mbtiMatched) continue;
        if (wantFree && !info.isFree) continue;
        if (wantPaid && info.isFree) continue;
        if (wantOnline && info.location != "온라인") continue;
        if (wantOffline && info.location != "오프라인") continue;
        if (!selectedDay.empty()) {
            bool dayMatch = false;
            for (auto& d : info.days) {
                if (d == selectedDay) { dayMatch = true; break; }
            }
            if (!dayMatch) continue;
        }
        if (!selectedTime.empty()) {
            bool timeMatch = false;
            for (auto& t : info.times) {
                if (t == selectedTime) { timeMatch = true; break; }
            }
            if (!timeMatch) continue;
        }
        filteredClubs.push_back(info.name);
    }

    if (filteredClubs.empty()) {
        std::cout << "해당되는 동아리가 없습니다.\n";
        return;
    }

    std::cout << "\n[종합 추천 결과]\n";
    for (auto& club : filteredClubs) {
        std::cout << "- " << club << "\n";
        if (countMode) {
            clubRecommendCount[club]++;
        }
    }
    std::cout << "\n";
    if (countMode) saveRecommendCount("recommend_count.txt");
}

// ─── 전체 인기 동아리 순위 출력 (Top 5) ─────────────────────────────────────────
void printOverallPopular() {
    if (clubRecommendCount.empty()) {
        std::cout << "아직 추천된 정보가 없습니다.\n";
        return;
    }
    std::vector<std::pair<std::string,int>> sorted;
    for (auto& p : clubRecommendCount) {
        sorted.push_back(p);
    }
    std::sort(sorted.begin(), sorted.end(),
              [](auto& a, auto& b){ return a.second > b.second; });

    std::cout << "[전체 인기 동아리 순위]\n";
    int rank = 1;
    for (auto& entry : sorted) {
        if (rank > 5) break;
        std::cout << rank << "위: " << entry.first << " (" << entry.second << "회 추천됨)\n";
        ++rank;
    }
}

// ─── 특정 컨텍스트별 인기 동아리 출력 (Top 5) ───────────────────────────────────
void printContextPopular(const std::string& context) {
    if (clubRecommendCount.empty()) {
        std::cout << "아직 추천된 정보가 없습니다.\n";
        return;
    }
    std::map<std::string,int> counts;
    for (auto& p : clubRecommendCount) {
        counts[p.first] = p.second;
    }

    std::vector<std::pair<std::string,int>> filtered;
    bool isMBTI = (validMBTIs.count(context) > 0);
    if (isMBTI) {
        for (auto& p : counts) {
            auto& tags = clubData[p.first].mbtiTags;
            if (std::find(tags.begin(), tags.end(), context) != tags.end()) {
                filtered.emplace_back(p.first, p.second);
            }
        }
    } else {
        std::string lower = toLower(context);
        bool wantFree = (lower == "무료");
        bool wantPaid = (lower == "유료");
        bool isDay = false, isTime = false;
        std::string targetDay = "", targetTime = "";
        for (auto& d : daysOfWeek) {
            if (toLower(d) == lower) {
                isDay = true;
                targetDay = d;
                break;
            }
        }
        if (!isDay) {
            for (auto& t : timeSlots) {
                if (toLower(t) == lower) {
                    isTime = true;
                    targetTime = t;
                    break;
                }
            }
        }
        bool isOnline = (lower == "온라인");
        bool isOffline = (lower == "오프라인");

        for (auto& p : counts) {
            auto& info = clubData[p.first];
            if (wantFree && !info.isFree) continue;
            if (wantPaid && info.isFree) continue;
            if (isOnline && info.location != "온라인") continue;
            if (isOffline && info.location != "오프라인") continue;
            if (isDay) {
                if (std::find(info.days.begin(), info.days.end(), targetDay) == info.days.end())
                    continue;
            }
            if (isTime) {
                if (std::find(info.times.begin(), info.times.end(), targetTime) == info.times.end())
                    continue;
            }
            filtered.emplace_back(p.first, p.second);
        }
    }

    if (filtered.empty()) {
        std::cout << "해당 컨텍스트에 맞춘 인기 동아리 정보가 없습니다.\n";
        return;
    }
    std::sort(filtered.begin(), filtered.end(),
              [](auto& a, auto& b){ return a.second > b.second; });

    std::cout << "[\"" << context << "\" 관련 인기 동아리 순위]\n";
    int rank = 1;
    for (auto& entry : filtered) {
        if (rank > 5) break;
        std::cout << rank << "위: " << entry.first << " (" << entry.second << "회 추천됨)\n";
        ++rank;
    }
}

// ─── main 함수 ────────────────────────────────────────────────────────────
int main(int argc, char* argv[]) {
    loadClubData("club_data.csv");
    loadSimilarWords("similar_words.txt");
    loadRecommendCount("recommend_count.txt");

    if (argc < 2) {
        std::cout << "MBTI 입력이 필요합니다.\n";
        return 1;
    }

    std::string firstArg = argv[1];
    if (firstArg == "인기" || firstArg == "인기동아리" || firstArg == "인기순위") {
        if (argc == 2) {
            printOverallPopular();
            return 0;
        }
        if (argc == 3) {
            printContextPopular(argv[1]);
            return 0;
        }
    }

    // 토큰 목록 추출
    std::vector<std::string> tokens;
    bool countMode = false;
    for (int i = 1; i < argc; ++i) {
        std::string t = argv[i];
        if (t == "__count__") {
            countMode = true;
            continue;
        }
        tokens.push_back(t);
    }

    // 첫 번째 토큰이 MBTI인지 검증
    if (tokens.empty() || validMBTIs.count(tokens[0]) == 0) {
        std::cout << "잘못된 MBTI입니다.\n";
        return 1;
    }

    recommendClubs(tokens, countMode);
    return 0;
}


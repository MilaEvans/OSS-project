#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

std::string clean(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (!isspace(c)) {
            result += std::tolower(c);
        }
    }
    return result;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "No input text file provided.\n";
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream infile(filename);
    if (!infile) {
        std::cout << "Failed to open input file.\n";
        return 1;
    }

    std::string input((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    infile.close();

    input = clean(input);

    struct Club {
        std::vector<std::string> names;
        std::string description;
        std::vector<std::string> keywords;
    };

    std::vector<Club> clubs = {
        {{"코딩하는친구들", "밤샘코딩", "코드수다방", "오픈소스동네", "디버깅모임"}, "코딩 관련 동아리", {"코딩", "프로그래밍", "html", "css", "javascript", "python", "웹"}},
        {{"땀흘리기", "운동하자", "뛰뛰빵빵", "런닝메이트", "주말농구"}, "운동 관련 동아리", {"운동"}},
        {{"춤추는우리", "리듬따라", "뽐내기댄스", "비트속으로", "즐거운댄스"}, "댄스 관련 동아리", {"댄스"}},
        {{"게임하러모여", "보드게임존", "게임친구들", "온라인모험단", "게임톡"}, "게임 관련 동아리", {"게임"}},
        {{"여행가자", "떠나볼까", "길따라걷기", "캠핑가족", "바람따라"}, "여행 관련 동아리", {"여행"}},
        {{"음악좋아", "노래방친구들", "멜로디모임", "악기연주회", "소리모아"}, "음악 관련 동아리", {"음악"}},
        {{"함께성장", "작은도전", "꿈꾸는우리", "스킬업", "자기관리"}, "자기개발 관련 동아리", {"자기개발"}},
        {{"책읽는시간", "책모임", "한줄토크", "읽고싶은책", "독서친구"}, "독서 관련 동아리", {"독서"}},
        {{"도움의손길", "함께나누기", "따뜻한마음", "봉사모임", "기쁨나누기"}, "봉사 관련 동아리", {"봉사"}},
        {{"찰칵찰칵", "사진찍기좋아", "풍경사진", "우리사진관", "사진산책"}, "사진 관련 동아리", {"사진"}},
        {{"영상찍자", "영상편집모임", "필름러버", "우리영상", "유튜브친구들"}, "영상 관련 동아리", {"영상"}},
        {{"그림그리기", "낙서모임", "아트타임", "색칠놀이", "손그림모임"}, "미술 관련 동아리", {"미술"}},
        {{"연극놀이", "무대연습", "연기하는우리", "즉흥연기", "연극모임"}, "연극 관련 동아리", {"연극"}},
        {{"요리해요", "간식만들기", "베이킹클럽", "맛있는시간", "요리친구들"}, "요리 관련 동아리", {"요리"}}
    };

    std::vector<std::string> matches;

    for (const auto& club : clubs) {
        for (const auto& kw : club.keywords) {
            if (input.find(clean(kw)) != std::string::npos) {
                for (const auto& name : club.names) {
                    matches.push_back(name + ": " + club.description); // 모든 이름 출력
                }
                break; // 중복 방지를 위해 다음 클럽으로
            }
        }
    }

    if (matches.empty()) {
        std::cout << "No matching clubs found.\n";
    }
    else {
        for (const auto& m : matches) {
            std::cout << m << "\n";
        }
    }

    return 0;
}

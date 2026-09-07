#include<iostream>
#include<fstream>
#include<string>
#include<Windows.h>

using namespace std;

/*
2. 파일에서 문자열 읽기파일에서 문자열 읽기

	문자와 숫자로 구성된 문장을 10줄 저장한 파일을 만든다. (파일 이름 예: data.txt)

	데이터 파일 이름을 입력 받고, 파일에서 문자열을 읽고 저장한다.
	- 1개 이상의 공백이 있는 경우는 공백을 1개로 취급한다.
	? 알파벳이나 숫자 외의 특수문자들은 (예, / , - * 같은 문자)
	  구분하지 않고 연결된 값으로 카운트 한다.

	읽은 문자열을 출력하고, 공백을 기준으로 단어를 구분한다.

	다음의 명령어를 수행한다.

	- a: 모든 문장의 문자들을 대소문자를 바꿔 출력한다. -> 다시 누르면 원래대로 출력한다.
	- b: 각 줄의 단어의 개수를 출력한다. 문장들을 모두 쓰고 각 문장의 맨 뒤에 해당 문장의 단어의 개수를 출력한다.
	- c: 대문자로 시작되는 단어를 찾아 그 단어를 다른 색으로 출력하고, 몇 개 있는지를 계산하여 출력한다. -> 다시 누르면 원래대로 출력한다.
	- d: 각 문장 별로 거꾸로 출력하기. -> 다시 누르면 원래대로 출력한다.
	- e: 모든 공백에 "*" 문자 삽입하기. -> 다시 누르면 원래대로 출력한다.
	- f: 공백을 기준으로 (*가 삽입되어 있다면 *를 공백으로 취급) 모든 단어들을 거꾸로 출력하기. -> 다시 누르면 원래대로 출력한다.
	- g: 문자 내부의 특정 문자를 다른 문자로 바꾸기 (바꿀 문자와 새롭게 입력할 문자 입력 받음) -> 다시 누르면 원래대로 출력한다.
	- h: 문장에 있는 숫자를 찾아 숫자 뒤에 오는 문장을 다음 줄로 넘긴다. -> 다시 누르면 원래대로 출력한다.
	- i: 명령어와 단어를 입력하면, 문장들을 모두 출력하면서 입력 받은 단어를 찾아 다른 색으로 출력하고, 몇 개가 있는지를 계산하여 출력한다.
	  (대소문자 구분하지 않는다)
	- j: 문장의 순서를 바꿔서 출력한다.
	  즉, 1번 문장 -> 2번 문장, 2번 문장 -> 3번 문장, ... , 9번 문장 -> 1번 문장
	- q: 프로그램 종료

*/

void commands(char command, string original[], int cnt[], int lineCount);
void commandI(string temp[], int lineCount, bool isOnE);

//---------------------------------------------------------------------------------------------------
int main()
//---------------------------------------------------------------------------------------------------
{
	string fileName{};
	std::cout << "데이터 파일 이름을 입력하세요 : ";
	std::cin >> fileName;

	ifstream readFile{ fileName };

	if (not readFile) {
		std::cout << "파일을 열 수 없습니다." << endl;
		return 0;
	}

	// 파일에서 읽은 문자열을 저장할 변수
	string line{};

	// 원본 문자열을 저장할 배열
	string orignal[10]{};

	// 현재 읽은 줄의 인덱스를 저장할 변수
	int Curline{};

	// 각 줄의 단어 개수를 저장할 배열
	int cnt[10]{};

	char command{};

	while (Curline < 10 && getline(readFile, line)) {

		orignal[Curline] = line;

		bool word = false;

		for (int i = 0; i < line.length(); ++i) {
			if (line[i] == ' ') {
				word = false;
			}
			else {
				if (!word) {
					cnt[Curline]++;
				}

				word = true;
			}
		}

		++Curline;
	}

	if (Curline == 0) {
		std::cout << "읽을 문장이 없습니다." << endl;
		return 0;
	}

	std::cout << "\n파일에서 읽은 문자열입니다.\n";
	for (int i = 0; i < Curline; ++i) {
		std::cout << i + 1 << ". " << orignal[i] << '\n';
	}

	while (true) {
		std::cout << "명령어를 입력하세요 : ( a, b, c, d, e, f, g, h, i, j, q ) : ";
		std::cin >> command;
		std::cout << '\n' << '\n';

		if (command == 'q') {
			std::cout << "프로그램을 종료합니다." << std::endl;
			return 0;
		}
		else {
			commands(command, orignal, cnt, Curline);
			std::cout << '\n';
		}
	}
}

void commands(char command, string orignal[], int cnt[], int lineCount)
{
	static bool isOnA = false;
	static bool isOnC = false;
	static bool isOnD = false;
	static bool isOnE = false;
	static bool isOnF = false;
	static bool isOnG = false;
	static bool isOnH = false;
	 bool isOnJ = false;

	static string temp[10]{};

	static char oldChar = '\0';
	static char newChar = '\0';

	static int jCount = 0;

	for (int i = 0; i < lineCount; ++i) {
		temp[i] = orignal[i];
	}

	if (command == 'j') {
		string result[10]{};

		for (int i = 0; i < lineCount; ++i) {
			result[(i + jCount) % lineCount] = temp[i];
		}

		for (int i = 0; i < lineCount; ++i) {
			temp[i] = result[i];
		}
	}

	if (command == 'a') {
		isOnA = !isOnA;
	}
	else if (command == 'c') {
		isOnC = !isOnC;
	}
	else if (command == 'd') {
		isOnD = !isOnD;
	}
	else if (command == 'e') {
		isOnE = !isOnE;
	}
	else if (command == 'f') {
		isOnF = !isOnF;
	}
	else if (command == 'g') {
		isOnG = !isOnG;

		if (isOnG) {
			std::cout << "바꿀 문자와 새롭게 변경할 문자를 입력해주세요 ( ex : a b ) : ";
			std::cin >> oldChar >> newChar;
		}
		else {
			oldChar = '\0';
			newChar = '\0';
		}
	}
	else if (command == 'h') {
		isOnH = !isOnH;
	}
	else if (command == 'j') {
		++jCount;
		jCount %= lineCount;
	}

	for (int i = 0; i < lineCount; ++i) {
		if (isOnA) {
			for (int j = 0; j < temp[i].length(); ++j) {
				if (temp[i][j] >= 'A' && temp[i][j] <= 'Z') {
					temp[i][j] = temp[i][j] - 'A' + 'a';
				}
				else if (temp[i][j] >= 'a' && temp[i][j] <= 'z') {
					temp[i][j] = temp[i][j] - 'a' + 'A';
				}
			}
		}

		if (isOnD) {
			for (int j = 0; j < temp[i].length() / 2; ++j) {
				char tempChar = temp[i][j];
				temp[i][j] = temp[i][temp[i].length() - 1 - j];
				temp[i][temp[i].length() - 1 - j] = tempChar;
			}
		}

		if (isOnG) {
			for (int j = 0; j < temp[i].length(); ++j) {
				if (temp[i][j] == oldChar) {
					temp[i][j] = newChar;
				}
			}
		}

		if (isOnF) {
			string words[100]{};
			int wordCount = 0;
			string word{};

			for (int j = 0; j < temp[i].length() / 2; ++j) {
				char tempChar = temp[i][j];
				temp[i][j] = temp[i][temp[i].length() - 1 - j];
				temp[i][temp[i].length() - 1 - j] = tempChar;
			}

			for (int j = 0; j < temp[i].length(); ++j) {
				if (temp[i][j] != ' ') {
					word += temp[i][j];
				}
				else {
					if (!word.empty()) {
						words[wordCount] = word;
						++wordCount;
						word.clear();
					}
				}
			}

			if (!word.empty()) {
				words[wordCount] = word;
				++wordCount;
			}

			string result;

			for (int j = wordCount - 1; j >= 0; --j) {
				result += words[j];
				if (j != 0) {
					result += ' ';
				}
			}

			temp[i] = result;
		}

		if (isOnE) {
			for (int j = 0; j < temp[i].length(); ++j) {
				if (temp[i][j] == ' ') {
					temp[i].replace(j, 1, "*");
				}
			}
		}

		if (isOnH) {
			for(int j = 0; j < temp[i].length(); ++j) {
				if (temp[i][j] >= '0' && temp[i][j] <= '9' &&
					(j == temp[i].length() - 1 ||
						!(temp[i][j + 1] >= '0' && temp[i][j + 1] <= '9'))) {
					temp[i].insert(j + 1, "\n");
					++j;
				}
			}
		}
	}

	// 수정: i는 검색어를 입력받아 단어 단위로 비교하고 결과를 출력한다.
	if (command == 'i') {
		commandI(temp, lineCount, isOnE);
		return;
	}

	int largeWordCount[10]{};

	for (int i = 0; i < lineCount; ++i) {
		bool isLargeWord = false;

		for (int j = 0; j < temp[i].length(); ++j) {
			if (j == 0 || temp[i][j - 1] == ' ' || (isOnE && temp[i][j - 1] == '*')) {
				if (temp[i][j] >= 'A' && temp[i][j] <= 'Z') {
					isLargeWord = true;
					largeWordCount[i]++;
				}
			}
			else if (temp[i][j] == ' ' || (isOnE && temp[i][j] == '*')) {
				isLargeWord = false;
			}

			if (isLargeWord && isOnC) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			}
			else {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			}
			std::cout << temp[i][j];
		}

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		if(isOnC) {
			std::cout << " (대문자 단어 수: " << largeWordCount[i] << ")  ";
		}
		if (command == 'b') {
			std::cout << "  ( 단어 수  : " << cnt[i] << ") ";
		}
		std::cout << '\n';
	}
}

void commandI(string temp[], int lineCount, bool isOnE)
{
	string targetWord{};
	int foundCount = 0;

	std::cout << "찾을 단어를 입력하세요 : ";
	std::cin >> targetWord;

	for (int i = 0; i < lineCount; ++i) {
		string word{};
		std::cout << i + 1 << ". ";

		for (int j = 0; j <= temp[i].length(); ++j) {
			bool separator = false;

			if (j == temp[i].length()) {
				separator = true;
			}
			else if (temp[i][j] == ' ') {
				separator = true;
			}
			else if (isOnE && temp[i][j] == '*') {
				separator = true;
			}

			if (!separator) {
				word += temp[i][j];
			}
			else {
				bool sameWord = true;

				if (word.length() != targetWord.length()) {
					sameWord = false;
				}
				else {
					for (int k = 0; k < word.length(); ++k) {
						char wordChar = word[k];
						char targetChar = targetWord[k];

						if (wordChar >= 'A' && wordChar <= 'Z') {
							wordChar = wordChar - 'A' + 'a';
						}
						if (targetChar >= 'A' && targetChar <= 'Z') {
							targetChar = targetChar - 'A' + 'a';
						}
						if (wordChar != targetChar) {
							sameWord = false;
						}
					}
				}

				if (sameWord && !word.empty()) {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
					++foundCount;
				}

				std::cout << word;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

				if (j < temp[i].length()) {
					std::cout << temp[i][j];
				}
				word.clear();
			}
		}
		std::cout << '\n';
	}

	std::cout << "찾은 단어 수 : " << foundCount << '\n';
}

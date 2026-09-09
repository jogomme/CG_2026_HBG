#include<iostream>
#include<Windows.h>
#include<algorithm>

struct Rect
{
	int x1;
	int y1;
	int x2;
	int y2;

	bool on = false;       // 현재 조작할 Rect인가?
	bool isLive = false;   // Rect가 실제로 생성되어 있는가?
};

void DrawBoard(char board[][40], int height, int wide);

void initBoard(char board[][40], Rect* r1, Rect* r2);

void MadeBoard(char board[][40], Rect* r1, Rect* r2, int wide, int height);

void Commands(char command, char board[][40], int* wide, int* height, Rect* r1, Rect* r2);

// flag 1 : x 축 +1
// flag 2 : y축 +1
// flag 3 : x, y 전부 +1
// flag 4 : x +1, y -1
// flag에 - 부호가 붙으면 반대로 행동한다.
void ReBuildRect(Rect* r, int flag, int wide, int height);

void ClearBoard(char board[][40]);

void MoveXRect(Rect* r1, Rect* r2, int flag, int wide);
void MoveYRect(Rect* r1, Rect* r2, int flag, int height);

bool IsInRect(Rect* r, int x, int y);

void limitedXRect(Rect* r, int wide, int flag = 1);
void limitedYRect(Rect* r, int height, int flag = 1);

//------------------------------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------------------------------
{
	int wide{ 30 };
	int height{ 30 };

	char board[40][40]{};

	Rect r1{};
	Rect r2{};

	char Command;

	initBoard(board, &r1, &r2);

	while (true) {

		std::cout << "input cord value : ";
		std::cin >> Command;

		if (Command == 'q') {
			return 0;
		}

		Commands(Command, board, &wide, &height, &r1, &r2);
		DrawBoard(board, height, wide);

	}
}

//------------------------------------------------------------------------------------------------
void Commands(char command, char board[][40], int* wide, int* height, Rect* r1, Rect* r2)
//------------------------------------------------------------------------------------------------
{
	//--------------------------------------------------------------------------------------------
	// 첫 번째 사각형 입력
	//--------------------------------------------------------------------------------------------
	if (command == 'h') {

		int x1, x2, y1, y2;

		r1->on = true;
		r2->on = false;
		r1->isLive = true;

		while (true) {
			std::cout << "input cord value 1 : ";
			std::cin >> x1 >> y1 >> x2 >> y2;

			if (x1 >= 0 && x2 >= 0 &&
				x1 != x2 &&
				x1 < *(wide) && x2 < *(wide)) {

				if (y1 >= 0 && y2 >= 0 &&
					y1 != y2 &&
					y1 < *(height) && y2 < *(height)) {

					r1->x1 = x1;
					r1->y1 = y1;
					r1->x2 = x2;
					r1->y2 = y2;

					break;
				}
			}

			std::cout << "값을 다시 입력하세요 " << '\n';
		}
	}

	//--------------------------------------------------------------------------------------------
	// 두 번째 사각형 입력
	//--------------------------------------------------------------------------------------------
	else if (command == 'H') {

		int x1, x2, y1, y2;

		r1->on = false;
		r2->on = true;
		r2->isLive = true;

		while (true) {
			std::cout << "input cord value 2 : ";
			std::cin >> x1 >> y1 >> x2 >> y2;

			if (x1 >= 0 && x2 >= 0 &&
				x1 != x2 &&
				x1 < *(wide) && x2 < *(wide)) {

				if (y1 >= 0 && y2 >= 0 &&
					y1 != y2 &&
					y1 < *(height) && y2 < *(height)) {

					r2->x1 = x1;
					r2->y1 = y1;
					r2->x2 = x2;
					r2->y2 = y2;

					break;
				}
			}

			std::cout << "값을 다시 입력하세요 " << '\n';
		}
	}

	//--------------------------------------------------------------------------------------------
	// x축 오른쪽 이동
	//--------------------------------------------------------------------------------------------
	else if (command == 'x') {
		MoveXRect(r1, r2, 1, *wide);
	}

	//--------------------------------------------------------------------------------------------
	// x축 왼쪽 이동
	//--------------------------------------------------------------------------------------------
	else if (command == 'X') {
		MoveXRect(r1, r2, 0, *wide);
	}

	//--------------------------------------------------------------------------------------------
	// y축 아래쪽 이동
	//--------------------------------------------------------------------------------------------
	else if (command == 'y') {
		MoveYRect(r1, r2, 1, *height);
	}

	//--------------------------------------------------------------------------------------------
	// y축 위쪽 이동
	//--------------------------------------------------------------------------------------------
	else if (command == 'Y') {
		MoveYRect(r1, r2, 0, *height);
	}

	//--------------------------------------------------------------------------------------------
	// x, y 모두 한 칸 축소
	//--------------------------------------------------------------------------------------------
	else if (command == 's') {

		char n;

		std::cout << "축소할 Rect를 알려주세요 : ";
		std::cin >> n;

		while (n != '1' && n != '2') {
			std::cout << "없는 Rect입니다 다시 입력해주세요." << '\n';
			std::cout << "축소할 Rect를 알려주세요 : ";
			std::cin >> n;
		}

		if (n == '1') {

			if (!r1->isLive) {
				std::cout << "없는 Rect입니다." << '\n';
				return;
			}

			r1->on = true;
			r2->on = false;

			ReBuildRect(r1, -3, *wide, *height);
		}
		else {

			if (!r2->isLive) {
				std::cout << "없는 Rect입니다." << '\n';
				return;
			}

			r1->on = false;
			r2->on = true;

			ReBuildRect(r2, -3, *wide, *height);
		}
	}

	//--------------------------------------------------------------------------------------------
	// x, y 모두 한 칸 확대
	//--------------------------------------------------------------------------------------------
	else if (command == 'S') {

		char n;

		std::cout << "확대할 Rect를 알려주세요 : ";
		std::cin >> n;

		while (n != '1' && n != '2') {
			std::cout << "없는 Rect입니다 다시 입력해주세요." << '\n';
			std::cout << "확대할 Rect를 알려주세요 : ";
			std::cin >> n;
		}

		if (n == '1') {

			if (!r1->isLive) {
				std::cout << "없는 Rect입니다." << '\n';
				return;
			}

			r1->on = true;
			r2->on = false;

			ReBuildRect(r1, 3, *wide, *height);
		}
		else {

			if (!r2->isLive) {
				std::cout << "없는 Rect입니다." << '\n';
				return;
			}

			r1->on = false;
			r2->on = true;

			ReBuildRect(r2, 3, *wide, *height);
		}
	}

	//--------------------------------------------------------------------------------------------
	// x축 한 칸 확대
	//--------------------------------------------------------------------------------------------
	else if (command == 'i') {

		char n;

		std::cout << "확대할 Rect를 알려주세요 : ";
		std::cin >> n;

		while (n != '1' && n != '2') {
			std::cout << "없는 Rect입니다 다시 입력해주세요." << '\n';
			std::cout << "확대할 Rect를 알려주세요 : ";
			std::cin >> n;
		}

		if (n == '1') {

			if (!r1->isLive) {
				std::cout << "없는 Rect입니다." << '\n';
				return;
			}

			r1->on = true;
			r2->on = false;

			ReBuildRect(r1, 1, *wide, *height);
		}
		else {

			if (!r2->isLive) {
				std::cout << "없는 Rect입니다." << '\n';
				return;
			}

			r1->on = false;
			r2->on = true;

			ReBuildRect(r2, 1, *wide, *height);
		}
	}

	//--------------------------------------------------------------------------------------------
	// x축 한 칸 축소
	//--------------------------------------------------------------------------------------------
	else if (command == 'I') {

		char n;

		std::cout << "축소할 Rect를 알려주세요 : ";
		std::cin >> n;

		while (n != '1' && n != '2') {
			std::cout << "없는 Rect입니다 다시 입력해주세요." << '\n';
			std::cout << "축소할 Rect를 알려주세요 : ";
			std::cin >> n;
		}

		if (n == '1') {

			if (!r1->isLive) {
				std::cout << "없는 Rect입니다." << '\n';
				return;
			}

			r1->on = true;
			r2->on = false;

			ReBuildRect(r1, -1, *wide, *height);
		}
		else {

			if (!r2->isLive) {
				std::cout << "없는 Rect입니다." << '\n';
				return;
			}

			r1->on = false;
			r2->on = true;

			ReBuildRect(r2, -1, *wide, *height);
		}
	}

	//--------------------------------------------------------------------------------------------
	// y축 한 칸 확대
	//--------------------------------------------------------------------------------------------
	else if (command == 'j') {

		char n;

		std::cout << "확대할 Rect를 알려주세요 : ";
		std::cin >> n;

		while (n != '1' && n != '2') {
			std::cout << "없는 Rect입니다 다시 입력해주세요." << '\n';
			std::cout << "확대할 Rect를 알려주세요 : ";
			std::cin >> n;
		}

		if (n == '1') {

			if (!r1->isLive) {
				std::cout << "없는 Rect입니다." << '\n';
				return;
			}

			r1->on = true;
			r2->on = false;

			ReBuildRect(r1, 2, *wide, *height);
		}
		else {

			if (!r2->isLive) {
				std::cout << "없는 Rect입니다." << '\n';
				return;
			}

			r1->on = false;
			r2->on = true;

			ReBuildRect(r2, 2, *wide, *height);
		}
	}

	//--------------------------------------------------------------------------------------------
	// y축 한 칸 축소
	//--------------------------------------------------------------------------------------------
	else if (command == 'J') {

		char n;

		std::cout << "축소할 Rect를 알려주세요 : ";
		std::cin >> n;

		while (n != '1' && n != '2') {
			std::cout << "없는 Rect입니다 다시 입력해주세요." << '\n';
			std::cout << "축소할 Rect를 알려주세요 : ";
			std::cin >> n;
		}

		if (n == '1') {

			if (!r1->isLive) {
				std::cout << "없는 Rect입니다." << '\n';
				return;
			}

			r1->on = true;
			r2->on = false;

			ReBuildRect(r1, -2, *wide, *height);
		}
		else {

			if (!r2->isLive) {
				std::cout << "없는 Rect입니다." << '\n';
				return;
			}

			r1->on = false;
			r2->on = true;

			ReBuildRect(r2, -2, *wide, *height);
		}
	}

	//--------------------------------------------------------------------------------------------
	// x축 확대 + y축 축소
	//--------------------------------------------------------------------------------------------
	else if (command == 'a') {

		char n;

		std::cout << "변형할 Rect를 알려주세요 : ";
		std::cin >> n;

		while (n != '1' && n != '2') {
			std::cout << "없는 Rect입니다 다시 입력해주세요." << '\n';
			std::cout << "변형할 Rect를 알려주세요 : ";
			std::cin >> n;
		}

		if (n == '1') {

			if (!r1->isLive) {
				std::cout << "없는 Rect입니다." << '\n';
				return;
			}

			r1->on = true;
			r2->on = false;

			ReBuildRect(r1, 4, *wide, *height);
		}
		else {

			if (!r2->isLive) {
				std::cout << "없는 Rect입니다." << '\n';
				return;
			}

			r1->on = false;
			r2->on = true;

			ReBuildRect(r2, 4, *wide, *height);
		}
	}

	//--------------------------------------------------------------------------------------------
	// x축 축소 + y축 확대
	//--------------------------------------------------------------------------------------------
	else if (command == 'A') {

		char n;

		std::cout << "변형할 Rect를 알려주세요 : ";
		std::cin >> n;

		while (n != '1' && n != '2') {
			std::cout << "없는 Rect입니다 다시 입력해주세요." << '\n';
			std::cout << "변형할 Rect를 알려주세요 : ";
			std::cin >> n;
		}

		if (n == '1') {

			if (!r1->isLive) {
				std::cout << "없는 Rect입니다." << '\n';
				return;
			}

			r1->on = true;
			r2->on = false;

			ReBuildRect(r1, -4, *wide, *height);
		}
		else {

			if (!r2->isLive) {
				std::cout << "없는 Rect입니다." << '\n';
				return;
			}

			r1->on = false;
			r2->on = true;

			ReBuildRect(r2, -4, *wide, *height);
		}
	}

	//--------------------------------------------------------------------------------------------
	// 면적 출력
	//--------------------------------------------------------------------------------------------
	else if (command == 'b') {

		char n;

		std::cout << "면적을 확인할 Rect를 알려주세요 : ";
		std::cin >> n;

		while (n != '1' && n != '2') {
			std::cout << "없는 Rect입니다." << '\n';
			std::cout << "면적을 확인할 Rect를 알려주세요 : ";
			std::cin >> n;
		}

		if (n == '1') {

			if (!r1->isLive) {
				std::cout << "없는 Rect 입니다." << '\n';
				return;
			}

			int rectWide;
			int rectHeight;

			if (r1->x1 <= r1->x2)
				rectWide = r1->x2 - r1->x1 + 1;
			else
				rectWide = *wide - r1->x1 + r1->x2 + 1;

			if (r1->y1 <= r1->y2)
				rectHeight = r1->y2 - r1->y1 + 1;
			else
				rectHeight = *height - r1->y1 + r1->y2 + 1;

			std::cout << "면적: "
				<< rectWide << " x "
				<< rectHeight << " = "
				<< rectWide * rectHeight << '\n';
		}
		else {

			if (!r2->isLive) {
				std::cout << "없는 Rect 입니다." << '\n';
				return;
			}

			int rectWide;
			int rectHeight;

			if (r2->x1 <= r2->x2)
				rectWide = r2->x2 - r2->x1 + 1;
			else
				rectWide = *wide - r2->x1 + r2->x2 + 1;

			if (r2->y1 <= r2->y2)
				rectHeight = r2->y2 - r2->y1 + 1;
			else
				rectHeight = *height - r2->y1 + r2->y2 + 1;

			std::cout << "면적: "
				<< rectWide << " x "
				<< rectHeight << " = "
				<< rectWide * rectHeight << '\n';
		}
	}

	//--------------------------------------------------------------------------------------------
	// 보드 x, y 모두 한 칸 확대
	//--------------------------------------------------------------------------------------------
	else if (command == 'c') {

		if (*wide < 40)
			(*wide)++;

		if (*height < 40)
			(*height)++;
	}

	//--------------------------------------------------------------------------------------------
	// 보드 x, y 모두 한 칸 축소
	//--------------------------------------------------------------------------------------------
	else if (command == 'd') {

		int newWide = *wide - 1;
		int newHeight = *height - 1;

		if (newWide < 10 || newHeight < 10) {
			std::cout << "한계이다." << '\n';
			return;
		}

		bool possible = true;

		// Rect 1 검사
		if (r1->isLive) {

			if (r1->x1 >= newWide || r1->x2 >= newWide ||
				r1->y1 >= newHeight || r1->y2 >= newHeight) {

				possible = false;
			}
		}

		// Rect 2 검사
		if (r2->isLive) {

			if (r2->x1 >= newWide || r2->x2 >= newWide ||
				r2->y1 >= newHeight || r2->y2 >= newHeight) {

				possible = false;
			}
		}

		if (possible) {
			*wide = newWide;
			*height = newHeight;
		}
		else {
			std::cout << "도형보다 작게 할 수 없다" << '\n';
		}
	}

	//--------------------------------------------------------------------------------------------
	// 전체 초기화
	//--------------------------------------------------------------------------------------------
	else if (command == 'r' || command == 'R') {

		*wide = 30;
		*height = 30;

		initBoard(board, r1, r2);
	}

	else {
		std::cout << "없는 명령어 입니다. 다시 입력해 주세요." << '\n';
		return;
	}

	MadeBoard(board, r1, r2, *(wide), *(height));
}

//------------------------------------------------------------------------------------------------
void MadeBoard(char board[][40], Rect* r1, Rect* r2, int wide, int height)
//------------------------------------------------------------------------------------------------
{
	ClearBoard(board);

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < wide; ++j) {

			if (r1->isLive && IsInRect(r1, j, i)) {

				if (board[i][j] == '*') {
					board[i][j] = '0';
				}
			}

			if (r2->isLive && IsInRect(r2, j, i)) {

				if (board[i][j] == '*') {
					board[i][j] = '1';
				}
				else {
					board[i][j] = '#';
				}
			}
		}
	}
}

//------------------------------------------------------------------------------------------------
void initBoard(char board[][40], Rect* r1, Rect* r2)
//------------------------------------------------------------------------------------------------
{
	for (int i = 0; i < 40; ++i) {
		for (int j = 0; j < 40; ++j) {
			board[i][j] = '*';
		}
	}

	r1->on = false;
	r2->on = false;

	r1->isLive = false;
	r2->isLive = false;

	r1->x1 = -1;
	r1->y1 = -1;
	r1->x2 = -1;
	r1->y2 = -1;

	r2->x1 = -1;
	r2->y1 = -1;
	r2->x2 = -1;
	r2->y2 = -1;
}

//------------------------------------------------------------------------------------------------
void DrawBoard(char board[][40], int height, int wide)
//------------------------------------------------------------------------------------------------
{
	std::cout << '\n';

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < wide; ++x) {

			if (board[y][x] == '#') {

				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

				SetConsoleTextAttribute(hConsole, 12);
				std::cout << board[y][x] << " ";
				SetConsoleTextAttribute(hConsole, 7);
			}
			else {
				std::cout << board[y][x] << " ";
			}
		}

		std::cout << '\n';
	}
}

//------------------------------------------------------------------------------------------------
void ReBuildRect(Rect* r, int flag, int wide, int height)
//------------------------------------------------------------------------------------------------
{
	if (flag > 0) {

		if (flag == 1) {
			r->x2++;
		}
		else if (flag == 2) {
			r->y2++;
		}
		else if (flag == 3) {
			r->x2++;
			r->y2++;
		}
		else if (flag == 4) {
			r->x2++;
			r->y2--;
		}
	}
	else {

		if (flag == -1) {
			r->x2--;
		}
		else if (flag == -2) {
			r->y2--;
		}
		else if (flag == -3) {
			r->x2--;
			r->y2--;
		}
		else if (flag == -4) {
			r->x2--;
			r->y2++;
		}
	}

	limitedXRect(r, wide, 2);
	limitedYRect(r, height, 2);
}

//------------------------------------------------------------------------------------------------
void ClearBoard(char board[][40])
//------------------------------------------------------------------------------------------------
{
	for (int i = 0; i < 40; ++i) {
		for (int j = 0; j < 40; ++j) {
			board[i][j] = '*';
		}
	}
}

//------------------------------------------------------------------------------------------------
void MoveXRect(Rect* r1, Rect* r2, int flag, int wide)
//------------------------------------------------------------------------------------------------
{
	char n;

	std::cout << "움직일 Rect를 알려주세요 : ";
	std::cin >> n;

	while (true) {

		if (n == '1') {

			if (!r1->isLive) {
				std::cout << "생성되지 않은 Rect입니다." << '\n';
				std::cout << "움직일 Rect를 알려주세요 : ";
				std::cin >> n;
				continue;
			}

			r1->on = true;
			r2->on = false;
			break;
		}
		else if (n == '2') {

			if (!r2->isLive) {
				std::cout << "생성되지 않은 Rect입니다." << '\n';
				std::cout << "움직일 Rect를 알려주세요 : ";
				std::cin >> n;
				continue;
			}

			r2->on = true;
			r1->on = false;
			break;
		}
		else {
			std::cout << "없는 Rect입니다 다시 입력해주세요." << '\n';
			std::cout << "움직일 Rect를 알려주세요 : ";
			std::cin >> n;
		}
	}

	if (flag == 1) {

		if (r1->on) {
			r1->x1 += 1;
			r1->x2 += 1;
			limitedXRect(r1, wide);
		}

		if (r2->on) {
			r2->x1 += 1;
			r2->x2 += 1;
			limitedXRect(r2, wide);
		}
	}
	else if (flag == 0) {

		if (r1->on) {
			r1->x1 -= 1;
			r1->x2 -= 1;
			limitedXRect(r1, wide);
		}

		if (r2->on) {
			r2->x1 -= 1;
			r2->x2 -= 1;
			limitedXRect(r2, wide);
		}
	}
}

//------------------------------------------------------------------------------------------------
void MoveYRect(Rect* r1, Rect* r2, int flag, int height)
//------------------------------------------------------------------------------------------------
{
	char n;

	while (true) {

		std::cout << "움직일 Rect를 알려주세요 : ";
		std::cin >> n;

		if (n == '1') {

			if (!r1->isLive) {
				std::cout << "생성되지 않은 Rect입니다." << '\n';
				continue;
			}

			r1->on = true;
			r2->on = false;
			break;
		}
		else if (n == '2') {

			if (!r2->isLive) {
				std::cout << "생성되지 않은 Rect입니다." << '\n';
				continue;
			}

			r2->on = true;
			r1->on = false;
			break;
		}
		else {
			std::cout << "없는 Rect입니다 다시 입력해주세요." << '\n';
		}
	}

	if (flag == 1) {

		if (r1->on) {
			r1->y1 += 1;
			r1->y2 += 1;
			limitedYRect(r1, height);
		}

		if (r2->on) {
			r2->y1 += 1;
			r2->y2 += 1;
			limitedYRect(r2, height);
		}
	}
	else if (flag == 0) {

		if (r1->on) {
			r1->y1 -= 1;
			r1->y2 -= 1;
			limitedYRect(r1, height);
		}

		if (r2->on) {
			r2->y1 -= 1;
			r2->y2 -= 1;
			limitedYRect(r2, height);
		}
	}
}

//------------------------------------------------------------------------------------------------
bool IsInRect(Rect* r, int x, int y)
//------------------------------------------------------------------------------------------------
{
	bool xCheck = false;
	bool yCheck = false;

	if (r->x1 <= r->x2) {

		if (x >= r->x1 && x <= r->x2) {
			xCheck = true;
		}
	}
	else {

		if (x >= r->x1 || x <= r->x2) {
			xCheck = true;
		}
	}

	if (r->y1 <= r->y2) {

		if (y >= r->y1 && y <= r->y2) {
			yCheck = true;
		}
	}
	else {

		if (y >= r->y1 || y <= r->y2) {
			yCheck = true;
		}
	}

	if (xCheck && yCheck) {
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------------------------
void limitedXRect(Rect* r, int wide, int flag)
//------------------------------------------------------------------------------------------------
{
	if (flag == 1) {

		if (r->x1 < 0)
			r->x1 = wide - 1;

		if (r->x2 < 0)
			r->x2 = wide - 1;

		if (r->x1 >= wide)
			r->x1 = 0;

		if (r->x2 >= wide)
			r->x2 = 0;
	}
	else {

		if (r->x1 < 0)
			r->x1 = 0;

		if (r->x2 < 0)
			r->x2 = 0;

		if (r->x1 >= wide)
			r->x1 = wide;

		if (r->x2 >= wide)
			r->x2 = wide;
	}
}

//------------------------------------------------------------------------------------------------
void limitedYRect(Rect* r, int height, int flag)
//------------------------------------------------------------------------------------------------
{
	if (flag == 1) {

		if (r->y1 < 0)
			r->y1 = height - 1;

		if (r->y2 < 0)
			r->y2 = height - 1;

		if (r->y1 >= height)
			r->y1 = 0;

		if (r->y2 >= height)
			r->y2 = 0;
	}
	else {

		if (r->y1 < 0)
			r->y1 = 0;

		if (r->y2 < 0)
			r->y2 = 0;

		if (r->y1 >= height)
			r->y1 = height;

		if (r->y2 >= height)
			r->y2 = height;
	}
}
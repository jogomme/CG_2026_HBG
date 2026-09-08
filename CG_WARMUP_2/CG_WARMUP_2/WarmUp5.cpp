#include<iostream>
#include<Windows.h>
#include<algorithm>

struct Rect
{
	int x1;
	int y1;
	int x2;
	int y2;
};

void DrawBoard(char board[][40], int height, int wide);

void initBoard(char board[][40], Rect* r1, Rect* r2);

void MadeBoard(char board[][40], Rect* r1, Rect* r2, int wide, int height);

void Commands(char command, char board[][40], int* wide, int* height, Rect* r1, Rect* r2);

void ReBuildRect(Rect* r);

void ClearBoard(char board[][40]);

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
	if (command == 's') {
		(*(wide))--;
		(*(height))--;

		if (*(wide) < 10) {
			*(wide) = 10;
		}
		if (*(height) < 10) {
			*(height) = 10;
		}

	}
	else if (command == 'S') {
		(*(wide))++;
		(*(height))++;

		if (*(wide) > 40) {
			*(wide) = 40;
		}
		if (*(height) > 40) {
			*(height) = 40;
		}
	}
	else if (command == 'i') {
		(*(wide))++;
		if (*(wide) > 40) {
			*(wide) = 40;
		}
	}
	else if (command == 'I') {
		(*(wide))--;
		if (*(wide) < 10) {
			*(wide) = 10;
		}
	}
	else if (command == 'j') {
		(*(height))++;
		if (*(height) > 40) {
			*(height) = 40;
		}
	}
	else if (command == 'J') {
		(*(height))--;
		if (*(height) < 10) {
			*(height) = 10;
		}
	}
	//a/A: x축 한 칸 확대, y축 한 칸 축소 / x축 한 칸 축소, y축 한 칸 확대
	else if (command == 'a') {
		(*(wide))++;
		if (*(wide) > 40) {
			*(wide) = 40;
		}

		(*(height))--;
		if (*(height) < 10) {
			*(height) = 10;
		}
	}
	else if (command == 'A') {
		(*(wide))--;
		if (*(wide) < 10) {
			*(wide) = 10;
		}

		(*(height))++;
		if (*(height) > 40) {
			*(height) = 40;
		}
	}
	else if (command == 'r' || command == 'R') {
		initBoard(board, r1, r2);
	}
	else if (command == 'h') {

		int x1, x2, y1, y2;

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

		MadeBoard(board, r1, r2, *(wide), *(height));
	}

	else if (command == 'H') {

		int x1, x2, y1, y2;

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

		MadeBoard(board, r1, r2, *(wide), *(height));
	}
	else if (command == 'b') {

	}
	else {
		std::cout << "없는 명령어 입니다. 다시 입력해 주세요." << '\n';
	}
}

//------------------------------------------------------------------------------------------------
void MadeBoard(char board[][40], Rect* r1, Rect* r2, int wide, int height)
//------------------------------------------------------------------------------------------------
{
	ReBuildRect(r1);
	ReBuildRect(r2);

	ClearBoard(board);

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < wide; ++j) {

			if (i >= r1->y1 && i <= r1->y2 && j >= r1->x1 && j <= r1->x2) {

				if (board[i][j] == '*') {
					board[i][j] = '0';
				}
			}
			if (i >= r2->y1 && i <= r2->y2 && j >= r2->x1 && j <= r2->x2) {
			
				if (board[i][j] == '*') {
					board[i][j] = '1';
				}
				else if (board[i][j] != '*') {
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
void ReBuildRect(Rect* r)
//------------------------------------------------------------------------------------------------
{
	if (r->x1 > r->x2) {
		std::swap(r->x1, r->x2);
	}
	if (r->y1 > r->y2) {
		std::swap(r->y1, r->y2);
	}
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
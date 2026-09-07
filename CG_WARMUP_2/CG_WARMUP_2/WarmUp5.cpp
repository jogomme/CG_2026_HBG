#include<iostream>
#include<Windows.h>

void DrawBoard(char board[][40], int height, int wide);

void initBoard(char board[][40]);

void MadeBoard(char board[][40], int x1, int x2, int y1, int y2, int wide, int height);

void Commands(char command, char board[][40], int* wide, int* height);


//------------------------------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------------------------------
{
	int wide{ 30 };
	int height{ 30 };

	char board[40][40]{};



	char Command;

	initBoard(board);

	while (true) {
		
		std::cout << "input cord value : ";
		std::cin >> Command;

		if (Command == 'q') {
			return 0;
		}

		Commands(Command, board, &wide, &height);
		DrawBoard(board, height, wide);
	
	}
}

void Commands(char command, char board[][40], int* wide, int* height)
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
		initBoard(board);
	}
	else if (command == 'h') {

		int x1, x2, y1, y2;
		while (true) {
			std::cout << "input cord value 1 : ";
			std::cin >> x1 >> y1 >> x2 >> y2;

			if (x1 >= 0 && x2 >= 0 && x1 != x2 && x1 <= *(wide) && x2 <= *(wide)) {
				if (y1 >= 0 && y2 >= 0 && y1 != x2 && y1 <= *(height) && x2 <= *(height)) {
					break;
				}

			}
			std::cout << "값을 다시 입력하세요 " << '\n';
		}

		MadeBoard(board, x1, x2, y1, y2, *(wide), *(height));
	}

	else if (command == 'H') {
		int x1, x2, y1, y2;
		while (true) {
			std::cout << "input cord value 2 : ";
			std::cin >> x1 >> y1 >> x2 >> y2;

			if (x1 >= 0 && x2 >= 0 && x1 != x2 && x1 <= *(wide) && x2 <= *(wide)) {
				if (y1 >= 0 && y2 >= 0 && y1 != x2 && y1 <= *(height) && x2 <= *(height)) {
					break;
				}
			}
			std::cout << "값을 다시 입력하세요 " << '\n';
		}
		MadeBoard(board, x1, x2, y1, y2, *(wide), *(height));
	}
	else if (command == 'b') {

	}
	else {
		std::cout << "없는 명령어 입니다. 다시 입력해 주세요." << '\n';
	}
}

void MadeBoard(char board[][40], int x1, int x2, int y1, int y2, int wide, int height)
{
	static int cnt{};

	// x1 > x2 라면 자리 바꿈
	if (x1 > x2) {
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if (y1 > y2) {
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	if (cnt == 0) {
		initBoard(board);
	}

	++cnt;

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < wide; ++j) {

			if (i >= y1 && i <= y2 && j >= x1 && j <= x2) {
				if (board[i][j] == '*' && cnt == 1) {
					board[i][j] = '0';
				}
				else if (board[i][j] == '*' && cnt == 2) {
					board[i][j] = '1';
				}
				else if (board[i][j] != '*') {
					board[i][j] = '#';
				}
			}

		}
	}

	if (cnt == 2) {
		cnt = 0;
	}

}

void initBoard(char board[][40])
{
	for (int i = 0; i < 40; ++i) {
		for (int j = 0; j < 40; ++j) {
			board[i][j] = '*';
		}
	}
}

void DrawBoard(char board[][40], int height, int wide)
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
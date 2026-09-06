#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>

// -------------------------
// 카드 구조체
// -------------------------

struct Card
{
    char value;       // 카드 문자
    bool open;        // 현재 공개되어 있는가?
    bool complete;    // 이미 짝을 맞췄는가?
};


// 최대 보드 크기
const int MAX_SIZE = 6;


// 보드
Card board[MAX_SIZE][MAX_SIZE];


// 보드 크기
int width;
int height;


// 게임 정보
int score;
int chance;


// -------------------------
// 색상 변경
// -------------------------

void SetColor(int color)
{
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE),
        color);
}


// -------------------------
// 화면 지우기
// -------------------------

void ClearScreen()
{
    system("cls");
}


// -------------------------
// 카드 대문자 변환
// -------------------------

char ToUpper(char value)
{
    if (value >= 'a' && value <= 'z')
    {
        return value - 'a' + 'A';
    }

    return value;
}


// -------------------------
// 보드 생성
// -------------------------

void CreateBoard()
{
    int total = width * height;

    int letterCount = total / 2;

    int index = 0;


    // 모든 카드 초기화
    for (int y = 0; y < MAX_SIZE; ++y)
    {
        for (int x = 0; x < MAX_SIZE; ++x)
        {
            board[y][x].value = ' ';
            board[y][x].open = false;
            board[y][x].complete = false;
        }
    }


    // 카드 만들기
    // 같은 문자를 2개씩 만든다.
    for (int i = 0; i < letterCount; ++i)
    {
        char letter = 'a' + i;


        board[index / width][index % width].value = letter;
        ++index;


        board[index / width][index % width].value = letter;
        ++index;
    }


    // 칸 수가 홀수라면 조커 추가
    if (total % 2 == 1)
    {
        board[index / width][index % width].value = 'J';

        ++index;
    }


    // -------------------------
    // 카드 섞기
    // -------------------------

    for (int i = 0; i < total; ++i)
    {
        int randomIndex = rand() % total;


        int row1 = i / width;
        int col1 = i % width;


        int row2 = randomIndex / width;
        int col2 = randomIndex % width;


        char temp = board[row1][col1].value;

        board[row1][col1].value =
            board[row2][col2].value;

        board[row2][col2].value = temp;
    }
}


// -------------------------
// 게임 초기화
// -------------------------

void ResetGame()
{
    CreateBoard();

    score = 0;

    // 기본 기회
    chance = (width * height) / 2 + 5;
}


// -------------------------
// 보드 출력
// -------------------------

void PrintBoard()
{
    std::cout << '\n';


    // -------------------------
    // 위쪽 열 번호
    // -------------------------

    std::cout << "      ";

    for (int x = 0; x < width; ++x)
    {
        std::cout << char('a' + x);
        std::cout << "    ";
    }

    std::cout << '\n';


    // -------------------------
    // 보드 출력
    // -------------------------

    for (int y = 0; y < height; ++y)
    {
        // 왼쪽 행 번호
        std::cout << y + 1;
        std::cout << "     ";


        for (int x = 0; x < width; ++x)
        {
            Card& card = board[y][x];


            std::cout << "[";


            // 이미 맞춘 카드
            if (card.complete == true)
            {
                SetColor(10);

                std::cout << ToUpper(card.value);

                SetColor(7);
            }


            // 현재 선택되어 있는 카드
            else if (card.open == true)
            {
                SetColor(14);

                std::cout << card.value;

                SetColor(7);
            }


            // 가려진 카드
            else
            {
                std::cout << "*";
            }


            std::cout << "]  ";
        }


        std::cout << '\n';
    }


    std::cout << '\n';


    // -------------------------
    // 게임 정보
    // -------------------------

    std::cout << "점수 : "
        << score
        << '\n';


    std::cout << "남은 횟수 : "
        << chance
        << '\n';
}


// -------------------------
// 위치 입력
// -------------------------

bool SelectPosition(
    int& row,
    int& col)
{
    std::cout << "행과 열을 입력하세요 : ";

    std::cin >> row >> col;


    // 사용자가 입력한 숫자는 1부터 시작한다.
    // 배열은 0부터 시작하므로 -1
    row--;
    col--;


    // 범위 검사
    if (row < 0 || row >= height ||
        col < 0 || col >= width)
    {
        std::cout << "잘못된 위치입니다.\n";

        return false;
    }


    // 이미 맞춘 카드
    if (board[row][col].complete == true)
    {
        std::cout << "이미 맞춘 카드입니다.\n";

        return false;
    }


    // 이미 선택된 카드
    if (board[row][col].open == true)
    {
        std::cout << "이미 선택된 카드입니다.\n";

        return false;
    }


    return true;
}


// -------------------------
// 카드 비교
// -------------------------

void CheckCards(
    int row1,
    int col1,
    int row2,
    int col2)
{
    Card& card1 = board[row1][col1];
    Card& card2 = board[row2][col2];


    // 선택한 카드 공개
    card1.open = true;
    card2.open = true;


    ClearScreen();

    PrintBoard();


    std::cout << '\n';

    std::cout << "첫 번째 카드 : "
        << card1.value
        << '\n';


    std::cout << "두 번째 카드 : "
        << card2.value
        << '\n';


    std::cout << '\n';

    std::cout << "Enter를 누르면 판정합니다.";

    std::cin.ignore();
    std::cin.get();


    // -------------------------
    // 조커
    // -------------------------

    if (card1.value == 'J' ||
        card2.value == 'J')
    {
        card1.complete = true;
        card2.complete = true;


        score += 10;


        ClearScreen();

        PrintBoard();


        std::cout << '\n';
        std::cout << "조커 매칭 성공! +10점\n";
    }


    // -------------------------
    // 같은 카드
    // -------------------------

    else if (card1.value == card2.value)
    {
        card1.complete = true;
        card2.complete = true;


        score += 10;


        ClearScreen();

        PrintBoard();


        std::cout << '\n';
        std::cout << "매칭 성공! +10점\n";
    }


    // -------------------------
    // 다른 카드
    // -------------------------

    else
    {
        score -= 2;


        ClearScreen();

        PrintBoard();


        std::cout << '\n';
        std::cout << "매칭 실패! -2점\n";


        std::cout << '\n';
        std::cout << "Enter를 누르면 카드를 가립니다.";

        std::cin.get();


        // 다시 가린다.
        card1.open = false;
        card2.open = false;
    }


    std::cout << '\n';

    std::cout << "Enter를 누르면 계속합니다.";

    std::cin.get();
}


// -------------------------
// 게임 클리어 확인
// -------------------------

bool CheckClear()
{
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (board[y][x].complete == false)
            {
                return false;
            }
        }
    }


    return true;
}


// -------------------------
// 힌트
// -------------------------

void Hint()
{
    ClearScreen();


    std::cout << "============================\n";
    std::cout << "            HINT\n";
    std::cout << "============================\n";


    std::cout << '\n';

    std::cout << "모든 카드를 공개합니다.\n";
    std::cout << "확인한 뒤 Enter를 누르세요.\n";


    // 모든 카드 공개
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            board[y][x].open = true;
        }
    }


    PrintBoard();


    std::cin.ignore();
    std::cin.get();


    // 이미 맞춘 카드가 아니라면 다시 가린다.
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (board[y][x].complete == false)
            {
                board[y][x].open = false;
            }
        }
    }
}


// -------------------------
// 게임 시작
// -------------------------

void StartGame()
{
    // -------------------------
    // 보드 크기 입력
    // -------------------------

    while (true)
    {
        ClearScreen();


        std::cout << "============================\n";
        std::cout << "       카드 짝맞추기 게임\n";
        std::cout << "============================\n";


        std::cout << '\n';


        std::cout << "가로 크기 (3~6) : ";
        std::cin >> width;


        if (width < 3 || width > 6)
        {
            std::cout << "3~6 사이로 입력하세요.\n";

            system("pause");

            continue;
        }


        std::cout << "세로 크기 (3~6) : ";
        std::cin >> height;


        if (height < 3 || height > 6)
        {
            std::cout << "3~6 사이로 입력하세요.\n";

            system("pause");

            continue;
        }


        break;
    }


    // 게임 초기화
    ResetGame();


    // -------------------------
    // 게임 반복
    // -------------------------

    while (true)
    {
        ClearScreen();


        std::cout << "============================\n";
        std::cout << "       카드 짝맞추기 게임\n";
        std::cout << "============================\n";


        PrintBoard();


        std::cout << '\n';


        // -------------------------
        // 명령어
        // -------------------------

        std::cout << "1 : 카드 선택\n";
        std::cout << "r : 게임 리셋\n";
        std::cout << "h : 힌트\n";
        std::cout << "q : 게임 종료\n";


        std::cout << '\n';

        std::cout << "입력 : ";


        char command;

        std::cin >> command;


        // -------------------------
        // 종료
        // -------------------------

        if (command == 'q')
        {
            std::cout << "게임을 종료합니다.\n";

            break;
        }


        // -------------------------
        // 리셋
        // -------------------------

        if (command == 'r')
        {
            ResetGame();

            continue;
        }


        // -------------------------
        // 힌트
        // -------------------------

        if (command == 'h')
        {
            Hint();

            continue;
        }


        // -------------------------
        // 카드 선택
        // -------------------------

        if (command == '1')
        {
            int row1;
            int col1;

            int row2;
            int col2;


            // 첫 번째 카드
            while (true)
            {
                ClearScreen();

                PrintBoard();


                std::cout << '\n';
                std::cout << "첫 번째 카드를 선택합니다.\n";


                if (SelectPosition(row1, col1))
                {
                    break;
                }


                system("pause");
            }


            // 첫 번째 카드 임시 공개
            board[row1][col1].open = true;


            // 두 번째 카드
            while (true)
            {
                ClearScreen();

                PrintBoard();


                std::cout << '\n';
                std::cout << "두 번째 카드를 선택합니다.\n";


                if (SelectPosition(row2, col2))
                {
                    break;
                }


                system("pause");
            }


            // 같은 칸을 선택했는지 확인
            if (row1 == row2 &&
                col1 == col2)
            {
                board[row1][col1].open = false;


                std::cout << "같은 카드를 선택할 수 없습니다.\n";

                system("pause");

                continue;
            }


            // 두 번째 카드 공개
            board[row2][col2].open = true;


            // 두 카드 비교
            CheckCards(
                row1,
                col1,
                row2,
                col2);


            // 한 번의 시도 사용
            chance--;


            // -------------------------
            // 클리어
            // -------------------------

            if (CheckClear())
            {
                ClearScreen();


                std::cout << "============================\n";
                std::cout << "         GAME CLEAR!\n";
                std::cout << "============================\n";


                PrintBoard();


                std::cout << '\n';

                std::cout << "게임을 클리어했습니다!\n";

                std::cout << "최종 점수 : "
                    << score
                    << '\n';


                std::cout << '\n';

                std::cout << "r : 다시 시작\n";
                std::cout << "q : 종료\n";


                char endCommand;

                std::cin >> endCommand;


                if (endCommand == 'q')
                {
                    break;
                }


                if (endCommand == 'r')
                {
                    ResetGame();
                }


                continue;
            }


            // -------------------------
            // 게임 오버
            // -------------------------

            if (chance <= 0)
            {
                ClearScreen();


                std::cout << "============================\n";
                std::cout << "          GAME OVER\n";
                std::cout << "============================\n";


                PrintBoard();


                std::cout << '\n';

                std::cout << "남은 횟수를 모두 사용했습니다.\n";


                std::cout << "최종 점수 : "
                    << score
                    << '\n';


                std::cout << '\n';

                std::cout << "r : 다시 시작\n";
                std::cout << "q : 종료\n";


                char endCommand;

                std::cin >> endCommand;


                if (endCommand == 'q')
                {
                    break;
                }


                if (endCommand == 'r')
                {
                    ResetGame();
                }
            }


            continue;
        }


        // 잘못된 명령
        std::cout << "잘못된 명령입니다.\n";

        system("pause");
    }
}


// -------------------------
// main
// -------------------------

int main()
{
    // 랜덤 초기화
    srand((unsigned)time(NULL));


    // 게임 시작
    StartGame();


    return 0;
}
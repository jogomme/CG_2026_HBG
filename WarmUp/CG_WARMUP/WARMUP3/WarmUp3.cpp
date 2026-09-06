#include <iostream>
#include <cstdlib>
#include <cmath>

struct Point
{
    int x;
    int y;
    int z;
};

struct MyPoint
{
    Point p[10];
    bool use[10]{ false };

    int top{ 0 };
    int bottom{ 0 };
};

bool isOnF = false;

int 제곱(Point p)
{
    return p.x * p.x + p.y * p.y + p.z * p.z;
}

int 비교(const void* a, const void* b)
{
    Point* p1 = (Point*)a;
    Point* p2 = (Point*)b;

    return 제곱(*p1) - 제곱(*p2);
}


void PrintList(MyPoint& list)
{
    if (isOnF)
    {
        Point temp[10];
        int cnt = 0;

        for (int i = 0; i < 10; ++i)
        {
            if (list.use[i])
            {
                temp[cnt] = list.p[i];
                ++cnt;
            }
        }

        qsort(temp, cnt, sizeof(Point), 비교);

        for (int i = 0; i < cnt; ++i)
        {
            std::cout << i << " : ( "
                << temp[i].x << ", "
                << temp[i].y << ", "
                << temp[i].z << " ) "
                << "거리 : " << std::sqrt(제곱(temp[i]))
                << '\n';
        }

        for (int i = cnt; i < 10; ++i)
        {
            std::cout << i << " : \n";
        }
    }
    else
    {
        for (int i = 9; i >= 0; --i)
        {
            std::cout << i << " : ";

            if (list.use[i])
            {
                std::cout << "( "
                    << list.p[i].x << ", "
                    << list.p[i].y << ", "
                    << list.p[i].z << " ) ";
            }

            std::cout << '\n';
        }
    }
}


void Commands(MyPoint& list, char Command)
{
    int x, y, z;

    if (Command == '+')
    {
        std::cin >> x >> y >> z;

        if (list.top < 10 && list.use[list.top] == false)
        {
            list.use[list.top] = true;

            list.p[list.top].x = x;
            list.p[list.top].y = y;
            list.p[list.top].z = z;

            ++list.top;
        }
        else
        {
            std::cout << "list가 포화상태 입니다." << '\n';
        }
    }
    else if (Command == '-')
    {
        if (list.top > 0)
        {
            --list.top;
            list.use[list.top] = false;
        }
        else
        {
            std::cout << "list가 공백상태 입니다." << '\n';
        }
    }
    else if (Command == 'e')
    {
        std::cin >> x >> y >> z;

        bool full = true;

        for (int i = 0; i < 10; ++i)
        {
            if (list.use[i] == false)
            {
                full = false;
                break;
            }
        }

        if (full)
        {
            std::cout << "list가 포화상태 입니다." << '\n';
        }
        else
        {
            for (int i = 9; i > 0; --i)
            {
                list.p[i] = list.p[i - 1];
                list.use[i] = list.use[i - 1];
            }

            list.p[0].x = x;
            list.p[0].y = y;
            list.p[0].z = z;
            list.use[0] = true;

            ++list.top;

            list.bottom = 0;
        }
    }
    else if (Command == 'd')
    {
        if (list.top > 0)
        {
            list.use[0] = false;

            for (int i = 0; i < 9; ++i)
            {
                list.p[i] = list.p[i + 1];
                list.use[i] = list.use[i + 1];
            }

            list.use[9] = false;

            --list.top;

            if (list.top == 0)
            {
                list.bottom = 0;
            }
        }
        else
        {
            std::cout << "list가 공백상태 입니다." << '\n';
        }
    }
    else if (Command == 'a')
    {
        int cnt{};

        for (int i = 0; i < 10; ++i)
        {
            if (list.use[i])
            {
                cnt++;
            }
        }

        std::cout << "현재 사용중인 list의 숫자는 "
            << cnt << "입니다." << '\n';
    }
    else if (Command == 'c')
    {
        for (int i = 0; i < 10; ++i)
        {
            list.use[i] = false;
        }

        std::cout << "모든 list를 초기화 하였습니다." << '\n';

        list.top = 0;
        list.bottom = 0;
    }
    else if (Command == 'b')
    {
        Point temp = list.p[9];
        bool tempUse = list.use[9];

        for (int i = 9; i > 0; --i)
        {
            list.p[i] = list.p[i - 1];
            list.use[i] = list.use[i - 1];
        }

        list.p[0] = temp;
        list.use[0] = tempUse;
    }
    else if (Command == 'f')
    {
        isOnF = !isOnF;
    }
    else if (Command == 'g')
    {
        int cnt = 0;

        for (int i = 0; i < 10; ++i)
        {
            if (list.use[i])
            {
                ++cnt;
            }
        }

        if (cnt < 2)
        {
            std::cout << "두 점 이상 있어야 합니다." << '\n';
        }
        else
        {
            Point closePoint1{};
            Point closePoint2{};
            Point farPoint1{};
            Point farPoint2{};

            int closeDistance = 0;
            int farDistance = 0;

            bool first = true;

            for (int i = 0; i < 10; ++i)
            {
                if (list.use[i] == false)
                {
                    continue;
                }

                for (int j = i + 1; j < 10; ++j)
                {
                    if (list.use[j] == false)
                    {
                        continue;
                    }

                    int dx = list.p[i].x - list.p[j].x;
                    int dy = list.p[i].y - list.p[j].y;
                    int dz = list.p[i].z - list.p[j].z;

                    int distance = dx * dx + dy * dy + dz * dz;

                    if (first)
                    {
                        closeDistance = distance;
                        farDistance = distance;

                        closePoint1 = list.p[i];
                        closePoint2 = list.p[j];

                        farPoint1 = list.p[i];
                        farPoint2 = list.p[j];

                        first = false;
                    }

                    if (distance < closeDistance)
                    {
                        closeDistance = distance;

                        closePoint1 = list.p[i];
                        closePoint2 = list.p[j];
                    }

                    if (distance > farDistance)
                    {
                        farDistance = distance;

                        farPoint1 = list.p[i];
                        farPoint2 = list.p[j];
                    }
                }
            }

            std::cout << "가장 가까운 두 점" << '\n';

            std::cout << "( "
                << closePoint1.x << ", "
                << closePoint1.y << ", "
                << closePoint1.z << " )"
                << " <-> "
                << "( "
                << closePoint2.x << ", "
                << closePoint2.y << ", "
                << closePoint2.z << " )"
                << '\n';

            std::cout << "거리 : "
                << std::sqrt(closeDistance)
                << '\n';

            std::cout << '\n';

            std::cout << "가장 먼 두 점" << '\n';

            std::cout << "( "
                << farPoint1.x << ", "
                << farPoint1.y << ", "
                << farPoint1.z << " )"
                << " <-> "
                << "( "
                << farPoint2.x << ", "
                << farPoint2.y << ", "
                << farPoint2.z << " )"
                << '\n';

            std::cout << "거리 : "
                << std::sqrt(farDistance)
                << '\n';
        }
        }

    std::cout << "현재 List\n";

    PrintList(list);
}


int main()
{
    MyPoint list{};

    char Command;

    std::cout << "현재 List\n";

    PrintList(list);

    while (true)
    {
        std::cout << "명령어를 입력해주세요 : ";

        std::cin >> Command;

        if (Command == 'q')
        {
            return 0;
        }

        Commands(list, Command);
    }
}
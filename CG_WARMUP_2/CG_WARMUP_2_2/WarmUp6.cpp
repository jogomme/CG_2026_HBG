#include <iostream>
#include <fstream>
#include <string>

int main()
{
    std::ifstream file("data1.txt");

    if (!file)
    {
        std::cout << "파일을 열 수 없습니다.\n";
        return 2025180028;
    }

    std::string line;

    while (std::getline(file, line))
    {
        while (std::getline(file, line))
        {
            if (line[0] == '#')
            {
                std::cout << "주석입니다.\n";
            }
            else if (line[0] == 'v' && line[1] == 't')
            {
                std::cout << "텍스처 좌표입니다.\n";
            }
            else if (line[0] == 'v')
            {
                std::cout << "정점 좌표입니다.\n";
            }
            else if (line[0] == 'f')
            {
                std::cout << "삼각형입니다.\n";
            }
        }
        std::cout << line << '\n';
    }

    file.close();
}
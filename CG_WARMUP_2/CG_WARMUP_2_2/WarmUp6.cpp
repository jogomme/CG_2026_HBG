#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct Vertex
{
    float x;
    float y;
    float z;
};

struct VT
{
    float x;
    float y;
};

struct Face
{
    int vertexIndex[3];
    int textureIndex[3];
    bool isTexture;
};

Vertex vertex[100];
int vertexCount{};

VT vt[100];
int vtCount{};

Face face[100];
int faceCount{};

void SaveVertex(std::string line);
void SaveVT(std::string line);
void SaveF(std::string line);

bool CheckIndex(int vertexIndex, int textureIndex);

//-------------------------------------------------------------------------------------------------
int main()
//-------------------------------------------------------------------------------------------------
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
        // 빈 줄은 건너뛰기
        if (line.empty())
        {
            continue;
        }

        // 주석
        if (line[0] == '#')
        {
            continue;
        }

        // 텍스처 좌표
        else if (line[0] == 'v' && line[1] == 't')
        {
            SaveVT(line);
        }

        // 정점 좌표
        else if (line[0] == 'v')
        {
            SaveVertex(line);
        }

        // 삼각형
        else if (line[0] == 'f')
        {
            SaveF(line);
        }

        // 그 외의 문자
        else
        {
            std::cout << "허용되지 않는 문자입니다.\n";
        }
    }

    file.close();

    //-------------------------------------------------------------------------------------------------
    // Vertex 출력
    //-------------------------------------------------------------------------------------------------

    std::cout << "\n===== Vertex =====\n";

    for (int i = 0; i < vertexCount; ++i)
    {
        std::cout << i + 1 << " : ("
            << vertex[i].x << ", "
            << vertex[i].y << ", "
            << vertex[i].z << ")\n";
    }

    //-------------------------------------------------------------------------------------------------
    // VT 출력
    //-------------------------------------------------------------------------------------------------

    std::cout << "\n===== VT =====\n";

    for (int i = 0; i < vtCount; ++i)
    {
        std::cout << i + 1 << " : ("
            << vt[i].x << ", "
            << vt[i].y << ")\n";
    }

    //-------------------------------------------------------------------------------------------------
    // Face 출력
    //-------------------------------------------------------------------------------------------------

    std::cout << "\n===== Face =====\n";

    for (int i = 0; i < faceCount; ++i)
    {
        std::cout << "\nFace " << i + 1 << " ("
            << face[i].vertexIndex[0] << ", "
            << face[i].vertexIndex[1] << ", "
            << face[i].vertexIndex[2] << ")\n";

        std::cout << "vertex\n";

        for (int j = 0; j < 3; ++j)
        {
            int index = face[i].vertexIndex[j] - 1;

            std::cout << "("
                << vertex[index].x << ", "
                << vertex[index].y << ", "
                << vertex[index].z << ")\n";
        }

        if (face[i].isTexture)
        {
            std::cout << "texture\n";

            for (int j = 0; j < 3; ++j)
            {
                int index = face[i].textureIndex[j] - 1;

                std::cout << "("
                    << vt[index].x << ", "
                    << vt[index].y << ")\n";
            }
        }
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
void SaveVertex(std::string line)
//-------------------------------------------------------------------------------------------------
{
    std::stringstream ss(line);

    char type;

    ss >> type
        >> vertex[vertexCount].x
        >> vertex[vertexCount].y
        >> vertex[vertexCount].z;

    ++vertexCount;
}

//-------------------------------------------------------------------------------------------------
void SaveVT(std::string line)
//-------------------------------------------------------------------------------------------------
{
    std::stringstream ss(line);

    std::string type;

    ss >> type
        >> vt[vtCount].x
        >> vt[vtCount].y;

    ++vtCount;
}

//-------------------------------------------------------------------------------------------------
void SaveF(std::string line)
//-------------------------------------------------------------------------------------------------
{
    std::stringstream ss(line);

    char type;

    std::string first;
    std::string second;
    std::string third;
    std::string fourth;

    int v1, t1;
    int v2, t2;
    int v3, t3;

    ss >> type;

    // 꼭짓점 정보가 3개인지 확인
    if (!(ss >> first >> second >> third))
    {
        std::cout << "삼각형을 만들 수 없습니다.\n";
        return;
    }

    // 4번째 정보가 있으면 삼각형이 아님
    if (ss >> fourth)
    {
        std::cout << "삼각형을 만들 수 없습니다.\n";
        return;
    }

    // 첫 번째 꼭짓점
    for (int i = 0; i < first.length(); ++i)
    {
        if (first[i] == '/')
        {
            first[i] = ' ';
        }
    }

    std::stringstream firstStream(first);
    firstStream >> v1 >> t1;

    // 두 번째 꼭짓점
    for (int i = 0; i < second.length(); ++i)
    {
        if (second[i] == '/')
        {
            second[i] = ' ';
        }
    }

    std::stringstream secondStream(second);
    secondStream >> v2 >> t2;

    // 세 번째 꼭짓점
    for (int i = 0; i < third.length(); ++i)
    {
        if (third[i] == '/')
        {
            third[i] = ' ';
        }
    }

    std::stringstream thirdStream(third);
    thirdStream >> v3 >> t3;

    // 인덱스 범위 확인
    if (!CheckIndex(v1, t1) ||
        !CheckIndex(v2, t2) ||
        !CheckIndex(v3, t3))
    {
        std::cout << "인덱스 범위를 벗어났습니다.\n";
        return;
    }

    face[faceCount].vertexIndex[0] = v1;
    face[faceCount].textureIndex[0] = t1;

    face[faceCount].vertexIndex[1] = v2;
    face[faceCount].textureIndex[1] = t2;

    face[faceCount].vertexIndex[2] = v3;
    face[faceCount].textureIndex[2] = t3;

    face[faceCount].isTexture = true;

    ++faceCount;
}
//-------------------------------------------------------------------------------------------------
bool CheckIndex(int vertexIndex, int textureIndex)
//-------------------------------------------------------------------------------------------------
{
    if (vertexIndex < 1 || vertexIndex > vertexCount)
    {
        return false;
    }

    if (textureIndex < 1 || textureIndex > vtCount)
    {
        return false;
    }

    return true;
}
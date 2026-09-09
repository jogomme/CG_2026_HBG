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
bool CheckFace(int v1, int v2, int v3);
bool CheckVertex(); 



//-------------------------------------------------------------------------------------------------
int main()
//-------------------------------------------------------------------------------------------------
{
    std::string filename;

    std::cout << "파일 이름을 입력해 주세요 ( ex : data1, data2, data3 ) : ";
    std::cin >> filename;

    std::ifstream file(filename);

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
            std::cout << "허용되지 않는 문자입니다.";
            std::cout << line << '\n';
        }
    }

    file.close();

    if (CheckVertex())
    {
        std::cout << "중복된 정점이 있습니다.\n";
        return 2025180028;
    }


    std::ofstream resultFile("result.txt");

    if (faceCount > 0) {

        for (int i = 0; i < faceCount; ++i)
        {
            resultFile << "Face " << i + 1 << " ("
                << face[i].vertexIndex[0] << ", "
                << face[i].vertexIndex[1] << ", "
                << face[i].vertexIndex[2] << ")\n";

            std::cout << "Face " << i + 1 << " ("
                << face[i].vertexIndex[0] << ", "
                << face[i].vertexIndex[1] << ", "
                << face[i].vertexIndex[2] << ")\n";

            resultFile << "vertex ";

            std::cout << "vertex";

            for (int j = 0; j < 3; ++j)
            {
                int index = face[i].vertexIndex[j] - 1;

                resultFile << "("
                    << vertex[index].x << ", "
                    << vertex[index].y << ", "
                    << vertex[index].z << ") ";

                std::cout <<"("
                    << vertex[index].x << ", "
                    << vertex[index].y << ", "
                    << vertex[index].z << ") ";
            }


            resultFile << "\ntexture ";

            std::cout << '\n' << "texture";

            for (int j = 0; j < 3; ++j)
            {
                int index = face[i].textureIndex[j] - 1;

                resultFile << "("
                    << vt[index].x << ", "
                    << vt[index].y << ") ";

                std::cout << "("
                    << vt[index].x << ", "
                    << vt[index].y << ") ";
            }

            resultFile << "\n\n";
            std::cout << '\n' << '\n';
        }
    }
    else {
        resultFile << "No duplicate vertex value" << '\n';
    }

    resultFile.close();
}

//-------------------------------------------------------------------------------------------------
void SaveVertex(std::string line)
//-------------------------------------------------------------------------------------------------
{
    std::stringstream ss(line);

    char type;
    float x, y, z;
    float ex;

    ss >> type;

    if (!(ss >> x >> y >> z)) {
        std::cout << "형식 오류 발생 : Vertex" << '\n';
        return;
    }

    if (ss >> ex) {
        std::cout << "너무 많은 vertex 정보" << '\n';
        return;
    }

    vertex[vertexCount].x = x;
    vertex[vertexCount].y = y;
    vertex[vertexCount++].z = z;
}

//-------------------------------------------------------------------------------------------------
void SaveVT(std::string line)
//-------------------------------------------------------------------------------------------------
{
    std::stringstream ss(line);

    std::string type;

    ss >> type;

    float x, y, ex;

    if (!(ss >> x >> y)) {
        std::cout << "잘못된 형식의 데이터 : VT" << '\n';
        return;
    }

    if (ss >> ex) {
        std::cout << "너무 많은 VT 정보" << '\n';
        return;
    }

    vt[vtCount].x = x;
    vt[vtCount++].y = y;
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

    // 첫 번째 정보에 '/'가 있는지 확인
    bool isTexture = false;

    for (int i = 0; i < first.length(); ++i)
    {
        if (first[i] == '/')
        {
            isTexture = true;
            break;
        }
    }

    // ---------------------------------------------------------
    // 텍스처가 있는 경우 : f 1/1 2/2 3/3
    // ---------------------------------------------------------
    if (isTexture)
    {
        // '/'를 공백으로 변경
        for (int i = 0; i < first.length(); ++i)
        {
            if (first[i] == '/')
            {
                first[i] = ' ';
            }
        }

        for (int i = 0; i < second.length(); ++i)
        {
            if (second[i] == '/')
            {
                second[i] = ' ';
            }
        }

        for (int i = 0; i < third.length(); ++i)
        {
            if (third[i] == '/')
            {
                third[i] = ' ';
            }
        }

        std::stringstream firstStream(first);
        std::stringstream secondStream(second);
        std::stringstream thirdStream(third);

        if (!(firstStream >> v1 >> t1))
        {
            std::cout << "형식 오류 : Face\n";
            return;
        }

        if (!(secondStream >> v2 >> t2))
        {
            std::cout << "형식 오류 : Face\n";
            return;
        }

        if (!(thirdStream >> v3 >> t3))
        {
            std::cout << "형식 오류 : Face\n";
            return;
        }

        // Vertex + Texture 인덱스 범위 검사
        if (!CheckIndex(v1, t1) ||
            !CheckIndex(v2, t2) ||
            !CheckIndex(v3, t3))
        {
            std::cout << "인덱스 범위 초과\n";
            return;
        }
    }

    // ---------------------------------------------------------
    // 텍스처가 없는 경우 : f 1 2 3
    // ---------------------------------------------------------
    else
    {
        std::stringstream firstStream(first);
        std::stringstream secondStream(second);
        std::stringstream thirdStream(third);

        if (!(firstStream >> v1))
        {
            std::cout << "형식 오류 : Face\n";
            return;
        }

        if (!(secondStream >> v2))
        {
            std::cout << "형식 오류 : Face\n";
            return;
        }

        if (!(thirdStream >> v3))
        {
            std::cout << "형식 오류 : Face\n";
            return;
        }

        // Texture는 없음
        t1 = 0;
        t2 = 0;
        t3 = 0;

        // Vertex 인덱스만 범위 검사
        if (v1 < 1 || v1 > vertexCount ||
            v2 < 1 || v2 > vertexCount ||
            v3 < 1 || v3 > vertexCount)
        {
            std::cout << "인덱스 범위 초과\n";
            return;
        }
    }

    // 삼각형 정점 중복 확인
    if (!CheckFace(v1, v2, v3))
    {
        std::cout << "삼각형의 정점이 중복\n";
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

//-------------------------------------------------------------------------------------------------
bool CheckFace(int v1, int v2, int v3)
//-------------------------------------------------------------------------------------------------
{
    if (v1 == v2 || v1 == v3 || v2 == v3)
    {
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
bool CheckVertex()
//-------------------------------------------------------------------------------------------------
{
    for (int i = 0; i < vertexCount; ++i)
    {
        for (int j = i + 1; j < vertexCount; ++j)
        {
            if (vertex[i].x == vertex[j].x &&
                vertex[i].y == vertex[j].y &&
                vertex[i].z == vertex[j].z)
            {
                return true;
            }
        }
    }

    return false;
}
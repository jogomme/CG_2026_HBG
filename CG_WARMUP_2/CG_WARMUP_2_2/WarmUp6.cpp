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
        if (line[0] == '#')
        {
            std::cout << "주석입니다.\n";
        }
        else if (line[0] == 'v' && line[1] == 't')
        {
            std::cout << "텍스처 좌표입니다.\n";
            SaveVT(line);
        }
        else if (line[0] == 'v')
        {
            std::cout << "정점 좌표입니다.\n";
            SaveVertex(line);
        }
        else if (line[0] == 'f')
        {
            std::cout << "삼각형입니다.\n";
            SaveF(line);
        }
    }

    file.close();
}

//-------------------------------------------------------------------------------------------------
void SaveVertex(std::string line)
//-------------------------------------------------------------------------------------------------
{
    std::stringstream ss(line);

    char type;

    ss >> type >> vertex[vertexCount].x >> vertex[vertexCount].y >> vertex[vertexCount++].z;
}

//-------------------------------------------------------------------------------------------------
void SaveVT(std::string line)
//-------------------------------------------------------------------------------------------------
{
    std::stringstream ss(line);

    std::string type;

    ss >> type >> vt[vtCount].x >> vt[vtCount++].y;
}

//-------------------------------------------------------------------------------------------------
void SaveF(std::string line)
//-------------------------------------------------------------------------------------------------
{
    for (int i = 0; i < line.length(); ++i) {
        if (line[i] == '/') {
            line[i] = ' ';
        }
    }

    std::stringstream ss(line);

    char type;

    ss >> type 
        >> face[faceCount].vertexIndex[0] >> face[faceCount].textureIndex[0]
        >> face[faceCount].vertexIndex[1] >> face[faceCount].textureIndex[1]
        >> face[faceCount].vertexIndex[2] >> face[faceCount].textureIndex[2];

    face[faceCount++].isTexture = true;

}
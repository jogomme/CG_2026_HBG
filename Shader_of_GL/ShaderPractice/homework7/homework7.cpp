#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include<random>
#include<cmath>

//------------------------------------------------------------------------------------------
// 랜덤 엔진
//------------------------------------------------------------------------------------------
std::random_device rd;
std::mt19937 gen(rd());

std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);
std::uniform_real_distribution<float> positionDist(-0.8f, 0.8f);
std::uniform_real_distribution<float> sizeDist(0.15f, 0.35f);


//------------------------------------------------------------------------------------------
// 구조체 선언
//------------------------------------------------------------------------------------------
struct POINT
{
    float x;
    float y;
    float z{ 0 };
};

struct COLOR
{
    float r;
    float g;
    float b;
};

struct SHAPE_TYPE
{
    int type;

    POINT point[4];

    COLOR color;

    bool selected{ false };
};

//------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------

void MakeVertexData(int index);

void AddShape(int type);

//------------------------------------------------------------------------------------------
// 전역 변수 
//------------------------------------------------------------------------------------------

// 0 : 점, 1 : 선, 2 : 삼각형, 3 : 사각형
const int shape_index[] =
{
    1, 2, 3, 4
};

SHAPE_TYPE types[50]{};
int ShapeCount{ 0 };

int selectedRect{ -1 };
bool isDragging{ false };

GLuint VAO;
GLuint VBO;


//------------------------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------------------------
{
    // GLFW 초기화
    if (!glfwInit())
    {
        std::cout << "GLFW 초기화 실패\n";
        return -1;
    }

    // OpenGL 3.3 설정
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 창 생성
    GLFWwindow* window = glfwCreateWindow(
        1600, 1200,
        "OpenGL Practice 7",
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cout << "Window 생성 실패\n";
        glfwTerminate();
        return -1;
    }

    // OpenGL Context 생성
    glfwMakeContextCurrent(window);

    // GLEW 초기화
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW 초기화 실패\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // 화면 크기 설정
    glViewport(0, 0, 1600, 1200);

    // --------------------------------------------------
    // 여기부터 실습 7 구현
    // --------------------------------------------------

    float vertexs[] =
    {
         -0.5f, -0.5f, 0.0f,
          0.5f, -0.5f, 0.0f,
          0.0f,  0.5f, 0.0f,
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexs), vertexs, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);

    const char* vertexShaderSource = R"(
    #version 330 core

    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;

    out vec3 ourColor;

    void main()
    {
        gl_Position = vec4(aPos, 1.0);
        ourColor = aColor;
    }
    
    )";

    const char* fragmentShaderSource = R"(
    #version 330 core
    
    in vec3 ourColor;
    
    out vec4 FragColor;
    
    void main()
    {
        FragColor = vec4(ourColor, 1.0);
    }
    )";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);

    glCompileShader(vertexShader);



    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);

    glCompileShader(fragmentShader);


    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);

    // --------------------------------------------------
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    AddShape(0);
    AddShape(1);
    AddShape(2);
    AddShape(3);

    // 메인 루프
    while (!glfwWindowShouldClose(window))
    {
        // 화면 지우기
        glClear(GL_COLOR_BUFFER_BIT);

        // --------------------------------------------------
        // 여기에서 네가 그리기 구현
        // --------------------------------------------------

        glBindVertexArray(VAO);

        for (int i = 0; i < ShapeCount; ++i)
        {
            MakeVertexData(i);

            int type = types[i].type;
            int count = shape_index[type];

            if (type == 0)
            {
                glPointSize(10.0f);
                // 윤곽선 색으로 그리기
                glDrawArrays(GL_POINTS, 0, 1);

                glPointSize(6.0f);
                // 원래 색으로 다시 그리기
                glDrawArrays(GL_POINTS, 0, 1);
            }
            else if (type == 1)
            {
                glDrawArrays(GL_LINES, 0, count);
            }
            else if (type == 2)
            {
                glDrawArrays(GL_TRIANGLES, 0, count);
            }
            else if (type == 3)
            {
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }

        // --------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 종료
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

//------------------------------------------------------------------------------------------
// 함수 구현
//------------------------------------------------------------------------------------------

void MakeVertexData(int index)
{
    int type = types[index].type;
    int count = shape_index[type];

    float vertexData[36]{};

    if (type == 3)
    {
        int triangleIndex[] =
        {
            0, 1, 2,
            0, 2, 3
        };

        for (int i = 0; i < 6; ++i)
        {
            int pointIndex = triangleIndex[i];

            vertexData[i * 6] = types[index].point[pointIndex].x;
            vertexData[i * 6 + 1] = types[index].point[pointIndex].y;
            vertexData[i * 6 + 2] = types[index].point[pointIndex].z;

            vertexData[i * 6 + 3] = types[index].color.r;
            vertexData[i * 6 + 4] = types[index].color.g;
            vertexData[i * 6 + 5] = types[index].color.b;
        }

        count = 6;
    }
    else
    {
        for (int i = 0; i < count; ++i)
        {
            vertexData[i * 6] = types[index].point[i].x;
            vertexData[i * 6 + 1] = types[index].point[i].y;
            vertexData[i * 6 + 2] = types[index].point[i].z;

            vertexData[i * 6 + 3] = types[index].color.r;
            vertexData[i * 6 + 4] = types[index].color.g;
            vertexData[i * 6 + 5] = types[index].color.b;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        count * 6 * sizeof(float),
        vertexData,
        GL_DYNAMIC_DRAW
    );
}

void AddShape(int type)
{
    if (ShapeCount >= 50) {
        std::cout << "갯수 초과 " << '\n';
        return;
    }

    SHAPE_TYPE& shape = types[ShapeCount];

    shape.type = type;

    shape.color.r = colorDist(gen);
    shape.color.g = colorDist(gen);
    shape.color.b = colorDist(gen);

    // 도형별 정점 저장
    if (type == 0)
    {
        // 점
        shape.point[0].x = positionDist(gen);
        shape.point[0].y = positionDist(gen);
    }
    else if (type == 1)
    {
        // 선
        for (int i = 0; i < 2; ++i) {
            shape.point[i].x = positionDist(gen);
            shape.point[i].y = positionDist(gen);
        }
    }
    else if (type == 2)
    {
        float x = positionDist(gen);
        float y = positionDist(gen);

        float size = sizeDist(gen);

        POINT center;
        center.x = x;
        center.y = y;

        float angle = 3.14159265f / 2.0f;

        // 첫 번째 점
        shape.point[0].x = center.x + cos(angle) * size;
        shape.point[0].y = center.y + sin(angle) * size;

        // 두 번째 점
        angle += 2.0f * 3.14159265f / 3.0f;

        shape.point[1].x = center.x + cos(angle) * size;
        shape.point[1].y = center.y + sin(angle) * size;

        // 세 번째 점
        angle += 2.0f * 3.14159265f / 3.0f;

        shape.point[2].x = center.x + cos(angle) * size;
        shape.point[2].y = center.y + sin(angle) * size;
    }
    else if (type == 3)
    {
        // 사각형의 대각선 양 끝점
        float x1 = positionDist(gen);
        float y1 = positionDist(gen);

        float x2 = positionDist(gen);
        float y2 = positionDist(gen);

        // 좌표 순서 정리
        if (x1 > x2)
        {
            float temp = x1;
            x1 = x2;
            x2 = temp;
        }

        if (y1 > y2)
        {
            float temp = y1;
            y1 = y2;
            y2 = temp;
        }

        // 사각형의 네 꼭짓점
        shape.point[0] = { x1, y1, 0.0f };
        shape.point[1] = { x2, y1, 0.0f };
        shape.point[2] = { x2, y2, 0.0f };
        shape.point[3] = { x1, y2, 0.0f };
    }

    ShapeCount++;

}
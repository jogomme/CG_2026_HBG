#include <GL/glew.h>
#include <GL/glfw3.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>

//------------------------------------------------------------------------------------------------------
// 랜덤 엔진
//------------------------------------------------------------------------------------------------------
std::random_device rd;
std::mt19937 gen(rd());

std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);

//------------------------------------------------------------------------------------------------------
// 구조체 선언
//------------------------------------------------------------------------------------------------------

struct ShapeType {
    // 0 : 사각형
    // 1 : 삼각형
    // 2 : 직각 삼각형
    int type;

    // 오른쪽 타일이랑 왼쪽 내가 끌고가는게 같아야 함
    int MatchType;

    // vertexs 의 몇 번째 도형을 사용할 지에 관한 내용
    int vertexIndex;

    // 정점의 중점 데이터
    glm::vec2 position;

    // 정점의 color 데이터
    glm::vec3 color;

    // 선택 된 것인지 확인
    bool selected{ false };

    // 완료 된 것인지 확인
    bool completed{ false };

};

enum class ShapeSide
{
    Left,
    Right
};


//------------------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------------------

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

// 시작 함수
void StartMap(ShapeType ls[], ShapeType rs[]);

void DrawCenterLine(GLuint modelLocation);

glm::mat4 MakeModelMatrix(ShapeType& shape);

void DrawScenes(ShapeType& shape, GLuint modelLocation, ShapeSide side);

void MakeVertexData(ShapeType& shape);

bool IsOverlap(glm::vec4 a, glm::vec4 b);

glm::vec2 GetRandomBoardPosition(glm::vec2 halfSize, glm::vec4 boards[], int boardCount);

int GetShapeType(int vertexIndex);

glm::vec2 GetHalfSize(int vertexIndex, int type);

glm::vec2 GetMousePosition(GLFWwindow* window);

bool IsInsideShape(ShapeType& shape, glm::vec2 point);

//------------------------------------------------------------------------------------------------------
// 정점의 위치 데이터
//------------------------------------------------------------------------------------------------------

glm::vec2 vertexs[20][4] = {
    // ------------------------------------------------------------
    // 위쪽 모양판 : 직사각형 4개
    // ------------------------------------------------------------

    // 0
    {
        {-0.08f,  0.06f},
        { 0.08f,  0.06f},
        { 0.08f, -0.06f},
        {-0.08f, -0.06f}
    },

    // 1
    {
        {-0.10f,  0.07f},
        { 0.10f,  0.07f},
        { 0.10f, -0.07f},
        {-0.10f, -0.07f}
    },

    // 2
    {
        {-0.12f,  0.08f},
        { 0.12f,  0.08f},
        { 0.12f, -0.08f},
        {-0.12f, -0.08f}
    },

    // 3
    {
        {-0.14f,  0.09f},
        { 0.14f,  0.09f},
        { 0.14f, -0.09f},
        {-0.14f, -0.09f}
    },


    // ------------------------------------------------------------
    // 가운데 모양판 : 삼각형 4개 + 가운데 사각형
    // ------------------------------------------------------------

    // 4 : 위쪽 삼각형
    {
        {-0.10f, -0.02f},
        { 0.10f, -0.02f},
        { 0.00f,  0.15f},
        { 0.00f,  0.15f}
    },

    // 5 : 오른쪽 삼각형
    {
        {-0.02f,  0.10f},
        {-0.02f, -0.10f},
        { 0.15f,  0.00f},
        { 0.15f,  0.00f}
    },

    // 6 : 아래쪽 삼각형
    {
        {-0.10f,  0.02f},
        { 0.10f,  0.02f},
        { 0.00f, -0.15f},
        { 0.00f, -0.15f}
    },

    // 7 : 왼쪽 삼각형
    {
        { 0.02f,  0.10f},
        { 0.02f, -0.10f},
        {-0.15f,  0.00f},
        {-0.15f,  0.00f}
    },

    // 8 : 가운데 사각형
    {
        {-0.1f,  0.1f},
        { 0.1f,  0.1f},
        { 0.1f, -0.1f},
        {-0.1f, -0.1f}
    },


    // ------------------------------------------------------------
    // 아래쪽 모양판 : 직각삼각형 2개
    // ------------------------------------------------------------

    // 9 : 왼쪽 직각삼각형
    {
        {-0.12f,  0.20f},
        { 0.12f,  0.20f},
        {-0.12f, -0.20f},
        {-0.12f, -0.20f}
    },

    // 10 : 오른쪽 직각삼각형
    {
        { 0.12f,  0.20f},
        { 0.12f, -0.20f},
        {-0.12f, -0.20f},
        {-0.12f, -0.20f}
    },


    // ------------------------------------------------------------
    // 사용자 정의 모형
    // ------------------------------------------------------------

    // 11
    {
        {-0.08f,  0.20f},
        { 0.08f,  0.20f},
        { 0.08f, -0.20f},
        {-0.08f, -0.20f}
    },

    // 12
    {
        {-0.20f,  0.08f},
        { 0.20f,  0.08f},
        { 0.20f, -0.08f},
        {-0.20f, -0.08f}
    },

    // 13 : 정삼각형
    {
        { 0.00f,  0.20f},
        {-0.18f, -0.10f},
        { 0.18f, -0.10f},
        { 0.18f, -0.10f}
    },

    // 14 : 역삼각형
    {
        { 0.00f, -0.20f},
        {-0.18f,  0.10f},
        { 0.18f,  0.10f},
        { 0.18f,  0.10f}
    },

    // 15 : 오른쪽 삼각형
    {
        {-0.15f,  0.10f},
        {-0.15f, -0.10f},
        { 0.18f,  0.00f},
        { 0.18f,  0.00f}
    },

    // 16 : 왼쪽 삼각형
    {
        { 0.15f,  0.10f},
        { 0.15f, -0.10f},
        {-0.18f,  0.00f},
        {-0.18f,  0.00f}
    },

    // 17 : 마름모
    {
        { 0.00f,  0.18f},
        { 0.18f,  0.00f},
        { 0.00f, -0.18f},
        {-0.18f,  0.00f}
    },

    // 18 : 평행사변형
    {
        {-0.15f,  0.10f},
        { 0.10f,  0.10f},
        { 0.15f, -0.10f},
        {-0.10f, -0.10f}
    },

    // 19 : 사다리꼴
    {
        {-0.10f,  0.15f},
        { 0.10f,  0.15f},
        { 0.18f, -0.15f},
        {-0.18f, -0.15f}
    }
};

//------------------------------------------------------------------------------------------------------
// 전역 변수
//------------------------------------------------------------------------------------------------------

int wide = 1600;
int height = 1200;

int mid = wide / 2;

// 타입에 따른 정점 갯수
// 사각형 0 -> 4
// 삼각형 1 -> 3
// 직각 삼각형 2 -> 3
int vertexCount[3]{ 4, 3, 3 };

GLuint VAO;
GLuint VBO;

// 왼쪽 도형 13개
ShapeType LS[13];

// 오른쪽 도형 13개
ShapeType RS[13];

int selectedShape = -1;
glm::vec2 dragOffset;

//------------------------------------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------------------------------------
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
        wide, height,
        "OpenGL Practice 10",
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
    glViewport(0, 0, wide, height);

    // 마우스, 키 콜 백 함수
    glfwSetMouseButtonCallback(window, MouseButtonCallback);

    glfwSetKeyCallback(window, KeyCallback);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * 6 * 4,
        nullptr,
        GL_DYNAMIC_DRAW
    );

    // 위치
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // 색상
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);

    //------------------------------------------------------------------------------------------
    // Vertex Shader
    //------------------------------------------------------------------------------------------
    const char* vertexShaderSource = R"(
    #version 330 core
    
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    
    uniform mat4 model;
    
    out vec3 ourColor;
    
    void main()
    {
        gl_Position = model * vec4(aPos, 1.0);
        ourColor = aColor;
    }
    )";

    //------------------------------------------------------------------------------------------
    // Fragment Shader
    //------------------------------------------------------------------------------------------
    const char* fragmentShaderSource = R"(
    #version 330 core

    in vec3 ourColor;

    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(ourColor, 1.0);
    }
    )";

    //------------------------------------------------------------------------------------------
    // Vertex Shader 생성
    //------------------------------------------------------------------------------------------
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        vertexShader,
        1,
        &vertexShaderSource,
        nullptr
    );

    glCompileShader(vertexShader);

    //------------------------------------------------------------------------------------------
    // Fragment Shader 생성
    //------------------------------------------------------------------------------------------
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        fragmentShader,
        1,
        &fragmentShaderSource,
        nullptr
    );

    glCompileShader(fragmentShader);

    //------------------------------------------------------------------------------------------
    // Shader Program 생성
    //------------------------------------------------------------------------------------------
    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);

    glClearColor(
        1.0f,
        1.0f,
        1.0f,
        1.0f
    );

    GLuint modelLocation = glGetUniformLocation(shaderProgram, "model");

    StartMap(LS, RS);

    //------------------------------------------------------------------------------------------
    // 메인 루프
    //------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // 화면 지우기
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);

        DrawCenterLine(modelLocation);

        if (selectedShape != -1) {
            glm::vec2 mousePosition = GetMousePosition(window);
            LS[selectedShape].position = mousePosition + dragOffset;
        }

        for (int i = 0; i < 13; ++i) {
            DrawScenes(LS[i], modelLocation, ShapeSide::Left);
            DrawScenes(RS[i], modelLocation, ShapeSide::Right);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 종료
    glfwDestroyWindow(window);
    glfwTerminate();

}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    glm::vec2 mousePosition = GetMousePosition(window);

    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
        for (int i = 12; i >= 0; --i) {
            if (LS[i].completed) {
                continue;
            }

            if (IsInsideShape(LS[i], mousePosition)) {
                selectedShape = i;

                for (int j = 0; j < 13; ++j) {
                    LS[j].selected = false;
                }

                LS[i].selected = true;

                dragOffset = LS[i].position - mousePosition;

                break;
            }

        }
    }
    else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
        if (selectedShape == -1) {
            return;
        }

        ShapeType& shape = LS[selectedShape];

        for (int i = 0; i < 13; ++i) {
            if (RS[i].MatchType != shape.MatchType) {
                continue;
            }

            if (RS[i].completed) {
                continue;
            }

            float dis = glm::distance(shape.position, RS[i].position);

            if (dis < 0.05) {
                shape.position = RS[i].position;
                shape.completed = true;
                RS[i].completed = true;
            }

            break;

        }

        shape.selected = false;
        selectedShape = -1;

    }

}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        StartMap(LS, RS);
    }
    else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        glfwDestroyWindow(window);
    }
}

glm::vec2 GetMousePosition(GLFWwindow* window)
{
    double mouseX;
    double mouseY;

    glfwGetCursorPos(window, &mouseX, &mouseY);

    float x = static_cast<float>(mouseX) / wide * 2.0f - 1.0f;
    float y = 1.0f - static_cast<float>(mouseY) / height * 2.0f;

    return glm::vec2(x, y);
}

bool IsInsideShape(ShapeType& shape, glm::vec2 point)
{
    int count = vertexCount[shape.type];
    int sign = 0;

    for (int i = 0; i < count; ++i)
    {
        int next = (i + 1) % count;

        glm::vec2 a = vertexs[shape.vertexIndex][i] + shape.position;
        glm::vec2 b = vertexs[shape.vertexIndex][next] + shape.position;

        float cross =
            (b.x - a.x) * (point.y - a.y) -
            (b.y - a.y) * (point.x - a.x);

        if (cross > 0.0f)
        {
            if (sign < 0)
                return false;

            sign = 1;
        }
        else if (cross < 0.0f)
        {
            if (sign > 0)
                return false;

            sign = -1;
        }
    }

    return true;
}

glm::vec2 GetHalfSize(int vertexIndex, int type)
{
    float maxX = 0.0f;
    float maxY = 0.0f;

    int count = vertexCount[type];

    for (int i = 0; i < count; ++i)
    {
        maxX = std::max(maxX, std::abs(vertexs[vertexIndex][i].x));
        maxY = std::max(maxY, std::abs(vertexs[vertexIndex][i].y));
    }

    return glm::vec2(maxX, maxY);
}

glm::mat4 MakeModelMatrix(ShapeType& shape)
{
    glm::mat4 model(1.0f);

    model = glm::translate(model, glm::vec3(shape.position, 0.0f));

    return model;
}

void DrawScenes(ShapeType& shape, GLuint modelLocation, ShapeSide side)
{
    MakeVertexData(shape);

    glm::mat4 model = MakeModelMatrix(shape);

    glUniformMatrix4fv(
        modelLocation,
        1,
        GL_FALSE,
        &model[0][0]
    );

    if (side == ShapeSide::Left) {
        glDrawArrays(
            GL_TRIANGLE_FAN,
            0,
            vertexCount[shape.type]
        );
    }
    else {
        glLineWidth(3.0f);

        glDrawArrays(
            GL_LINE_LOOP,
            0,
            vertexCount[shape.type]
        );
    }
}

void MakeVertexData(ShapeType& shape)
{
    float vertexData[24]{};

    int count = vertexCount[shape.type];

    for (int i = 0; i < count; ++i)
    {
        vertexData[i * 6 + 0] = vertexs[shape.vertexIndex][i].x;
        vertexData[i * 6 + 1] = vertexs[shape.vertexIndex][i].y;
        vertexData[i * 6 + 2] = 0.0f;

        vertexData[i * 6 + 3] = shape.color.r;
        vertexData[i * 6 + 4] = shape.color.g;
        vertexData[i * 6 + 5] = shape.color.b;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(float) * 6 * count,
        vertexData
    );
}

bool IsOverlap(glm::vec4 a, glm::vec4 b)
{
    if (a.y < b.x) return false;
    if (a.x > b.y) return false;
    if (a.w < b.z) return false;
    if (a.z > b.w) return false;

    return true;
}

glm::vec2 GetRandomBoardPosition(
    glm::vec2 halfSize,
    glm::vec4 boards[],
    int boardCount
)
{
    const float gap = 0.03f;
    const int maxAttempt = 10000;

    std::uniform_real_distribution<float> xDist(
        halfSize.x + gap,
        1.0f - halfSize.x - gap
    );

    std::uniform_real_distribution<float> yDist(
        -1.0f + halfSize.y + gap,
        1.0f - halfSize.y - gap
    );

    // 랜덤 위치 탐색
    for (int attempt = 0; attempt < maxAttempt; ++attempt)
    {
        glm::vec2 position(
            xDist(gen),
            yDist(gen)
        );

        glm::vec4 newBoard(
            position.x - halfSize.x - gap,
            position.x + halfSize.x + gap,
            position.y - halfSize.y - gap,
            position.y + halfSize.y + gap
        );

        bool overlap = false;

        for (int i = 0; i < boardCount; ++i)
        {
            if (IsOverlap(newBoard, boards[i]))
            {
                overlap = true;
                break;
            }
        }

        if (!overlap)
        {
            boards[boardCount] = newBoard;
            return position;
        }
    }

    // 랜덤으로 찾지 못하면 일정한 간격으로 다시 탐색
    const float step = 0.02f;

    for (float x = halfSize.x + gap;
        x <= 1.0f - halfSize.x - gap;
        x += step)
    {
        for (float y = -1.0f + halfSize.y + gap;
            y <= 1.0f - halfSize.y - gap;
            y += step)
        {
            glm::vec2 position(x, y);

            glm::vec4 newBoard(
                position.x - halfSize.x - gap,
                position.x + halfSize.x + gap,
                position.y - halfSize.y - gap,
                position.y + halfSize.y + gap
            );

            bool overlap = false;

            for (int i = 0; i < boardCount; ++i)
            {
                if (IsOverlap(newBoard, boards[i]))
                {
                    overlap = true;
                    break;
                }
            }

            if (!overlap)
            {
                boards[boardCount] = newBoard;
                return position;
            }
        }
    }

    // 현재 배치에서는 들어갈 공간이 없음
    return glm::vec2(-2.0f);
}

int GetShapeType(int vertexIndex)
{
    if (vertexIndex >= 13 && vertexIndex <= 16)
    {
        return 1;
    }

    return 0;
}

void StartMap(ShapeType ls[], ShapeType rs[])
{
    // ------------------------------------------------------------
    // 왼쪽 조각
    // ------------------------------------------------------------

    // 사각형 4개
    for (int i = 0; i < 4; ++i)
    {
        ls[i].type = 0;
        ls[i].MatchType = i;
        ls[i].vertexIndex = 0;
        ls[i].position = glm::vec2(-0.7f, 0.7f - i * 0.18f);
        ls[i].color = glm::vec3(
            colorDist(gen),
            colorDist(gen),
            colorDist(gen)
        );
        ls[i].selected = false;
        ls[i].completed = false;
    }

    // 삼각형 4개
    for (int i = 0; i < 4; ++i)
    {
        int index = 4 + i;

        ls[index].type = 1;
        ls[index].MatchType = index;
        ls[index].vertexIndex = 4 + i;
        ls[index].position = glm::vec2(
            -0.4f,
            0.7f - i * 0.18f
        );
        ls[index].color = glm::vec3(
            colorDist(gen),
            colorDist(gen),
            colorDist(gen)
        );
        ls[index].selected = false;
        ls[index].completed = false;
    }

    // 가운데 사각형
    ls[8].type = 0;
    ls[8].MatchType = 8;
    ls[8].vertexIndex = 8;
    ls[8].position = glm::vec2(-0.1f, 0.3f);
    ls[8].color = glm::vec3(
        colorDist(gen),
        colorDist(gen),
        colorDist(gen)
    );
    ls[8].selected = false;
    ls[8].completed = false;

    // 직각삼각형 2개
    for (int i = 0; i < 2; ++i)
    {
        int index = 9 + i;

        ls[index].type = 2;
        ls[index].MatchType = index;
        ls[index].vertexIndex = 9 + i;
        ls[index].position = glm::vec2(
            -0.7f,
            -0.2f - i * 0.3f
        );
        ls[index].color = glm::vec3(
            colorDist(gen),
            colorDist(gen),
            colorDist(gen)
        );
        ls[index].selected = false;
        ls[index].completed = false;
    }

    // 사용자 정의 도형 11
    ls[11].type = 0;
    ls[11].MatchType = 11;
    ls[11].vertexIndex = 11;
    ls[11].position = glm::vec2(-0.4f, -0.25f);
    ls[11].color = glm::vec3(
        colorDist(gen),
        colorDist(gen),
        colorDist(gen)
    );
    ls[11].selected = false;
    ls[11].completed = false;

    // 사용자 정의 도형 12
    ls[12].type = 0;
    ls[12].MatchType = 12;
    ls[12].vertexIndex = 12;
    ls[12].position = glm::vec2(-0.1f, -0.25f);
    ls[12].color = glm::vec3(
        colorDist(gen),
        colorDist(gen),
        colorDist(gen)
    );
    ls[12].selected = false;
    ls[12].completed = false;


    // ------------------------------------------------------------
    // 오른쪽 모양판
    // ------------------------------------------------------------

    glm::vec2 board0;
    glm::vec2 board1;
    glm::vec2 board2;
    glm::vec2 board3;
    glm::vec2 board4;

    // 랜덤 배치가 불가능한 경우 전체 보드 다시 배치
    while (true)
    {
        glm::vec4 boards[5]{};

        // ------------------------------------------------------------
        // 첫 번째 모양판
        // 사각형 4개
        // ------------------------------------------------------------

        board0 = GetRandomBoardPosition(
            glm::vec2(0.16f, 0.12f),
            boards,
            0
        );

        if (board0.x < -1.5f)
            continue;


        // ------------------------------------------------------------
        // 두 번째 모양판
        // 삼각형 4개 + 가운데 사각형
        // ------------------------------------------------------------

        board1 = GetRandomBoardPosition(
            glm::vec2(0.27f, 0.27f),
            boards,
            1
        );

        if (board1.x < -1.5f)
            continue;


        // ------------------------------------------------------------
        // 세 번째 모양판
        // 직각삼각형 2개
        // ------------------------------------------------------------

        board2 = GetRandomBoardPosition(
            glm::vec2(0.12f, 0.20f),
            boards,
            2
        );

        if (board2.x < -1.5f)
            continue;


        // ------------------------------------------------------------
        // 사용자 정의 도형 11
        // ------------------------------------------------------------

        board3 = GetRandomBoardPosition(
            glm::vec2(0.08f, 0.20f),
            boards,
            3
        );

        if (board3.x < -1.5f)
            continue;


        // ------------------------------------------------------------
        // 사용자 정의 도형 12
        // ------------------------------------------------------------

        board4 = GetRandomBoardPosition(
            glm::vec2(0.20f, 0.08f),
            boards,
            4
        );

        if (board4.x < -1.5f)
            continue;

        break;
    }


    // ------------------------------------------------------------
    // 첫 번째 모양판
    // 사각형 4개
    // ------------------------------------------------------------

    rs[0].type = 0;
    rs[0].MatchType = 0;
    rs[0].vertexIndex = 0;
    rs[0].position = board0 + glm::vec2(-0.08f, 0.06f);
    rs[0].color = glm::vec3(0.3f);
    rs[0].selected = false;
    rs[0].completed = false;

    rs[1].type = 0;
    rs[1].MatchType = 1;
    rs[1].vertexIndex = 0;
    rs[1].position = board0 + glm::vec2(0.08f, 0.06f);
    rs[1].color = glm::vec3(0.3f);
    rs[1].selected = false;
    rs[1].completed = false;

    rs[2].type = 0;
    rs[2].MatchType = 2;
    rs[2].vertexIndex = 0;
    rs[2].position = board0 + glm::vec2(-0.08f, -0.06f);
    rs[2].color = glm::vec3(0.3f);
    rs[2].selected = false;
    rs[2].completed = false;

    rs[3].type = 0;
    rs[3].MatchType = 3;
    rs[3].vertexIndex = 0;
    rs[3].position = board0 + glm::vec2(0.08f, -0.06f);
    rs[3].color = glm::vec3(0.3f);
    rs[3].selected = false;
    rs[3].completed = false;


    // ------------------------------------------------------------
    // 두 번째 모양판
    // 삼각형 4개 + 가운데 사각형
    // ------------------------------------------------------------

    rs[4].type = 1;
    rs[4].MatchType = 4;
    rs[4].vertexIndex = 4;
    rs[4].position = board1 + glm::vec2(0.0f, 0.12f);
    rs[4].color = glm::vec3(0.3f);
    rs[4].selected = false;
    rs[4].completed = false;

    rs[5].type = 1;
    rs[5].MatchType = 5;
    rs[5].vertexIndex = 5;
    rs[5].position = board1 + glm::vec2(0.12f, 0.0f);
    rs[5].color = glm::vec3(0.3f);
    rs[5].selected = false;
    rs[5].completed = false;

    rs[6].type = 1;
    rs[6].MatchType = 6;
    rs[6].vertexIndex = 6;
    rs[6].position = board1 + glm::vec2(0.0f, -0.12f);
    rs[6].color = glm::vec3(0.3f);
    rs[6].selected = false;
    rs[6].completed = false;

    rs[7].type = 1;
    rs[7].MatchType = 7;
    rs[7].vertexIndex = 7;
    rs[7].position = board1 + glm::vec2(-0.12f, 0.0f);
    rs[7].color = glm::vec3(0.3f);
    rs[7].selected = false;
    rs[7].completed = false;

    rs[8].type = 0;
    rs[8].MatchType = 8;
    rs[8].vertexIndex = 8;
    rs[8].position = board1;
    rs[8].color = glm::vec3(0.3f);
    rs[8].selected = false;
    rs[8].completed = false;


    // ------------------------------------------------------------
    // 세 번째 모양판
    // 직각삼각형 2개
    // ------------------------------------------------------------

    rs[9].type = 2;
    rs[9].MatchType = 9;
    rs[9].vertexIndex = 9;
    rs[9].position = board2;
    rs[9].color = glm::vec3(0.3f);
    rs[9].selected = false;
    rs[9].completed = false;

    rs[10].type = 2;
    rs[10].MatchType = 10;
    rs[10].vertexIndex = 10;
    rs[10].position = board2;
    rs[10].color = glm::vec3(0.3f);
    rs[10].selected = false;
    rs[10].completed = false;


    // ------------------------------------------------------------
    // 사용자 정의 도형 2개
    // ------------------------------------------------------------

    rs[11].type = 0;
    rs[11].MatchType = 11;
    rs[11].vertexIndex = 11;
    rs[11].position = board3;
    rs[11].color = glm::vec3(0.3f);
    rs[11].selected = false;
    rs[11].completed = false;

    rs[12].type = 0;
    rs[12].MatchType = 12;
    rs[12].vertexIndex = 12;
    rs[12].position = board4;
    rs[12].color = glm::vec3(0.3f);
    rs[12].selected = false;
    rs[12].completed = false;
}

void DrawCenterLine(GLuint modelLocation)
{
    float vertexData[12] =
    {
        0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(vertexData),
        vertexData
    );

    glm::mat4 model(1.0f);

    glUniformMatrix4fv(
        modelLocation,
        1,
        GL_FALSE,
        &model[0][0]
    );

    glLineWidth(3.0f);

    glDrawArrays(
        GL_LINES,
        0,
        2
    );
}
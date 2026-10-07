#include <GL/glew.h>
#include <GL/glfw3.h>
#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>

#include <iostream>
#include<random>
#include <cmath>


int wide = 1200;
int height = 1200;

int BoardX = 30;
int BoardY = 30;    

//------------------------------------------------------------------------------------------------------
// 랜덤 엔진
//------------------------------------------------------------------------------------------------------
std::random_device rd;
std::mt19937 gen(rd());

std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);
std::uniform_int_distribution<int> typeDist(1,3);
std::uniform_int_distribution<int> rowDist(1, BoardY - 1);
std::uniform_int_distribution<int> colDist(1, BoardX - 1);

//------------------------------------------------------------------------------------------------------
// 구조체 선언
//------------------------------------------------------------------------------------------------------

struct Obstacle
{
    int row{-1};
    int col{-1};
    int type;
    float size;
    glm::vec3 color;

    int vertexStart{};
    int vertexCount{};
};

//------------------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------------------


// 셰이더 컴파일 / 링크 결과 확인
bool CheckShader(GLuint shader);

bool CheckProgram(GLuint program);

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void SetAll();

void SetMap();

void SetPlayer();

void SetObstacleCount();

void SetObstacle();

void MovePlayer();

int CheckAttack();

//------------------------------------------------------------------------------------------------------
// 정점의 위치 데이터
//------------------------------------------------------------------------------------------------------

// 위치 3개 + 색상 3개 = 한 정점 6 float
float vertices[] = {
    // ------------------------------------------------------------
    // 삼각형
    // ------------------------------------------------------------

    // 0 : 왼쪽 아래
    -0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,

    // 1 : 오른쪽 아래
     0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,

    // 2 : 위쪽
     0.0f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f
};

//------------------------------------------------------------------------------------------------------
// 전역 변수
//------------------------------------------------------------------------------------------------------


GLuint VAO;
GLuint VBO;


double cellWidth{};
double cellHeight{};

int BoardVertexCount = 0;

//Player

int pType{2};

int Pcol{};
int Prow{};

glm::vec2 startPosition;
glm::vec2 playerPosition;
glm::vec2 targetPosition;

int targetCol{};
int targetRow{};

const int PlayerVertexCount = 6;

float moveTime = 0.2f;
float moveProgress = 0.0f;
bool moving = false;

double lastTime{};

// 1 오른 , -1 왼
int moveDirection = 1;

// 장애물

Obstacle ob[1000]{};

int obstacleCount;

int ObstacleVertexCount{};

const int MaxObstacleCount = 1000;

//------------------------------------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------------------------------------
{
    //------------------------------------------------------------------------------------------
    // GLFW 초기화
    //------------------------------------------------------------------------------------------
    if (!glfwInit())
    {
        std::cout << "GLFW 초기화 실패\n";
        return -1;
    }

    //------------------------------------------------------------------------------------------
    // OpenGL 3.3 설정
    //------------------------------------------------------------------------------------------
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    //------------------------------------------------------------------------------------------
    // 창 생성
    //------------------------------------------------------------------------------------------
    GLFWwindow* window = glfwCreateWindow(
        wide, height,
        "__WINDOWTITLE__",
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cout << "Window 생성 실패\n";
        glfwTerminate();
        return -1;
    }

    //------------------------------------------------------------------------------------------
    // OpenGL Context 생성
    //------------------------------------------------------------------------------------------
    glfwMakeContextCurrent(window);

    //------------------------------------------------------------------------------------------
    // GLEW 초기화
    //------------------------------------------------------------------------------------------
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW 초기화 실패\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    //------------------------------------------------------------------------------------------
    // 화면 크기 설정
    //------------------------------------------------------------------------------------------
    glViewport(0, 0, wide, height);

    //------------------------------------------------------------------------------------------
    // 키 콜백 함수
    //------------------------------------------------------------------------------------------
    glfwSetKeyCallback(window, KeyCallback);

    //------------------------------------------------------------------------------------------
    // VAO / VBO 설정
    //------------------------------------------------------------------------------------------
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
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

    if (!CheckShader(vertexShader))
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

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

    if (!CheckShader(fragmentShader))
    {
        glDeleteShader(vertexShader);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    //------------------------------------------------------------------------------------------
    // Shader Program 생성
    //------------------------------------------------------------------------------------------
    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    if (!CheckProgram(shaderProgram))
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(shaderProgram);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    //------------------------------------------------------------------------------------------
    // Program 사용
    //------------------------------------------------------------------------------------------
    glUseProgram(shaderProgram);

    //------------------------------------------------------------------------------------------
    // 화면 배경색 설정
    //------------------------------------------------------------------------------------------
    glClearColor(
        1.0f,
        1.0f,
        1.0f,
        1.0f
    );

    GLint modelLocation = glGetUniformLocation(shaderProgram, "model");

    glm::mat4 model(1.0f);

    glUniformMatrix4fv(
        modelLocation,
        1,
        GL_FALSE,
        &model[0][0]
    );

    SetAll();

    //------------------------------------------------------------------------------------------
    // 메인 루프
    //------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // ------------------------------------------------------------
        // 화면 지우기
        // ------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT);


        MovePlayer();

        // ------------------------------------------------------------
        // 그리기
        // ------------------------------------------------------------
        glBindVertexArray(VAO);

        glDrawArrays(
            GL_LINES,
            0,
            BoardVertexCount
        );

        glDrawArrays(
            GL_TRIANGLES,
            BoardVertexCount,
            PlayerVertexCount
        );

        for (int i = 0; i < obstacleCount; ++i)
        {
            if (ob[i].type == 2)
            {
                glDrawArrays(
                    GL_TRIANGLE_STRIP,
                    ob[i].vertexStart,
                    ob[i].vertexCount
                );
            }
            else
            {
                glDrawArrays(
                    GL_TRIANGLES,
                    ob[i].vertexStart,
                    ob[i].vertexCount
                );
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //------------------------------------------------------------------------------------------
    // 종료
    //------------------------------------------------------------------------------------------
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glfwDestroyWindow(window);
    glfwTerminate();
}
//------------------------------------------------------------------------------------------------------


void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
    {
        return;
    }

    if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (key == GLFW_KEY_R) {
        SetAll();
    }
}

void SetAll()
{
    SetMap();
    SetPlayer();
    SetObstacleCount();
    SetObstacle();

    moving = false;
    moveProgress = 0.0f;
    lastTime = glfwGetTime();
    targetPosition = playerPosition;

    targetRow = Prow;
    targetCol = Pcol;
}

void SetMap()
{
    std::cout << "=============================================================" << '\n';

    while (true) {
        std::cout << "( x, y ) : ";

        std::cin >> BoardX >> BoardY;

        if (BoardX >= 10 && BoardX <= 30 && BoardY >= 10 && BoardY <= 30) {
            break;
        }
        else {
            std::cout << "다른 값 다시 입력하세요 '\n";
        }
    }

    cellWidth = 2.0f / BoardX;
    cellHeight = 2.0f / BoardY;
    
    // 가로 세로 줄 그리기
    float BoardVertices[124 * 6]{};

    int index{};

    for (int row = 0; row <= BoardY; ++row) {
        
        float y = 1.0f - cellHeight * row;


        BoardVertices[index++] = -1.0f;     //x
        BoardVertices[index++] = y;         //y
        BoardVertices[index++] = 0.0f;      //z

        BoardVertices[index++] = 0.5f;   // r
        BoardVertices[index++] = 0.5f;   // g
        BoardVertices[index++] = 0.5f;   // b

        // 오른쪽
        BoardVertices[index++] = 1.0f;
        BoardVertices[index++] = y;
        BoardVertices[index++] = 0.0f;

        BoardVertices[index++] = 0.5f;
        BoardVertices[index++] = 0.5f;
        BoardVertices[index++] = 0.5f;

    }

    for (int col = 0; col <= BoardX; col++)
    {
        float x = -1.0f + cellWidth * col;

        // 아래
        BoardVertices[index++] = x;
        BoardVertices[index++] = -1.0f;
        BoardVertices[index++] = 0.0f;

        BoardVertices[index++] = 0.5f;
        BoardVertices[index++] = 0.5f;
        BoardVertices[index++] = 0.5f;

        // 위
        BoardVertices[index++] = x;
        BoardVertices[index++] = 1.0f;
        BoardVertices[index++] = 0.0f;

        BoardVertices[index++] = 0.5f;
        BoardVertices[index++] = 0.5f;
        BoardVertices[index++] = 0.5f;
    }

    BoardVertexCount = index / 6;

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        (BoardVertexCount + PlayerVertexCount + MaxObstacleCount * 4) * 6 * sizeof(float),
        nullptr,
        GL_DYNAMIC_DRAW
    );

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        BoardVertexCount * 6 * sizeof(float),
        BoardVertices
    );

    playerPosition.x = -1.0f + cellWidth * 0.5f;
    playerPosition.y = 1.0f - cellHeight * 0.5f;

    Prow = 0;
    Pcol = 0;
}

void SetPlayer()
{
    float playerWidth = cellWidth * 0.8f;
    float playerHeight = cellHeight * 0.8f;

    float left = playerPosition.x - playerWidth * 0.5f;
    float right = playerPosition.x + playerWidth * 0.5f;

    float bottom = playerPosition.y - playerHeight * 0.5f;
    float top = playerPosition.y + playerHeight * 0.5f;

    float PlayerVertices[6 * 6]{};

    int index{};

    // 왼쪽 아래
    PlayerVertices[index++] = left;
    PlayerVertices[index++] = bottom;
    PlayerVertices[index++] = 0.0f;
    PlayerVertices[index++] = 1.0f;
    PlayerVertices[index++] = 0.0f;
    PlayerVertices[index++] = 0.0f;

    // 오른쪽 아래
    PlayerVertices[index++] = right;
    PlayerVertices[index++] = bottom;
    PlayerVertices[index++] = 0.0f;
    PlayerVertices[index++] = 1.0f;
    PlayerVertices[index++] = 0.0f;
    PlayerVertices[index++] = 0.0f;

    // 오른쪽 위
    PlayerVertices[index++] = right;
    PlayerVertices[index++] = top;
    PlayerVertices[index++] = 0.0f;
    PlayerVertices[index++] = 1.0f;
    PlayerVertices[index++] = 0.0f;
    PlayerVertices[index++] = 0.0f;

    // 왼쪽 아래
    PlayerVertices[index++] = left;
    PlayerVertices[index++] = bottom;
    PlayerVertices[index++] = 0.0f;
    PlayerVertices[index++] = 1.0f;
    PlayerVertices[index++] = 0.0f;
    PlayerVertices[index++] = 0.0f;

    // 오른쪽 위
    PlayerVertices[index++] = right;
    PlayerVertices[index++] = top;
    PlayerVertices[index++] = 0.0f;
    PlayerVertices[index++] = 1.0f;
    PlayerVertices[index++] = 0.0f;
    PlayerVertices[index++] = 0.0f;

    // 왼쪽 위
    PlayerVertices[index++] = left;
    PlayerVertices[index++] = top;
    PlayerVertices[index++] = 0.0f;
    PlayerVertices[index++] = 1.0f;
    PlayerVertices[index++] = 0.0f;
    PlayerVertices[index++] = 0.0f;

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        BoardVertexCount * 6 * sizeof(float),
        sizeof(PlayerVertices),
        PlayerVertices
    );
}

void SetObstacleCount()
{
    std::cout << "=============================================================" << '\n';

    while (true) {
        std::cout << "배치 갯수 : ";
        std::cin >> obstacleCount;

        if (obstacleCount >= 1 && obstacleCount <= BoardX * BoardY) {
            break;
        }
    }

    for (int i = 0; i < obstacleCount; ++i) {

        while (true) {
            int row = rowDist(gen);
            int col = colDist(gen);

            if (!(row >= BoardY || col >= BoardX)) {
                
                bool isgood{ true };

                for (int j = 0; j < i; ++j) {
                    if (row == ob[j].row && col == ob[j].col) {
                        isgood = false;
                    }
                }

                if (isgood) {
                    ob[i].col = col;
                    ob[i].row = row;

                    glm::vec3 color = { colorDist(gen), colorDist(gen), colorDist(gen) };
                    ob[i].color = color;

                    ob[i].size = (cellWidth < cellHeight ? cellWidth : cellHeight) * 0.8f;

                    ob[i].type = typeDist(gen);

                    break;
                }

            }
        }

    }



}

void SetObstacle()
{
    float ObstacleVertices[1000 * 4 * 6]{};

    int index{};

    for (int i = 0; i < obstacleCount; ++i) {

        float x = -1.0f + cellWidth * (ob[i].col + 0.5f);
        float y = 1.0f - cellHeight * (ob[i].row + 0.5f);

        float halfSize = ob[i].size * 0.5f;

        int type = ob[i].type;

        float left = x - halfSize;
        float right = x + halfSize;
        float bottom = y - halfSize;
        float top = y + halfSize;

        ob[i].vertexStart = BoardVertexCount + PlayerVertexCount + index / 6;
        ob[i].vertexCount = 3;

        if (type == 1) {
            // 왼쪽 아래
            ObstacleVertices[index++] = left;
            ObstacleVertices[index++] = bottom;
            ObstacleVertices[index++] = 0.0f;

            ObstacleVertices[index++] = ob[i].color.r;
            ObstacleVertices[index++] = ob[i].color.g;
            ObstacleVertices[index++] = ob[i].color.b;

            // 오른쪽 아래
            ObstacleVertices[index++] = right;
            ObstacleVertices[index++] = bottom;
            ObstacleVertices[index++] = 0.0f;

            ObstacleVertices[index++] = ob[i].color.r;
            ObstacleVertices[index++] = ob[i].color.g;
            ObstacleVertices[index++] = ob[i].color.b;

            // 위쪽
            ObstacleVertices[index++] = x;
            ObstacleVertices[index++] = top;
            ObstacleVertices[index++] = 0.0f;

            ObstacleVertices[index++] = ob[i].color.r;
            ObstacleVertices[index++] = ob[i].color.g;
            ObstacleVertices[index++] = ob[i].color.b;
        }
        else if (type == 2) {

            // 왼쪽 아래
            ObstacleVertices[index++] = left;
            ObstacleVertices[index++] = bottom;
            ObstacleVertices[index++] = 0.0f;

            ObstacleVertices[index++] = ob[i].color.r;
            ObstacleVertices[index++] = ob[i].color.g;
            ObstacleVertices[index++] = ob[i].color.b;

            // 오른쪽 아래
            ObstacleVertices[index++] = right;
            ObstacleVertices[index++] = bottom;
            ObstacleVertices[index++] = 0.0f;

            ObstacleVertices[index++] = ob[i].color.r;
            ObstacleVertices[index++] = ob[i].color.g;
            ObstacleVertices[index++] = ob[i].color.b;

            // 오른쪽 위
            ObstacleVertices[index++] = left;
            ObstacleVertices[index++] = top;
            ObstacleVertices[index++] = 0.0f;

            ObstacleVertices[index++] = ob[i].color.r;
            ObstacleVertices[index++] = ob[i].color.g;
            ObstacleVertices[index++] = ob[i].color.b;

            ObstacleVertices[index++] = right;
            ObstacleVertices[index++] = top;
            ObstacleVertices[index++] = 0.0f;

            ObstacleVertices[index++] = ob[i].color.r;
            ObstacleVertices[index++] = ob[i].color.g;
            ObstacleVertices[index++] = ob[i].color.b;

            ob[i].vertexCount = 4;

        }
        else if (type == 3)
        {
            // 역삼각형

            // 왼쪽 위
            ObstacleVertices[index++] = left;
            ObstacleVertices[index++] = top;
            ObstacleVertices[index++] = 0.0f;

            ObstacleVertices[index++] = ob[i].color.r;
            ObstacleVertices[index++] = ob[i].color.g;
            ObstacleVertices[index++] = ob[i].color.b;

            // 오른쪽 위
            ObstacleVertices[index++] = right;
            ObstacleVertices[index++] = top;
            ObstacleVertices[index++] = 0.0f;

            ObstacleVertices[index++] = ob[i].color.r;
            ObstacleVertices[index++] = ob[i].color.g;
            ObstacleVertices[index++] = ob[i].color.b;

            // 아래쪽
            ObstacleVertices[index++] = x;
            ObstacleVertices[index++] = bottom;
            ObstacleVertices[index++] = 0.0f;

            ObstacleVertices[index++] = ob[i].color.r;
            ObstacleVertices[index++] = ob[i].color.g;
            ObstacleVertices[index++] = ob[i].color.b;
        }
    }

    ObstacleVertexCount = index / 6;

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        (BoardVertexCount + PlayerVertexCount) * 6 * sizeof(float),
        ObstacleVertexCount * 6 * sizeof(float),
        ObstacleVertices
    );
}

void MovePlayer()
{
    double currentTime = glfwGetTime();
    double deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    if (!moving)
    {
        targetRow = Prow;
        targetCol = Pcol;

        if (moveDirection == 1)
        {
            if (Pcol < BoardX - 1)
            {
                targetCol = Pcol + 1;
            }
            else if (Prow < BoardY - 1)
            {
                targetRow = Prow + 1;
            }
            else
            {
                return;
            }
        }
        else
        {
            if (Pcol > 0)
            {
                targetCol = Pcol - 1;
            }
            else if (Prow < BoardY - 1)
            {
                targetRow = Prow + 1;
            }
            else
            {
                return;
            }
        }

        startPosition = playerPosition;

        targetPosition.x = -1.0f + cellWidth * (targetCol + 0.5f);
        targetPosition.y = 1.0f - cellHeight * (targetRow + 0.5f);

        moveProgress = 0.0f;
        moving = true;
    }

    if (moving)
    {
        moveProgress += static_cast<float>(deltaTime / moveTime);

        if (moveProgress >= 1.0f)
        {
            moveProgress = 1.0f;
            moving = false;

            playerPosition = targetPosition;

            bool rowChanged = (targetRow != Prow);

            Prow = targetRow;
            Pcol = targetCol;

            if (rowChanged)
            {
                moveDirection *= -1;
            }
        }
        else
        {
            playerPosition = startPosition +
                (targetPosition - startPosition) * moveProgress;
        }

        SetPlayer();
    }
    int checked = CheckAttack();

    if (checked >= 0) {
        int tmp = pType;
        pType = ob[checked].type;
        ob[checked].type = tmp;
    }

}

int CheckAttack()
{
    for (int i = 0; i < obstacleCount; ++i)
    {
        if (Prow == ob[i].row && Pcol == ob[i].col)
        {
            std::cout << i << " is Checked" << '\n';
            return i;
        }
    }

    return -1;
}

bool CheckShader(GLuint shader)
{
    int success = 0;

    glGetShaderiv(
        shader,
        GL_COMPILE_STATUS,
        &success
    );

    if (success == GL_FALSE)
    {
        char log[1024];

        glGetShaderInfoLog(
            shader,
            sizeof(log),
            nullptr,
            log
        );

        std::cout << "셰이더 컴파일 실패\n";
        std::cout << log << "\n";

        return false;
    }

    return true;
}

bool CheckProgram(GLuint program)
{
    int success = 0;

    glGetProgramiv(
        program,
        GL_LINK_STATUS,
        &success
    );

    if (success == GL_FALSE)
    {
        char log[1024];

        glGetProgramInfoLog(
            program,
            sizeof(log),
            nullptr,
            log
        );

        std::cout << "Program 링크 실패\n";
        std::cout << log << "\n";

        return false;
    }

    return true;
}


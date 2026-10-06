#include <GL/glew.h>
#include <GL/glfw3.h>
#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cmath>

//------------------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------------------


// 셰이더 컴파일 / 링크 결과 확인
bool CheckShader(GLuint shader);

bool CheckProgram(GLuint program);

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void SetMap();

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

int wide = 1200;
int height = 1200;

GLuint VAO;
GLuint VBO;

int BoardX = 20;
int BoardY = 20;

double cellWidth{};
double cellHeight{};

int BoardVertexCount = 0;

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

    SetMap();

    //------------------------------------------------------------------------------------------
    // 메인 루프
    //------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // ------------------------------------------------------------
        // 화면 지우기
        // ------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT);

        // ------------------------------------------------------------
        // 그리기
        // ------------------------------------------------------------
        glBindVertexArray(VAO);

        glDrawArrays(
            GL_LINES,
            0,
            BoardVertexCount
        );

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
    float BoardVertices[124]{};

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
        BoardVertexCount * 6 * sizeof(float),
        BoardVertices,
        GL_STATIC_DRAW
    );

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


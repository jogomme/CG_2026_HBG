#include <windows.h>

#include <GL/glew.h>
#include <GL/glfw3.h>
#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cmath>

//------------------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------------------

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

// 셰이더 컴파일 / 링크 결과 확인
bool CheckShader(GLuint shader);

bool CheckProgram(GLuint program);

glm::mat4 MakeModelMatrix(float angleX, float angleY);

//------------------------------------------------------------------------------------------------------
// 정점의 위치 데이터
//------------------------------------------------------------------------------------------------------

// 위치 3개 + 색상 3개 = 한 정점 6 float
float vertices[] = {
    // ------------------------------------------------------------
     // X축 - 빨간색
     // ------------------------------------------------------------
     -0.8f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,
      0.8f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,

      // ------------------------------------------------------------
      // Y축 - 초록색
      // ------------------------------------------------------------
      0.0f, -0.8f, 0.0f,    0.0f, 1.0f, 0.0f,
      0.0f,  0.8f, 0.0f,    0.0f, 1.0f, 0.0f,

      // ------------------------------------------------------------
      // Z축 - 파란색
      // ------------------------------------------------------------
      0.0f, 0.0f, -0.8f,    0.0f, 0.0f, 1.0f,
      0.0f, 0.0f,  0.8f,    0.0f, 0.0f, 1.0f
};

//------------------------------------------------------------------------------------------------------
// 전역 변수
//------------------------------------------------------------------------------------------------------

int wide = 1600;
int height = 1200;

GLuint VAO;
GLuint VBO;

//------------------------------------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------------------------------------
{
    //------------------------------------------------------------------------------------------
    // 콘솔 출력 코드페이지를 UTF-8 로 설정
    //   /utf-8 로 컴파일되므로 문자열은 UTF-8 로 나간다.
    //   콘솔 기본 코드페이지(949)로는 깨지므로 맞춰준다.
    //------------------------------------------------------------------------------------------
    SetConsoleOutputCP(CP_UTF8);

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
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    
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

    glEnable(GL_DEPTH_TEST);

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

    //------------------------------------------------------------------------------------------
    // 메인 루프
    //------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // ------------------------------------------------------------
        // 화면 지우기
        // ------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ------------------------------------------------------------
        // 회전 각도 계산
        // ------------------------------------------------------------
        float angle = static_cast<float>(glfwGetTime()) * glm::radians(90.0f);

        glm::mat4 model = MakeModelMatrix(
            glm::radians(30.0f),
            glm::radians(30.0f)
        );

        glUniformMatrix4fv(
            modelLocation,
            1,
            GL_FALSE,
            &model[0][0]
        );

        // ------------------------------------------------------------
        // 그리기
        // ------------------------------------------------------------
        glBindVertexArray(VAO);
        
        glDrawArrays(
            GL_LINES,
            0,
            6
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

glm::mat4 MakeModelMatrix(float angleX, float angleY)
{
    glm::mat4 model(1.0f);

    model = glm::rotate(
        model,
        angleX,
        glm::vec3(1.0f, 0.0f, 0.0f)
    );

    model = glm::rotate(
        model,
        angleY,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    return model;
}
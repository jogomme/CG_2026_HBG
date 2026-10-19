//------------------------------------------------------------------------------------------------------
// OpenGL 3.3 실습 기본 템플릿
//
// 반복되는 설정은 GLCommon.h 에 모아 두었습니다. 이 파일은 과제 로직만 작성하면 됩니다.
//   CreateGLWindow()         창 생성 + GLEW 초기화 + viewport + 콘솔 UTF-8
//   CreateProgram( vs, fs )  셰이더 컴파일 + 링크 + 오류 로그 출력
//
// 링커 설정은 vcxproj 에 내장되어 있습니다. include 경로 설정도 필요 없습니다.
//   링커   : opengl32.lib / glew32.lib / glfw3dll.lib
//   언어   : C++20
//   옵션   : /utf-8   ( 한글 주석이 CP949 로 해석되어 줄이 병합되는 문제 방지 )
//
// 헤더는 Windows SDK 에서 옵니다.
//   #include <GL/glew.h>
//   #include <GL/glfw3.h>          <- <GLFW/glfw3.h> 가 아닙니다
//   #include <GL/glm/glm.hpp>      <- 앞에 GL/ 이 붙습니다
//
// 빌드는 x64 만 지원합니다. glew32.lib / glfw3dll.lib 이 SDK 에 x64 로만 존재합니다.
//------------------------------------------------------------------------------------------------------

#include "GLCommon.h"

#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>

#include <cmath>

//------------------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------------------

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

glm::mat4 MakeModelMatrix(float angle);

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

int wide = 1600;
int height = 1200;

GLuint VAO;
GLuint VBO;

int main()
//------------------------------------------------------------------------------------------------------
{
    //------------------------------------------------------------------------------------------
    // 창 생성 + GLEW 초기화 + viewport + 콘솔 UTF-8
    //------------------------------------------------------------------------------------------
    GLFWwindow* window = CreateGLWindow(
        wide,
        height,
        "__WINDOWTITLE__"
    );

    if (!window)
    {
        return -1;
    }

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
    // Shader Program 생성 ( 컴파일 + 링크 )
    //------------------------------------------------------------------------------------------
    GLuint shaderProgram = CreateProgram(
        vertexShaderSource,
        fragmentShaderSource
    );

    if (shaderProgram == 0)
    {
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
        glClear(GL_COLOR_BUFFER_BIT);

        // ------------------------------------------------------------
        // 회전 각도 계산
        // ------------------------------------------------------------
        float angle = static_cast<float>(glfwGetTime()) * glm::radians(90.0f);

        glm::mat4 model = MakeModelMatrix(angle);

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
            GL_TRIANGLES,
            0,
            3
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

    glfwDestroyWindow(window);
    glfwTerminate();
}

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

glm::mat4 MakeModelMatrix(float angle)
{
    glm::mat4 model(1.0f);

    model = glm::rotate(
        model,
        angle,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    return model;
}

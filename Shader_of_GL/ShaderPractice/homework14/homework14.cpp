#include "GLCommon.h"

#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>

#include <cmath>

//------------------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------------------

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

glm::mat4 MakeAxisModelMatrix();
glm::mat4 MakeObjectModelMatrix();

void DrawAll(GLint modelLocation);
void DrawAxis(GLint modelLocation);
void DrawObj(GLint modelLocation);

//------------------------------------------------------------------------------------------------------
// 정점의 위치 데이터
//------------------------------------------------------------------------------------------------------

// 위치 3개 + 색상 3개 = 한 정점 6 float
float axisVertices[] = {
    // X축 - 빨간색
    -0.8f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
     0.8f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,

     // Y축 - 초록색
      0.0f, -0.8f, 0.0f,  0.0f, 1.0f, 0.0f,
      0.0f,  0.8f, 0.0f,  0.0f, 1.0f, 0.0f,

      // Z축 - 파란색
       0.0f, 0.0f, -0.8f,  0.0f, 0.0f, 1.0f,
       0.0f, 0.0f,  0.8f,  0.0f, 0.0f, 1.0f
};

//------------------------------------------------------------------------------------------------------
// 전역 변수
//------------------------------------------------------------------------------------------------------

int wide = 1600;
int height = 1200;

float angleX = 30.0f;
float angleY = 30.0f;
float angleZ = 0.0f;

float axisangle = 30.0f;
glm::mat4 axismodel = MakeAxisModelMatrix();

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
        sizeof(axisVertices),
        axisVertices,
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
        0.0f,
        0.0f,
        0.0f,
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

        DrawAll(modelLocation);

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

glm::mat4 MakeAxisModelMatrix()
{
    glm::mat4 model(1.0f);

    model = glm::rotate(model, glm::radians(axisangle), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(axisangle), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    return model;
}

glm::mat4 MakeObjectModelMatrix()
{
    glm::mat4 model(1.0f);

    model = glm::rotate(model, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angleZ), glm::vec3(0.0f, 0.0f, 1.0f));

    return model;
}

void DrawAll(GLint modelLocation)
{
    DrawAxis(modelLocation);
    DrawObj(modelLocation);
}

void DrawAxis(GLint modelLocation)
{

    glUniformMatrix4fv(
        modelLocation,
        1,
        GL_FALSE,
        &axismodel[0][0]
    );

    glBindVertexArray(VAO);

    glDrawArrays(
        GL_LINES,
        0,
        6
    );
}
void DrawObj(GLint modelLocation)
{
    glm::mat4 objmodel = MakeObjectModelMatrix();

    glUniformMatrix4fv(
        modelLocation,
        1,
        GL_FALSE,
        &objmodel[0][0]
    );

}
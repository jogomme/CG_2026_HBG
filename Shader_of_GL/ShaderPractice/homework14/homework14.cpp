#include "GLCommon.h"

#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <iostream>
#include <random>
//------------------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------------------

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

glm::mat4 MakeAxisModelMatrix();
glm::mat4 MakeObjectModelMatrix();

void MakeVertices(float* vertices, int& index, const glm::vec3& position, const glm::vec3& color);

void MakeCubeVertices();
void MakeCubeIndices();

void MakePyramidVertices();

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

float h{ 0.3f };

glm::vec3 cubeCorners[8] = {
    {-h, -h,  h},  // 0
    { h, -h,  h},  // 1
    {-h,  h,  h},  // 2
    { h,  h,  h},  // 3
    {-h, -h, -h},  // 4
    { h, -h, -h},  // 5
    {-h,  h, -h},  // 6
    { h,  h, -h}   // 7
};

int cubeFaces[6][4] = {
    {0, 1, 2, 3}, // 앞면
    {4, 5, 6, 7}, // 뒷면
    {1, 5, 3, 7}, // 오른쪽 면
    {4, 0, 6, 2}, // 왼쪽 면
    {2, 3, 6, 7}, // 윗면
    {4, 5, 0, 1}  // 아랫면
};

float cubeVertices[8 * 6]{};

unsigned int cubeIndices[6 * 2 * 3]{};

glm::vec3 pyramidCorners[5] = {
    {-h, -h,  h},  // 0: 바닥 앞쪽 왼쪽
    { h, -h,  h},  // 1: 바닥 앞쪽 오른쪽
    {-h, -h, -h},  // 2: 바닥 뒤쪽 왼쪽
    { h, -h, -h},  // 3: 바닥 뒤쪽 오른쪽
    {0.0f, h, 0.0f} // 4: 꼭대기
};

int pyramidFaces[4][3] = {
    {0, 4, 1}, // 앞쪽 삼각형
    {1, 4, 2}, // 오른쪽 삼각형
    {2, 4, 3}, // 뒤쪽 삼각형
    {3, 4, 0}  // 왼쪽 삼각형
};

int pyramidBottom[4] = { 0, 1, 2, 3 };

float pyramidVertices[5 * 6]{};

bool pyramidMode{};            // false: 육면체, true: 사각뿔

//------------------------------------------------------------------------------------------------------
// 전역 변수
//------------------------------------------------------------------------------------------------------

int wide = 1600;
int height = 1200;

std::random_device rd;
std::mt19937 gen(rd());

float angleX = 30.0f;
float angleY = 30.0f;
float angleZ = 0.0f;

float axisangle = 30.0f;
glm::mat4 axismodel = MakeAxisModelMatrix();

GLuint VAO;
GLuint VBO;

GLuint cubeVAO;
GLuint cubeVBO;
GLuint cubeEBO;

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

    MakeCubeVertices();
    MakeCubeIndices();

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

    // Cube 관련

    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(cubeVertices),
        cubeVertices,
        GL_STATIC_DRAW
    );

    glGenBuffers(1, &cubeEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(cubeIndices),
        cubeIndices,
        GL_STATIC_DRAW
    );

    // 육면체 정점 위치 설정
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // 육면체 정점 색상 설정
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
    else if (key == GLFW_KEY_C) {
        MakeCubeVertices();

        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

        glBufferSubData(
            GL_ARRAY_BUFFER,
            0,
            sizeof(cubeVertices),
            cubeVertices
        );
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
    glBindVertexArray(cubeVAO);

    glDrawElements(
        GL_TRIANGLES,
        36,
        GL_UNSIGNED_INT,
        0
    );
}

void MakeCubeIndices()
{
    int index = 0;

    for (int i = 0; i < 6; i++)
    {
        int a = cubeFaces[i][0];
        int b = cubeFaces[i][1];
        int c = cubeFaces[i][2];
        int d = cubeFaces[i][3];

        if (i == 1 || i == 3 || i == 5)
        {
            cubeIndices[index++] = a;
            cubeIndices[index++] = c;
            cubeIndices[index++] = b;

            cubeIndices[index++] = c;
            cubeIndices[index++] = d;
            cubeIndices[index++] = b;
        }
        else
        {
            cubeIndices[index++] = a;
            cubeIndices[index++] = b;
            cubeIndices[index++] = c;

            cubeIndices[index++] = c;
            cubeIndices[index++] = b;
            cubeIndices[index++] = d;
        }
    }
}

void MakeVertices(float* vertices, int& index, const glm::vec3& position, const glm::vec3& color)
{
    vertices[index++] = position.x;
    vertices[index++] = position.y;
    vertices[index++] = position.z;

    vertices[index++] = color.r;
    vertices[index++] = color.g;
    vertices[index++] = color.b;
}
void MakeCubeVertices()
{
    int index{};

    std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);

    for (int i = 0; i < 8; ++i) {

        glm::vec3 color = { colorDist(gen), colorDist(gen), colorDist(gen) };

        MakeVertices(cubeVertices, index, cubeCorners[i], color);
    }
}

void MakePyramidVertices()
{
    int index{};

    std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);

    for (int i = 0; i < 5; ++i) {

        glm::vec3 color = { colorDist(gen), colorDist(gen), colorDist(gen) };

        MakeVertices(pyramidVertices, index, pyramidCorners[i], color);
    }
    glm::vec3 color = { colorDist(gen), colorDist(gen), colorDist(gen) };


}
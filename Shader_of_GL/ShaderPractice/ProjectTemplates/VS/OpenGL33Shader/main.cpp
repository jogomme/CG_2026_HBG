//------------------------------------------------------------------------------------------
// OpenGL 3.3 Core + GLFW + GLEW + GLM 기본 틀
//
// 이 템플릿의 vcxproj 에 다음 설정이 이미 내장되어 있습니다. 별도 수정이 필요 없습니다.
//   - 링커: opengl32.lib / glew32.lib / glfw3dll.lib
//   - C++20
//   - /utf-8  (한글 주석이 CP949 로 오해되어 줄이 병합되는 문제 방지)
//
// 헤더는 모두 Windows SDK 에서 옵니다. include 경로 설정이 필요 없습니다.
//   #include <GL/glew.h>
//   #include <GL/glfw3.h>          <- GLFW 입니다. <GLFW/glfw3.h> 가 아닙니다
//   #include <GL/glm/glm.hpp>      <- 앞에 GL/ 이 붙습니다
//
// 자주 추가로 쓰는 GLM 헤더:
//   #include <GL/glm/gtc/matrix_transform.hpp>   // translate / rotate / scale
//   #include <GL/glm/gtc/type_ptr.hpp>           // glm::value_ptr
//
// 빌드: x64 만 지원합니다 ( glew32.lib / glfw3dll.lib 이 SDK 에 x64 로만 존재 ).
//------------------------------------------------------------------------------------------

#include <GL/glew.h>
#include <GL/glfw3.h>

#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>

#include <iostream>

int wide = 1600;
int height = 1200;

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

const char* fragmentShaderSource = R"(
#version 330 core

in vec3 ourColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}
)";

int main()
{
    if (!glfwInit())
    {
        std::cout << "GLFW 초기화 실패\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        wide, height, "__WINDOWTITLE__", nullptr, nullptr
    );

    if (!window)
    {
        std::cout << "Window 생성 실패\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW 초기화 실패\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, wide, height);

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

    GLint modelLocation = glGetUniformLocation(shaderProgram, "model");

    // x, y, z, r, g, b
    float vertexs[] =
    {
        -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
         0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f
    };

    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertexs),
        vertexs,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
            || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);

        // GLM 사용 예시 : 삼각형 위치 회전
        glm::mat4 model(1.0f);
        model = glm::rotate(
            model,
            glm::radians(0.1f),
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

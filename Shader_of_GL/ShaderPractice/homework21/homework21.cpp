#include "PracticeScene.h"

// 함수 선언
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void UpdateScene(float dt);
void DrawAll(common::Shader& shader, int width, int height);

// 프로그램 시작
int main()
{
    return practice::RunScene(
        "OpenGL Homework",
        KeyCallback,
        UpdateScene,
        DrawAll
    );
}

// 키보드 입력 처리
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

// 매 프레임 물체의 상태 변경
void UpdateScene(float dt)
{
    // 이동, 회전, 충돌 등의 계산
}

// 장면 그리기
void DrawAll(common::Shader& shader, int width, int height)
{
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 2.0f, 6.0f),
        glm::vec3(0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    glm::mat4 projection =
        practice::MakeProjection(width, height, true, 4.0f);

    shader.setMatrix(shader.uniformLocation("view"), view);
    shader.setMatrix(shader.uniformLocation("projection"), projection);

    GLint modelLocation = shader.uniformLocation("model");

    glm::mat4 model = practice::MakeModelMatrix(
        glm::vec3(0.0f),  // 위치
        glm::vec3(1.0f)   // 크기
    );

    practice::DrawObj(
        practice::cube,
        model,
        glm::vec3(1.0f, 0.0f, 0.0f),
        modelLocation,
        shader
    );
}
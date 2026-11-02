#include "PracticeScene.h"

// 함수 선언
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void UpdateScene(float dt);
void DrawAll(common::Shader& shader, int width, int height);
glm::mat4 rotationModel(glm::mat4 model);

glm::vec3 ballpos{};

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

    glm::mat4 projection = practice::MakeProjection(width, height, true, 4.0f);

    shader.setMatrix(shader.uniformLocation("view"), view);
    shader.setMatrix(shader.uniformLocation("projection"), projection);

    GLint modelLocation = shader.uniformLocation("model");

    glm::mat4 model = practice::MakeModelMatrix(
        ballpos,
        glm::vec3(1.0f)   // 크기
    );
    
    model = rotationModel(model);

    practice::DrawObj(
        practice::cube,
        model,
        glm::vec3(1.0f, 0.0f, 0.0f),
        modelLocation,
        shader
    );
}

glm::mat4 rotationModel(glm::mat4 model)
{
    static float angleY = 0;
    // 1. 위치 이동
    model = glm::translate(model, ballpos);

    // 2. 자신의 Y축을 중심으로 회전
    model = glm::rotate(
        model,
        glm::radians(angleY),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // 3. 크기 변경
    model = glm::scale(model, glm::vec3(0.3f));

    angleY += 1;

    return model;
}
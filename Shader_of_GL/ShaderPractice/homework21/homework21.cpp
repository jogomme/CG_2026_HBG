#include "PracticeScene.h"

// 함수 선언
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void UpdateScene(float dt);
void DrawAll(common::Shader& shader, int width, int height);
glm::mat4 rotationModel(glm::mat4 model, float angle);
glm::mat4 Revolution(glm::mat4 model);

glm::vec3 cubePos{};

float angleX = 0;
float angleY = 0;
float angleZ = 0;

float CubeAngle = 90;

void MakeCube(glm::mat4 model, GLint modelLocation, common::Shader& shader);

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
    glClearColor(0, 0, 0, 1.0f);

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    glm::mat4 projection = practice::MakeProjection(width, height, true, 4.0f);

    shader.setMatrix(shader.uniformLocation("view"), view);
    shader.setMatrix(shader.uniformLocation("projection"), projection);

    GLint modelLocation = shader.uniformLocation("model");

    glm::mat4 wallModel = practice::MakeModelMatrix(
        cubePos,
        glm::vec3(1.0f)   // 크기
    );

    MakeCube(wallModel, modelLocation, shader);

}

glm::mat4 Revolution(glm::mat4 model, float angle)
{
    // 2. 자신의 Y축을 중심으로 회전
    model = glm::rotate( model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f) );
    model = glm::translate(model, glm::vec3(3,0,0));

    return model;
}

glm::mat4 rotationModel(glm::mat4 model, float angle)
{
    model = glm::translate(model, cubePos);
    model = glm::rotate(model, glm::radians(angleX), glm::vec3(1, 0, 0.0f));
    model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1, 0));
    model = glm::rotate(model, glm::radians(angleZ), glm::vec3(0.0f, 0, 1.0f));

    return model;
}

void MakeCube(glm::mat4 model, GLint modelLocation, common::Shader& shader)
{
    glm::mat4 wallmodel(1);

    wallmodel = glm::rotate(wallmodel, glm::radians(CubeAngle), glm::vec3(0, 1, 0));

    practice::DrawObj(
        practice::plane,
        wallmodel,
        glm::vec3(0.7f),
        modelLocation,
        shader
    );

    for (int i = 0; i < 4; ++i)
    {
        glm::vec3 cubeColor = (i % 2 == 0)
            ? glm::vec3(0.7f)    // 1, 3번 면: 회색
            : glm::vec3(0.3f);   // 2, 4번 면: 진한 회색

        glm::mat4 model(1.0f);

        // 1. Z축 중심으로 90도씩 회전
        model = glm::rotate(
            model,
            glm::radians(CubeAngle * i),
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        // 2. 원점에서 아래쪽으로 이동
        model = glm::translate(
            model,
            glm::vec3(0.0f, -0.5, 0.0f)
        );

        practice::DrawObj(
            practice::plane,
            model,
            cubeColor,
            modelLocation,
            shader
        );
    }
}
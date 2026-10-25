#include "Common/PracticeScene.h"

//------------------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------------------
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void UpdateScene(float deltaTime);
void DrawAll(common::Shader& shader, int width, int height);
void ResetScene();
glm::mat4 MakeBeltModelMatrix(float distance);
void DrawNumber(int number, glm::mat4 model, GLint modelLocation, common::Shader& shader);

//------------------------------------------------------------------------------------------------------
// 전역 변수와 플래그
//------------------------------------------------------------------------------------------------------
const float beltHeight = 0.85f;
const float cornerRadius = 0.45f;
const float firstLength = 4.05f;
const float cornerLength = glm::radians(90.0f) * cornerRadius;
const float lastLength = 3.55f;
const float pathLength = firstLength + cornerLength + lastLength;
float beltDistance = 0;
float cameraDistance = 10;
float cameraAngle = 40;
bool isB = true;
bool isPerspective = true;
const glm::vec3 boxColor[5] = {
    {0.95f,0.4f,0.2f}, {0.2f,0.6f,0.95f}, {0.85f,0.7f,0.2f},
    {0.45f,0.75f,0.35f}, {0.75f,0.35f,0.8f}
};

void ResetScene()
{
    beltDistance = 0;
    cameraDistance = 10;
    cameraAngle = 40;
    isB = true;
    isPerspective = true;
}

void UpdateScene(float deltaTime)
{
    if (isB) beltDistance = std::fmod(beltDistance + 0.85f * deltaTime, pathLength);
}

glm::mat4 MakeBeltModelMatrix(float distance)
{
    distance = std::fmod(distance, pathLength);
    if (distance < 0) distance += pathLength;
    glm::vec3 position;
    float angle;
    if (distance < firstLength)
    {
        position = glm::vec3(-3 + distance, beltHeight, -1.5f);
        angle = 90;
    }
    else if (distance < firstLength + cornerLength)
    {
        // 코너의 원호를 따라 위치와 방향을 함께 바꾼다.
        float a = -glm::radians(90.0f) + (distance - firstLength) / cornerRadius;
        position = glm::vec3(1.05f + cornerRadius * cos(a), beltHeight, -1.05f + cornerRadius * sin(a));
        angle = -glm::degrees(a);
    }
    else
    {
        position = glm::vec3(1.5f, beltHeight, -1.05f + distance - firstLength - cornerLength);
        angle = 0;
    }
    return glm::rotate(glm::translate(glm::mat4(1), position), glm::radians(angle), glm::vec3(0,1,0));
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
    if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GLFW_TRUE);
    else if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD) cameraDistance = std::max(5.0f, cameraDistance - 0.3f);
    else if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) cameraDistance = std::min(22.0f, cameraDistance + 0.3f);
    else if (key == GLFW_KEY_LEFT) cameraAngle -= 5;
    else if (key == GLFW_KEY_RIGHT) cameraAngle += 5;
    else if (key == GLFW_KEY_P) isPerspective = (mods & GLFW_MOD_SHIFT) == 0;
    else if (key == GLFW_KEY_B && action == GLFW_PRESS) isB = !isB;
    else if (key == GLFW_KEY_R) ResetScene();
}

void DrawNumber(int number, glm::mat4 model, GLint modelLocation, common::Shader& shader)
{
    // 일곱 개의 가는 육면체로 상자 윗면에 1~5를 표시한다.
    const int segments[5][7] = {
        {0,1,1,0,0,0,0}, {1,1,0,1,1,0,1}, {1,1,1,1,0,0,1},
        {0,1,1,0,0,1,1}, {1,0,1,1,0,1,1}
    };
    const glm::vec3 positions[7] = {
        {0,0.526f,-0.16f}, {0.11f,0.526f,-0.08f}, {0.11f,0.526f,0.08f},
        {0,0.526f,0.16f}, {-0.11f,0.526f,0.08f}, {-0.11f,0.526f,-0.08f}, {0,0.526f,0}
    };
    for (int i = 0; i < 7; ++i)
    {
        if (!segments[number - 1][i]) continue;
        glm::vec3 size = (i == 0 || i == 3 || i == 6) ? glm::vec3(0.24f,0.012f,0.035f) : glm::vec3(0.035f,0.012f,0.15f);
        practice::DrawObj(practice::cube, practice::MakeModelMatrix(positions[i], size, model), glm::vec3(0.06f), modelLocation, shader);
    }
}

void DrawAll(common::Shader& shader, int width, int height)
{
    GLint modelLocation = shader.uniformLocation("model");
    float angle = glm::radians(cameraAngle);
    glm::vec3 target(-0.5f,0.4f,0.3f);
    glm::vec3 camera = target + cameraDistance * glm::vec3(0.75f * sin(angle),0.65f,0.75f * cos(angle));
    shader.setMatrix(shader.uniformLocation("view"), glm::lookAt(camera, target, glm::vec3(0,1,0)));
    shader.setMatrix(shader.uniformLocation("projection"), practice::MakeProjection(width, height, isPerspective, cameraDistance * 0.42f));
    practice::DrawObj(practice::plane, practice::MakeModelMatrix(glm::vec3(0), glm::vec3(10,1,9)), glm::vec3(0.78f,0.82f,0.87f), modelLocation, shader);
    practice::DrawObj(practice::plane, practice::MakeModelMatrix(glm::vec3(-0.75f,beltHeight,-1.5f), glm::vec3(5.5f,1,1)), glm::vec3(0.18f,0.23f,0.28f), modelLocation, shader);
    practice::DrawObj(practice::plane, practice::MakeModelMatrix(glm::vec3(1.5f,beltHeight,0.5f), glm::vec3(1,1,5)), glm::vec3(0.18f,0.23f,0.28f), modelLocation, shader);
    const glm::vec3 legs[4] = {{-3,beltHeight / 2,-1.5f}, {-1,beltHeight / 2,-1.5f}, {1.5f,beltHeight / 2,-1.5f}, {1.5f,beltHeight / 2,2.5f}};
    for (glm::vec3 position : legs)
        practice::DrawObj(practice::cube, practice::MakeModelMatrix(position, glm::vec3(0.16f,beltHeight,0.6f)), glm::vec3(0.4f,0.45f,0.5f), modelLocation, shader);
    for (int i = 0; i < 36; ++i)
    {
        glm::mat4 model = MakeBeltModelMatrix(beltDistance + i * pathLength / 36);
        practice::DrawObj(practice::cube, practice::MakeModelMatrix(glm::vec3(0,0.008f,0), glm::vec3(0.96f,0.016f,0.045f), model), glm::vec3(0.55f,0.6f,0.65f), modelLocation, shader);
    }
    for (int i = 0; i < 5; ++i)
    {
        glm::mat4 model = MakeBeltModelMatrix(beltDistance - i * pathLength / 5);
        practice::DrawObj(practice::cube, practice::MakeModelMatrix(glm::vec3(0,0.28f,0), glm::vec3(0.52f,0.52f,0.52f), model), boxColor[i], modelLocation, shader);
        // 상자는 줄무늬의 윗면 위에 놓이며 숫자도 상자와 같은 변환을 받는다.
        DrawNumber(i + 1, glm::translate(model, glm::vec3(0,0.02f,0)), modelLocation, shader);
    }
}

int main()
{
    return practice::RunScene("homework20 - Parcel conveyor", KeyCallback, UpdateScene, DrawAll);
}

#include "Common/PracticeScene.h"
#include <random>

//------------------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------------------
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void UpdateScene(float deltaTime);
void DrawAll(common::Shader& shader, int width, int height);
void ResetScene();
void ChangeColors();
void DrawCube(glm::mat4 model, glm::vec3 position, glm::vec3 size, int color, GLint modelLocation, common::Shader& shader);
glm::mat4 MakeViewMatrix();

//------------------------------------------------------------------------------------------------------
// 전역 변수와 플래그
//------------------------------------------------------------------------------------------------------
float positionX = 0, positionZ = 0;
float lowerHeight = 0.4f, heightAngle = 0;
float centerAngle = 0, upperAngle = 0, upperTarget = 0;
float gunAngle = 0, flagAngle = 0;
float cameraYaw = 0;
float cameraSpinSpeed = 0, cameraOrbitSpeed = 0;
glm::vec3 cameraPosition(5,4,7);
bool isD = false, isT = false, isL = false, isG = false, isP = false;
glm::vec3 objColor[10] = {
    {0.25f,0.32f,0.38f}, {0.32f,0.38f,0.43f}, {0.35f,0.55f,0.32f},
    {0.55f,0.8f,0.25f}, {0.25f,0.65f,0.65f}, {0.55f,0.6f,0.2f},
    {0.2f,0.45f,0.35f}, {1,0.65f,0.15f}, {0.9f,0.3f,0.2f}, {0.35f,0.75f,0.9f}
};
std::mt19937 gen(std::random_device{}());

void ChangeColors()
{
    std::uniform_real_distribution<float> colorDist(0.2f, 0.95f);
    for (glm::vec3& color : objColor) color = glm::vec3(colorDist(gen), colorDist(gen), colorDist(gen));
}

void ResetScene()
{
    positionX = positionZ = 0;
    lowerHeight = 0.4f;
    heightAngle = centerAngle = upperAngle = upperTarget = gunAngle = flagAngle = 0;
    cameraYaw = cameraSpinSpeed = cameraOrbitSpeed = 0;
    cameraPosition = glm::vec3(5,4,7);
    isD = isT = isL = isG = isP = false;
    const glm::vec3 initial[10] = {
        {0.25f,0.32f,0.38f}, {0.32f,0.38f,0.43f}, {0.35f,0.55f,0.32f},
        {0.55f,0.8f,0.25f}, {0.25f,0.65f,0.65f}, {0.55f,0.6f,0.2f},
        {0.2f,0.45f,0.35f}, {1,0.65f,0.15f}, {0.9f,0.3f,0.2f}, {0.35f,0.75f,0.9f}
    };
    for (int i = 0; i < 10; ++i) objColor[i] = initial[i];
}

void UpdateScene(float deltaTime)
{
    if (isD)
    {
        heightAngle = std::fmod(heightAngle + 90 * deltaTime, 360.0f);
        lowerHeight = 0.4f + 0.15f * sin(glm::radians(heightAngle));
    }
    if (isT) centerAngle = std::fmod(centerAngle + 40 * deltaTime, 360.0f);
    if (isL)
    {
        // 두 상부 몸체를 반원 경로로 이동시켜 서로 통과하지 않고 교환한다.
        upperAngle = std::min(upperTarget, upperAngle + 90 * deltaTime);
        if (upperAngle >= upperTarget)
        {
            isL = false;
            upperAngle = std::fmod(upperAngle, 360.0f);
        }
    }
    if (isG) gunAngle = std::fmod(gunAngle + 45 * deltaTime, 360.0f);
    if (isP) flagAngle = std::fmod(flagAngle + 45 * deltaTime, 360.0f);
    cameraYaw = std::fmod(cameraYaw + cameraSpinSpeed * deltaTime, 360.0f);
    glm::mat4 orbit = glm::rotate(glm::mat4(1), glm::radians(cameraOrbitSpeed * deltaTime), glm::vec3(0,1,0));
    cameraPosition = glm::vec3(orbit * glm::vec4(cameraPosition, 1));
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
    float direction = (mods & GLFW_MOD_SHIFT) ? -1.0f : 1.0f;
    if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GLFW_TRUE);
    else if (key == GLFW_KEY_LEFT) positionX -= 0.15f;
    else if (key == GLFW_KEY_RIGHT) positionX += 0.15f;
    else if (key == GLFW_KEY_UP) positionZ -= 0.15f;
    else if (key == GLFW_KEY_DOWN) positionZ += 0.15f;
    else if (key == GLFW_KEY_Z) cameraPosition.z += direction * 0.2f;
    else if (key == GLFW_KEY_X) cameraPosition.x += direction * 0.2f;
    else if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_D) isD = !isD;
        else if (key == GLFW_KEY_T) isT = !isT;
        else if (key == GLFW_KEY_L && !isL) { upperTarget = upperAngle + 180; isL = true; }
        else if (key == GLFW_KEY_G) isG = !isG;
        else if (key == GLFW_KEY_P) isP = !isP;
        else if (key == GLFW_KEY_Y)
        {
            float speed = -direction * 35.0f;
            cameraSpinSpeed = cameraSpinSpeed == speed ? 0.0f : speed;
        }
        else if (key == GLFW_KEY_R)
        {
            float speed = -direction * 25.0f;
            cameraOrbitSpeed = cameraOrbitSpeed == speed ? 0.0f : speed;
        }
        else if (key == GLFW_KEY_O) ChangeColors();
        else if (key == GLFW_KEY_C) ResetScene();
    }
    // 중앙 몸체가 회전해도 탱크 전체가 바닥 안에 머물도록 여유를 둔다.
    positionX = std::clamp(positionX, -2.5f, 2.5f);
    positionZ = std::clamp(positionZ, -2.5f, 2.5f);
}

glm::mat4 MakeViewMatrix()
{
    glm::vec3 forward = glm::normalize(-cameraPosition);
    glm::vec3 worldUp = std::abs(forward.y) > 0.9999f ? glm::vec3(0,0,-1) : glm::vec3(0,1,0);
    glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
    glm::vec3 up = glm::normalize(glm::cross(right, forward));
    // 공전은 위치와 원점을 향하는 기준 방향을 함께 바꾸고, 자전은 위치를 유지한다.
    forward = glm::vec3(glm::rotate(glm::mat4(1), glm::radians(cameraYaw), up) * glm::vec4(forward, 0));
    return glm::lookAt(cameraPosition, cameraPosition + forward, up);
}

void DrawCube(glm::mat4 model, glm::vec3 position, glm::vec3 size, int color, GLint modelLocation, common::Shader& shader)
{
    practice::DrawObj(practice::cube, practice::MakeModelMatrix(position, size, model), objColor[color], modelLocation, shader);
}

void DrawAll(common::Shader& shader, int width, int height)
{
    GLint modelLocation = shader.uniformLocation("model");
    shader.setMatrix(shader.uniformLocation("view"), MakeViewMatrix());
    shader.setMatrix(shader.uniformLocation("projection"), practice::MakeProjection(width, height, true, 5));
    practice::DrawObj(practice::plane, practice::MakeModelMatrix(glm::vec3(0), glm::vec3(8,1,8)), glm::vec3(0.78f,0.83f,0.72f), modelLocation, shader);
    glm::mat4 base = glm::translate(glm::mat4(1), glm::vec3(positionX,0,positionZ));
    for (int i = 0; i < 2; ++i)
        DrawCube(base, glm::vec3(i == 0 ? -0.62f : 0.62f, lowerHeight / 2, 0), glm::vec3(0.5f,lowerHeight,1.7f), i, modelLocation, shader);
    glm::mat4 center = glm::translate(base, glm::vec3(0,lowerHeight + 0.13f,0));
    center = glm::rotate(center, glm::radians(centerAngle), glm::vec3(0,1,0));
    DrawCube(center, glm::vec3(0), glm::vec3(1.8f,0.26f,1.45f), 2, modelLocation, shader);
    for (int i = 0; i < 2; ++i)
    {
        float side = i == 0 ? -1.0f : 1.0f;
        float a = glm::radians(upperAngle);
        glm::mat4 upper = glm::translate(center, glm::vec3(side * 0.48f * cos(a),0.32f,side * 0.48f * sin(a)));
        DrawCube(upper, glm::vec3(0), i == 0 ? glm::vec3(0.65f,0.38f,0.7f) : glm::vec3(0.6f,0.32f,0.65f), 3+i, modelLocation, shader);
        glm::mat4 gun = glm::translate(upper, glm::vec3(0,0,0.32f));
        gun = glm::rotate(gun, glm::radians(side * gunAngle), glm::vec3(0,1,0));
        DrawCube(gun, glm::vec3(0,0,0.3f), glm::vec3(0.1f,0.1f,0.65f), 5+i, modelLocation, shader);
        glm::mat4 flag = glm::translate(upper, glm::vec3(0,0.2f,0));
        flag = glm::rotate(flag, glm::radians(side * flagAngle), glm::vec3(1,0,0));
        DrawCube(flag, glm::vec3(0,0.3f,0), glm::vec3(0.055f,0.6f,0.055f), 7+i, modelLocation, shader);
        DrawCube(flag, glm::vec3(0.12f,0.5f,0), glm::vec3(0.24f,0.16f,0.025f), 8+i, modelLocation, shader);
    }
}

int main()
{
    return practice::RunScene("homework19 - Tank and camera", KeyCallback, UpdateScene, DrawAll);
}

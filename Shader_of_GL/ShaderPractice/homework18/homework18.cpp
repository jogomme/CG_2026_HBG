#include "Common/PracticeScene.h"

//------------------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------------------
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void UpdateScene(float deltaTime);
void DrawAll(common::Shader& shader, int width, int height);
glm::mat4 MakeOrbitMatrix(int planet);

//------------------------------------------------------------------------------------------------------
// 전역 변수와 플래그
//------------------------------------------------------------------------------------------------------
float orbitAngle[3] = {0,120,240};
float orbitSpeed[3] = {25,40,60};
float orbitTilt[3] = {0,45,-45};
float moonAngle[3] = {0,120,240};
float moonOrbitAngle = 0;
float animationAngle = 0;
float radiusScale = 1;
glm::vec3 position(0);
bool isPerspective = false;
bool isW = false;
bool is1 = false, is2 = false, is3 = false;
const glm::vec3 planetColor[3] = { {0.2f,0.6f,1}, {0.4f,0.8f,0.25f}, {0.9f,0.25f,0.4f} };
const glm::vec3 moonColor[3] = { {0.65f,0.35f,0.9f}, {0.15f,0.75f,0.7f}, {0.8f,0.55f,0.2f} };

void UpdateScene(float deltaTime)
{
    for (int i = 0; i < 3; ++i)
    {
        orbitAngle[i] = std::fmod(orbitAngle[i] + orbitSpeed[i] * deltaTime, 360.0f);
        moonAngle[i] = std::fmod(moonAngle[i] + (90 + i * 20) * deltaTime, 360.0f);
    }
    if (is1) radiusScale += deltaTime * 0.3f;
    else radiusScale -= deltaTime * 0.3f;
    radiusScale = std::clamp(radiusScale, 1.0f, 1.6f);
    if (is2) animationAngle = std::fmod(animationAngle + 90 * deltaTime, 360.0f);
    if (is3) moonOrbitAngle = std::fmod(moonOrbitAngle + 60 * deltaTime, 360.0f);
}

glm::mat4 MakeOrbitMatrix(int planet)
{
    glm::mat4 model = glm::translate(glm::mat4(1), position);
    return glm::rotate(model, glm::radians(orbitTilt[planet]), glm::vec3(0,0,1));
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
    bool shift = (mods & GLFW_MOD_SHIFT) != 0;
    if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GLFW_TRUE);
    else if (key == GLFW_KEY_P) isPerspective = shift;
    else if (key == GLFW_KEY_M) isW = shift;
    else if (key == GLFW_KEY_A) position.x -= 0.15f;
    else if (key == GLFW_KEY_D) position.x += 0.15f;
    else if (key == GLFW_KEY_W) position.y += 0.15f;
    else if (key == GLFW_KEY_S) position.y -= 0.15f;
    else if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD) position.z += 0.15f;
    else if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) position.z -= 0.15f;
    else if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_1) is1 = !is1;
        else if (key == GLFW_KEY_2) is2 = !is2;
        else if (key == GLFW_KEY_3) is3 = !is3;
    }
}

void DrawAll(common::Shader& shader, int width, int height)
{
    GLint modelLocation = shader.uniformLocation("model");
    glm::mat4 view = glm::lookAt(glm::vec3(0,4.5f,11), glm::vec3(0), glm::vec3(0,1,0));
    shader.setMatrix(shader.uniformLocation("view"), view);
    shader.setMatrix(shader.uniformLocation("projection"), practice::MakeProjection(width, height, isPerspective, 4.8f));
    glPolygonMode(GL_FRONT_AND_BACK, isW ? GL_LINE : GL_FILL);
    float wave = sin(glm::radians(animationAngle));
    glm::vec3 centerColor(1.0f, 0.65f + 0.25f * wave, 0.15f + 0.1f * wave);
    practice::DrawObj(practice::sphere, practice::MakeModelMatrix(position, glm::vec3(0.5f + 0.12f * wave)), centerColor, modelLocation, shader);
    for (int i = 0; i < 3; ++i)
    {
        float radius = (1.5f + i * 0.75f) * radiusScale;
        glm::mat4 orbit = MakeOrbitMatrix(i);
        practice::DrawObj(practice::circle, glm::scale(orbit, glm::vec3(radius)), planetColor[i], modelLocation, shader);
        float angle = glm::radians(orbitAngle[i]);
        glm::vec3 planetPosition = glm::vec3(orbit * glm::vec4(radius * cos(angle), 0, radius * sin(angle), 1));
        practice::DrawObj(practice::sphere, practice::MakeModelMatrix(planetPosition, glm::vec3(0.22f)), planetColor[i], modelLocation, shader);

        // 화면의 오른쪽/위쪽 방향을 사용하여 달의 궤도를 화면에 평행하게 유지한다.
        glm::mat4 moonFrame = glm::translate(glm::mat4(1), planetPosition) * glm::mat4(glm::mat3(glm::inverse(view)));
        moonFrame = glm::rotate(moonFrame, glm::radians(moonOrbitAngle), glm::vec3(0,0,1));
        float moonRadius = 0.43f * radiusScale;
        glm::mat4 moonPath = glm::rotate(moonFrame, glm::radians(90.0f), glm::vec3(1,0,0));
        practice::DrawObj(practice::circle, glm::scale(moonPath, glm::vec3(moonRadius)), moonColor[i], modelLocation, shader);
        angle = glm::radians(moonAngle[i]);
        glm::vec3 moonPosition = glm::vec3(moonFrame * glm::vec4(moonRadius * cos(angle), moonRadius * sin(angle), 0, 1));
        practice::DrawObj(practice::sphere, practice::MakeModelMatrix(moonPosition, glm::vec3(0.085f)), moonColor[i], modelLocation, shader);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

int main()
{
    return practice::RunScene("homework18 - Planets and moons", KeyCallback, UpdateScene, DrawAll);
}

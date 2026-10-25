#include "Common/PracticeScene.h"

//------------------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------------------
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void UpdateScene(float deltaTime);
void DrawAll(common::Shader& shader, int width, int height);
void ResetScene();
void StartSwap(int mode);
glm::mat4 MakeAxisModelMatrix();
glm::mat4 MakeModelMatrix(int obj);

//------------------------------------------------------------------------------------------------------
// 전역 변수와 플래그
//------------------------------------------------------------------------------------------------------
glm::vec3 position[2] = { {-0.55f,0,0}, {0.55f,0,0} };
glm::vec3 swapStart[2]{};
float angleX[2]{}, angleY[2]{}, orbitAngle[2]{};
float objScale[2] = {1,1}, originScale[2] = {1,1};
int selectedObj = 3;
int swapMode = 0;
float swapTime = 0.0f, animationAngle = 0.0f;
bool isV = false, isC = false;

glm::mat4 MakeAxisModelMatrix()
{
    glm::mat4 model(1.0f);
    model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1,0,0));
    return glm::rotate(model, glm::radians(30.0f), glm::vec3(0,1,0));
}

glm::mat4 MakeModelMatrix(int obj)
{
    glm::mat4 model = MakeAxisModelMatrix();
    model = glm::rotate(model, glm::radians(orbitAngle[obj]), glm::vec3(0,1,0));
    model = glm::scale(model, glm::vec3(originScale[obj]));
    model = glm::translate(model, position[obj]);
    model = glm::rotate(model, glm::radians(angleX[obj]), glm::vec3(1,0,0));
    model = glm::rotate(model, glm::radians(angleY[obj]), glm::vec3(0,1,0));
    float size = objScale[obj];
    if (isV) size *= 1.0f + (obj == 0 ? 0.4f : -0.4f) * sin(glm::radians(animationAngle));
    return glm::scale(model, glm::vec3(0.35f * size));
}

void ResetScene()
{
    for (int i = 0; i < 2; ++i)
    {
        position[i] = glm::vec3(i == 0 ? -0.55f : 0.55f, 0, 0);
        angleX[i] = angleY[i] = orbitAngle[i] = 0.0f;
        objScale[i] = originScale[i] = 1.0f;
    }
    selectedObj = 3;
    swapMode = 0;
    swapTime = animationAngle = 0.0f;
    isV = isC = false;
}

void StartSwap(int mode)
{
    if (swapMode != 0) return;
    for (int i = 0; i < 2; ++i)
    {
        glm::mat4 orbit = glm::rotate(glm::mat4(1), glm::radians(orbitAngle[i]), glm::vec3(0,1,0));
        position[i] = glm::vec3(orbit * glm::vec4(position[i] * originScale[i], 1));
        orbitAngle[i] = 0;
        objScale[i] *= originScale[i];
        originScale[i] = 1;
        swapStart[i] = position[i];
    }
    isV = false;
    swapMode = mode;
    swapTime = 0;
}

void UpdateScene(float deltaTime)
{
    if (swapMode != 0)
    {
        swapTime = std::min(1.0f, swapTime + deltaTime * 0.5f);
        for (int i = 0; i < 2; ++i)
        {
            position[i] = glm::mix(swapStart[i], swapStart[1-i], swapTime);
            if (swapMode == 1)
            {
                // 개별 이동 후에도 두 도형은 반드시 원점을 통과한다.
                position[i] = swapTime < 0.5f
                    ? glm::mix(swapStart[i], glm::vec3(0), swapTime * 2)
                    : glm::mix(glm::vec3(0), swapStart[1-i], (swapTime - 0.5f) * 2);
            }
            if (swapMode == 2) position[i].y += (i == 0 ? 0.45f : -0.45f) * sin(glm::radians(180.0f * swapTime));
        }
        if (swapTime >= 1) swapMode = 0;
    }
    if (isV)
    {
        animationAngle = std::fmod(animationAngle + 90.0f * deltaTime, 360.0f);
        for (int i = 0; i < 2; ++i)
        {
            angleY[i] = std::fmod(angleY[i] + 90.0f * deltaTime, 360.0f);
            orbitAngle[i] = std::fmod(orbitAngle[i] + 45.0f * deltaTime, 360.0f);
        }
    }
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
    if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GLFW_TRUE);
    else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_3) selectedObj = key - GLFW_KEY_0;
    else if (key == GLFW_KEY_S) ResetScene();
    else if (key == GLFW_KEY_T && action == GLFW_PRESS) StartSwap(1);
    else if (key == GLFW_KEY_U && action == GLFW_PRESS) StartSwap(2);
    else if (key == GLFW_KEY_C && action == GLFW_PRESS) isC = !isC;
    else if (key == GLFW_KEY_V && action == GLFW_PRESS)
    {
        if (swapMode == 0) isV = !isV;
    }
    else if (swapMode == 0)
    {
        float direction = (mods & GLFW_MOD_SHIFT) ? -1.0f : 1.0f;
        for (int i = 0; i < 2; ++i)
        {
            if (selectedObj != 3 && selectedObj != i + 1) continue;
            if (key == GLFW_KEY_X) angleX[i] += direction * 5.0f;
            else if (key == GLFW_KEY_Y) angleY[i] += direction * 5.0f;
            else if (key == GLFW_KEY_R) orbitAngle[i] += direction * 5.0f;
            else if (key == GLFW_KEY_A) objScale[i] = std::clamp(objScale[i] * (direction > 0 ? 1.05f : 1.0f / 1.05f), 0.1f, 4.0f);
            else if (key == GLFW_KEY_B) originScale[i] = std::clamp(originScale[i] * (direction > 0 ? 1.05f : 1.0f / 1.05f), 0.1f, 3.0f);
            else if (key == GLFW_KEY_D) position[i].x -= direction * 0.05f;
            else if (key == GLFW_KEY_E) position[i].y += direction * 0.05f;
        }
    }
}

void DrawAll(common::Shader& shader, int width, int height)
{
    GLint modelLocation = shader.uniformLocation("model");
    shader.setMatrix(shader.uniformLocation("view"), glm::lookAt(glm::vec3(0,0,3.5f), glm::vec3(0), glm::vec3(0,1,0)));
    shader.setMatrix(shader.uniformLocation("projection"), practice::MakeProjection(width, height, true, 1.5f));
    practice::DrawObj(practice::axis, MakeAxisModelMatrix(), glm::vec3(1), modelLocation, shader);
    for (int i = 0; i < 2; ++i)
    {
        const practice::Mesh& mesh = isC ? (i == 0 ? practice::sphere : practice::cone) : (i == 0 ? practice::cone : practice::cube);
        glm::mat4 model = MakeModelMatrix(i);
        if (isC && i == 0) model = glm::scale(model, glm::vec3(0.5f));
        practice::DrawObj(mesh, model, i == 0 ? glm::vec3(0.95f,0.4f,0.15f) : glm::vec3(0.15f,0.45f,0.9f), modelLocation, shader);
    }
}

int main()
{
    return practice::RunScene("homework17 - Modeling transforms", KeyCallback, UpdateScene, DrawAll);
}

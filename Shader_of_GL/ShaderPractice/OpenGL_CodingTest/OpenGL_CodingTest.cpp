#include "TankPractice.h"

#include <cmath>

namespace
{
const glm::vec3 orbitCenter(0.0f);
const glm::vec3 orbitOffset(2.0f, 0.0f, 0.0f);
const glm::vec3 orbitAxis(0.0f, 1.0f, 0.0f);
const glm::vec3 spinAxis(0.0f, 1.0f, 0.0f);
const glm::vec3 objectSize(1.0f);

// 초당 회전각. 0이면 해당 각도가 유지된다. 음수는 반대 방향이다.
constexpr float orbitSpeed = 30.0f;
constexpr float spinSpeed = 90.0f;
float orbitAngle = 0.0f;
float spinAngle = 0.0f;

void KeyCallback(GLFWwindow* window, int key, int, int action, int)
{
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void UpdateScene(float dt)
{
    orbitAngle = std::fmod(orbitAngle + orbitSpeed * dt, 360.0f);
    spinAngle = std::fmod(spinAngle + spinSpeed * dt, 360.0f);
}

void DrawAll(common::Shader& shader, int width, int height)
{
    const glm::mat4 view = tank::MakeYawView(
        glm::vec3(0.0f, 2.0f, 6.0f),
        glm::vec3(0.0f),
        0.0f
    );
    const glm::mat4 projection = practice::MakeProjection(width, height, true, 4.0f);
    shader.setMatrix(shader.uniformLocation("view"), view);
    shader.setMatrix(shader.uniformLocation("projection"), projection);

    const glm::vec3 position = motion::OrbitPosition(orbitCenter, orbitOffset, orbitAxis, orbitAngle);
    const glm::mat4 frame = tank::PartFrame(glm::mat4(1.0f), position, spinAngle, spinAxis);
    tank::DrawCube(shader, shader.uniformLocation("model"), frame,
                   glm::vec3(0.0f), objectSize, glm::vec3(1.0f, 0.0f, 0.0f));
}
}

int main()
{
    return practice::RunScene("OpenGL CodingTest", KeyCallback, UpdateScene, DrawAll);
}

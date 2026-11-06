#include "TankPractice.h"

#include <cmath>

bool isPerspective = true;

// 초당 회전각. 0이면 해당 각도가 유지된다. 음수는 반대 방향이다.

void KeyCallback(GLFWwindow* window, int key, int, int action, int)
{
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (action == GLFW_PRESS && key == GLFW_KEY_P) {
        isPerspective = !isPerspective;
    }
}

void UpdateScene(float dt)
{

}

void DrawAll(common::Shader& shader, int width, int height)
{
    // 카메라 설정
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 2.0f, 6.0f),
        glm::vec3(0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // 원근 투영 설정
    glm::mat4 projection =
        practice::MakeProjection(width, height, isPerspective, 4.0f);

    shader.setMatrix(shader.uniformLocation("view"), view);
    shader.setMatrix(shader.uniformLocation("projection"), projection);

    GLint modelLocation = shader.uniformLocation("model");

    // 여기부터 직접 도형을 추가할 예정

    glm::mat4 model = practice::MakeModelMatrix(
        glm::vec3(0.0f, 0.0f, 0.0f),  // 위치
        glm::vec3(1.0f)               // 크기
    );

    practice::DrawObj(
        practice::cube,               // 출력할 도형
        model,                        // 모델 행렬
        glm::vec3(1.0f, 0.0f, 0.0f),  // 빨간색
        modelLocation,
        shader
    );
    
}

int main()
{
    return practice::RunScene("OpenGL CodingTest", KeyCallback, UpdateScene, DrawAll);
}

// 재사용하는 공통 기능은 Common에 두고, 과제별 장면과 입력은 이 파일에서 구현합니다.
// ../Common/Common.props를 참조하도록 프로젝트를 ShaderPractice 바로 아래에 생성합니다.
#include "GLCommon.h"
#include "Shader.h"
#include "ShaderSources.h"
#include "Camera.h"
#include "GLUtils.h"

#include <GL/glm/gtc/matrix_transform.hpp>

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
glm::mat4 MakeModelMatrix(float angle);

// 과제별 정점 데이터: 각 정점은 위치 xyz와 색상 rgb 순서로 구성합니다.
const float vertices[] = {
    -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f
};

int main()
{
    // 1. 창을 만들고 OpenGL 컨텍스트를 활성화한 뒤 GLEW를 초기화합니다.
    GLFWwindow* window = CreateGLWindow(1600, 1200, "__WINDOWTITLE__");
    if (!window) return -1;
    glfwSetKeyCallback(window, KeyCallback);

    // 2. 공통 기본 셰이더를 컴파일하고 링크합니다. 직접 작성한 셰이더를 사용하려면
    // 같은 Shader 생성자에 정점/프래그먼트 GLSL 소스 문자열을 전달합니다.
    int result = 0;
    {
        common::Shader shader(common::shader_sources::positionColorVertex,
                              common::shader_sources::positionColorFragment);
        if (!shader.valid()) {
            result = -1;
        } else {
            // 3. 과제 도형의 정점 데이터를 GPU에 올리고 위치 xyz/색상 rgb 배치를 설정합니다.
            GLuint vao = 0;
            GLuint vbo = 0;
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            common::configurePositionColorAttributes();

            shader.use();
            const GLint modelLocation = shader.uniformLocation("model");
            const GLint viewLocation = shader.uniformLocation("view");
            const GLint projectionLocation = shader.uniformLocation("projection");
            common::Camera camera(glm::vec3(0.0f, 0.0f, 2.5f),
                                  glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glEnable(GL_DEPTH_TEST);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

            // 4. 입력 처리 → 과제 상태 갱신 → 행렬 유니폼 설정 → 그리기 → 화면 출력 순서로 반복합니다.
            while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();
                if (glfwWindowShouldClose(window)) break;
                int width = 0;
                int height = 0;
                glfwGetFramebufferSize(window, &width, &height);
                if (width <= 0 || height <= 0) continue;
                glViewport(0, 0, width, height);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // 과제별 애니메이션과 변환은 이곳에서 구현합니다.
                const float angle = static_cast<float>(glfwGetTime()) * glm::radians(90.0f);
                shader.setMatrix(modelLocation, MakeModelMatrix(angle));
                shader.setMatrix(viewLocation, camera.view());
                shader.setMatrix(projectionLocation, camera.projection(width, height));
                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                glfwSwapBuffers(window);
            }

            // 5. OpenGL 컨텍스트가 활성화된 상태에서 과제 도형의 GPU 자원을 해제합니다.
            glDeleteBuffers(1, &vbo);
            glDeleteVertexArrays(1, &vao);
        }
    } // 컨텍스트를 종료하기 전에 Shader 소멸자가 셰이더 프로그램을 해제합니다.
    glfwDestroyWindow(window);
    glfwTerminate();
    return result;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS) return;
    // 과제별 키보드 명령은 이곳에 추가합니다.
    if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

glm::mat4 MakeModelMatrix(float angle)
{
    return glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
}

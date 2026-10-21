#include "GLCommon.h"
#include "Shader.h"
#include "ShaderSources.h"
#include "Camera.h"
#include "GLUtils.h"

#include <GL/glm/gtc/matrix_transform.hpp>



//------------------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------------------

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
glm::mat4 MakeModelMatrix(float angle);

//------------------------------------------------------------------------------------------------------
// 정점의 위치 데이터
//------------------------------------------------------------------------------------------------------
float axisVertices[] = {
    // X축 - 빨간색
    -0.8f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
     0.8f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,

     // Y축 - 초록색
      0.0f, -0.8f, 0.0f,  0.0f, 1.0f, 0.0f,
      0.0f,  0.8f, 0.0f,  0.0f, 1.0f, 0.0f,

      // Z축 - 파란색
       0.0f, 0.0f, -0.8f,  0.0f, 0.0f, 1.0f,
       0.0f, 0.0f,  0.8f,  0.0f, 0.0f, 1.0f
};

//------------------------------------------------------------------------------------------------------
// 플래그들
//------------------------------------------------------------------------------------------------------

bool is1 = false;
bool is2 = false;
bool is3 = false;

bool isX = false;
bool isY = false;

bool isR = false;
bool isA = false;
bool isB = false;

bool isD = false;
bool isE = false;

bool isT = false;
bool isU = false;
bool isV = false;

bool isC = false;

bool isS = false;

//------------------------------------------------------------------------------------------------------
// 전역 변수
//------------------------------------------------------------------------------------------------------
int width =  1600;
int height = 1200;

int main()
{
    GLFWwindow* window = CreateGLWindow(1600, 1200, "__WINDOWTITLE__");
    if (!window) return -1;
    glfwSetKeyCallback(window, KeyCallback);

    int result = 0;
    {
        common::Shader shader(common::shader_sources::positionColorVertex,
                              common::shader_sources::positionColorFragment);
        if (!shader.valid()) {
            result = -1;
        } else {
            GLuint vao = 0;
            GLuint vbo = 0;
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);
            common::configurePositionColorAttributes();

            shader.use();
            const GLint modelLocation = shader.uniformLocation("model");
            const GLint viewLocation = shader.uniformLocation("view");
            const GLint projectionLocation = shader.uniformLocation("projection");
            common::Camera camera(glm::vec3(0.0f, 0.0f, 2.5f),
                                  glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glEnable(GL_DEPTH_TEST);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

            while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();
                if (glfwWindowShouldClose(window)) break;

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

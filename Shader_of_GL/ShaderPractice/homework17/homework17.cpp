#include "GLCommon.h"
#include "Shader.h"
#include "ShaderSources.h"
#include "Camera.h"
#include "GLUtils.h"
#include "Scene.h"
#include "Geometry.h"
#include <array>
#include <iostream>

namespace {
// 이 구조체는 과제 도형의 GPU 핸들만 보관합니다. 범용 렌더러를 새로 만들지 않습니다.
// 생성과 해제 위치를 main에 명시하여 OpenGL 컨텍스트와 자원의 수명 관계를 드러냅니다.
struct LineMesh {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLsizei vertexCount = 0;
};

LineMesh upload(const homework17::LineVertices& vertices) {
    LineMesh mesh;
    mesh.vertexCount = static_cast<GLsizei>(vertices.size() / 6);
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
                 vertices.data(), GL_STATIC_DRAW);
    // VAO는 아래 설정과 연결된 VBO를 기억합니다. 매 프레임 속성을 다시 설정하지 않습니다.
    common::configurePositionColorAttributes();
    return mesh;
}

void draw(const LineMesh& mesh) {
    glBindVertexArray(mesh.vao);
    glDrawArrays(GL_LINES, 0, mesh.vertexCount);
}

void release(LineMesh& mesh) {
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteVertexArrays(1, &mesh.vao);
    mesh = {};
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
    if (action == GLFW_PRESS && (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
}

int main() {
    // GLCommon은 창, OpenGL 3.3 컨텍스트, GLEW 초기화를 담당합니다.
    GLFWwindow* window = CreateGLWindow(1400, 1000, "실습 17 - 모델링 변환");
    if (!window) return -1;
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);
    homework17::Scene scene;
    int result = 0;
    {
        // Shader는 GLCommon의 컴파일/링크 구현을 재사용하고 프로그램 삭제를 책임집니다.
        // 소멸 시 GPU에 접근하므로 이 중괄호 범위는 창/context 종료보다 먼저 끝나야 합니다.
        common::Shader shader(common::shader_sources::positionColorVertex,
                              common::shader_sources::positionColorFragment);
        if (!shader.valid()) {
            result = -1;
        } else {
            LineMesh axes = upload(homework17::makeAxes());
            std::array<LineMesh, 4> meshes = {upload(homework17::makeCone()),
                upload(homework17::makeCube()), upload(homework17::makeSphere()),
                upload(homework17::makePyramid())};
            shader.use();
            const GLint model = shader.uniformLocation("model");
            const GLint view = shader.uniformLocation("view");
            const GLint projection = shader.uniformLocation("projection");
            // 카메라 이동은 이번 과제의 명령이 아닙니다. 모델 변환만 바꾸고 시점은 고정합니다.
            common::Camera camera({0, 0, 7}, {0, 0, 0}, {0, 1, 0});
            shader.setMatrix(view, camera.view());
            glEnable(GL_DEPTH_TEST);
            glClearColor(1, 1, 1, 1);
            std::cout << "실습 17: 왼쪽 원뿔, 오른쪽 육면체 / q: 종료\n";

            while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();
                if (glfwWindowShouldClose(window)) break;
                int width = 0, height = 0;
                glfwGetFramebufferSize(window, &width, &height);
                if (width <= 0 || height <= 0) { glfwWaitEventsTimeout(0.05); continue; }
                glViewport(0, 0, width, height);
                shader.setMatrix(projection, camera.projection(width, height));
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                // 축에는 도형의 개별 변환을 적용하지 않고 공통 30도 기울기만 적용합니다.
                shader.setMatrix(model, homework17::sceneTilt());
                draw(axes);
                for (const auto& object : scene.objects) {
                    shader.setMatrix(model, homework17::objectModel(object));
                    draw(meshes[static_cast<std::size_t>(object.shape)]);
                }
                glfwSwapBuffers(window);
            }
            for (auto& mesh : meshes) release(mesh);
            release(axes);
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return result;
}

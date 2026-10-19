// Common owns reusable helpers; this file owns the assignment scene and input.
// Create the project directly under ShaderPractice to use ../Common/Common.props.
#include "GLCommon.h"
#include "Shader.h"
#include "ShaderSources.h"
#include "Camera.h"
#include "GLUtils.h"

#include <GL/glm/gtc/matrix_transform.hpp>

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
glm::mat4 MakeModelMatrix(float angle);

// Assignment data: xyz position followed by rgb color for each vertex.
const float vertices[] = {
    -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f
};

int main()
{
    // 1. Create the window and current OpenGL context, then initialize GLEW.
    GLFWwindow* window = CreateGLWindow(1600, 1200, "__WINDOWTITLE__");
    if (!window) return -1;
    glfwSetKeyCallback(window, KeyCallback);

    // 2. Compile/link the common defaults. For custom shaders, pass your own
    // vertex/fragment GLSL strings to this same Shader constructor.
    int result = 0;
    {
        common::Shader shader(common::shader_sources::positionColorVertex,
                              common::shader_sources::positionColorFragment);
        if (!shader.valid()) {
            result = -1;
        } else {
            // 3. Upload assignment geometry and describe its xyz/rgb layout.
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

            // 4. Input -> assignment update -> matrix uniforms -> draw -> present.
            while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();
                if (glfwWindowShouldClose(window)) break;
                int width = 0;
                int height = 0;
                glfwGetFramebufferSize(window, &width, &height);
                if (width <= 0 || height <= 0) continue;
                glViewport(0, 0, width, height);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // Assignment animation and transformations stay here.
                const float angle = static_cast<float>(glfwGetTime()) * glm::radians(90.0f);
                shader.setMatrix(modelLocation, MakeModelMatrix(angle));
                shader.setMatrix(viewLocation, camera.view());
                shader.setMatrix(projectionLocation, camera.projection(width, height));
                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                glfwSwapBuffers(window);
            }

            // 5. Release assignment geometry while the context is still current.
            glDeleteBuffers(1, &vbo);
            glDeleteVertexArrays(1, &vao);
        }
    } // Shader destructor releases the program before the context is destroyed.
    glfwDestroyWindow(window);
    glfwTerminate();
    return result;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS) return;
    // Add assignment-specific key commands here.
    if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

glm::mat4 MakeModelMatrix(float angle)
{
    return glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
}

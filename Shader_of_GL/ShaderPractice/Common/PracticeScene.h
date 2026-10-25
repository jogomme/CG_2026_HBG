#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "Shader.h"
#include "ShaderSources.h"
#include "GLUtils.h"
#include <GL/glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>
#include <vector>

// 실습 17~20의 정점 생성과 그리기만 공유한다. 변환과 입력은 각 cpp에 둔다.
namespace practice {
struct Mesh
{
    GLuint VAO{}, VBO{};
    GLsizei count{};
    GLenum mode = GL_TRIANGLES;
};

inline Mesh cube, sphere, cone, circle, axis, plane;
inline GLint colorLocation{};

inline void MakeVertices(std::vector<float>& vertices, glm::vec3 position, float shade = 1.0f)
{
    vertices.insert(vertices.end(), { position.x, position.y, position.z, shade, shade, shade });
}

inline void MakeMesh(Mesh& mesh, const std::vector<float>& vertices, GLenum mode = GL_TRIANGLES)
{
    mesh.mode = mode;
    mesh.count = static_cast<GLsizei>(vertices.size() / 6);
    glGenVertexArrays(1, &mesh.VAO);
    glBindVertexArray(mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    common::configurePositionColorAttributes();
}

inline void MakeObjects()
{
    const glm::vec3 corners[8] = {
        {-0.5f,-0.5f,0.5f}, {0.5f,-0.5f,0.5f}, {-0.5f,0.5f,0.5f}, {0.5f,0.5f,0.5f},
        {-0.5f,-0.5f,-0.5f}, {0.5f,-0.5f,-0.5f}, {-0.5f,0.5f,-0.5f}, {0.5f,0.5f,-0.5f}
    };
    const int faces[6][4] = { {0,1,2,3}, {5,4,7,6}, {1,5,3,7}, {4,0,6,2}, {2,3,6,7}, {4,5,0,1} };
    const int order[6] = {0,1,2,2,1,3};
    std::vector<float> vertices;
    for (int face = 0; face < 6; ++face)
        for (int i : order) MakeVertices(vertices, corners[faces[face][i]], 1.0f - face * 0.08f);
    MakeMesh(cube, vertices);

    vertices.clear();
    for (int y = 0; y < 20; ++y)
    {
        for (int x = 0; x < 32; ++x)
        {
            glm::vec3 points[4];
            for (int i = 0; i < 4; ++i)
            {
                float latitude = glm::radians(-90.0f + (y + i / 2) * 9.0f);
                float longitude = glm::radians((x + i % 2) * 11.25f);
                points[i] = {cos(latitude) * cos(longitude), sin(latitude), cos(latitude) * sin(longitude)};
            }
            for (int i : order) MakeVertices(vertices, points[i], 0.65f + 0.35f * (points[i].y + 1.0f) * 0.5f);
        }
    }
    MakeMesh(sphere, vertices);

    vertices.clear();
    for (int i = 0; i < 32; ++i)
    {
        float a = glm::radians(i * 11.25f), b = glm::radians((i + 1) * 11.25f);
        glm::vec3 first(0.5f * cos(a), -0.5f, 0.5f * sin(a));
        glm::vec3 second(0.5f * cos(b), -0.5f, 0.5f * sin(b));
        float shade = 0.65f + 0.3f * (cos(a) + 1.0f) * 0.5f;
        MakeVertices(vertices, first, shade); MakeVertices(vertices, second, shade);
        MakeVertices(vertices, {0,0.5f,0}, shade);
        MakeVertices(vertices, first, 0.6f); MakeVertices(vertices, {0,-0.5f,0}, 0.6f);
        MakeVertices(vertices, second, 0.6f);
    }
    MakeMesh(cone, vertices);

    vertices.clear();
    for (int i = 0; i < 128; ++i)
    {
        float a = glm::radians(i * 360.0f / 128);
        MakeVertices(vertices, {cos(a), 0, sin(a)});
    }
    MakeMesh(circle, vertices, GL_LINE_LOOP);
    MakeMesh(axis, { -1,0,0, 1,0,0, 1,0,0, 1,0,0,
                     0,-1,0, 0,1,0, 0,1,0, 0,1,0,
                     0,0,-1, 0,0,1, 0,0,1, 0,0,1 }, GL_LINES);
    vertices.clear();
    for (int i : order) MakeVertices(vertices, glm::vec3(corners[faces[4][i]].x, 0, corners[faces[4][i]].z));
    MakeMesh(plane, vertices);
}

inline glm::mat4 MakeModelMatrix(glm::vec3 position, glm::vec3 size, glm::mat4 parent = glm::mat4(1.0f))
{
    return glm::scale(glm::translate(parent, position), size);
}

inline void DrawObj(const Mesh& mesh, const glm::mat4& model, glm::vec3 color,
                    GLint modelLocation, common::Shader& shader)
{
    shader.setMatrix(modelLocation, model);
    glUniform3fv(colorLocation, 1, &color[0]);
    glBindVertexArray(mesh.VAO);
    glDrawArrays(mesh.mode, 0, mesh.count);
}

inline glm::mat4 MakeProjection(int width, int height, bool perspective, float extent)
{
    float aspect = static_cast<float>(width) / height;
    if (perspective) return glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    return glm::ortho(-extent * aspect, extent * aspect, -extent, extent, 0.1f, 100.0f);
}

inline int RunScene(const char* title, GLFWkeyfun keyCallback, void (*UpdateScene)(float),
                    void (*DrawAll)(common::Shader&, int, int))
{
    GLFWwindow* window = CreateGLWindow(1200, 900, title);
    if (!window) return -1;
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);
    int result = 0;
    {
        const char* fragment = R"glsl(
            #version 330 core
            in vec3 ourColor;
            uniform vec3 objectColor;
            out vec4 FragColor;
            void main() { FragColor = vec4(ourColor * objectColor, 1.0); }
        )glsl";
        common::Shader shader(common::shader_sources::positionColorVertex, fragment);
        if (!shader.valid()) result = -1;
        else
        {
            shader.use();
            colorLocation = shader.uniformLocation("objectColor");
            MakeObjects();
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.95f, 0.96f, 0.98f, 1.0f);
            double previousTime = glfwGetTime();
            while (!glfwWindowShouldClose(window))
            {
                glfwPollEvents();
                if (glfwWindowShouldClose(window)) break;
                double currentTime = glfwGetTime();
                float deltaTime = static_cast<float>(std::min(currentTime - previousTime, 0.05));
                previousTime = currentTime;
                int width, height;
                glfwGetFramebufferSize(window, &width, &height);
                if (width <= 0 || height <= 0) continue;
                UpdateScene(deltaTime);
                glViewport(0, 0, width, height);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                DrawAll(shader, width, height);
                glfwSwapBuffers(window);
            }
            for (Mesh* mesh : {&cube, &sphere, &cone, &circle, &axis, &plane})
            {
                glDeleteBuffers(1, &mesh->VBO);
                glDeleteVertexArrays(1, &mesh->VAO);
                *mesh = Mesh{};
            }
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return result;
}
}

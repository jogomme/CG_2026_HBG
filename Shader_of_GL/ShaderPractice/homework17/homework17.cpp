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

glm::mat4 MakeModelMatrix();
glm::mat4 MakeAxisModelMatrix();


void MakeVertices(float* vertices, int& index, const glm::vec3& position, const glm::vec3& color);

void MakeCubeVertices();

void MakeConeVertices();
void MakeConeCorners();
void MakeConeEdges();

void DrawAll(GLint modelLocation, common::Shader& shader);
void DrawAxis(GLint modelLocation, common::Shader& shader);
void DrawObj(GLint modelLocation, common::Shader& shader);

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

float h{ 0.1f };


glm::vec3 cubeCorners[8] = {
    {-h, -h,  h},  // 0
    { h, -h,  h},  // 1
    {-h,  h,  h},  // 2
    { h,  h,  h},  // 3
    {-h, -h, -h},  // 4
    { h, -h, -h},  // 5
    {-h,  h, -h},  // 6
    { h,  h, -h}   // 7
};

int cubeEdges[12][2] = {
    {0, 1}, {1, 3}, {3, 2}, {2, 0}, // 앞면
    {4, 5}, {5, 7}, {7, 6}, {6, 4}, // 뒷면
    {0, 4}, {1, 5}, {2, 6}, {3, 7}  // 앞뒤 연결
};

float cubeVertices[24 * 6]{};

glm::vec3 pyramidCorners[5] = {
    {-h, -h,  h},  // 0: 바닥 앞쪽 왼쪽
    { h, -h,  h},  // 1: 바닥 앞쪽 오른쪽
    {-h, -h, -h},  // 2: 바닥 뒤쪽 왼쪽
    { h, -h, -h},  // 3: 바닥 뒤쪽 오른쪽
    {0.0f, h, 0.0f} // 4: 꼭대기
};

int pyramidEdges[8][2] = {
    // 밑면 모서리 4개
    {0, 1}, {1, 3}, {3, 2}, {2, 0},

    // 꼭대기로 연결되는 모서리 4개
    {0, 4}, {1, 4}, {2, 4}, {3, 4}
};

float pyramidVertices[16 * 6]{};

const int coneSegments = 12;

glm::vec3 coneCorners[coneSegments + 1];

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

float axisAngle = 30.0f;
glm::mat4 axisModel = MakeAxisModelMatrix();

GLuint axisVAO;
GLuint axisVBO;

float positionX[2]{ 0.3f, 0.0f };
float positionY[2]{ };
float positionZ[2]{ };


float angleX[2]{};
float angleY[2]{};
float angleZ[2]{};

GLuint objVAO;
GLuint objVBO;

//------------------------------------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------------------------------------
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

            glGenVertexArrays(1, &axisVAO);
            glBindVertexArray(axisVAO);
            glGenBuffers(1, &axisVBO);
            glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);
            common::configurePositionColorAttributes();

            MakeCubeVertices();
            glGenVertexArrays(1, &objVAO);
            glBindVertexArray(objVAO);

            glGenBuffers(1, &objVBO);
            glBindBuffer(GL_ARRAY_BUFFER, objVBO);

            glBufferData(
                GL_ARRAY_BUFFER,
                sizeof(cubeVertices),
                cubeVertices,
                GL_STATIC_DRAW
            );


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
                shader.setMatrix(viewLocation, camera.view());
                shader.setMatrix(projectionLocation, camera.projection(width, height));
                shader.setMatrix(modelLocation, axisModel);
               
                glBindVertexArray(axisVAO);
                
                DrawAll(modelLocation, shader);
                glfwSwapBuffers(window);
            }

            // 5. OpenGL 컨텍스트가 활성화된 상태에서 과제 도형의 GPU 자원을 해제합니다.
            glDeleteBuffers(1, &axisVBO);
            glDeleteVertexArrays(1, &axisVAO);

            glDeleteBuffers(1, &objVBO);
            glDeleteVertexArrays(1, &objVAO);
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

glm::mat4 MakeModelMatrix()
{
    glm::mat4 model(1.0f);

    glm::vec3 move = glm::vec3(axisModel * glm::vec4(positionX[0], positionY[0], positionZ[0], 0.0f));

    model = glm::translate(model, move);

    model *= axisModel;

    model = glm::rotate(model, glm::radians(angleX[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angleY[0]), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angleZ[0]), glm::vec3(0.0f, 0.0f, 1.0f));

    return model;
}

glm::mat4 MakeAxisModelMatrix()
{
    glm::mat4 model(1.0f);

    model = glm::rotate(model, glm::radians(axisAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(axisAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    return model;
}

void DrawAll(GLint modelLocation, common::Shader& shader)
{
    DrawAxis(modelLocation, shader);
    DrawObj(modelLocation, shader);
}

void DrawAxis(GLint modelLocation, common::Shader& shader)
{
    shader.setMatrix(modelLocation, axisModel);

    glBindVertexArray(axisVAO);
    glDrawArrays(GL_LINES, 0, 6);
}

void DrawObj(GLint modelLocation, common::Shader& shader)
{
    glm::mat4 model = MakeModelMatrix();

    shader.setMatrix(modelLocation, model);

    glBindVertexArray(objVAO);

    glDrawArrays(GL_LINES, 0, 24);
}

void MakeCubeVertices()
{
    int index = 0;

    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            const glm::vec3 color(0, 0, 0);

            int corner = cubeEdges[i][j];

            MakeVertices(cubeVertices, index, cubeCorners[corner], color);

        }
    }
}

void MakeConeVertices()
{
    int index = 0;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            const glm::vec3 color(0, 0, 0);

            int corner = pyramidEdges[i][j];

            MakeVertices(pyramidVertices, index, pyramidCorners[corner], color);

        }
    }
}

void MakeConeCorners()
{
    for (int i = 0; i < coneSegments; i++)
    {
        float angle = glm::radians(360.0f * i / coneSegments);

        coneCorners[i] = glm::vec3(
            h * cos(angle),
            -h,
            h * sin(angle)
        );
    }

    coneCorners[coneSegments] = glm::vec3(0.0f, h, 0.0f);
}

int coneEdges[coneSegments * 2][2];

void MakeConeEdges()
{
    for (int i = 0; i < coneSegments; i++)
    {
        // 밑면의 이웃한 점 연결
        coneEdges[i][0] = i;
        coneEdges[i][1] = (i + 1) % coneSegments;

        // 밑면에서 꼭대기까지 연결
        coneEdges[coneSegments + i][0] = i;
        coneEdges[coneSegments + i][1] = coneSegments;
    }
}

void MakeVertices(float* vertices, int& index, const glm::vec3& position, const glm::vec3& color)
{
    vertices[index++] = position.x;
    vertices[index++] = position.y;
    vertices[index++] = position.z;

    vertices[index++] = color.r;
    vertices[index++] = color.g;
    vertices[index++] = color.b;
}
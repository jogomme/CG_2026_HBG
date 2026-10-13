#include "GLCommon.h"

#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <iostream>
#include <random>
//------------------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------------------

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

glm::mat4 MakeAxisModelMatrix();
glm::mat4 MakeObjectModelMatrix();

void MakeVertices(float* vertices, int& index, const glm::vec3& position, const glm::vec3& color);

void MakeCubeVertices();
void MakeCubeIndices();

void MakePyramidVertices();
void MakePyramidIndices();

void DrawAll(GLint modelLocation);
void DrawAxis(GLint modelLocation);
void DrawObj(GLint modelLocation);

//------------------------------------------------------------------------------------------------------
// 정점의 위치 데이터
//------------------------------------------------------------------------------------------------------

// 위치 3개 + 색상 3개 = 한 정점 6 float
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

float h{ 0.3f };

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

int cubeFaces[6][4] = {
    {0, 1, 2, 3}, // 앞면
    {4, 5, 6, 7}, // 뒷면
    {1, 5, 3, 7}, // 오른쪽 면
    {4, 0, 6, 2}, // 왼쪽 면
    {2, 3, 6, 7}, // 윗면
    {4, 5, 0, 1}  // 아랫면
};

float cubeVertices[6 * 4 * 6]{};

unsigned int cubeIndices[6 * 2 * 3]{};
unsigned int cubeEdgeIndices[6 * 8]{}; // 4 outline edges per face

glm::vec3 pyramidCorners[5] = {
    {-h, -h,  h},  // 0: 바닥 앞쪽 왼쪽
    { h, -h,  h},  // 1: 바닥 앞쪽 오른쪽
    {-h, -h, -h},  // 2: 바닥 뒤쪽 왼쪽
    { h, -h, -h},  // 3: 바닥 뒤쪽 오른쪽
    {0.0f, h, 0.0f} // 4: 꼭대기
};

int pyramidFaces[4][3] = {
    {0, 1, 4},
    {1, 3, 4},
    {3, 2, 4},
    {2, 0, 4}
};

int pyramidBottom[4] = { 0, 1, 2, 3 };

float pyramidVertices[5 * 6]{};

unsigned int pyramidIndices[6 * 3]{};

bool pyramidMode{};            // false: 육면체, true: 사각뿔

//------------------------------------------------------------------------------------------------------
// 전역 변수
//------------------------------------------------------------------------------------------------------

int wide = 1600;
int height = 1200;

std::random_device rd;
std::mt19937 gen(rd());

float angleX = 30.0f;
float angleY = 30.0f;
float angleZ = 0.0f;

float rotationSpeedX = 0.0f;
float rotationSpeedY = 0.0f;
float rotationSpeedZ = 0.0f;

bool isTopRotating = false;
float topAngle = 0.0f;
float topRotationSpeed = 30.0f;

bool isSideRotating = false;
float sideAngle = 0.0f;
float sideRotationSpeed = 30.0f;

bool isFrontAnimating = false;
float frontAngle = 0.0f;
bool isOpen = false;
float frontRotationSpeed = 30.0f;

float positionX = 0.0f;
float positionY = 0.0f;

float posSpeedXR = 0.0f;
float posSpeedXL = 0.0f;

float posSpeedYU = 0.0f;
float posSpeedYD = 0.0f;

float axisangle = 30.0f;
glm::mat4 axismodel = MakeAxisModelMatrix();

bool isH = true;
bool isW = false;

bool isX = false;
bool isShiftX = false;

bool isY = false;
bool isShiftY = false;

bool isZ = false;
bool isShiftZ = false;

bool isT = false;
bool isShiftT = false;

bool isL = false;
bool isR = false;

bool isUp = false;
bool isDown = false;

bool isB = false;

float backScale = 1.0f;
float ScaleSpeed = 0.5f;

GLuint VAO;
GLuint VBO;

GLuint cubeVAO;
GLuint cubeVBO;
GLuint cubeEBO;
GLuint cubeEdgeEBO;

GLuint pyramidVAO;
GLuint pyramidVBO;
GLuint pyramidEBO;

int main()
//------------------------------------------------------------------------------------------------------
{
    //------------------------------------------------------------------------------------------
    // 창 생성 + GLEW 초기화 + viewport + 콘솔 UTF-8
    //------------------------------------------------------------------------------------------
    GLFWwindow* window = CreateGLWindow(
        wide,
        height,
        "__WINDOWTITLE__"
    );

    if (!window)
    {
        return -1;
    }

    //------------------------------------------------------------------------------------------
    // 키 콜백 함수
    //------------------------------------------------------------------------------------------
    glfwSetKeyCallback(window, KeyCallback);

    //------------------------------------------------------------------------------------------
    // VAO / VBO 설정
    //------------------------------------------------------------------------------------------
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    MakeCubeVertices();
    MakeCubeIndices();

    // Face-local vertices are ordered 0,1,2,3; boundary order is 0,1,3,2.
    for (int face = 0; face < 6; ++face) {
        unsigned int base = face * 4;
        int i = face * 8;
        cubeEdgeIndices[i + 0] = base + 0;
        cubeEdgeIndices[i + 1] = base + 1;
        cubeEdgeIndices[i + 2] = base + 1;
        cubeEdgeIndices[i + 3] = base + 3;
        cubeEdgeIndices[i + 4] = base + 3;
        cubeEdgeIndices[i + 5] = base + 2;
        cubeEdgeIndices[i + 6] = base + 2;
        cubeEdgeIndices[i + 7] = base + 0;
    }

    MakePyramidVertices();
    MakePyramidIndices();

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(axisVertices),
        axisVertices,
        GL_STATIC_DRAW
    );

    // 위치
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // 색상
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    // Cube 관련

    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(cubeVertices),
        cubeVertices,
        GL_STATIC_DRAW
    );

    glGenBuffers(1, &cubeEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(cubeIndices),
        cubeIndices,
        GL_STATIC_DRAW
    );

    glGenBuffers(1, &cubeEdgeEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEdgeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeEdgeIndices), cubeEdgeIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);

    // 육면체 정점 위치 설정
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // 육면체 정점 색상 설정
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);

    // 사각뿔

    glGenVertexArrays(1, &pyramidVAO);
    glBindVertexArray(pyramidVAO);

    glGenBuffers(1, &pyramidVBO);
    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(pyramidVertices),
        pyramidVertices,
        GL_STATIC_DRAW
    );

    glGenBuffers(1, &pyramidEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramidEBO);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(pyramidIndices),
        pyramidIndices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);

    //------------------------------------------------------------------------------------------
    // Vertex Shader
    //------------------------------------------------------------------------------------------
    const char* vertexShaderSource = R"(
    #version 330 core

    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;

    uniform mat4 model;

    out vec3 ourColor;

    void main()
    {
        gl_Position = model * vec4(aPos, 1.0);
        ourColor = aColor;
    }
    )";

    //------------------------------------------------------------------------------------------
    // Fragment Shader
    //------------------------------------------------------------------------------------------
    const char* fragmentShaderSource = R"(
    #version 330 core

    in vec3 ourColor;

    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(ourColor, 1.0);
    }
    )";

    //------------------------------------------------------------------------------------------
    // Shader Program 생성 ( 컴파일 + 링크 )
    //------------------------------------------------------------------------------------------
    GLuint shaderProgram = CreateProgram(
        vertexShaderSource,
        fragmentShaderSource
    );

    if (shaderProgram == 0)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    //------------------------------------------------------------------------------------------
    // Program 사용
    //------------------------------------------------------------------------------------------
    glUseProgram(shaderProgram);
    glEnable(GL_DEPTH_TEST);

    //------------------------------------------------------------------------------------------
    // 화면 배경색 설정
    //------------------------------------------------------------------------------------------
    glClearColor(
        0.0f,
        0.0f,
        0.0f,
        1.0f
    );

    GLint modelLocation = glGetUniformLocation(shaderProgram, "model");

    double endTime = glfwGetTime();

    //------------------------------------------------------------------------------------------
    // 메인 루프
    //------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // ------------------------------------------------------------
        // 화면 지우기
        // ------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ------------------------------------------------------------
        // 회전
        // ------------------------------------------------------------
        double startTime = glfwGetTime();
        double dTime = startTime - endTime;
        endTime = startTime;

        positionX += (posSpeedXL + posSpeedXR) * dTime;
        positionY += (posSpeedYD + posSpeedYU) * dTime;

        angleX = fmodf(angleX + rotationSpeedX * dTime, 360.0f);
        angleY = fmodf(angleY + rotationSpeedY * dTime, 360.0f);
        angleZ = fmodf(angleZ + rotationSpeedZ * dTime, 360.0f);

        if (isTopRotating) {
            topAngle = fmodf(
                topAngle + topRotationSpeed * dTime,
                360.0f
            );
        }

        if (isSideRotating) {
            sideAngle = fmodf(
                sideAngle + sideRotationSpeed * dTime,
                360.0f
            );
        }

        if (isFrontAnimating) {
            frontAngle += frontRotationSpeed * dTime;

            if (frontAngle >= 90.0f) {
                frontAngle = 90.0f;
                frontRotationSpeed = -abs(frontRotationSpeed);
            }
            else if (frontAngle <= 0.0f) {
                frontAngle = 0.0f;
                frontRotationSpeed = abs(frontRotationSpeed);
            }
        }

        if (isB) {
            backScale += ScaleSpeed * dTime;

            if (backScale >= 1.0f) {
                backScale = 1.0f;
                ScaleSpeed = -abs(ScaleSpeed);
            }
            else if (backScale <= 0.0f) {
                backScale = 0.0f;
                ScaleSpeed = abs(ScaleSpeed);
            }

        }

        // ------------------------------------------------------------
        // 그리기
        // ------------------------------------------------------------
        DrawAll(modelLocation);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //------------------------------------------------------------------------------------------
    // 종료
    //------------------------------------------------------------------------------------------
    glDeleteBuffers(1, &cubeEdgeEBO);
    glDeleteBuffers(1, &cubeEBO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_RIGHT) {

        if (action == GLFW_PRESS) {
            isR = true;
        }
        else if (action == GLFW_RELEASE) {
            isR = false;
        }

        if (isR) {
            posSpeedXR = 0.5f;
        }
        else {
            posSpeedXR = 0;
        }
    }
    if (key == GLFW_KEY_LEFT) {
        if (action == GLFW_PRESS) {
            isL = true;
        }
        else if (action == GLFW_RELEASE) {
            isL = false;
        }

        if (isL) {
            posSpeedXL = -0.5f;
        }
        else {
            posSpeedXL = 0;
        }
    }
    if (key == GLFW_KEY_UP) {
        if (action == GLFW_PRESS) {
            isUp = true;
        }
        else if (action == GLFW_RELEASE) {
            isUp = false;
        }

        if (isUp) {
            posSpeedYU = 0.5f;
        }
        else {
            posSpeedYU = 0;
        }
    }
    if (key == GLFW_KEY_DOWN) {
        if (action == GLFW_PRESS) {
            isDown = true;
        }
        else if (action == GLFW_RELEASE) {
            isDown = false;
        }

        if (isDown) {
            posSpeedYD = -0.5f;
        }
        else {
            posSpeedYD = 0;
        }
    }

    if (action != GLFW_PRESS)
    {
        return;
    }

    if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (key == GLFW_KEY_C) {
        pyramidMode = false;

        MakeCubeVertices();

        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

        glBufferSubData(
            GL_ARRAY_BUFFER,
            0,
            sizeof(cubeVertices),
            cubeVertices
        );
    }
    else if (key == GLFW_KEY_P) {
        pyramidMode = !pyramidMode;

        MakePyramidVertices();

        glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);

        glBufferSubData(
            GL_ARRAY_BUFFER,
            0,
            sizeof(pyramidVertices),
            pyramidVertices
        );
    }
    else if (key == GLFW_KEY_H) {

        isH = !isH;

        if (isH)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }
    }
    else if (key == GLFW_KEY_W) {
        isW = !isW;

        if (isW) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 와이어프레임
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // 솔리드
        }
    }
    else if (key == GLFW_KEY_X) {

        if (mods & GLFW_MOD_SHIFT) {
            isShiftX = !isShiftX;

            if (isShiftX) {
                rotationSpeedX -= 30.0f;
            }
            else {
                rotationSpeedX += 30.0f;
            }
        }
        else {
            isX = !isX;

            if (isX) {
                rotationSpeedX += 30.0f;
            }
            else {
                rotationSpeedX -= 30.0f;
            }
        }
        std::cout << "현재 rotateX 확인 : Shift X - " << isShiftX <<
            " X - " << isX <<
            '\n';
    }
    else if (key == GLFW_KEY_Y) {

        if (mods & GLFW_MOD_SHIFT) {
            isShiftY = !isShiftY;

            if (isShiftY) {
                rotationSpeedY -= 30.0f;
            }
            else {
                rotationSpeedY += 30.0f;
            }
        }
        else {
            isY = !isY;

            if (isY) {
                rotationSpeedY += 30.0f;
            }
            else {
                rotationSpeedY -= 30.0f;
            }
        }
        std::cout << "현재 rotateY 확인 : Shift Y - " << isShiftY <<
            " Y - " << isY << '\n';
    }
    else if (key == GLFW_KEY_Z) {

        if (mods & GLFW_MOD_SHIFT) {
            isShiftZ = !isShiftZ;

            if (isShiftZ) {
                rotationSpeedZ -= 30.0f;
            }
            else {
                rotationSpeedZ += 30.0f;
            }
        }
        else {
            isZ = !isZ;

            if (isZ) {
                rotationSpeedZ += 30.0f;
            }
            else {
                rotationSpeedZ -= 30.0f;
            }
        }
        
            }
    else if (key == GLFW_KEY_T) {
        isTopRotating = !isTopRotating;
    }
    else if (key == GLFW_KEY_S) {
        isSideRotating = !isSideRotating;
    }
    else if (key == GLFW_KEY_F) {
        isFrontAnimating = !isFrontAnimating;
    }
    else if (key == GLFW_KEY_R) {
        pyramidMode = false;

        // 객체 위치 초기화
        positionX = 0.0f;
        positionY = 0.0f;

        // 회전 정지
        rotationSpeedX = 0.0f;
        rotationSpeedY = 0.0f;
        rotationSpeedZ = 0.0f;

        // 회전 키 토글 상태 초기화
        isX = false;
        isShiftX = false;
        isY = false;
        isShiftY = false;
        isT = false;
        isShiftT = false;

        // 이동 속도 초기화
        posSpeedXR = 0.0f;
        posSpeedXL = 0.0f;
        posSpeedYU = 0.0f;
        posSpeedYD = 0.0f;

        // 이동 키 상태 초기화
        isR = false;
        isL = false;
        isUp = false;
        isDown = false;

        angleX = 30.0f;
        angleY = 30.0f;
        angleZ = 0.0f;

        isTopRotating = false;
        topAngle = 0.0f;
        isSideRotating = false;
        sideAngle = 0.0f;

        isFrontAnimating = false;
        frontAngle = 0.0f;
        isOpen = false;
        frontRotationSpeed = 30.0f;

        isB = false;
        backScale = 1.0f;
        ScaleSpeed = 0.5f;
    }
    else if (key == GLFW_KEY_B) {
        isB = !isB;
    }
}

glm::mat4 MakeAxisModelMatrix()
{
    glm::mat4 model(1.0f);

    model = glm::rotate(model, glm::radians(axisangle), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(axisangle), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    return model;
}

glm::mat4 MakeObjectModelMatrix()
{
    glm::mat4 model(1.0f);

    glm::vec3 movement = glm::vec3(axismodel * glm::vec4(positionX, positionY, 0.0f, 0.0f));

    model = glm::translate(model, movement);

    model = glm::rotate(model, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angleZ), glm::vec3(0.0f, 0.0f, 1.0f));

    return model;
}

void DrawAll(GLint modelLocation)
{
    DrawAxis(modelLocation);
    DrawObj(modelLocation);
}

void DrawAxis(GLint modelLocation)
{

    glUniformMatrix4fv(
        modelLocation,
        1,
        GL_FALSE,
        &axismodel[0][0]
    );

    glBindVertexArray(VAO);

    glDrawArrays(
        GL_LINES,
        0,
        6
    );
}
void DrawObj(GLint modelLocation)
{
    glm::mat4 objmodel = MakeObjectModelMatrix();

    glUniformMatrix4fv(
        modelLocation,
        1,
        GL_FALSE,
        &objmodel[0][0]
    );

    if (!pyramidMode) {
        glBindVertexArray(cubeVAO);

        if (isW) {
            // Wireframe: draw face boundaries, not triangulation diagonals.
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEdgeEBO);
            glDrawElements(GL_LINES, 6 * 8, GL_UNSIGNED_INT, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
        }
        else {
            for (int face = 0; face < 6; ++face)
            {
                glm::mat4 faceModel = objmodel;

                if (face == 4)
                {
                    faceModel = glm::translate(
                        faceModel,
                        glm::vec3(0.0f, h, 0.0f)
                    );

                    faceModel = glm::rotate(
                        faceModel,
                        glm::radians(topAngle),
                        glm::vec3(0.0f, 0.0f, 1.0f)
                    );

                    faceModel = glm::translate(
                        faceModel,
                        glm::vec3(0.0f, -h, 0.0f)
                    );
                }

                if (face == 2 || face == 3)
                {
                    const float sideX = (face == 2) ? h : -h;
                    const float direction = (face == 2) ? 1.0f : -1.0f;

                    faceModel = glm::translate(faceModel, glm::vec3(sideX, 0.0f, 0.0f));
                    faceModel = glm::rotate(faceModel, glm::radians(sideAngle), glm::vec3(direction, 0.0f, 0.0f));
                    faceModel = glm::translate(faceModel, glm::vec3(-sideX, 0.0f, 0.0f));
                }

                if (face == 0)
                {
                    faceModel = glm::translate(
                        faceModel,
                        glm::vec3(0.0f, h, h)
                    );

                    faceModel = glm::rotate(
                        faceModel,
                        glm::radians(-frontAngle),
                        glm::vec3(1.0f, 0.0f, 0.0f)
                    );

                    faceModel = glm::translate(
                        faceModel,
                        glm::vec3(0.0f, -h, -h)
                    );
                }

                if (face == 1) {
                    faceModel = glm::translate(
                        faceModel, glm::vec3(0.0f, 0.0f, -h)
                    );

                    faceModel = glm::scale(
                        faceModel, glm::vec3(backScale)
                    );

                    faceModel = glm::translate(
                        faceModel, glm::vec3(0.0f, 0.0f, h)
                    );
                }

                glUniformMatrix4fv(
                    modelLocation,
                    1,
                    GL_FALSE,
                    &faceModel[0][0]
                );

                glDrawElements(
                    GL_TRIANGLES,
                    6,
                    GL_UNSIGNED_INT,
                    (void*)(face * 6 * sizeof(unsigned int))
                );
            }
        }
    }
    else {
        glBindVertexArray(pyramidVAO);

        glDrawElements(
            GL_TRIANGLES,
            18,
            GL_UNSIGNED_INT,
            0
        );
    }
}

void MakeCubeIndices()
{
    int index = 0;

    for (int face = 0; face < 6; ++face)
    {
        unsigned int base = face * 4;

        if (face == 1 || face == 3 || face == 5)
        {
            cubeIndices[index++] = base + 0;
            cubeIndices[index++] = base + 2;
            cubeIndices[index++] = base + 1;

            cubeIndices[index++] = base + 2;
            cubeIndices[index++] = base + 3;
            cubeIndices[index++] = base + 1;
        }
        else
        {
            cubeIndices[index++] = base + 0;
            cubeIndices[index++] = base + 1;
            cubeIndices[index++] = base + 2;

            cubeIndices[index++] = base + 2;
            cubeIndices[index++] = base + 1;
            cubeIndices[index++] = base + 3;
        }
    }
}

void MakePyramidIndices()
{
    int index = 0;

    for (int i = 0; i < 4; i++)
    {
        int a = pyramidFaces[i][0];
        int b = pyramidFaces[i][1];
        int c = pyramidFaces[i][2];

        pyramidIndices[index++] = a;
        pyramidIndices[index++] = b;
        pyramidIndices[index++] = c;
    }

    int a = pyramidBottom[0];
    int b = pyramidBottom[1];
    int c = pyramidBottom[2];
    int d = pyramidBottom[3];

    pyramidIndices[index++] = a;
    pyramidIndices[index++] = c;
    pyramidIndices[index++] = b;

    pyramidIndices[index++] = c;
    pyramidIndices[index++] = d;
    pyramidIndices[index++] = b;
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

void MakeCubeVertices()
{
    int index{};

    std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);

    for (int face = 0; face < 6; ++face) {

    for (int vertex = 0; vertex < 4; ++vertex) {

        glm::vec3 color = { colorDist(gen),colorDist(gen), colorDist(gen) };

        int cornerIndex = cubeFaces[face][vertex];

        MakeVertices(
            cubeVertices,
            index,
            cubeCorners[cornerIndex],
            color
        );
    }
}
}

void MakePyramidVertices()
{
    int index{};

    std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);

    for (int i = 0; i < 5; ++i) {

        glm::vec3 color = { colorDist(gen), colorDist(gen), colorDist(gen) };

        MakeVertices(pyramidVertices, index, pyramidCorners[i], color);
    }
}
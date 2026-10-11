#define NOMINMAX
#include <windows.h>

#include <GL/glew.h>
#include <GL/glfw3.h>
#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <iostream>
#include <random>

// ------------------------------------------------------------
// 함수 선언
// ------------------------------------------------------------
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
bool CheckShader(GLuint shader);
bool CheckProgram(GLuint program);
glm::mat4 MakeModelMatrix(float angleX, float angleY);
void MakeCubeVertices();
void DrawCube();
void MakePyramidVertices();
void DrawPyramid();

// ------------------------------------------------------------
// 좌표축 정점: 위치 3개 + 색상 3개
// ------------------------------------------------------------
float vertices[] = {
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

// ------------------------------------------------------------
// 육면체 데이터
// ------------------------------------------------------------
const float h = 0.4f;

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

glm::vec3 faceColors[6] = {
    {1.0f, 0.0f, 0.0f}, // 빨강
    {1.0f, 0.5f, 0.0f}, // 주황
    {0.0f, 1.0f, 0.0f}, // 초록
    {0.0f, 0.0f, 1.0f}, // 파랑
    {1.0f, 1.0f, 0.0f}, // 노랑
    {1.0f, 0.0f, 1.0f}  // 보라
};

float cubeVertices[6 * 4 * 6]{};
bool cubeFaceVisible[6]{};

// ------------------------------------------------------------
// 사각뿔 데이터
// ------------------------------------------------------------
const float pyramidSize = 0.4f;

glm::vec3 pyramidCorners[5] = {
    {-pyramidSize, -pyramidSize, -pyramidSize}, // 0: 밑면 앞 왼쪽
    { pyramidSize, -pyramidSize, -pyramidSize}, // 1: 밑면 앞 오른쪽
    { pyramidSize, -pyramidSize,  pyramidSize}, // 2: 밑면 뒤 오른쪽
    {-pyramidSize, -pyramidSize,  pyramidSize}, // 3: 밑면 뒤 왼쪽
    { 0.0f, pyramidSize, 0.0f}                  // 4: 위쪽 꼭짓점
};

int pyramidFaces[4][3] = {
    {0, 4, 1}, // 앞쪽 삼각형
    {1, 4, 2}, // 오른쪽 삼각형
    {2, 4, 3}, // 뒤쪽 삼각형
    {3, 4, 0}  // 왼쪽 삼각형
};

int pyramidBottom[4] = { 0, 1, 2, 3 };

glm::vec3 pyramidBottomColor = { 0.3f, 0.3f, 0.3f };

glm::vec3 pyramidFaceColors[4] = {
    {0.0f, 1.0f, 1.0f}, // 하늘색
    {1.0f, 0.5f, 0.0f}, // 주황색
    {1.0f, 0.0f, 0.5f}, // 분홍색
    {0.5f, 0.0f, 1.0f}  // 보라색
};

// 옆면 4개(정점 12개) + 바닥면(정점 4개)
float pyramidVertices[(4 * 3 + 4) * 6]{};
bool pyramidFaceVisible[5]{}; // 0~3: 삼각형, 4: 바닥면
bool pyramidMode{};            // false: 육면체, true: 사각뿔

// ------------------------------------------------------------
// 전역 변수
// ------------------------------------------------------------
int wide = 1600;
int height = 1200;

GLuint VAO{};
GLuint VBO{};

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> cubeFaceDistribution(0, 5);
std::uniform_int_distribution<int> pyramidFaceDistribution(0, 3);

// 카메라 회전 상태(방향키로 조절)
float cameraYaw = 45.0f;
float cameraPitch = 25.0f;
const float cameraDistance = 2.5f;

// ------------------------------------------------------------
// main
// ------------------------------------------------------------
int main()
{
    SetConsoleOutputCP(CP_UTF8);

    if (!glfwInit())
    {
        std::cout << "GLFW 초기화 실패\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    GLFWwindow* window = glfwCreateWindow(
        wide, height, "Homework 13", nullptr, nullptr
    );

    if (!window)
    {
        std::cout << "Window 생성 실패\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW 초기화 실패\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, wide, height);
    glEnable(GL_DEPTH_TEST);
    glfwSetKeyCallback(window, KeyCallback);

    // VAO / VBO 설정
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    MakeCubeVertices();
    MakePyramidVertices();

    // 좌표축, 육면체, 사각뿔 데이터를 한 버퍼에 저장
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices) + sizeof(cubeVertices) + sizeof(pyramidVertices),
        nullptr,
        GL_STATIC_DRAW
    );

    glBufferSubData(
        GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices
    );

    glBufferSubData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        sizeof(cubeVertices),
        cubeVertices
    );

    glBufferSubData(
        GL_ARRAY_BUFFER,
        sizeof(vertices) + sizeof(cubeVertices),
        sizeof(pyramidVertices),
        pyramidVertices
    );

    // 위치
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0
    );
    glEnableVertexAttribArray(0);

    // 색상
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    // Vertex Shader
    const char* vertexShaderSource = R"(
    #version 330 core

    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    out vec3 ourColor;

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        ourColor = aColor;
    }
    )";

    // Fragment Shader
    const char* fragmentShaderSource = R"(
    #version 330 core

    in vec3 ourColor;

    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(ourColor, 1.0);
    }
    )";

    // Vertex Shader 생성
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    if (!CheckShader(vertexShader))
    {
        glDeleteShader(vertexShader);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Fragment Shader 생성
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    if (!CheckShader(fragmentShader))
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Shader Program 생성
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    if (!CheckProgram(shaderProgram))
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(shaderProgram);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glUseProgram(shaderProgram);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    GLint modelLocation = glGetUniformLocation(shaderProgram, "model");
    GLint viewLocation = glGetUniformLocation(shaderProgram, "view");
    GLint projectionLocation = glGetUniformLocation(shaderProgram, "projection");

    float lastFrame = static_cast<float>(glfwGetTime());

    // ------------------------------------------------------------
    // 메인 루프
    // ------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        const float rotateSpeed = 60.0f;

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            cameraYaw -= rotateSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            cameraYaw += rotateSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            cameraPitch += rotateSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            cameraPitch -= rotateSpeed * deltaTime;
        }

        cameraPitch = std::clamp(cameraPitch, -80.0f, 80.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(VAO);

        // 카메라 위치 계산: GLM 회전 행렬 사용
        glm::mat4 cameraRotation(1.0f);

        cameraRotation = glm::rotate(
            cameraRotation,
            glm::radians(cameraYaw),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        cameraRotation = glm::rotate(
            cameraRotation,
            glm::radians(-cameraPitch),
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

        glm::vec3 cameraPos = glm::vec3(
            cameraRotation * glm::vec4(0.0f, 0.0f, cameraDistance, 1.0f)
        );

        glm::mat4 view = glm::lookAt(
            cameraPos,
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            static_cast<float>(wide) / static_cast<float>(height),
            0.1f,
            100.0f
        );

        glUniformMatrix4fv(
            viewLocation, 1, GL_FALSE, &view[0][0]
        );

        glUniformMatrix4fv(
            projectionLocation, 1, GL_FALSE, &projection[0][0]
        );

        // 핵심: 좌표축과 도형이 동일한 모델 행렬을 공유한다.
        glm::mat4 model = MakeModelMatrix(
            glm::radians(30.0f),
            glm::radians(30.0f)
        );

        glUniformMatrix4fv(
            modelLocation, 1, GL_FALSE, &model[0][0]
        );

        // 좌표축을 그리고 같은 행렬로 도형도 그린다.
        glDrawArrays(GL_LINES, 0, 6);

        if (pyramidMode)
        {
            DrawPyramid();
        }
        else
        {
            DrawCube();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 종료
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

// ------------------------------------------------------------
// 키 입력
// ------------------------------------------------------------
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
    {
        return;
    }

    // 종료
    if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }

    // 1~6: 육면체의 해당 면 표시/숨기기
    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6)
    {
        if (pyramidMode)
        {
            for (int i = 0; i < 5; ++i)
            {
                pyramidFaceVisible[i] = false;
            }
        }

        pyramidMode = false;
        cubeFaceVisible[key - GLFW_KEY_1] =
            !cubeFaceVisible[key - GLFW_KEY_1];
    }
    // C: 육면체에서 랜덤한 두 면을 새로 선택
    else if (key == GLFW_KEY_C)
    {
        std::uniform_int_distribution<int> faceDistribution(0, 5);

        if (pyramidMode)
        {
            for (int i = 0; i < 5; ++i)
            {
                pyramidFaceVisible[i] = false;
            }
        }

        pyramidMode = false;

        int cnt{};

        for (int i = 0; i < 6; ++i)
        {
            if (cubeFaceVisible[i])
            {
                cnt++;
            }
        }

        int target{};

        if (cnt < 5)
        {
            target = 2;
        }
        else if (cnt == 5)
        {
            target = 1;
        }

        int n{};

        while (n < target)
        {
            int Face = faceDistribution(gen);

            if (!cubeFaceVisible[Face])
            {
                cubeFaceVisible[Face] = true;
                n++;
            }
        }



    }
    // 7~9: 사각뿔의 첫 번째~세 번째 삼각형 면 표시/숨기기
    else if (key >= GLFW_KEY_7 && key <= GLFW_KEY_9)
    {
        if (!pyramidMode)
        {
            for (int i = 0; i < 6; ++i)
            {
                cubeFaceVisible[i] = false;
            }
        }

        pyramidMode = true;

        int face = key - GLFW_KEY_7;
        pyramidFaceVisible[face] = !pyramidFaceVisible[face];
    }
    // 0: 사각뿔의 네 번째 삼각형 면 표시/숨기기
    else if (key == GLFW_KEY_0)
    {
        if (!pyramidMode)
        {
            for (int i = 0; i < 6; ++i)
            {
                cubeFaceVisible[i] = false;
            }
        }

        pyramidMode = true;
        pyramidFaceVisible[3] = !pyramidFaceVisible[3];
    }
    // T: 바닥면과 랜덤한 삼각형 옆면 하나만 표시
    else if (key == GLFW_KEY_T) {

        if (!pyramidMode)
        {
            for (int i = 0; i < 6; ++i)
            {
                cubeFaceVisible[i] = false;
            }
        }

        pyramidMode = true;

        int cnt{};

        for (int i = 0; i < 4; ++i)
        {
            if (pyramidFaceVisible[i])
            {
                cnt++;
            }
        }

        int target{};

        if (cnt < 4)
        {
            target = 1;
        }

        int n{};

        while (n < target) {
            std::uniform_int_distribution<int> pDist(0, 3);

            int Face = pDist(gen);

            if (!pyramidFaceVisible[Face])
            {
                pyramidFaceVisible[Face] = true;
                n++;
            }
        }



        pyramidFaceVisible[4] = true;

    }

    if (pyramidMode)
    {
        std::cout << "현재 그려진 '사각뿔' 면: ";

        for (int i = 0; i < 5; ++i)
        {
            if (pyramidFaceVisible[i])
            {
                std::cout << i << ", ";
            }
        }
    }
    else
    {
        std::cout << "현재 그려진 '육면체' 면: ";

        for (int i = 0; i < 6; ++i)
        {
            if (cubeFaceVisible[i])
            {
                std::cout << i << ", ";
            }
        }
    }

    std::cout << '\n';
}

// ------------------------------------------------------------
// 육면체 정점 생성
// ------------------------------------------------------------
void MakeCubeVertices()
{
    int index{};

    for (int face = 0; face < 6; ++face)
    {
        for (int vertex = 0; vertex < 4; ++vertex)
        {
            glm::vec3 position = cubeCorners[cubeFaces[face][vertex]];

            cubeVertices[index++] = position.x;
            cubeVertices[index++] = position.y;
            cubeVertices[index++] = position.z;

            cubeVertices[index++] = faceColors[face].r;
            cubeVertices[index++] = faceColors[face].g;
            cubeVertices[index++] = faceColors[face].b;
        }
    }
}

void DrawCube()
{
    for (int i = 0; i < 6; ++i)
    {
        if (!cubeFaceVisible[i])
        {
            continue;
        }

        glDrawArrays(
            GL_TRIANGLE_STRIP,
            6 + i * 4,
            4
        );
    }
}

// ------------------------------------------------------------
// 셰이더 확인
// ------------------------------------------------------------
bool CheckShader(GLuint shader)
{
    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        char log[1024]{};

        glGetShaderInfoLog(
            shader,
            sizeof(log),
            nullptr,
            log
        );

        std::cout << "셰이더 컴파일 실패\n" << log << '\n';
        return false;
    }

    return true;
}

bool CheckProgram(GLuint program)
{
    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {
        char log[1024]{};

        glGetProgramInfoLog(
            program,
            sizeof(log),
            nullptr,
            log
        );

        std::cout << "Program 링크 실패\n" << log << '\n';
        return false;
    }

    return true;
}

// ------------------------------------------------------------
// 모델 행렬: X축 30도, Y축 30도 회전
// ------------------------------------------------------------
glm::mat4 MakeModelMatrix(float angleX, float angleY)
{
    glm::mat4 model(1.0f);

    model = glm::rotate(
        model,
        angleX,
        glm::vec3(1.0f, 0.0f, 0.0f)
    );

    model = glm::rotate(
        model,
        angleY,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    return model;
}

// ------------------------------------------------------------
// 사각뿔 정점 생성
// ------------------------------------------------------------
void MakePyramidVertices()
{
    int index{};

    // 삼각형 옆면 4개
    for (int face = 0; face < 4; ++face)
    {
        for (int vertex = 0; vertex < 3; ++vertex)
        {
            glm::vec3 position =
                pyramidCorners[pyramidFaces[face][vertex]];

            pyramidVertices[index++] = position.x;
            pyramidVertices[index++] = position.y;
            pyramidVertices[index++] = position.z;

            pyramidVertices[index++] = pyramidFaceColors[face].r;
            pyramidVertices[index++] = pyramidFaceColors[face].g;
            pyramidVertices[index++] = pyramidFaceColors[face].b;
        }
    }

    // 바닥면 정점 데이터
    for (int vertex = 0; vertex < 4; ++vertex)
    {
        glm::vec3 position =
            pyramidCorners[pyramidBottom[vertex]];

        pyramidVertices[index++] = position.x;
        pyramidVertices[index++] = position.y;
        pyramidVertices[index++] = position.z;

        pyramidVertices[index++] = pyramidBottomColor.r;
        pyramidVertices[index++] = pyramidBottomColor.g;
        pyramidVertices[index++] = pyramidBottomColor.b;
    }
}

void DrawPyramid()
{
    // 삼각형 옆면 4개
    for (int i = 0; i < 4; ++i)
    {
        if (!pyramidFaceVisible[i])
        {
            continue;
        }

        glDrawArrays(
            GL_TRIANGLES,
            30 + i * 3,
            3
        );
    }

    // 바닥면
    if (pyramidFaceVisible[4])
    {
        glDrawArrays(
            GL_TRIANGLE_FAN,
            42,
            4
        );
    }
}
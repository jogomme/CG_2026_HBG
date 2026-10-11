#include <windows.h>

#include <GL/glew.h>
#include <GL/glfw3.h>
#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <random>

//------------------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------------------

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

// 셰이더 컴파일 / 링크 결과 확인
bool CheckShader(GLuint shader);

bool CheckProgram(GLuint program);

glm::mat4 MakeModelMatrix(float angleX, float angleY);

void MakeCubeVertices();
void DrawCube();

void MakePyramidVertices();
void DrawPyramid();

//------------------------------------------------------------------------------------------------------
// 정점의 위치 데이터
//------------------------------------------------------------------------------------------------------

// 위치 3개 + 색상 3개 = 한 정점 6 float
float vertices[] = {
    // ------------------------------------------------------------
    // X축 - 빨간색
    // ------------------------------------------------------------
    -0.8f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,
     0.8f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,

     // ------------------------------------------------------------
     // Y축 - 초록색
     // ------------------------------------------------------------
      0.0f, -0.8f, 0.0f,    0.0f, 1.0f, 0.0f,
      0.0f,  0.8f, 0.0f,    0.0f, 1.0f, 0.0f,

      // ------------------------------------------------------------
      // Z축 - 파란색
      // ------------------------------------------------------------
       0.0f, 0.0f, -0.8f,    0.0f, 0.0f, 1.0f,
       0.0f, 0.0f,  0.8f,    0.0f, 0.0f, 1.0f
};

//------------------------------------------------------------------------------------------------------
// 육면체 데이터
//------------------------------------------------------------------------------------------------------

const float h = 0.4f;

// 육면체의 실제 꼭짓점 8개
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

// 각 면에서 사용할 꼭짓점 번호
int cubeFaces[6][4] = {
    {0, 1, 2, 3}, // 앞면
    {4, 5, 6, 7}, // 뒷면
    {1, 5, 3, 7}, // 오른쪽 면
    {4, 0, 6, 2}, // 왼쪽 면
    {2, 3, 6, 7}, // 윗면
    {4, 5, 0, 1}  // 아랫면
};

// 면마다 사용할 색상
glm::vec3 faceColors[6] = {
    {1.0f, 0.0f, 0.0f}, // 빨강
    {1.0f, 0.5f, 0.0f}, // 주황
    {0.0f, 1.0f, 0.0f}, // 초록
    {0.0f, 0.0f, 1.0f}, // 파랑
    {1.0f, 1.0f, 0.0f}, // 노랑
    {1.0f, 0.0f, 1.0f}  // 보라
};

// 면 6개 * 정점 4개 * 정점당 float 6개
float cubeVertices[6 * 4 * 6]{};

// 각 면을 화면에 표시할지 저장
bool cubeFaceVisible[6]{};

// 랜덤 면 선택에 사용할 난수 생성기
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> faceDistribution(0, 5);

//------------------------------------------------------------------------------------------------------
// 사각뿔 데이터
//------------------------------------------------------------------------------------------------------

const float pyramidSize = 0.4f;

// 사각뿔의 꼭짓점 5개
glm::vec3 pyramidCorners[5] = {
    {-pyramidSize, -pyramidSize, -pyramidSize}, // 0: 밑면 앞 왼쪽
    { pyramidSize, -pyramidSize, -pyramidSize}, // 1: 밑면 앞 오른쪽
    { pyramidSize,  pyramidSize, -pyramidSize}, // 2: 밑면 뒤 오른쪽
    {-pyramidSize,  pyramidSize, -pyramidSize}, // 3: 밑면 뒤 왼쪽
    { 0.0f, 0.0f, pyramidSize}                  // 4: 꼭짓점
};

// 삼각형 옆면 4개에서 사용할 꼭짓점 번호
int pyramidFaces[4][3] = {
    {0, 1, 4}, // 앞쪽 삼각형
    {1, 2, 4}, // 오른쪽 삼각형
    {2, 3, 4}, // 뒤쪽 삼각형
    {3, 0, 4}  // 왼쪽 삼각형
};

// 면마다 사용할 색상
glm::vec3 pyramidFaceColors[4] = {
    {0.0f, 1.0f, 1.0f}, // 하늘색
    {1.0f, 0.5f, 0.0f}, // 주황색
    {1.0f, 0.0f, 0.5f}, // 분홍색
    {0.5f, 0.0f, 1.0f}  // 보라색
};

// 면 4개 * 정점 3개 * 정점당 float 6개
float pyramidVertices[4 * 3 * 6]{};

// 각 삼각형을 화면에 표시할지 저장
bool pyramidFaceVisible[4]{};

// 현재 표시할 도형: false는 육면체, true는 사각뿔
bool pyramidMode{};

//------------------------------------------------------------------------------------------------------
// 전역 변수
//------------------------------------------------------------------------------------------------------

int wide = 1600;
int height = 1200;

GLuint VAO;
GLuint VBO;

//------------------------------------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------------------------------------
{
    //------------------------------------------------------------------------------------------
    // 콘솔 출력 코드페이지를 UTF-8 로 설정
    //------------------------------------------------------------------------------------------
    SetConsoleOutputCP(CP_UTF8);

    //------------------------------------------------------------------------------------------
    // GLFW 초기화
    //------------------------------------------------------------------------------------------
    if (!glfwInit())
    {
        std::cout << "GLFW 초기화 실패\n";
        return -1;
    }

    //------------------------------------------------------------------------------------------
    // OpenGL 3.3 설정
    //------------------------------------------------------------------------------------------
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    //------------------------------------------------------------------------------------------
    // 창 생성
    //------------------------------------------------------------------------------------------
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    GLFWwindow* window = glfwCreateWindow(
        wide, height,
        "__WINDOWTITLE__",
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cout << "Window 생성 실패\n";
        glfwTerminate();
        return -1;
    }

    //------------------------------------------------------------------------------------------
    // OpenGL Context 생성
    //------------------------------------------------------------------------------------------
    glfwMakeContextCurrent(window);

    //------------------------------------------------------------------------------------------
    // GLEW 초기화
    //------------------------------------------------------------------------------------------
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW 초기화 실패\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    //------------------------------------------------------------------------------------------
    // 화면 크기 설정
    //------------------------------------------------------------------------------------------
    glViewport(0, 0, wide, height);

    // 은면 제거를 위한 깊이 검사
    glEnable(GL_DEPTH_TEST);

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
    MakePyramidVertices();

    // 좌표축 데이터와 육면체 데이터를 한 버퍼에 저장
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices) + sizeof(cubeVertices) + sizeof(pyramidVertices),
        nullptr,
        GL_STATIC_DRAW
    );

    // 좌표축 데이터 저장
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(vertices),
        vertices
    );

    // 육면체 데이터 저장
    glBufferSubData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        sizeof(cubeVertices),
        cubeVertices
    );

    // 사각뿔 데이터 저장
    glBufferSubData(
        GL_ARRAY_BUFFER,
        sizeof(vertices) + sizeof(cubeVertices),
        sizeof(pyramidVertices),
        pyramidVertices
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
    // Vertex Shader 생성
    //------------------------------------------------------------------------------------------
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        vertexShader,
        1,
        &vertexShaderSource,
        nullptr
    );

    glCompileShader(vertexShader);

    if (!CheckShader(vertexShader))
    {
        glDeleteShader(vertexShader);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    //------------------------------------------------------------------------------------------
    // Fragment Shader 생성
    //------------------------------------------------------------------------------------------
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        fragmentShader,
        1,
        &fragmentShaderSource,
        nullptr
    );

    glCompileShader(fragmentShader);

    if (!CheckShader(fragmentShader))
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    //------------------------------------------------------------------------------------------
    // Shader Program 생성
    //------------------------------------------------------------------------------------------
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

    //------------------------------------------------------------------------------------------
    // Program 사용
    //------------------------------------------------------------------------------------------
    glUseProgram(shaderProgram);

    //------------------------------------------------------------------------------------------
    // 화면 배경색 설정
    //------------------------------------------------------------------------------------------
    glClearColor(
        1.0f,
        1.0f,
        1.0f,
        1.0f
    );

    GLint modelLocation = glGetUniformLocation(shaderProgram, "model");

    //------------------------------------------------------------------------------------------
    // 메인 루프
    //------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // ------------------------------------------------------------
        // 화면 지우기
        // ------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);

        // ------------------------------------------------------------
        // 좌표축 그리기: 회전하지 않음
        // ------------------------------------------------------------
        glm::mat4 model(1.0f);

        glUniformMatrix4fv(
            modelLocation,
            1,
            GL_FALSE,
            &model[0][0]
        );

        glDrawArrays(
            GL_LINES,
            0,
            6
        );

        // ------------------------------------------------------------
        // 육면체 그리기: X축 30도, Y축 30도 회전
        // ------------------------------------------------------------
        model = MakeModelMatrix(
            glm::radians(30.0f),
            glm::radians(30.0f)
        );

        glUniformMatrix4fv(
            modelLocation,
            1,
            GL_FALSE,
            &model[0][0]
        );

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

    //------------------------------------------------------------------------------------------
    // 종료
    //------------------------------------------------------------------------------------------
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glfwDestroyWindow(window);
    glfwTerminate();
}

//------------------------------------------------------------------------------------------------------

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

    // 1~6: 육면체의 해당 면 하나를 표시
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

        // 입력한 번호에 해당하는 면 표시
        cubeFaceVisible[key - GLFW_KEY_1] = !cubeFaceVisible[key - GLFW_KEY_1];
    }
    // c: 육면체에서 랜덤한 두 면 표시
    else if (key == GLFW_KEY_C)
    {
        int cnt{};

        for (int i = 0; i < 6; ++i) {
            if (cubeFaceVisible[i]) {
                cnt++;
            }
        }

        int target{};

        if (cnt < 5) {
            target = 2;
        }
        else if (cnt == 5) {
            target = 1;
        }

        int n{};

        while (n < target) {
            int Face = faceDistribution(gen);

            if (!cubeFaceVisible[Face]) {
                cubeFaceVisible[Face] = true;
                n++;
            }
        }
    }
    // 7~9: 사각뿔의 삼각형 면 표시·숨기기
    else if (key >= GLFW_KEY_7 && key <= GLFW_KEY_9)
    {
        // 육면체에서 사각뿔로 전환할 때 육면체 면 숨기기
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
    // 0: 사각뿔의 네 번째 삼각형 면 표시·숨기기
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


    std::cout << "현재 그려진 면 : ";
    if (pyramidMode)
    {
        for (int i = 0; i < 5; ++i) {
            if (pyramidFaceVisible[i]) {
                std::cout << i << ", ";
            }
        }
    }
    else
    {
        for (int i = 0; i < 6; ++i) {
            if (cubeFaceVisible[i]) {
                std::cout << i << ", ";
            }
        }
    }
    std::cout << '\n';
}

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
        // 선택되지 않은 면은 그리지 않음
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

bool CheckShader(GLuint shader)
{
    int success = 0;

    glGetShaderiv(
        shader,
        GL_COMPILE_STATUS,
        &success
    );

    if (success == GL_FALSE)
    {
        char log[1024];

        glGetShaderInfoLog(
            shader,
            sizeof(log),
            nullptr,
            log
        );

        std::cout << "셰이더 컴파일 실패\n";
        std::cout << log << "\n";

        return false;
    }

    return true;
}

bool CheckProgram(GLuint program)
{
    int success = 0;

    glGetProgramiv(
        program,
        GL_LINK_STATUS,
        &success
    );

    if (success == GL_FALSE)
    {
        char log[1024];

        glGetProgramInfoLog(
            program,
            sizeof(log),
            nullptr,
            log
        );

        std::cout << "Program 링크 실패\n";
        std::cout << log << "\n";

        return false;
    }

    return true;
}

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

void MakePyramidVertices()
{
    int index{};

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
}

void DrawPyramid()
{
    for (int i = 0; i < 4; ++i)
    {
        // 선택되지 않은 면은 그리지 않음
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
}
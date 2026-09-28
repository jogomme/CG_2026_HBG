#include <GL/glew.h>
#include <GL/glfw3.h>

#include <iostream>
#include <random>
#include <cmath>


//------------------------------------------------------------------------------------------
// 랜덤 엔진
//------------------------------------------------------------------------------------------
std::random_device rd;
std::mt19937 gen(rd());

std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);


//------------------------------------------------------------------------------------------
// 구조체 선언
//------------------------------------------------------------------------------------------
struct POINT
{
    float x;
    float y;
    float z{ 0 };
};

struct COLOR
{
    float r;
    float g;
    float b;
};

struct TRIANGLE
{
    // 삼각형의 세 정점
    POINT point[3];

    // 삼각형 색상
    COLOR color;

    // 삼각형 크기
    float size;

    // 선택 여부
    bool selected{ false };
};


//------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------

void AddTriangle(
    TRIANGLE& triangle,
    float x,
    float y,
    float size
);

void MakeVertexData(int index);

void MouseButtonCallback(
    GLFWwindow* window,
    int button,
    int action,
    int mods
);

void KeyCallback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods
);

POINT SetPosToGL(float x, float y);



//------------------------------------------------------------------------------------------
// 전역 변수
//------------------------------------------------------------------------------------------

int wide = 1600;
int height = 1200;


// 삼각형 4개
TRIANGLE triangles[4]{};

int triangleCount = 4;


// OpenGL
GLuint VAO;
GLuint VBO;


//------------------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------------------
int main()
{
    // GLFW 초기화
    if (!glfwInit())
    {
        std::cout << "GLFW 초기화 실패\n";
        return -1;
    }


    // OpenGL 3.3 설정
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );


    // 창 생성
    GLFWwindow* window =
        glfwCreateWindow(
            wide,
            height,
            "OpenGL Practice 8",
            nullptr,
            nullptr
        );


    if (!window)
    {
        std::cout << "Window 생성 실패\n";

        glfwTerminate();

        return -1;
    }


    // OpenGL Context
    glfwMakeContextCurrent(window);


    // GLEW 초기화
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW 초기화 실패\n";

        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }


    // 뷰포트
    glViewport(
        0,
        0,
        wide,
        height
    );


    //--------------------------------------------------------------------------------------
    // Callback
    //--------------------------------------------------------------------------------------

    glfwSetMouseButtonCallback(
        window,
        MouseButtonCallback
    );

    glfwSetKeyCallback(
        window,
        KeyCallback
    );


    //--------------------------------------------------------------------------------------
    // VAO / VBO
    //--------------------------------------------------------------------------------------

    glGenVertexArrays(
        1,
        &VAO
    );

    glBindVertexArray(VAO);


    glGenBuffers(
        1,
        &VBO
    );

    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO
    );


    // 처음에는 임시 데이터
    float vertexs[] =
    {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };


    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertexs),
        vertexs,
        GL_DYNAMIC_DRAW
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


    //--------------------------------------------------------------------------------------
    // Vertex Shader
    //--------------------------------------------------------------------------------------

    const char* vertexShaderSource = R"(
    #version 330 core

    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;

    out vec3 ourColor;

    void main()
    {
        gl_Position = vec4(aPos, 1.0);
        ourColor = aColor;
    }
    )";


    //--------------------------------------------------------------------------------------
    // Fragment Shader
    //--------------------------------------------------------------------------------------

    const char* fragmentShaderSource = R"(
    #version 330 core

    in vec3 ourColor;

    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(ourColor, 1.0);
    }
    )";


    //--------------------------------------------------------------------------------------
    // Vertex Shader 생성
    //--------------------------------------------------------------------------------------

    GLuint vertexShader =
        glCreateShader(GL_VERTEX_SHADER);


    glShaderSource(
        vertexShader,
        1,
        &vertexShaderSource,
        nullptr
    );


    glCompileShader(vertexShader);


    //--------------------------------------------------------------------------------------
    // Fragment Shader 생성
    //--------------------------------------------------------------------------------------

    GLuint fragmentShader =
        glCreateShader(GL_FRAGMENT_SHADER);


    glShaderSource(
        fragmentShader,
        1,
        &fragmentShaderSource,
        nullptr
    );


    glCompileShader(fragmentShader);


    //--------------------------------------------------------------------------------------
    // Shader Program
    //--------------------------------------------------------------------------------------

    GLuint shaderProgram =
        glCreateProgram();


    glAttachShader(
        shaderProgram,
        vertexShader
    );

    glAttachShader(
        shaderProgram,
        fragmentShader
    );


    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);


    //--------------------------------------------------------------------------------------
    // 배경
    //--------------------------------------------------------------------------------------

    glClearColor(
        1.0f,
        1.0f,
        1.0f,
        1.0f
    );


    //--------------------------------------------------------------------------------------
    // 삼각형 생성
    //--------------------------------------------------------------------------------------

    // 1사분면
    AddTriangle(
        triangles[0],
        -0.5f,
        0.5f,
        0.18f
    );


    // 2사분면
    AddTriangle(
        triangles[1],
        0.5f,
        0.5f,
        0.18f
    );


    // 3사분면
    AddTriangle(
        triangles[2],
        -0.5f,
        -0.5f,
        0.18f
    );


    // 4사분면
    AddTriangle(
        triangles[3],
        0.5f,
        -0.5f,
        0.18f
    );


    //--------------------------------------------------------------------------------------
    // 메인 루프
    //--------------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        // 화면 초기화
        glClear(GL_COLOR_BUFFER_BIT);


        glBindVertexArray(VAO);


        // 삼각형 4개 그리기
        for (int i = 0; i < triangleCount; ++i)
        {
            MakeVertexData(i);

            glDrawArrays(
                GL_TRIANGLES,
                0,
                3
            );
        }


        glfwSwapBuffers(window);

        glfwPollEvents();
    }


    // 종료
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


//------------------------------------------------------------------------------------------
// 삼각형 생성
//------------------------------------------------------------------------------------------

void AddTriangle(
    TRIANGLE& triangle,
    float x,
    float y,
    float size
)
{
    triangle.size = size;


    // 랜덤 색상
    triangle.color.r =
        colorDist(gen);

    triangle.color.g =
        colorDist(gen);

    triangle.color.b =
        colorDist(gen);


    //--------------------------------------------------------------------------------------
    // 정삼각형 생성
    //--------------------------------------------------------------------------------------

    float angle =
        3.14159265f / 2.0f;


    // 첫 번째 점
    triangle.point[0].x =
        x + cos(angle) * size;

    triangle.point[0].y =
        y + sin(angle) * size;


    // 두 번째 점
    angle +=
        2.0f * 3.14159265f / 3.0f;

    triangle.point[1].x =
        x + cos(angle) * size;

    triangle.point[1].y =
        y + sin(angle) * size;


    // 세 번째 점
    angle +=
        2.0f * 3.14159265f / 3.0f;

    triangle.point[2].x =
        x + cos(angle) * size;

    triangle.point[2].y =
        y + sin(angle) * size;
}


//------------------------------------------------------------------------------------------
// VBO 데이터 생성
//------------------------------------------------------------------------------------------

void MakeVertexData(int index)
{
    float vertexData[18]{};


    for (int i = 0; i < 3; ++i)
    {
        // 위치
        vertexData[i * 6] =
            triangles[index].point[i].x;

        vertexData[i * 6 + 1] =
            triangles[index].point[i].y;

        vertexData[i * 6 + 2] =
            triangles[index].point[i].z;


        // 색상
        vertexData[i * 6 + 3] =
            triangles[index].color.r;

        vertexData[i * 6 + 4] =
            triangles[index].color.g;

        vertexData[i * 6 + 5] =
            triangles[index].color.b;
    }


    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO
    );


    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertexData),
        vertexData,
        GL_DYNAMIC_DRAW
    );
}


//------------------------------------------------------------------------------------------
// Mouse Callback
//------------------------------------------------------------------------------------------

void MouseButtonCallback(
    GLFWwindow* window,
    int button,
    int action,
    int mods
)
{
    // 나중에 삼각형 선택 구현
}


//------------------------------------------------------------------------------------------
// Key Callback
//------------------------------------------------------------------------------------------

void KeyCallback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods
)
{
    // 나중에 키 입력 구현
}

POINT SetPosToGL(float x, float y)
{
    POINT p{};

    // x
    p.x = (x / wide) * 2 - 1;

    // y
    p.y = 1 - (y / height) * 2;

    return p;
}
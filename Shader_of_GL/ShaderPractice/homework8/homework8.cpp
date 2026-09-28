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
std::uniform_real_distribution<float> sizeDist(0.05f, 0.25);
std::uniform_real_distribution<float> posDist(-1.0f, 1.0f);


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

    bool isUp{ false };
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

POINT GetMousePosition(GLFWwindow* window);

void DrawQuadrant();

int Quadrant(POINT mouse);

void ChangeSize(POINT mouse);


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

// 십자선 전용
GLuint quadrantVAO;
GLuint quadrantVBO;

bool isFill{ true };

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
    // 삼각형 VAO / VBO
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
    // 십자선 VAO / VBO
    //--------------------------------------------------------------------------------------

    glGenVertexArrays(
        1,
        &quadrantVAO
    );

    glBindVertexArray(quadrantVAO);


    glGenBuffers(
        1,
        &quadrantVBO
    );

    glBindBuffer(
        GL_ARRAY_BUFFER,
        quadrantVBO
    );


    // 십자선 데이터
    // x, y, z, r, g, b
    float quadrantVertexData[] =
    {
        // 가로선
        -1.0f,  0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         1.0f,  0.0f, 0.0f,   0.0f, 0.0f, 0.0f,

         // 세로선
          0.0f, -1.0f, 0.0f,   0.0f, 0.0f, 0.0f,
          0.0f,  1.0f, 0.0f,   0.0f, 0.0f, 0.0f
    };


    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(quadrantVertexData),
        quadrantVertexData,
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

    glEnableVertexAttribArray(1);


    // 다시 삼각형 VAO 선택
    glBindVertexArray(VAO);


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
        0.5f,
        0.5f,
        0.18f
    );


    // 2사분면
    AddTriangle(
        triangles[1],
        -0.5f,
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


        // 십자선 그리기
        DrawQuadrant();


        // 삼각형 VAO 선택
        glBindVertexArray(VAO);

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            POINT mouse = GetMousePosition(window);
            ChangeSize(mouse);
        }


        // 삼각형 4개 그리기
        for (int i = 0; i < triangleCount; ++i)
        {
            MakeVertexData(i);

            if (isFill)
            {
                glDrawArrays(
                    GL_TRIANGLES,
                    0,
                    3
                );
            }
            else
            {
                glDrawArrays(
                    GL_LINE_LOOP,
                    0,
                    3
                );
            }
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
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        POINT mouse = GetMousePosition(window);

        int qua = Quadrant(mouse);

        if (qua == -1) {
            return;
        }

        float size = sizeDist(gen);

        if (qua == 0) {
            AddTriangle(triangles[0], mouse.x, mouse.y, size);
        }
        else if (qua == 1) {
            AddTriangle(triangles[1], mouse.x, mouse.y, size);
        }
        else if (qua == 2) {
            AddTriangle(triangles[2], mouse.x, mouse.y, size);
        }
        else if (qua == 3) {
            AddTriangle(triangles[3], mouse.x, mouse.y, size);
        }

    }
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
    if (action != GLFW_PRESS) {
        return;
    }

    if (key == GLFW_KEY_Q)
    {
        glfwSetWindowShouldClose(
            window,
            GLFW_TRUE
        );
    }
    else if (key == GLFW_KEY_C) {
        for (int i = 0; i < 4; ++i) {

            float x = posDist(gen);
            float y = posDist(gen);

            POINT p{ x,y };

            while (Quadrant(p) != i) {
                x = posDist(gen);
                y = posDist(gen);

                p.x = x ;
                p.y = y;
            }

            AddTriangle(triangles[i], x, y, sizeDist(gen));
        }
    }
    else if (key == GLFW_KEY_A) {
        isFill = true;
    }
    else if (key == GLFW_KEY_B) {
        isFill = false;
    }
}




//------------------------------------------------------------------------------------------
// 마우스 좌표 -> OpenGL 좌표
//------------------------------------------------------------------------------------------

POINT SetPosToGL(float x, float y)
{
    POINT p{};

    // x
    p.x = (x / wide) * 2 - 1;

    // y
    p.y = 1 - (y / height) * 2;

    return p;
}


//------------------------------------------------------------------------------------------
// 마우스 위치 가져오기
//------------------------------------------------------------------------------------------

POINT GetMousePosition(GLFWwindow* window)
{
    POINT position{};

    double mouseX{};
    double mouseY{};

    glfwGetCursorPos(
        window,
        &mouseX,
        &mouseY
    );

    position = SetPosToGL(
        mouseX,
        mouseY
    );

    return position;
}


//------------------------------------------------------------------------------------------
// 사분면 십자선 그리기
//------------------------------------------------------------------------------------------

void DrawQuadrant()
{
    glBindVertexArray(quadrantVAO);

    glLineWidth(2.0f);

    glDrawArrays(
        GL_LINES,
        0,
        4
    );
}

// 몇 사분면인지 return
int Quadrant(POINT mouse)
{
    float x = mouse.x;
    float y = mouse.y;

    if (x > 0 && y > 0) {
        return 0;
    }
    else if (x < 0 && y > 0) {
        return 1;
    }
    else if (x < 0 && y < 0) {
        return 2;
    }
    else if (x > 0 && y < 0) {
        return 3;
    }

    return -1;
}

void ChangeSize(POINT mouse)
{
    int qua = Quadrant(mouse);

    if (qua == -1) {
        return;
    }

    float speed = 0.001f;
    
    TRIANGLE& triangle = triangles[qua];

    float oldSize = triangle.size;

    float size = triangle.size;

    if (triangle.isUp) {
        if (triangle.size >= 0.25f) {
            triangle.isUp = !triangle.isUp;
        }
        else {
            triangle.size += speed;
        }
    }
    else {
        if (triangle.size <= 0.05f) {
            triangle.isUp = !triangle.isUp;
        }
        else {
            triangle.size -= speed;
        }
    }
    



    float scale = triangle.size / oldSize;

    // 삼각형 중심
    float centerX =
        (triangle.point[0].x +
            triangle.point[1].x +
            triangle.point[2].x) / 3.0f;

    float centerY =
        (triangle.point[0].y +
            triangle.point[1].y +
            triangle.point[2].y) / 3.0f;

    // 중심을 기준으로 크기 변경
    for (int i = 0; i < 3; ++i)
    {
        triangle.point[i].x =
            centerX +
            (triangle.point[i].x - centerX) * scale;

        triangle.point[i].y =
            centerY +
            (triangle.point[i].y - centerY) * scale;
    }

}
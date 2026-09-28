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
std::uniform_real_distribution<float> sizeDist(0.05f, 0.25f);
std::uniform_real_distribution<float> posDist(-1.0f, 1.0f);
std::uniform_real_distribution<float> speedDist(0.0005f, 0.0015f);
std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159265f);


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

    // 크기 증가 / 감소
    bool isUp{ false };

    // 이동 속도
    float speed;

    // 이동 모드
    int mode;

    // 스파이럴 각도
    float angle;
};


//------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------

void RotateTriangle(TRIANGLE& triangle);

void AddTriangle(TRIANGLE& triangle, float x, float y, float size);
void MakeVertexData(int index);

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

POINT SetPosToGL(float x, float y);
POINT GetMousePosition(GLFWwindow* window);

void CreateTriangle(POINT mouse);
int FindNearTriangle(POINT mouse);

void ChangeSize(POINT mouse);

void MoveTriangle(TRIANGLE& triangle);
void MoveBounce(TRIANGLE& triangle);
void MoveHorizontalZigzag(TRIANGLE& triangle);
void MoveVerticalZigzag(TRIANGLE& triangle);
void MoveSpiral(TRIANGLE& triangle);


//------------------------------------------------------------------------------------------
// 전역 변수
//------------------------------------------------------------------------------------------

int wide = 1200;
int height = 1200;


// 삼각형 2개
TRIANGLE triangles[2]{};

int triangleCount = 2;


// 현재 이동 모드
int moveMode = 1;


// 면 / 선
bool isFill{ true };


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
            "OpenGL Practice 9",
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

    AddTriangle(
        triangles[0],
        0.5f,
        0.5f,
        0.18f
    );


    AddTriangle(
        triangles[1],
        -0.5f,
        0.5f,
        0.18f
    );


    //--------------------------------------------------------------------------------------
    // 메인 루프
    //--------------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        // 화면 초기화
        glClear(GL_COLOR_BUFFER_BIT);


        // 삼각형 VAO 선택
        glBindVertexArray(VAO);


        //----------------------------------------------------------------------------------
        // 오른쪽 마우스
        //----------------------------------------------------------------------------------

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            POINT mouse = GetMousePosition(window);
            ChangeSize(mouse);
        }


        //----------------------------------------------------------------------------------
        // 삼각형 이동
        //----------------------------------------------------------------------------------

        for (int i = 0; i < triangleCount; ++i)
        {
            MoveTriangle(triangles[i]);
        }


        //----------------------------------------------------------------------------------
        // 삼각형 2개 그리기
        //----------------------------------------------------------------------------------

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
    triangle.color.r = colorDist(gen);
    triangle.color.g = colorDist(gen);
    triangle.color.b = colorDist(gen);


    // 랜덤 속도
    triangle.speed = speedDist(gen);


    // 현재 이동 모드
    triangle.mode = moveMode;


    // 스파이럴 각도
    triangle.angle = angleDist(gen);


    //--------------------------------------------------------------------------------------
    // 이등변 삼각형 생성
    //--------------------------------------------------------------------------------------

    // 위쪽 꼭짓점
    triangle.point[0].x = x;
    triangle.point[0].y = y + size;


    // 왼쪽 아래
    triangle.point[1].x = x - size;
    triangle.point[1].y = y - size;


    // 오른쪽 아래
    triangle.point[2].x = x + size;
    triangle.point[2].y = y - size;

    RotateTriangle(triangle);
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
        vertexData[i * 6] = triangles[index].point[i].x;
        vertexData[i * 6 + 1] = triangles[index].point[i].y;
        vertexData[i * 6 + 2] = triangles[index].point[i].z;


        // 색상
        vertexData[i * 6 + 3] = triangles[index].color.r;
        vertexData[i * 6 + 4] = triangles[index].color.g;
        vertexData[i * 6 + 5] = triangles[index].color.b;
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
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        POINT mouse = GetMousePosition(window);

        CreateTriangle(mouse);
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
    if (action != GLFW_PRESS)
    {
        return;
    }


    //--------------------------------------------------------------------------------------
    // Q
    //--------------------------------------------------------------------------------------

    if (key == GLFW_KEY_Q)
    {
        glfwSetWindowShouldClose(
            window,
            GLFW_TRUE
        );
    }


    //--------------------------------------------------------------------------------------
    // A
    //--------------------------------------------------------------------------------------

    else if (key == GLFW_KEY_A)
    {
        isFill = true;
    }


    //--------------------------------------------------------------------------------------
    // B
    //--------------------------------------------------------------------------------------

    else if (key == GLFW_KEY_B)
    {
        isFill = false;
    }


    //--------------------------------------------------------------------------------------
    // C
    //--------------------------------------------------------------------------------------

    else if (key == GLFW_KEY_C)
    {
        AddTriangle(
            triangles[0],
            posDist(gen),
            posDist(gen),
            sizeDist(gen)
        );

        AddTriangle(
            triangles[1],
            posDist(gen),
            posDist(gen),
            sizeDist(gen)
        );
    }


    //--------------------------------------------------------------------------------------
    // 1
    //--------------------------------------------------------------------------------------

    else if (key == GLFW_KEY_1)
    {
        moveMode = 1;

        for (int i = 0; i < triangleCount; ++i)
        {
            triangles[i].mode = moveMode;
        }
    }


    //--------------------------------------------------------------------------------------
    // 2
    //--------------------------------------------------------------------------------------

    else if (key == GLFW_KEY_2)
    {
        moveMode = 2;

        for (int i = 0; i < triangleCount; ++i)
        {
            triangles[i].mode = moveMode;
        }
    }


    //--------------------------------------------------------------------------------------
    // 3
    //--------------------------------------------------------------------------------------

    else if (key == GLFW_KEY_3)
    {
        moveMode = 3;

        for (int i = 0; i < triangleCount; ++i)
        {
            triangles[i].mode = moveMode;
        }
    }


    //--------------------------------------------------------------------------------------
    // 4
    //--------------------------------------------------------------------------------------

    else if (key == GLFW_KEY_4)
    {
        moveMode = 4;

        for (int i = 0; i < triangleCount; ++i)
        {
            triangles[i].mode = moveMode;
        }
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
// 가까운 삼각형 찾기
//------------------------------------------------------------------------------------------

int FindNearTriangle(POINT mouse)
{
    // 삼각형 0의 중심
    float centerX0 =
        (triangles[0].point[0].x +
            triangles[0].point[1].x +
            triangles[0].point[2].x) / 3.0f;

    float centerY0 =
        (triangles[0].point[0].y +
            triangles[0].point[1].y +
            triangles[0].point[2].y) / 3.0f;


    // 삼각형 1의 중심
    float centerX1 =
        (triangles[1].point[0].x +
            triangles[1].point[1].x +
            triangles[1].point[2].x) / 3.0f;

    float centerY1 =
        (triangles[1].point[0].y +
            triangles[1].point[1].y +
            triangles[1].point[2].y) / 3.0f;


    // 삼각형 0까지 거리
    float dx0 = mouse.x - centerX0;
    float dy0 = mouse.y - centerY0;

    float distance0 =
        sqrt(
            dx0 * dx0 +
            dy0 * dy0
        );


    // 삼각형 1까지 거리
    float dx1 = mouse.x - centerX1;
    float dy1 = mouse.y - centerY1;

    float distance1 =
        sqrt(
            dx1 * dx1 +
            dy1 * dy1
        );


    if (distance0 < distance1)
    {
        return 0;
    }

    return 1;
}


//------------------------------------------------------------------------------------------
// 좌클릭으로 새로운 삼각형 생성
//------------------------------------------------------------------------------------------

void CreateTriangle(POINT mouse)
{
    int nearIndex = FindNearTriangle(mouse);

    AddTriangle(
        triangles[nearIndex],
        mouse.x,
        mouse.y,
        sizeDist(gen)
    );
}


//------------------------------------------------------------------------------------------
// 삼각형 크기 변경
//------------------------------------------------------------------------------------------

void ChangeSize(POINT mouse)
{
    int nearIndex = FindNearTriangle(mouse);

    TRIANGLE& triangle = triangles[nearIndex];


    float speed = 0.001f;

    float oldSize = triangle.size;


    if (triangle.isUp)
    {
        if (triangle.size >= 0.25f)
        {
            triangle.isUp = false;
        }
        else
        {
            triangle.size += speed;
        }
    }
    else
    {
        if (triangle.size <= 0.05f)
        {
            triangle.isUp = true;
        }
        else
        {
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


//------------------------------------------------------------------------------------------
// 삼각형 이동
//------------------------------------------------------------------------------------------

void MoveTriangle(TRIANGLE& triangle)
{
    if (triangle.mode == 1)
    {
        MoveBounce(triangle);
    }
    else if (triangle.mode == 2)
    {
        MoveHorizontalZigzag(triangle);
    }
    else if (triangle.mode == 3)
    {
        MoveVerticalZigzag(triangle);
    }
    else if (triangle.mode == 4)
    {
        MoveSpiral(triangle);
    }
}


//------------------------------------------------------------------------------------------
// 1. 튕기기 이동
//------------------------------------------------------------------------------------------

void MoveBounce(TRIANGLE& triangle)
{
    // 나중에 구현
}


//------------------------------------------------------------------------------------------
// 2. 좌우 지그재그 이동
//------------------------------------------------------------------------------------------

void MoveHorizontalZigzag(TRIANGLE& triangle)
{
    // 나중에 구현
}


//------------------------------------------------------------------------------------------
// 3. 상하 뾰족 지그재그 이동
//------------------------------------------------------------------------------------------

void MoveVerticalZigzag(TRIANGLE& triangle)
{
    // 나중에 구현
}


//------------------------------------------------------------------------------------------
// 4. 원 스파이럴 이동
//------------------------------------------------------------------------------------------

void MoveSpiral(TRIANGLE& triangle)
{
    // 나중에 구현
}


//------------------------------------------------------------------------------------------
// 삼각형 회전
//------------------------------------------------------------------------------------------

void RotateTriangle(TRIANGLE& triangle)
{
    float centerX = (triangle.point[0].x + triangle.point[1].x + triangle.point[2].x) / 3.0f;
    float centerY = (triangle.point[0].y + triangle.point[1].y + triangle.point[2].y) / 3.0f;

    float cosAngle = cos(triangle.angle);
    float sinAngle = sin(triangle.angle);

    for (int i = 0; i < 3; ++i)
    {
        float x = triangle.point[i].x - centerX;
        float y = triangle.point[i].y - centerY;

        float rotatedX = x * cosAngle - y * sinAngle;
        float rotatedY = x * sinAngle + y * cosAngle;

        triangle.point[i].x = centerX + rotatedX;
        triangle.point[i].y = centerY + rotatedY;
    }
}
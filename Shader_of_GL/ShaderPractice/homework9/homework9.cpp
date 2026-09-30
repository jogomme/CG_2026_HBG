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
std::uniform_real_distribution<float> speedDist(0.0025f, 0.0075f);
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

    // 현재 이동 방향
    float angle;

    // X 이동 방향
    int dirX;

    // Y 이동 방향
    int dirY;

    // 2번 운동 상태
    // 0 = 가로 이동
    // 1 = 세로 이동
    int zigzagStep;

    // 2번 세로 이동 남은 프레임
    int verticalCount;

    // 4번 스파이럴 각도
    float spiralAngle;

    // 4번 스파이럴 반지름
    float spiralRadius;
};


//------------------------------------------------------------------------------------------
// 자취
//------------------------------------------------------------------------------------------

const int MAX_TRAIL_POINTS = 10000;

POINT trails[2][MAX_TRAIL_POINTS]{};

int trailCount[2]{ 0, 0 };


//------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------

void RotateTriangle(TRIANGLE& triangle, float angle);
void SetTriangleDirection(TRIANGLE& triangle, float moveX, float moveY);

void AddTriangle(TRIANGLE& triangle, float x, float y, float size);
void MakeVertexData(int index);

void MakeTrailData(int index);
void AddTrailPoint(int index);
void ClearTrail(int index);
void ClearAllTrails();
void DrawTrails();

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

POINT SetPosToGL(float x, float y);
POINT GetMousePosition(GLFWwindow* window);

POINT GetTriangleCenter(const TRIANGLE& triangle);
void KeepTriangleInside(TRIANGLE& triangle);

void CreateTriangle(POINT mouse);
int FindNearTriangle(POINT mouse);

void ChangeSize(POINT mouse);

void MoveTriangle(TRIANGLE& triangle);
void MoveBounce(TRIANGLE& triangle);
void MoveHorizontalZigzag(TRIANGLE& triangle);
void MoveVerticalZigzag(TRIANGLE& triangle);
void MoveSpiral(TRIANGLE& triangle);

void StartMoveMode(int mode);
void StopMove();


//------------------------------------------------------------------------------------------
// 전역 변수
//------------------------------------------------------------------------------------------

int wide = 1200;
int height = 1200;


// 삼각형 2개
TRIANGLE triangles[2]{};

int triangleCount = 2;


// 현재 이동 모드
// 0 = 정지
// 1 = 튕기기
// 2 = ㄹ자
// 3 = /\/\/\/
// 4 = 스파이럴
int moveMode = 0;


// 기본 상태는 정지
bool isMoving{ false };


// 4번에서 움직이는 삼각형
int spiralIndex = 0;


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
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // 창 생성
    GLFWwindow* window = glfwCreateWindow(wide, height, "OpenGL Practice 9", nullptr, nullptr);

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
    glViewport(0, 0, wide, height);


    //--------------------------------------------------------------------------------------
    // Callback
    //--------------------------------------------------------------------------------------

    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetKeyCallback(window, KeyCallback);


    //--------------------------------------------------------------------------------------
    // 삼각형 VAO / VBO
    //--------------------------------------------------------------------------------------

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);


    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);


    // 처음에는 임시 데이터
    float vertexs[] =
    {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f, 0.5f, 0.0f
    };


    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexs), vertexs, GL_DYNAMIC_DRAW);


    // 위치
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // 색상
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
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

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);


    //--------------------------------------------------------------------------------------
    // Fragment Shader 생성
    //--------------------------------------------------------------------------------------

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);


    //--------------------------------------------------------------------------------------
    // Shader Program
    //--------------------------------------------------------------------------------------

    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);


    //--------------------------------------------------------------------------------------
    // 배경
    //--------------------------------------------------------------------------------------

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);


    //--------------------------------------------------------------------------------------
    // 삼각형 생성
    //--------------------------------------------------------------------------------------

    AddTriangle(triangles[0], 0.5f, 0.5f, 0.18f);
    AddTriangle(triangles[1], -0.5f, 0.5f, 0.18f);


    // 자취 초기화
    ClearAllTrails();


    //--------------------------------------------------------------------------------------
    // 메인 루프
    //--------------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        // 화면 초기화
        glClear(GL_COLOR_BUFFER_BIT);


        // VAO 선택
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

        if (isMoving)
        {
            for (int i = 0; i < triangleCount; ++i)
            {
                MoveTriangle(triangles[i]);
            }


            // 자취 추가
            for (int i = 0; i < triangleCount; ++i)
            {
                if (moveMode == 4 && i != spiralIndex)
                {
                    continue;
                }

                AddTrailPoint(i);
            }
        }


        //----------------------------------------------------------------------------------
        // 붉은 자취
        //----------------------------------------------------------------------------------

        DrawTrails();


        //----------------------------------------------------------------------------------
        // 삼각형 그리기
        //----------------------------------------------------------------------------------

        for (int i = 0; i < triangleCount; ++i)
        {
            MakeVertexData(i);


            if (isFill)
            {
                glDrawArrays(GL_TRIANGLES, 0, 3);
            }
            else
            {
                glDrawArrays(GL_LINE_LOOP, 0, 3);
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

void AddTriangle(TRIANGLE& triangle, float x, float y, float size)
{
    triangle.size = size;


    // 랜덤 색상
    triangle.color.r = colorDist(gen);
    triangle.color.g = colorDist(gen);
    triangle.color.b = colorDist(gen);


    // 랜덤 속도
    triangle.speed = speedDist(gen);


    // 기본은 정지
    triangle.mode = 0;


    // 랜덤 이동 방향
    triangle.angle = angleDist(gen);


    float moveX = -sin(triangle.angle);
    float moveY = cos(triangle.angle);


    if (moveX >= 0.0f)
    {
        triangle.dirX = 1;
    }
    else
    {
        triangle.dirX = -1;
    }


    if (moveY >= 0.0f)
    {
        triangle.dirY = 1;
    }
    else
    {
        triangle.dirY = -1;
    }


    // 2번 상태
    triangle.zigzagStep = 0;
    triangle.verticalCount = 0;


    // 4번 상태
    triangle.spiralAngle = 0.0f;
    triangle.spiralRadius = 0.0f;


    //--------------------------------------------------------------------------------------
    // 이등변 삼각형
    //--------------------------------------------------------------------------------------

    triangle.point[0].x = x;
    triangle.point[0].y = y + size;

    triangle.point[1].x = x - size;
    triangle.point[1].y = y - size;

    triangle.point[2].x = x + size;
    triangle.point[2].y = y - size;


    // 랜덤 방향으로 회전
    RotateTriangle(triangle, triangle.angle);


    // 화면 안으로 보정
    KeepTriangleInside(triangle);
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


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_DYNAMIC_DRAW);
}


//------------------------------------------------------------------------------------------
// 자취 데이터 생성
//------------------------------------------------------------------------------------------

void MakeTrailData(int index)
{
    if (trailCount[index] < 2)
    {
        return;
    }


    float vertexData[MAX_TRAIL_POINTS * 6]{};


    for (int i = 0; i < trailCount[index]; ++i)
    {
        // 위치
        vertexData[i * 6] = trails[index][i].x;
        vertexData[i * 6 + 1] = trails[index][i].y;
        vertexData[i * 6 + 2] = trails[index][i].z;


        // 빨간색
        vertexData[i * 6 + 3] = 1.0f;
        vertexData[i * 6 + 4] = 0.0f;
        vertexData[i * 6 + 5] = 0.0f;
    }


    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * trailCount[index] * 6, vertexData, GL_DYNAMIC_DRAW);
}


//------------------------------------------------------------------------------------------
// 자취 추가
//------------------------------------------------------------------------------------------

void AddTrailPoint(int index)
{
    if (trailCount[index] >= MAX_TRAIL_POINTS)
    {
        for (int i = 1; i < MAX_TRAIL_POINTS; ++i)
        {
            trails[index][i - 1] = trails[index][i];
        }

        trailCount[index] = MAX_TRAIL_POINTS - 1;
    }


    trails[index][trailCount[index]] = GetTriangleCenter(triangles[index]);

    ++trailCount[index];
}


//------------------------------------------------------------------------------------------
// 자취 삭제
//------------------------------------------------------------------------------------------

void ClearTrail(int index)
{
    trailCount[index] = 0;
}


void ClearAllTrails()
{
    ClearTrail(0);
    ClearTrail(1);
}


//------------------------------------------------------------------------------------------
// 자취 그리기
//------------------------------------------------------------------------------------------

void DrawTrails()
{
    glLineWidth(2.0f);


    for (int i = 0; i < triangleCount; ++i)
    {
        if (trailCount[i] < 2)
        {
            continue;
        }


        if (moveMode == 4 && i != spiralIndex)
        {
            continue;
        }


        MakeTrailData(i);

        glDrawArrays(GL_LINE_STRIP, 0, trailCount[i]);
    }
}


//------------------------------------------------------------------------------------------
// Mouse Callback
//------------------------------------------------------------------------------------------

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
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

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
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
        glfwSetWindowShouldClose(window, GLFW_TRUE);
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
        AddTriangle(triangles[0], posDist(gen), posDist(gen), sizeDist(gen));
        AddTriangle(triangles[1], posDist(gen), posDist(gen), sizeDist(gen));

        moveMode = 0;
        isMoving = false;

        ClearAllTrails();
    }


    //--------------------------------------------------------------------------------------
    // 1
    //--------------------------------------------------------------------------------------

    else if (key == GLFW_KEY_1)
    {
        if (moveMode == 1 && isMoving)
        {
            StopMove();
        }
        else
        {
            StartMoveMode(1);
        }
    }


    //--------------------------------------------------------------------------------------
    // 2
    //--------------------------------------------------------------------------------------

    else if (key == GLFW_KEY_2)
    {
        if (moveMode == 2 && isMoving)
        {
            StopMove();
        }
        else
        {
            StartMoveMode(2);
        }
    }


    //--------------------------------------------------------------------------------------
    // 3
    //--------------------------------------------------------------------------------------

    else if (key == GLFW_KEY_3)
    {
        if (moveMode == 3 && isMoving)
        {
            StopMove();
        }
        else
        {
            StartMoveMode(3);
        }
    }


    //--------------------------------------------------------------------------------------
    // 4
    //--------------------------------------------------------------------------------------

    else if (key == GLFW_KEY_4)
    {
        if (moveMode == 4 && isMoving)
        {
            StopMove();
        }
        else
        {
            StartMoveMode(4);
        }
    }
}


//------------------------------------------------------------------------------------------
// 이동 모드 시작
//------------------------------------------------------------------------------------------

void StartMoveMode(int mode)
{
    // 새 운동이므로 기존 자취 삭제
    ClearAllTrails();


    moveMode = mode;
    isMoving = true;


    //--------------------------------------------------------------------------------------
    // 1. 튕기기
    //--------------------------------------------------------------------------------------

    if (mode == 1)
    {
        for (int i = 0; i < triangleCount; ++i)
        {
            triangles[i].mode = 1;
        }
    }


    //--------------------------------------------------------------------------------------
    // 2. ㄹ자
    //--------------------------------------------------------------------------------------

    else if (mode == 2)
    {
        for (int i = 0; i < triangleCount; ++i)
        {
            triangles[i].mode = 2;


            // 현재 방향에서 X 방향만 가져옴
            float moveX = -sin(triangles[i].angle);


            if (moveX >= 0.0f)
            {
                triangles[i].dirX = 1;
            }
            else
            {
                triangles[i].dirX = -1;
            }


            // Y 방향도 저장
            float moveY = cos(triangles[i].angle);


            if (moveY >= 0.0f)
            {
                triangles[i].dirY = 1;
            }
            else
            {
                triangles[i].dirY = -1;
            }


            // 가로 이동부터 시작
            triangles[i].zigzagStep = 0;
            triangles[i].verticalCount = 0;


            // 가로 방향으로 삼각형 회전
            SetTriangleDirection(triangles[i], triangles[i].speed * triangles[i].dirX, 0.0f);
        }
    }


    //--------------------------------------------------------------------------------------
    // 3. /\/\/\/
    //--------------------------------------------------------------------------------------

    else if (mode == 3)
    {
        for (int i = 0; i < triangleCount; ++i)
        {
            triangles[i].mode = 3;


            float moveX = -sin(triangles[i].angle);
            float moveY = cos(triangles[i].angle);


            if (moveX >= 0.0f)
            {
                triangles[i].dirX = 1;
            }
            else
            {
                triangles[i].dirX = -1;
            }


            if (moveY >= 0.0f)
            {
                triangles[i].dirY = 1;
            }
            else
            {
                triangles[i].dirY = -1;
            }


            // 처음부터 대각선 방향
            moveX = triangles[i].speed * 0.7071f * triangles[i].dirX;
            moveY = triangles[i].speed * 0.7071f * triangles[i].dirY;


            SetTriangleDirection(triangles[i], moveX, moveY);
        }
    }


    //--------------------------------------------------------------------------------------
    // 4. 스파이럴
    //--------------------------------------------------------------------------------------

    else if (mode == 4)
    {
        POINT origin{};


        origin.x = 0.0f;
        origin.y = 0.0f;


        // 원점에서 가장 가까운 삼각형
        spiralIndex = FindNearTriangle(origin);


        TRIANGLE& triangle = triangles[spiralIndex];


        // 현재 중심
        POINT center = GetTriangleCenter(triangle);


        // 원점으로 이동
        float moveX = -center.x;
        float moveY = -center.y;


        for (int i = 0; i < 3; ++i)
        {
            triangle.point[i].x += moveX;
            triangle.point[i].y += moveY;
        }


        // 스파이럴 초기화
        triangle.spiralAngle = 0.0f;
        triangle.spiralRadius = 0.0f;


        // 원점에서 위쪽으로 시작
        float oldAngle = triangle.angle;

        triangle.angle = 0.0f;


        float rotateAngle = triangle.angle - oldAngle;


        if (rotateAngle > 3.14159265f)
        {
            rotateAngle -= 2.0f * 3.14159265f;
        }

        if (rotateAngle < -3.14159265f)
        {
            rotateAngle += 2.0f * 3.14159265f;
        }


        RotateTriangle(triangle, rotateAngle);


        triangle.mode = 4;


        // 나머지는 정지
        for (int i = 0; i < triangleCount; ++i)
        {
            if (i != spiralIndex)
            {
                triangles[i].mode = 0;
            }
        }
    }
}


//------------------------------------------------------------------------------------------
// 이동 정지
//------------------------------------------------------------------------------------------

void StopMove()
{
    moveMode = 0;
    isMoving = false;


    ClearAllTrails();


    for (int i = 0; i < triangleCount; ++i)
    {
        triangles[i].mode = 0;
    }
}


//------------------------------------------------------------------------------------------
// 마우스 좌표 -> OpenGL 좌표
//------------------------------------------------------------------------------------------

POINT SetPosToGL(float x, float y)
{
    POINT p{};


    p.x = (x / wide) * 2.0f - 1.0f;
    p.y = 1.0f - (y / height) * 2.0f;


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


    glfwGetCursorPos(window, &mouseX, &mouseY);


    position = SetPosToGL(mouseX, mouseY);


    return position;
}


//------------------------------------------------------------------------------------------
// 삼각형 중심
//------------------------------------------------------------------------------------------

POINT GetTriangleCenter(const TRIANGLE& triangle)
{
    POINT center{};


    center.x = (triangle.point[0].x + triangle.point[1].x + triangle.point[2].x) / 3.0f;
    center.y = (triangle.point[0].y + triangle.point[1].y + triangle.point[2].y) / 3.0f;


    return center;
}


//------------------------------------------------------------------------------------------
// 화면 안으로 보정
//------------------------------------------------------------------------------------------

void KeepTriangleInside(TRIANGLE& triangle)
{
    float minX = triangle.point[0].x;
    float maxX = triangle.point[0].x;
    float minY = triangle.point[0].y;
    float maxY = triangle.point[0].y;


    for (int i = 1; i < 3; ++i)
    {
        if (triangle.point[i].x < minX)
        {
            minX = triangle.point[i].x;
        }


        if (triangle.point[i].x > maxX)
        {
            maxX = triangle.point[i].x;
        }


        if (triangle.point[i].y < minY)
        {
            minY = triangle.point[i].y;
        }


        if (triangle.point[i].y > maxY)
        {
            maxY = triangle.point[i].y;
        }
    }


    float moveX = 0.0f;
    float moveY = 0.0f;


    if (minX < -1.0f)
    {
        moveX = -1.0f - minX;
    }


    if (maxX > 1.0f)
    {
        moveX = 1.0f - maxX;
    }


    if (minY < -1.0f)
    {
        moveY = -1.0f - minY;
    }


    if (maxY > 1.0f)
    {
        moveY = 1.0f - maxY;
    }


    for (int i = 0; i < 3; ++i)
    {
        triangle.point[i].x += moveX;
        triangle.point[i].y += moveY;
    }
}


//------------------------------------------------------------------------------------------
// 가까운 삼각형 찾기
//------------------------------------------------------------------------------------------

int FindNearTriangle(POINT mouse)
{
    POINT center0 = GetTriangleCenter(triangles[0]);
    POINT center1 = GetTriangleCenter(triangles[1]);


    float dx0 = mouse.x - center0.x;
    float dy0 = mouse.y - center0.y;

    float distance0 = sqrt(dx0 * dx0 + dy0 * dy0);


    float dx1 = mouse.x - center1.x;
    float dy1 = mouse.y - center1.y;

    float distance1 = sqrt(dx1 * dx1 + dy1 * dy1);


    if (distance0 < distance1)
    {
        return 0;
    }


    return 1;
}


//------------------------------------------------------------------------------------------
// 좌클릭으로 삼각형 생성
//------------------------------------------------------------------------------------------

void CreateTriangle(POINT mouse)
{
    int nearIndex = FindNearTriangle(mouse);


    AddTriangle(triangles[nearIndex], mouse.x, mouse.y, sizeDist(gen));


    ClearTrail(nearIndex);
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


    POINT center = GetTriangleCenter(triangle);


    for (int i = 0; i < 3; ++i)
    {
        triangle.point[i].x = center.x + (triangle.point[i].x - center.x) * scale;
        triangle.point[i].y = center.y + (triangle.point[i].y - center.y) * scale;
    }


    KeepTriangleInside(triangle);
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
    float moveX = -sin(triangle.angle) * triangle.speed;
    float moveY = cos(triangle.angle) * triangle.speed;


    bool hitX = false;
    bool hitY = false;


    for (int i = 0; i < 3; ++i)
    {
        float nextX = triangle.point[i].x + moveX;
        float nextY = triangle.point[i].y + moveY;


        if (nextX > 1.0f || nextX < -1.0f)
        {
            hitX = true;
        }


        if (nextY > 1.0f || nextY < -1.0f)
        {
            hitY = true;
        }
    }


    if (hitX)
    {
        moveX = -moveX;
    }


    if (hitY)
    {
        moveY = -moveY;
    }


    if (hitX || hitY)
    {
        SetTriangleDirection(triangle, moveX, moveY);
    }


    for (int i = 0; i < 3; ++i)
    {
        triangle.point[i].x += moveX;
        triangle.point[i].y += moveY;
    }


    KeepTriangleInside(triangle);
}


//------------------------------------------------------------------------------------------
// 2. ㄹ자 이동
//------------------------------------------------------------------------------------------

void MoveHorizontalZigzag(TRIANGLE& triangle)
{
    float moveX = 0.0f;
    float moveY = 0.0f;


    //--------------------------------------------------------------------------------------
    // 가로 이동
    //--------------------------------------------------------------------------------------

    if (triangle.zigzagStep == 0)
    {
        moveX = triangle.speed * triangle.dirX;


        bool hitX = false;


        for (int i = 0; i < 3; ++i)
        {
            float nextX = triangle.point[i].x + moveX;


            if (nextX > 1.0f || nextX < -1.0f)
            {
                hitX = true;
            }
        }


        if (hitX)
        {
            // X 반전
            triangle.dirX *= -1;


            // 세로 이동 시작
            triangle.zigzagStep = 1;
            triangle.verticalCount = 12;


            moveX = 0.0f;
            moveY = triangle.speed * triangle.dirY;


            // 가로 -> 세로 회전
            SetTriangleDirection(triangle, moveX, moveY);
        }
    }


    //--------------------------------------------------------------------------------------
    // 세로 이동
    //--------------------------------------------------------------------------------------

    else if (triangle.zigzagStep == 1)
    {
        moveX = 0.0f;
        moveY = triangle.speed * triangle.dirY;


        bool hitY = false;


        for (int i = 0; i < 3; ++i)
        {
            float nextY = triangle.point[i].y + moveY;


            if (nextY > 1.0f || nextY < -1.0f)
            {
                hitY = true;
            }
        }


        if (hitY)
        {
            // Y 반전
            triangle.dirY *= -1;


            moveY = triangle.speed * triangle.dirY;
        }


        --triangle.verticalCount;


        // 세로 이동 종료 -> 다시 가로 이동
        if (triangle.verticalCount <= 0)
        {
            triangle.zigzagStep = 0;


            moveX = triangle.speed * triangle.dirX;
            moveY = 0.0f;


            // 세로 -> 가로 회전
            SetTriangleDirection(triangle, moveX, moveY);
        }
    }


    // 이동
    for (int i = 0; i < 3; ++i)
    {
        triangle.point[i].x += moveX;
        triangle.point[i].y += moveY;
    }


    KeepTriangleInside(triangle);
}


//------------------------------------------------------------------------------------------
// 3. /\/\/\/ 이동
//------------------------------------------------------------------------------------------

void MoveVerticalZigzag(TRIANGLE& triangle)
{
    float moveX = triangle.speed * 0.7071f * triangle.dirX;
    float moveY = triangle.speed * 0.7071f * triangle.dirY;


    bool hitX = false;
    bool hitY = false;


    // 다음 위치 검사
    for (int i = 0; i < 3; ++i)
    {
        float nextX = triangle.point[i].x + moveX;
        float nextY = triangle.point[i].y + moveY;


        // 좌우 벽
        if (nextX > 1.0f || nextX < -1.0f)
        {
            hitX = true;
        }


        // 위아래 벽
        if (nextY > 1.0f || nextY < -1.0f)
        {
            hitY = true;
        }
    }


    // 좌우 벽
    if (hitX)
    {
        triangle.dirX *= -1;
    }


    // 위아래 벽
    if (hitY)
    {
        triangle.dirY *= -1;
    }


    // 방향이 바뀌었으면 회전
    if (hitX || hitY)
    {
        moveX = triangle.speed * 0.7071f * triangle.dirX;
        moveY = triangle.speed * 0.7071f * triangle.dirY;


        SetTriangleDirection(triangle, moveX, moveY);
    }


    // 이동
    for (int i = 0; i < 3; ++i)
    {
        triangle.point[i].x += moveX;
        triangle.point[i].y += moveY;
    }


    KeepTriangleInside(triangle);
}


//------------------------------------------------------------------------------------------
// 4. 원 스파이럴
//------------------------------------------------------------------------------------------

void MoveSpiral(TRIANGLE& triangle)
{
    // 선택된 삼각형만
    if (&triangle != &triangles[spiralIndex])
    {
        return;
    }


    //--------------------------------------------------------------------------------------
    // 이전 위치
    //--------------------------------------------------------------------------------------

    float oldX = cos(triangle.spiralAngle) * triangle.spiralRadius;
    float oldY = sin(triangle.spiralAngle) * triangle.spiralRadius;


    //--------------------------------------------------------------------------------------
    // 각도와 반지름 변화
    //--------------------------------------------------------------------------------------

    float angleStep =  (triangle.speed * 0.05  ) * 12.0f;
    float radiusStep = (triangle.speed * 0.05 ) * 0.1f;


    triangle.spiralAngle  += angleStep;
    triangle.spiralRadius += radiusStep;


    // 삼각형 크기를 고려한 최대 반지름
    float maxRadius = 0.75f - triangle.size;


    //--------------------------------------------------------------------------------------
    // 한 바퀴 이상 바깥으로 나가면 다시 원점
    //--------------------------------------------------------------------------------------

    bool reset = false;


    if (triangle.spiralRadius >= maxRadius)
    {
        triangle.spiralRadius = 0.0f;
        triangle.spiralAngle = 0.0f;

        reset = true;


        // 새로운 나선이므로 기존 자취 제거
        ClearTrail(spiralIndex);
    }


    float newX = cos(triangle.spiralAngle) * triangle.spiralRadius;
    float newY = sin(triangle.spiralAngle) * triangle.spiralRadius;


    //--------------------------------------------------------------------------------------
    // 원점 -> 새로운 위치
    //--------------------------------------------------------------------------------------

    POINT center = GetTriangleCenter(triangle);


    // 현재 중심에서 새로운 위치까지 이동
    float moveX = newX - center.x;
    float moveY = newY - center.y;


    if (reset)
    {
        // 리셋하는 순간 원점으로 이동
        moveX = -center.x;
        moveY = -center.y;
    }


    // 삼각형 이동
    for (int i = 0; i < 3; ++i)
    {
        triangle.point[i].x += moveX;
        triangle.point[i].y += moveY;
    }


    //--------------------------------------------------------------------------------------
    // 실제 이동 방향으로 회전
    //--------------------------------------------------------------------------------------

    if (std::fabs(moveX) > 0.000001f || std::fabs(moveY) > 0.000001f)
    {
        SetTriangleDirection(triangle, moveX, moveY);
    }


    KeepTriangleInside(triangle);
}


//------------------------------------------------------------------------------------------
// 삼각형 이동 방향에 맞게 회전
//------------------------------------------------------------------------------------------

void SetTriangleDirection(TRIANGLE& triangle, float moveX, float moveY)
{
    if (std::fabs(moveX) < 0.000001f && std::fabs(moveY) < 0.000001f)
    {
        return;
    }


    // 위쪽이 0도인 현재 삼각형 기준으로 이동 방향을 각도로 변환
    float newAngle = atan2(-moveX, moveY);


    // 기존 각도와 새 각도의 차이
    float rotateAngle = newAngle - triangle.angle;


    // 가장 짧은 회전 방향 사용
    if (rotateAngle > 3.14159265f)
    {
        rotateAngle -= 2.0f * 3.14159265f;
    }

    if (rotateAngle < -3.14159265f)
    {
        rotateAngle += 2.0f * 3.14159265f;
    }


    // 실제 삼각형 회전
    RotateTriangle(triangle, rotateAngle);


    // 현재 방향 저장
    triangle.angle = newAngle;
}


//------------------------------------------------------------------------------------------
// 삼각형 회전
//------------------------------------------------------------------------------------------

void RotateTriangle(TRIANGLE& triangle, float angle)
{
    POINT center = GetTriangleCenter(triangle);


    float cosAngle = cos(angle);
    float sinAngle = sin(angle);


    for (int i = 0; i < 3; ++i)
    {
        float x = triangle.point[i].x - center.x;
        float y = triangle.point[i].y - center.y;


        float rotatedX = x * cosAngle - y * sinAngle;
        float rotatedY = x * sinAngle + y * cosAngle;


        triangle.point[i].x = center.x + rotatedX;
        triangle.point[i].y = center.y + rotatedY;
    }
}
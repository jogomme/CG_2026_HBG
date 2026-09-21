#include <GL/glew.h>
#include <GL/glfw3.h>

#include <iostream>
#include <random>


// ------------------------------------------------------------
// 랜덤
// ------------------------------------------------------------

std::random_device rd;
std::mt19937 gen(rd());

std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);
std::uniform_real_distribution<float> sizeDist(0.04f, 0.08f);
std::uniform_real_distribution<float> positionDist(-0.9f, 0.9f);
std::uniform_int_distribution<int> countDist(20, 40);


// ------------------------------------------------------------
// 구조체
// ------------------------------------------------------------

struct POINT
{
    float x1;
    float y1;

    float x2;
    float y2;
};


struct COLOR
{
    float r;
    float g;
    float b;
};


struct POSITION
{
    float x;
    float y;
};


struct RECTANGLE
{
    // 현재 사각형 위치
    POINT point;

    // 처음 생성했을 때의 위치
    POINT originalPoint;

    // 현재 색상
    COLOR color;

    // 처음 색상
    COLOR originalColor;

    // 현재 사각형이 보이는지 확인
    bool isVisible{ true };

    // 마우스 충돌로 상태가 변경되었는지 확인
    bool isChanged{ false };
};


// ------------------------------------------------------------
// 전역 변수
// ------------------------------------------------------------

int wide{ 1600 };
int height{ 1200 };


// rects[0]은 지우개 전용
// rects[1]부터 일반 사각형
RECTANGLE rects[50]{};


// 일반 사각형 개수
int rectangleCount{ 0 };


// 처음 생성되는 사각형 개수
int originalRectangleCount{ 0 };


// 오른쪽 마우스로 추가한 개수
int addedRectangleCount{ 0 };


// 최대 10개 추가
const int maxAddRectangle{ 10 };


// ------------------------------------------------------------
// 지우개
// ------------------------------------------------------------

// 지우개의 현재 크기
float eraserSizeX{};
float eraserSizeY{};


// 처음 지우개의 크기
float originalEraserSizeX{};
float originalEraserSizeY{};


// 지우개가 현재 보이는지 확인
bool isEraserVisible{ false };


// ------------------------------------------------------------
// 마우스 상태
// ------------------------------------------------------------

// 왼쪽 마우스를 누르고 있는지 확인
bool isLeftDown{ false };


// ------------------------------------------------------------
// 함수 선언
// ------------------------------------------------------------

POSITION SetPosToGL(float x, float y);


// 화면 그리기
void DrawScene();


// 랜덤 색상
COLOR randColor();


// 사각형 생성
void AddRect();


// 처음 사각형 생성
void CreateInitialRects();


// 지우개 생성
void CreateEraser(GLFWwindow* window);


// 마우스 위치
POSITION GetMousePosition(GLFWwindow* window);


// 지우개와 사각형 충돌 확인
bool IsCollision(RECTANGLE& eraser, RECTANGLE& rect);


// 지우개
void Eraser(GLFWwindow* window);


// 충돌했을 때 상태 변경
void ChangeCollision(RECTANGLE& rect);


// 원래 상태 복구
void RestoreRectangles();


// 전체 리셋
void Reset();


// ------------------------------------------------------------
// Callback
// ------------------------------------------------------------

// 키보드 Callback
void KeyCallback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods
);


// 마우스 버튼 Callback
void MouseButtonCallback(
    GLFWwindow* window,
    int button,
    int action,
    int mods
);


// ------------------------------------------------------------
// main
// ------------------------------------------------------------

int main()
{
    // GLFW 초기화
    if (!glfwInit())
    {
        std::cerr << "GLFW 초기화 실패!" << std::endl;
        return -1;
    }


    // OpenGL 버전 설정
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_COMPAT_PROFILE
    );


    // 윈도우 생성
    GLFWwindow* window =
        glfwCreateWindow(
            wide,
            height,
            "OpenGL Practice 5",
            nullptr,
            nullptr
        );


    if (!window)
    {
        std::cerr << "윈도우 생성 실패!" << std::endl;

        glfwTerminate();

        return -1;
    }


    // OpenGL Context 활성화
    glfwMakeContextCurrent(window);


    // GLEW 초기화
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "GLEW 초기화 실패!" << std::endl;

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


    // Callback 등록
    glfwSetKeyCallback(
        window,
        KeyCallback
    );

    glfwSetMouseButtonCallback(
        window,
        MouseButtonCallback
    );


    // 처음 사각형 생성
    CreateInitialRects();


    // --------------------------------------------------------
    // 메인 루프
    // --------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        // ----------------------------------------------------
        // 왼쪽 마우스를 누르고 있는 동안
        // ----------------------------------------------------

        if (isLeftDown)
        {
            Eraser(window);
        }


        // 화면 그리기
        DrawScene();


        glfwSwapBuffers(window);

        glfwPollEvents();
    }


    // 종료
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


// ------------------------------------------------------------
// 화면 그리기
// ------------------------------------------------------------

void DrawScene()
{
    // 배경
    glClearColor(
        1.0f,
        1.0f,
        1.0f,
        1.0f
    );

    glClear(GL_COLOR_BUFFER_BIT);


    // --------------------------------------------------------
    // 지우개 그리기
    // --------------------------------------------------------

    if (isEraserVisible)
    {
        glColor3f(
            rects[0].color.r,
            rects[0].color.g,
            rects[0].color.b
        );


        glRectf(
            rects[0].point.x1,
            rects[0].point.y1,
            rects[0].point.x2,
            rects[0].point.y2
        );
    }


    // --------------------------------------------------------
    // 작은 사각형 그리기
    // --------------------------------------------------------

    for (int i = 1; i <= rectangleCount; ++i)
    {
        // 삭제된 상태라면 그리지 않음
        if (!rects[i].isVisible)
        {
            continue;
        }


        // 색상 설정
        glColor3f(
            rects[i].color.r,
            rects[i].color.g,
            rects[i].color.b
        );


        // 사각형 그리기
        glRectf(
            rects[i].point.x1,
            rects[i].point.y1,
            rects[i].point.x2,
            rects[i].point.y2
        );
    }
}


// ------------------------------------------------------------
// 랜덤 색상
// ------------------------------------------------------------

COLOR randColor()
{
    COLOR color{};

    color.r = colorDist(gen);
    color.g = colorDist(gen);
    color.b = colorDist(gen);

    return color;
}


// ------------------------------------------------------------
// 처음 사각형 생성
// ------------------------------------------------------------

void CreateInitialRects()
{
    // 20 ~ 40개 랜덤
    originalRectangleCount = countDist(gen);


    for (int i = 0; i < originalRectangleCount; ++i)
    {
        AddRect();
    }


    // 지우개 기본 크기 설정
    float rectWidth =
        rects[1].point.x2 - rects[1].point.x1;

    float rectHeight =
        rects[1].point.y1 - rects[1].point.y2;


    originalEraserSizeX = rectWidth * 2;
    originalEraserSizeY = rectHeight * 2;

    eraserSizeX = originalEraserSizeX;
    eraserSizeY = originalEraserSizeY;
}


// ------------------------------------------------------------
// 사각형 생성
// ------------------------------------------------------------

void AddRect()
{
    // 일반 사각형 + 추가 사각형 최대 50개
    if (rectangleCount >= 49)
    {
        return;
    }


    // rects[0]은 지우개이므로
    // 일반 사각형은 1번부터 사용
    RECTANGLE& rect = rects[rectangleCount + 1];


    // --------------------------------------------------------
    // 랜덤 위치
    // --------------------------------------------------------

    float sizeX = sizeDist(gen);
    float sizeY = sizeDist(gen);

    float centerX = positionDist(gen);
    float centerY = positionDist(gen);


    rect.point.x1 = centerX - sizeX / 2;
    rect.point.y1 = centerY + sizeY / 2;

    rect.point.x2 = centerX + sizeX / 2;
    rect.point.y2 = centerY - sizeY / 2;


    // 처음 위치 저장
    rect.originalPoint = rect.point;


    // 랜덤 색상
    rect.color = randColor();


    // 처음 색상 저장
    rect.originalColor = rect.color;


    // 기본 상태
    rect.isVisible = true;
    rect.isChanged = false;


    // 개수 증가
    rectangleCount++;
}


// ------------------------------------------------------------
// 지우개 생성
// ------------------------------------------------------------

void CreateEraser(GLFWwindow* window)
{
    POSITION mouse = GetMousePosition(window);


    // 지우개 위치
    rects[0].point.x1 =
        mouse.x - eraserSizeX / 2;

    rects[0].point.x2 =
        mouse.x + eraserSizeX / 2;

    rects[0].point.y1 =
        mouse.y + eraserSizeY / 2;

    rects[0].point.y2 =
        mouse.y - eraserSizeY / 2;


    // 처음에는 검정색
    rects[0].color = COLOR{ 0.0f, 0.0f, 0.0f };


    // 지우개 보이기
    rects[0].isVisible = true;
    isEraserVisible = true;
}


// ------------------------------------------------------------
// 지우개와 사각형 충돌 확인
// ------------------------------------------------------------

bool IsCollision(
    RECTANGLE& eraser,
    RECTANGLE& rect
)
{
    if (eraser.point.x2 < rect.point.x1)
    {
        return false;
    }

    if (eraser.point.x1 > rect.point.x2)
    {
        return false;
    }

    if (eraser.point.y2 > rect.point.y1)
    {
        return false;
    }

    if (eraser.point.y1 < rect.point.y2)
    {
        return false;
    }


    return true;
}


// ------------------------------------------------------------
// 지우개
// ------------------------------------------------------------

void Eraser(GLFWwindow* window)
{
    // 지우개를 현재 마우스 위치로 이동
    POSITION mouse = GetMousePosition(window);


    float width =
        eraserSizeX;

    float heightValue =
        eraserSizeY;


    // 지우개 위치 변경
    rects[0].point.x1 =
        mouse.x - width / 2;

    rects[0].point.x2 =
        mouse.x + width / 2;

    rects[0].point.y1 =
        mouse.y + heightValue / 2;

    rects[0].point.y2 =
        mouse.y - heightValue / 2;


    // --------------------------------------------------------
    // 충돌 확인
    // --------------------------------------------------------

    for (int i = 1; i <= rectangleCount; ++i)
    {
        RECTANGLE& rect = rects[i];


        // 이미 사라진 사각형은 제외
        if (!rect.isVisible)
        {
            continue;
        }


        // 지우개와 충돌했는지 확인
        if (IsCollision(rects[0], rect))
        {
            ChangeCollision(rect);

        }
    }
}


// ------------------------------------------------------------
// 충돌 처리
// ------------------------------------------------------------

void ChangeCollision(RECTANGLE& rect)
{
    // 충돌한 사각형의 색상 저장
    rects[0].color = rect.color;


    // 충돌한 사각형 삭제
    rect.isVisible = false;


    // 상태 변경
    rect.isChanged = true;


    // --------------------------------------------------------
    // 여기서 지우개 크기 증가
    // --------------------------------------------------------

    eraserSizeX += 0.03f;
    eraserSizeY += 0.03f;
}


// ------------------------------------------------------------
// 원래 상태 복구
// ------------------------------------------------------------

void RestoreRectangles()
{
    for (int i = 1; i <= rectangleCount; ++i)
    {
        RECTANGLE& rect = rects[i];


        if (rect.isChanged)
        {
            // 원래 위치
            rect.point = rect.originalPoint;


            // 원래 색상
            rect.color = rect.originalColor;


            // 다시 보이게
            rect.isVisible = true;


            // 변경 상태 해제
            rect.isChanged = false;
        }
    }


    // --------------------------------------------------------
    // 지우개 원래 상태로 복구
    // --------------------------------------------------------

    eraserSizeX = originalEraserSizeX;
    eraserSizeY = originalEraserSizeY;


    rects[0].color =
        COLOR{ 0.0f, 0.0f, 0.0f };


    rects[0].isVisible = false;

    isEraserVisible = false;
}


// ------------------------------------------------------------
// 전체 리셋
// ------------------------------------------------------------

void Reset()
{
    for (int i = 0; i < 50; ++i)
    {
        rects[i] = RECTANGLE{};
    }


    rectangleCount = 0;
    originalRectangleCount = 0;
    addedRectangleCount = 0;
    isLeftDown = false;


    // 지우개 초기화
    eraserSizeX = 0;
    eraserSizeY = 0;

    originalEraserSizeX = 0;
    originalEraserSizeY = 0;

    isEraserVisible = false;


    // 처음부터 다시 생성
    CreateInitialRects();
}


// ------------------------------------------------------------
// 마우스 좌표
// ------------------------------------------------------------

POSITION GetMousePosition(GLFWwindow* window)
{
    POSITION position{};

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


// ------------------------------------------------------------
// Keyboard Callback
// ------------------------------------------------------------

void KeyCallback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods
)
{
    // 키를 눌렀을 때만 처리
    if (action != GLFW_PRESS)
    {
        return;
    }


    // --------------------------------------------------------
    // R
    // --------------------------------------------------------

    if (key == GLFW_KEY_R)
    {
        // 전체 리셋
        Reset();
    }


    // --------------------------------------------------------
    // Q
    // --------------------------------------------------------

    if (key == GLFW_KEY_Q)
    {
        // 프로그램 종료
        glfwSetWindowShouldClose(
            window,
            GLFW_TRUE
        );
    }
}


// ------------------------------------------------------------
// Mouse Button Callback
// ------------------------------------------------------------

void MouseButtonCallback(
    GLFWwindow* window,
    int button,
    int action,
    int mods
)
{
    // --------------------------------------------------------
    // 왼쪽 마우스
    // --------------------------------------------------------

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            // 왼쪽 마우스를 누름
            isLeftDown = true;


            // 지우개 생성
            CreateEraser(window);
        }
        else if (action == GLFW_RELEASE)
        {
            // 마우스를 놓음
            isLeftDown = false;


            // 변경된 사각형 복구
            RestoreRectangles();
        }
    }


    // --------------------------------------------------------
    // 오른쪽 마우스
    // --------------------------------------------------------

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            // 추가 가능한 개수 확인
            if (addedRectangleCount < maxAddRectangle)
            {
                AddRect();

                addedRectangleCount++;


                // ------------------------------------------------
                // 지우개 크기 감소
                // ------------------------------------------------
                eraserSizeX -= 0.01f;
                eraserSizeY -= 0.01f;

                originalEraserSizeX -= 0.01f;
                originalEraserSizeY -= 0.01f;
            }
        }
    }
}


// ------------------------------------------------------------
// 윈도우 좌표 -> OpenGL 좌표
// ------------------------------------------------------------

POSITION SetPosToGL(float x, float y)
{
    POSITION p{};


    // x
    p.x = (x / wide) * 2 - 1;


    // y
    p.y = 1 - (y / height) * 2;


    return p;
}
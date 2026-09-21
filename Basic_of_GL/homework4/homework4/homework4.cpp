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
std::uniform_real_distribution<float> sizeDist(0.1f, 0.5f);
std::uniform_real_distribution<float> speedDist(0.003f, 0.008f);
std::uniform_int_distribution pmDist(0, 1);

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
    // 현재 사각형의 위치
    POINT point;

    // 처음 생성했을 때의 위치
    POINT originalPoint;

    // 사각형 색상
    COLOR color;

    // 이동 방향
    POSITION speed;

    // 크기 변화에 사용할 값
    float size;

    // 크기가 커지는지 확인
    bool sizeIncreasing{ true };

    // Move2에서 지금까지 내려간 높이
    float nowheight{};

    // 지금 아래로 내려가는 중인지 확인
    bool isGoingDown{ false };

    // 현재 향하고 있는 방향 Move3 전용
    // 상 우 하 좌 0 1 2 3 기본은 0
    int Dir{ 0 };

    // Move3에서 벽에 붙었는지를 알려주는 변수
    bool isMove3Ready{ false };
};


// ------------------------------------------------------------
// 전역 변수
// ------------------------------------------------------------

int wide{ 1600 };
int height{ 1200 };


// 최대 5개
RECTANGLE rects[5]{};

int rectangleCount{ 0 };


// ------------------------------------------------------------
// 애니메이션 상태
// ------------------------------------------------------------

// 위치 변화 1
bool isMove1{ false };

// 위치 변화 2
bool isMove2{ false };

// 위치 변화 3
bool isMove3{ false };

// 크기 변화
bool isSizeChange{ false };

// 색상 변화
bool isColorChange{ false };


// ------------------------------------------------------------
// 함수 선언
// ------------------------------------------------------------

POSITION SetPosToGL(float x, float y);


// 화면 그리기
void DrawScene();


// 랜덤 색상
COLOR randColor();


// 사각형 생성
void AddRect(GLFWwindow* window);


// 위치 변화 1
void Move1();


// 위치 변화 2
void Move2();


// 위치 변화 3
void Move3();

void CheckWall(RECTANGLE& rect);


// 크기 변화
void ChangeSize();


// 색상 변화
void ChangeColor();


// 모든 애니메이션 정지
void StopAnimation();


// 원래 위치로 이동
void MoveOriginal();


// 사각형 삭제
void Reset();


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


// 마우스 좌표
POSITION GetMousePosition(GLFWwindow* window);


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
            "OpenGL Practice 4",
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
    glViewport(0, 0, wide, height);


    // Callback 등록
    glfwSetKeyCallback(
        window,
        KeyCallback
    );

    glfwSetMouseButtonCallback(
        window,
        MouseButtonCallback
    );


    // --------------------------------------------------------
    // 메인 루프
    // --------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        // 위치 변화 1
        if (isMove1)
        {
            Move1();
        }


        // 위치 변화 2
        if (isMove2)
        {
            Move2();
        }


        // 위치 변화 3
        if (isMove3)
        {
            Move3();
        }


        // 크기 변화
        if (isSizeChange)
        {
            ChangeSize();
        }


        // 색상 변화
        if (isColorChange)
        {
            ChangeColor();
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
    // 배경색
    // 짙은 회색
    glClearColor(
        0.2f,
        0.2f,
        0.2f,
        1.0f
    );


    // 화면 초기화
    glClear(GL_COLOR_BUFFER_BIT);


    // --------------------------------------------------------
    // 사각형 그리기
    // --------------------------------------------------------

    for (int i = 0; i < rectangleCount; ++i)
    {
        // 사각형 색상 설정
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

    // r, g, b 값을 각각 0 ~ 1 사이의 랜덤값으로 설정
    color.r = colorDist(gen);
    color.g = colorDist(gen);
    color.b = colorDist(gen);

    return color;
}


// ------------------------------------------------------------
// 사각형 생성
// ------------------------------------------------------------

void AddRect(GLFWwindow* window)
{
    // 최대 5개인지 확인
    if (rectangleCount >= 5)
    {
        return;
    }


    // 마우스 위치 가져오기
    POSITION mouse = GetMousePosition(window);


    // 새 사각형
    RECTANGLE& rect = rects[rectangleCount];


    // --------------------------------------------------------
    // 사각형 크기 설정
    // --------------------------------------------------------

    // 기존 과제에서 사용했던 랜덤 크기 방식
    float sizeX = sizeDist(gen);
    float sizeY = sizeDist(gen);


    // --------------------------------------------------------
    // 클릭한 위치를 중심으로 사각형 생성
    // --------------------------------------------------------

    rect.point.x1 = mouse.x - sizeX / 2;
    rect.point.y1 = mouse.y + sizeY / 2;

    rect.point.x2 = mouse.x + sizeX / 2;
    rect.point.y2 = mouse.y - sizeY / 2;


    // --------------------------------------------------------
    // 처음 생성한 위치 저장
    // --------------------------------------------------------

    // M을 눌렀을 때 이 위치로 돌아가기 위해 저장
    rect.originalPoint = rect.point;


    // 랜덤 색상
    rect.color = randColor();


    // --------------------------------------------------------
    // 이동 속도
    // --------------------------------------------------------

    // 나중에 위치 애니메이션에서 사용할 값
    // x와 y의 방향을 각 사각형마다 다르게 줄 수 있음

    if (pmDist(gen) == 1) {
        rect.speed.x = speedDist(gen);
    }
    else {
        rect.speed.x = -speedDist(gen);
    }

    if (pmDist(gen) == 1) {
        rect.speed.y = speedDist(gen);
    }
    else {
        rect.speed.y = -speedDist(gen);
    }


    // 크기 변화에 사용할 값
    rect.size = 1.0f;


    // 처음에는 커지는 상태
    rect.sizeIncreasing = true;


    // 개수 증가
    rectangleCount++;
}


// ------------------------------------------------------------
// 위치 변화 1
// ------------------------------------------------------------

void Move1()
{
    // 모든 사각형을 반복해서 처리
    for (int i = 0; i < rectangleCount; ++i)
    {
        // 현재 사각형 가져오기
        RECTANGLE& rect = rects[i];


        // ----------------------------------------------------
        // 대각선 이동
        // ----------------------------------------------------

        // x와 y를 동시에 변경하면
        // 사각형이 대각선 방향으로 이동한다.

        rect.point.x1 += rect.speed.x;
        rect.point.x2 += rect.speed.x;

        rect.point.y1 += rect.speed.y;
        rect.point.y2 += rect.speed.y;


        // ----------------------------------------------------
        // 벽에 닿았는지 확인
        // ----------------------------------------------------

        if (rect.point.x1 <= -1.0f) {
            rect.speed.x = -rect.speed.x;
        }
        else if (rect.point.x2 >= 1.0f) {
            rect.speed.x = -rect.speed.x;
        }

        if (rect.point.y1 >= 1.0f) {
            rect.speed.y = -rect.speed.y;
        }
        else if (rect.point.y2 <= -1.0f) {
            rect.speed.y = -rect.speed.y;
        }
    }
}


// ------------------------------------------------------------
// 위치 변화 2
// ------------------------------------------------------------

void Move2()
{
    // 모든 사각형을 반복
    for (int i = 0; i < rectangleCount; ++i)
    {
        RECTANGLE& rect = rects[i];

        float rectHeight = rect.point.y1 - rect.point.y2;

        // ----------------------------------------------------
        // 아래로 내려가는 중
        // ----------------------------------------------------

        if (rect.isGoingDown)
        {
            if (rect.point.y1 >= 1.0f || rect.point.y2 <= -1.0f) {
                rect.speed.y = -rect.speed.y;
            }

            // 한 프레임마다 조금씩 아래로 이동
            rect.point.y1 -= rect.speed.y;
            rect.point.y2 -= rect.speed.y;

            // 이번에 내려간 만큼 저장
            float s{ rect.speed.y };

            if (s < 0)
            {
                s = -s;
            }

            rect.nowheight += s;


            // 사각형 높이만큼 내려갔다면
            if (rect.nowheight >= rectHeight)
            {
                // 다시 처음 상태로
                rect.nowheight = 0;
                rect.isGoingDown = false;

                // 다시 반대 방향으로 가로 이동
                rect.speed.x = -rect.speed.x;
            }

            // 이번 프레임은 여기까지
            continue;
        }


        // ----------------------------------------------------
        // 가로 방향 이동
        // ----------------------------------------------------

        rect.point.x1 += rect.speed.x;
        rect.point.x2 += rect.speed.x;


        // ----------------------------------------------------
        // 화면 끝에 도착했는지 확인
        // ----------------------------------------------------

        if (rect.point.x1 <= -1.0f || rect.point.x2 >= 1.0f)
        {
            // 벽에 닿으면 아래로 내려가기 시작
            rect.isGoingDown = true;
        }
    }
}


// ------------------------------------------------------------
// 위치 변화 3
// ------------------------------------------------------------

void CheckWall(RECTANGLE& rect)
{
    POINT pt = rect.point;

    if (pt.x1 <= -1.0f) {
        rect.Dir = 0;
        rect.isMove3Ready = true;
    }
    else if (pt.x2 >= 1.0f) {
        rect.Dir = 2;
        rect.isMove3Ready = true;
    }
    else if (pt.y1 >= 1.0f) {
        rect.Dir = 1;
        rect.isMove3Ready = true;
    }
    else if (pt.y2 <= -1.0f) {
        rect.Dir = 3;
        rect.isMove3Ready = true;
    }
}

void Move3()
{
    // 모든 사각형을 반복
    for (int i = 0; i < rectangleCount; ++i)
    {
        RECTANGLE& rect = rects[i];


        // ----------------------------------------------------
        // 윈도우 가장자리를 따라 이동
        // ----------------------------------------------------

        if (!rect.isMove3Ready) {

            //x < 0 && y > 0->왼쪽 위
            //x > 0 && y > 0->오른쪽 위
            //x < 0 && y < 0->왼쪽 아래
            //x > 0 && y < 0->오른쪽 아래
            float cx = (rect.point.x1 + rect.point.x2) / 2;
            float cy = (rect.point.y1 + rect.point.y2) / 2;

            POSITION p{ cx,cy };

            float sx = abs(rect.speed.x);
            float sy = abs(rect.speed.y);

            if (p.x < 0 && p.y > 0)
            {
                // 왼쪽 위쪽으로 이동
                rect.point.x1 -= sx;
                rect.point.x2 -= sx;

                rect.point.y1 += sy;
                rect.point.y2 += sy;
            }
            else if (p.x > 0 && p.y > 0)
            {
                // 오른쪽 위쪽으로 이동
                rect.point.x1 += sx;
                rect.point.x2 += sx;

                rect.point.y1 += sy;
                rect.point.y2 += sy;
            }
            else if (p.x < 0 && p.y < 0)
            {
                // 왼쪽 아래쪽으로 이동
                rect.point.x1 -= sx;
                rect.point.x2 -= sx;

                rect.point.y1 -= sy;
                rect.point.y2 -= sy;
            }
            else if (p.x > 0 && p.y < 0)
            {
                // 오른쪽 아래쪽으로 이동
                rect.point.x1 += sx;
                rect.point.x2 += sx;

                rect.point.y1 -= sy;
                rect.point.y2 -= sy;
            }

            CheckWall(rect);
        }


        else
        {
            float sx = abs(rect.speed.x);
            float sy = abs(rect.speed.y);

            // 왼쪽 벽 -> 위쪽으로 이동
            if (rect.Dir == 0)
            {
                rect.point.y1 += sy;
                rect.point.y2 += sy;

                // 위쪽 벽에 도착하면
                // 오른쪽으로 이동하도록 방향 변경
                if (rect.point.y1 >= 1.0f)
                {
                    rect.Dir = 1;
                }
            }


            // 위쪽 벽 -> 오른쪽으로 이동
            else if (rect.Dir == 1)
            {
                rect.point.x1 += sx;
                rect.point.x2 += sx;

                // 오른쪽 벽에 도착하면
                // 아래쪽으로 이동하도록 방향 변경
                if (rect.point.x2 >= 1.0f)
                {
                    rect.Dir = 2;
                }
            }


            // 오른쪽 벽 -> 아래쪽으로 이동
            else if (rect.Dir == 2)
            {
                rect.point.y1 -= sy;
                rect.point.y2 -= sy;

                // 아래쪽 벽에 도착하면
                // 왼쪽으로 이동하도록 방향 변경
                if (rect.point.y2 <= -1.0f)
                {
                    rect.Dir = 3;
                }
            }


            // 아래쪽 벽 -> 왼쪽으로 이동
            else if (rect.Dir == 3)
            {
                rect.point.x1 -= sx;
                rect.point.x2 -= sx;

                // 왼쪽 벽에 도착하면
                // 위쪽으로 이동하도록 방향 변경
                if (rect.point.x1 <= -1.0f)
                {
                    rect.Dir = 0;
                }
            }
        }
    }
}


// ------------------------------------------------------------
// 크기 변화
// ------------------------------------------------------------

void ChangeSize()
{
    // 모든 사각형의 크기를 변경
    for (int i = 0; i < rectangleCount; ++i)
    {
        RECTANGLE& rect = rects[i];


        // ----------------------------------------------------
        // 크기 증가 / 감소
        // ----------------------------------------------------

        if (rect.sizeIncreasing)
        {
            // 크기 증가
            rect.size += 0.01f;


            // 일정 크기에 도달하면 감소 시작
            if (rect.size >= 1.5f)
            {
                rect.size = 1.5f;
                rect.sizeIncreasing = false;
            }
        }
        else
        {
            // 크기 감소
            rect.size -= 0.01f;


            // 일정 크기에 도달하면 증가 시작
            if (rect.size <= 0.5f)
            {
                rect.size = 0.5f;
                rect.sizeIncreasing = true;
            }
        }


        // ----------------------------------------------------
        // 현재 중심 위치 구하기
        // ----------------------------------------------------

        float centerX =
            (rect.point.x1 + rect.point.x2) / 2.0f;

        float centerY =
            (rect.point.y1 + rect.point.y2) / 2.0f;


        // ----------------------------------------------------
        // 처음 사각형의 크기 구하기
        // ----------------------------------------------------

        float originalWidth =
            rect.originalPoint.x2 - rect.originalPoint.x1;

        float originalHeight =
            rect.originalPoint.y1 - rect.originalPoint.y2;


        // ----------------------------------------------------
        // 현재 크기 계산
        // ----------------------------------------------------

        float width =
            originalWidth * rect.size;

        float height =
            originalHeight * rect.size;


        // ----------------------------------------------------
        // 중심을 기준으로 크기 변경
        // ----------------------------------------------------

        rect.point.x1 =
            centerX - width / 2.0f;

        rect.point.x2 =
            centerX + width / 2.0f;

        rect.point.y1 =
            centerY + height / 2.0f;

        rect.point.y2 =
            centerY - height / 2.0f;
    }
}


// ------------------------------------------------------------
// 색상 변화
// ------------------------------------------------------------

void ChangeColor()
{
    // 모든 사각형의 색상을 변경
    for (int i = 0; i < rectangleCount; ++i)
    {
        // 기존에 사용했던 randColor()를 사용하면 됨
        rects[i].color = randColor();
    }
}


// ------------------------------------------------------------
// 모든 애니메이션 정지
// ------------------------------------------------------------

void StopAnimation()
{
    // 모든 애니메이션 상태를 false로 변경

    isMove1 = false;
    isMove2 = false;
    isMove3 = false;

    isSizeChange = false;
    isColorChange = false;
}


// ------------------------------------------------------------
// 원래 위치로 이동
// ------------------------------------------------------------

void MoveOriginal()
{
    // 모든 사각형을 처음 생성했을 때의 위치로 이동
    for (int i = 0; i < rectangleCount; ++i)
    {
        rects[i].point = rects[i].originalPoint;
    }
}


// ------------------------------------------------------------
// 리셋
// ------------------------------------------------------------

void Reset()
{
    // 현재 존재하는 사각형 정보 삭제
    for (int i = 0; i < rectangleCount; ++i)
    {
        rects[i] = RECTANGLE{};
    }


    // 개수 초기화
    rectangleCount = 0;


    // 모든 애니메이션 정지
    StopAnimation();
}


// ------------------------------------------------------------
// 마우스 좌표
// ------------------------------------------------------------

POSITION GetMousePosition(GLFWwindow* window)
{
    POSITION position{};

    double mouseX{};
    double mouseY{};


    // 현재 마우스 위치 가져오기
    glfwGetCursorPos(
        window,
        &mouseX,
        &mouseY
    );


    // 윈도우 좌표
    // -> OpenGL 좌표
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


    if (key == GLFW_KEY_1)
    {
        // 위치 변화 1 시작 / 정지
        if (isMove1)
        {
            isMove1 = false;
        }
        else
        {
            StopAnimation();
            isMove1 = true;
        }
    }


    if (key == GLFW_KEY_2)
    {
        // 위치 변화 2 시작 / 정지
        if (isMove2)
        {
            isMove2 = false;
        }
        else
        {
            StopAnimation();
            isMove2 = true;
        }
    }


    if (key == GLFW_KEY_3)
    {
        // 위치 변화 3 시작 / 정지
        if (isMove3)
        {
            isMove3 = false;
        }
        else
        {
            StopAnimation();
            isMove3 = true;
        }
    }


    if (key == GLFW_KEY_4)
    {
        // 크기 변화 시작 / 정지
        if (isSizeChange)
        {
            isSizeChange = false;
        }
        else
        {
            StopAnimation();
            isSizeChange = true;
        }
    }


    if (key == GLFW_KEY_5)
    {
        // 색상 변화 시작 / 정지
        if (isColorChange)
        {
            isColorChange = false;
        }
        else
        {
            StopAnimation();
            isColorChange = true;
        }
    }


    // --------------------------------------------------------
    // S
    // --------------------------------------------------------

    if (key == GLFW_KEY_S)
    {
        // 모든 애니메이션 정지
        StopAnimation();
    }


    // --------------------------------------------------------
    // M
    // --------------------------------------------------------

    if (key == GLFW_KEY_M)
    {
        // 원래 위치로 이동
        MoveOriginal();
    }


    // --------------------------------------------------------
    // R
    // --------------------------------------------------------

    if (key == GLFW_KEY_R)
    {
        // 사각형 삭제
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
            // 클릭한 위치를 중심으로
            // 사각형 생성
            AddRect(window);
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
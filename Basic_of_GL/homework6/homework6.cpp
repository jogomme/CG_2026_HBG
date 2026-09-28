#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <random>
#include <algorithm>

using namespace std;


// ------------------------------------------------------------
// 랜덤 값 생성
// ------------------------------------------------------------

random_device rd;
mt19937 gen(rd());

uniform_real_distribution<float> colorDist(0.0f, 1.0f);
uniform_real_distribution<float> positionDist(-0.8f, 0.8f);
uniform_real_distribution<float> sizeDist(0.1f, 0.5f);


// ------------------------------------------------------------
// 화면 크기
// ------------------------------------------------------------

int wide = 1600;
int height = 1200;


// ------------------------------------------------------------
// 최대 사각형 개수
// ------------------------------------------------------------

const int MAX_RECT = 200;


// ------------------------------------------------------------
// 사각형 좌표
// ------------------------------------------------------------

struct POINT
{
    float x1;
    float y1;
    float x2;
    float y2;
};


// ------------------------------------------------------------
// 색상
// ------------------------------------------------------------

struct COLOR
{
    float r;
    float g;
    float b;
};


// ------------------------------------------------------------
// 위치
// ------------------------------------------------------------

struct POSITION
{
    float x;
    float y;
};


// ------------------------------------------------------------
// 사각형
//
// animation
// 0 : 일반 사각형
// 1 : 좌우상하 이동
// 2 : 대각선 이동
// 3 : 사등분 된 사각형이 한쪽 방향으로 같이 이동
// 4 : 8방향 이동
// ------------------------------------------------------------

struct RECTANGLE
{
    POINT point;
    COLOR color;

    // 애니메이션 실행 여부
    bool animated{ false };

    // 현재 애니메이션 종류
    int animation{ 0 };

    // 이동 속도
    float dx{ 0.0f };
    float dy{ 0.0f };

    // 색상 변화 방향
    float colorDirection{ 1.0f };
};


// ------------------------------------------------------------
// 사각형 배열
// ------------------------------------------------------------

RECTANGLE rects[MAX_RECT]{};

int rectangleCount = 0;


// ------------------------------------------------------------
// 함수 선언
// ------------------------------------------------------------

POSITION SetPosToGL(float x, float y);

POSITION GetMousePosition(GLFWwindow* window);

COLOR randColor();

void AddRect();

void DrawScene();

int SelectRect(GLFWwindow* window);

void SplitRect4(int index);

void SplitRect8(int index);

void SetAnimation(RECTANGLE& rect);

void Animation();

void Animation1(RECTANGLE& rect);

void Animation2(RECTANGLE& rect);

void Animation3(RECTANGLE& rect);

void Animation4(RECTANGLE& rect);

void ShrinkRect(RECTANGLE& rect);

void ChangeColor(RECTANGLE& rect);

void RemoveRect();

void KeyCallback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods
);

void MouseButtonCallback(
    GLFWwindow* window,
    int button,
    int action,
    int mods
);


// ============================================================
// 화면 좌표를 OpenGL 좌표로 변환
// ============================================================

POSITION SetPosToGL(float x, float y)
{
    POSITION p{};

    p.x = (x / wide) * 2.0f - 1.0f;

    p.y = 1.0f - (y / height) * 2.0f;

    return p;
}


// ============================================================
// 현재 마우스 위치 가져오기
// ============================================================

POSITION GetMousePosition(GLFWwindow* window)
{
    double x;
    double y;

    glfwGetCursorPos(window, &x, &y);

    return SetPosToGL(
        static_cast<float>(x),
        static_cast<float>(y)
    );
}


// ============================================================
// 랜덤 색상 생성
// ============================================================

COLOR randColor()
{
    COLOR color{};

    color.r = colorDist(gen);
    color.g = colorDist(gen);
    color.b = colorDist(gen);

    return color;
}


// ============================================================
// 랜덤 사각형 하나 생성
// ============================================================

void AddRect()
{
    if (rectangleCount >= MAX_RECT)
    {
        return;
    }


    RECTANGLE& rect = rects[rectangleCount];


    // --------------------------------------------------------
    // 랜덤 중심 위치
    // --------------------------------------------------------

    float centerX = positionDist(gen);
    float centerY = positionDist(gen);


    // --------------------------------------------------------
    // 랜덤 크기
    // --------------------------------------------------------

    float rectWidth = sizeDist(gen);
    float rectHeight = sizeDist(gen);


    rect.point.x1 =
        centerX - rectWidth / 2.0f;

    rect.point.x2 =
        centerX + rectWidth / 2.0f;

    rect.point.y1 =
        centerY - rectHeight / 2.0f;

    rect.point.y2 =
        centerY + rectHeight / 2.0f;


    // --------------------------------------------------------
    // 랜덤 색상
    // --------------------------------------------------------

    rect.color = randColor();


    // --------------------------------------------------------
    // 처음 생성된 사각형은 움직이지 않는다.
    // --------------------------------------------------------

    rect.animated = false;

    rect.animation = 0;

    rect.dx = 0.0f;
    rect.dy = 0.0f;

    rect.colorDirection = 1.0f;


    rectangleCount++;
}


// ============================================================
// 화면에 사각형 그리기
// ============================================================

void DrawScene()
{
    glClearColor(
        0.15f,
        0.15f,
        0.15f,
        1.0f
    );

    glClear(GL_COLOR_BUFFER_BIT);


    for (int i = 0;
        i < rectangleCount;
        i++)
    {
        RECTANGLE& rect = rects[i];


        glColor3f(
            rect.color.r,
            rect.color.g,
            rect.color.b
        );


        glBegin(GL_QUADS);


        glVertex2f(
            rect.point.x1,
            rect.point.y1
        );


        glVertex2f(
            rect.point.x2,
            rect.point.y1
        );


        glVertex2f(
            rect.point.x2,
            rect.point.y2
        );


        glVertex2f(
            rect.point.x1,
            rect.point.y2
        );


        glEnd();
    }
}


// ============================================================
// 마우스가 위치한 사각형 찾기
//
// 배열의 뒤쪽부터 검사한다.
// 나중에 생성된 사각형을 먼저 선택한다.
// ============================================================

int SelectRect(GLFWwindow* window)
{
    POSITION mp = GetMousePosition(window);


    for (int i = rectangleCount - 1;
        i >= 0;
        i--)
    {
        RECTANGLE& rect = rects[i];


        if (mp.x >= rect.point.x1 &&
            mp.x <= rect.point.x2 &&
            mp.y >= rect.point.y1 &&
            mp.y <= rect.point.y2)
        {
            return i;
        }
    }


    return -1;
}


// ============================================================
// 애니메이션 설정
//
// 매우 중요한 함수
//
// 분할된 각각의 사각형마다
// ①~④ 중 하나를 독립적으로 랜덤 선택한다.
//
// 따라서
//
// 1 1 3 2
//
// 2 4 1 3
//
// 3 3 3 1
//
// 등의 조합이 가능하다.
// ============================================================

void SetAnimation(RECTANGLE& rect)
{
    uniform_int_distribution<int>
        animationDist(1, 4);


    rect.animation =
        animationDist(gen);


    rect.animated = true;


    // --------------------------------------------------------
    // ① 좌우상하 이동
    // --------------------------------------------------------

    if (rect.animation == 1)
    {
        uniform_int_distribution<int>
            directionDist(0, 3);

        int direction =
            directionDist(gen);


        if (direction == 0)
        {
            // 오른쪽
            rect.dx = 0.008f;
            rect.dy = 0.0f;
        }
        else if (direction == 1)
        {
            // 왼쪽
            rect.dx = -0.008f;
            rect.dy = 0.0f;
        }
        else if (direction == 2)
        {
            // 위
            rect.dx = 0.0f;
            rect.dy = 0.008f;
        }
        else
        {
            // 아래
            rect.dx = 0.0f;
            rect.dy = -0.008f;
        }
    }


    // --------------------------------------------------------
    // ② 대각선 이동
    // --------------------------------------------------------

    else if (rect.animation == 2)
    {
        uniform_int_distribution<int>
            directionDist(0, 3);

        int direction =
            directionDist(gen);


        if (direction == 0)
        {
            // 오른쪽 위
            rect.dx = 0.008f;
            rect.dy = 0.008f;
        }
        else if (direction == 1)
        {
            // 왼쪽 위
            rect.dx = -0.008f;
            rect.dy = 0.008f;
        }
        else if (direction == 2)
        {
            // 오른쪽 아래
            rect.dx = 0.008f;
            rect.dy = -0.008f;
        }
        else
        {
            // 왼쪽 아래
            rect.dx = -0.008f;
            rect.dy = -0.008f;
        }
    }


    // --------------------------------------------------------
    // ③ 한쪽 방향으로 이동
    // --------------------------------------------------------

    else if (rect.animation == 3)
    {
        uniform_int_distribution<int>
            directionDist(0, 3);

        int direction =
            directionDist(gen);


        if (direction == 0)
        {
            // 오른쪽
            rect.dx = 0.008f;
            rect.dy = 0.0f;
        }
        else if (direction == 1)
        {
            // 왼쪽
            rect.dx = -0.008f;
            rect.dy = 0.0f;
        }
        else if (direction == 2)
        {
            // 위
            rect.dx = 0.0f;
            rect.dy = 0.008f;
        }
        else
        {
            // 아래
            rect.dx = 0.0f;
            rect.dy = -0.008f;
        }
    }


    // --------------------------------------------------------
    // ④ 8방향 이동
    //
    // SetAnimation()에서는 방향을 하나만 설정한다.
    //
    // 실제 8개 생성은 SplitRect8()에서 처리한다.
    // --------------------------------------------------------

    else if (rect.animation == 4)
    {
        rect.dx = 0.0f;
        rect.dy = 0.0f;
    }
}


// ============================================================
// 사각형을 4개로 분할
//
// 좌클릭한 사각형을 사등분한다.
//
// 중요한 점:
//
// 4개의 사각형을 만든 후
// 각각 SetAnimation()을 호출한다.
//
// 따라서 4개가 서로 독립적으로
// 애니메이션을 선택한다.
// ============================================================

void SplitRect4(int index)
{
    if (index < 0 ||
        index >= rectangleCount)
    {
        return;
    }


    RECTANGLE original =
        rects[index];


    // --------------------------------------------------------
    // 기존 사각형의 중심
    // --------------------------------------------------------

    float centerX =
        (original.point.x1 +
            original.point.x2) / 2.0f;

    float centerY =
        (original.point.y1 +
            original.point.y2) / 2.0f;


    // --------------------------------------------------------
    // 기존 사각형의 절반 크기
    // --------------------------------------------------------

    float halfWidth =
        (original.point.x2 -
            original.point.x1) / 2.0f;

    float halfHeight =
        (original.point.y2 -
            original.point.y1) / 2.0f;


    // --------------------------------------------------------
    // 기존 사각형 삭제
    // --------------------------------------------------------

    for (int i = index;
        i < rectangleCount - 1;
        i++)
    {
        rects[i] =
            rects[i + 1];
    }


    rectangleCount--;


    // --------------------------------------------------------
    // 새로운 4개를 만들 수 있는지 확인
    // --------------------------------------------------------

    if (rectangleCount + 4 >
        MAX_RECT)
    {
        return;
    }


    // --------------------------------------------------------
    // 4개의 사각형이 공유할 랜덤 색상
    //
    // 색상은 하나만 생성한다.
    // --------------------------------------------------------

    COLOR newColor =
        randColor();


    // --------------------------------------------------------
    // 왼쪽 아래
    // --------------------------------------------------------

    RECTANGLE rect1{};

    rect1.point.x1 =
        centerX - halfWidth;

    rect1.point.x2 =
        centerX;

    rect1.point.y1 =
        centerY - halfHeight;

    rect1.point.y2 =
        centerY;

    rect1.color =
        newColor;


    // --------------------------------------------------------
    // 오른쪽 아래
    // --------------------------------------------------------

    RECTANGLE rect2{};

    rect2.point.x1 =
        centerX;

    rect2.point.x2 =
        centerX + halfWidth;

    rect2.point.y1 =
        centerY - halfHeight;

    rect2.point.y2 =
        centerY;

    rect2.color =
        newColor;


    // --------------------------------------------------------
    // 왼쪽 위
    // --------------------------------------------------------

    RECTANGLE rect3{};

    rect3.point.x1 =
        centerX - halfWidth;

    rect3.point.x2 =
        centerX;

    rect3.point.y1 =
        centerY;

    rect3.point.y2 =
        centerY + halfHeight;

    rect3.color =
        newColor;


    // --------------------------------------------------------
    // 오른쪽 위
    // --------------------------------------------------------

    RECTANGLE rect4{};

    rect4.point.x1 =
        centerX;

    rect4.point.x2 =
        centerX + halfWidth;

    rect4.point.y1 =
        centerY;

    rect4.point.y2 =
        centerY + halfHeight;

    rect4.color =
        newColor;


    // --------------------------------------------------------
    // 배열에 추가
    // --------------------------------------------------------

    rects[rectangleCount++] =
        rect1;

    rects[rectangleCount++] =
        rect2;

    rects[rectangleCount++] =
        rect3;

    rects[rectangleCount++] =
        rect4;


    // --------------------------------------------------------
    // ★ 가장 중요한 부분
    //
    // 각각 독립적으로 애니메이션을 결정한다.
    // --------------------------------------------------------

    SetAnimation(
        rects[rectangleCount - 4]
    );

    SetAnimation(
        rects[rectangleCount - 3]
    );

    SetAnimation(
        rects[rectangleCount - 2]
    );

    SetAnimation(
        rects[rectangleCount - 1]
    );
}


// ============================================================
// 8방향 애니메이션을 위한 8개 생성
//
// 애니메이션 ④가 선택된 경우 사용한다.
//
//     ↖  ↑  ↗
//
//     ←  ●  →
//
//     ↙  ↓  ↘
//
// 8개의 작은 사각형이 각각 다른 방향으로 이동한다.
// ============================================================

void SplitRect8(int index)
{
    if (index < 0 ||
        index >= rectangleCount)
    {
        return;
    }


    RECTANGLE original =
        rects[index];


    // --------------------------------------------------------
    // 기존 사각형 중심
    // --------------------------------------------------------

    float centerX =
        (original.point.x1 +
            original.point.x2) / 2.0f;

    float centerY =
        (original.point.y1 +
            original.point.y2) / 2.0f;


    // --------------------------------------------------------
    // 8개가 나타날 수 있도록 작은 크기로 만든다.
    // --------------------------------------------------------

    float halfWidth =
        (original.point.x2 -
            original.point.x1) / 8.0f;

    float halfHeight =
        (original.point.y2 -
            original.point.y1) / 8.0f;


    // --------------------------------------------------------
    // 기존 사각형 삭제
    // --------------------------------------------------------

    for (int i = index;
        i < rectangleCount - 1;
        i++)
    {
        rects[i] =
            rects[i + 1];
    }


    rectangleCount--;


    if (rectangleCount + 8 >
        MAX_RECT)
    {
        return;
    }


    // --------------------------------------------------------
    // 8개의 사각형은 같은 색상을 사용한다.
    // --------------------------------------------------------

    COLOR newColor =
        randColor();


    // --------------------------------------------------------
    // 8방향
    //
    // 0 : 위
    // 1 : 아래
    // 2 : 왼쪽
    // 3 : 오른쪽
    // 4 : 왼쪽 위
    // 5 : 오른쪽 위
    // 6 : 왼쪽 아래
    // 7 : 오른쪽 아래
    // --------------------------------------------------------

    float directions[8][2] =
    {
        {  0.0f,  1.0f },
        {  0.0f, -1.0f },
        { -1.0f,  0.0f },
        {  1.0f,  0.0f },

        { -1.0f,  1.0f },
        {  1.0f,  1.0f },
        { -1.0f, -1.0f },
        {  1.0f, -1.0f }
    };


    // --------------------------------------------------------
    // 8개의 작은 사각형 생성
    // --------------------------------------------------------

    for (int i = 0;
        i < 8;
        i++)
    {
        RECTANGLE rect{};


        rect.point.x1 =
            centerX - halfWidth;

        rect.point.x2 =
            centerX + halfWidth;

        rect.point.y1 =
            centerY - halfHeight;

        rect.point.y2 =
            centerY + halfHeight;


        rect.color =
            newColor;


        rect.animated =
            true;

        rect.animation =
            4;


        // 각각의 방향으로 이동
        rect.dx =
            directions[i][0] * 0.008f;

        rect.dy =
            directions[i][1] * 0.008f;


        rects[rectangleCount++] =
            rect;
    }
}


// ============================================================
// 애니메이션 ①
//
// 좌우상하 이동
//
// 수평 또는 수직 방향으로 움직인다.
// ============================================================

void Animation1(RECTANGLE& rect)
{
    rect.point.x1 +=
        rect.dx;

    rect.point.x2 +=
        rect.dx;

    rect.point.y1 +=
        rect.dy;

    rect.point.y2 +=
        rect.dy;


    // --------------------------------------------------------
    // 좌우 벽에 부딪히면 방향 변경
    // --------------------------------------------------------

    if (rect.point.x2 >= 1.0f ||
        rect.point.x1 <= -1.0f)
    {
        rect.dx *= -1.0f;
    }


    // --------------------------------------------------------
    // 위아래 벽에 부딪히면 방향 변경
    // --------------------------------------------------------

    if (rect.point.y2 >= 1.0f ||
        rect.point.y1 <= -1.0f)
    {
        rect.dy *= -1.0f;
    }
}


// ============================================================
// 애니메이션 ②
//
// 대각선 이동
// ============================================================

void Animation2(RECTANGLE& rect)
{
    rect.point.x1 +=
        rect.dx;

    rect.point.x2 +=
        rect.dx;

    rect.point.y1 +=
        rect.dy;

    rect.point.y2 +=
        rect.dy;


    if (rect.point.x2 >= 1.0f ||
        rect.point.x1 <= -1.0f)
    {
        rect.dx *= -1.0f;
    }


    if (rect.point.y2 >= 1.0f ||
        rect.point.y1 <= -1.0f)
    {
        rect.dy *= -1.0f;
    }
}


// ============================================================
// 애니메이션 ③
//
// 사등분된 4개의 사각형이
// 한쪽 방향으로 같이 이동한다.
//
// SplitRect4()에서 같은 그룹의 4개에게
// 각각 같은 방향을 부여하는 것이 아니라,
// 각 사각형의 독립 애니메이션이므로
// 여기서는 해당 사각형의 dx, dy를 그대로 사용한다.
// ============================================================

void Animation3(RECTANGLE& rect)
{
    rect.point.x1 +=
        rect.dx;

    rect.point.x2 +=
        rect.dx;

    rect.point.y1 +=
        rect.dy;

    rect.point.y2 +=
        rect.dy;


    if (rect.point.x2 >= 1.0f ||
        rect.point.x1 <= -1.0f)
    {
        rect.dx *= -1.0f;
    }


    if (rect.point.y2 >= 1.0f ||
        rect.point.y1 <= -1.0f)
    {
        rect.dy *= -1.0f;
    }
}


// ============================================================
// 애니메이션 ④
//
// 8방향 이동
//
// 각각의 작은 사각형은
// 자신에게 지정된 방향으로 이동한다.
// ============================================================

void Animation4(RECTANGLE& rect)
{
    rect.point.x1 +=
        rect.dx;

    rect.point.x2 +=
        rect.dx;

    rect.point.y1 +=
        rect.dy;

    rect.point.y2 +=
        rect.dy;
}


// ============================================================
// 크기 축소
//
// 애니메이션이 실행되는 동안
// 사각형의 크기를 점점 작게 만든다.
// ============================================================

void ShrinkRect(RECTANGLE& rect)
{
    const float shrinkSpeed =
        0.0015f;


    // 사각형 중심
    float centerX =
        (rect.point.x1 +
            rect.point.x2) / 2.0f;

    float centerY =
        (rect.point.y1 +
            rect.point.y2) / 2.0f;


    // 현재 절반 크기
    float halfWidth =
        (rect.point.x2 -
            rect.point.x1) / 2.0f;

    float halfHeight =
        (rect.point.y2 -
            rect.point.y1) / 2.0f;


    // 크기 감소
    halfWidth -=
        shrinkSpeed;

    halfHeight -=
        shrinkSpeed;


    // 중심을 유지하면서 다시 좌표 설정
    rect.point.x1 =
        centerX - halfWidth;

    rect.point.x2 =
        centerX + halfWidth;

    rect.point.y1 =
        centerY - halfHeight;

    rect.point.y2 =
        centerY + halfHeight;
}


// ============================================================
// 색상 밝기 변화
//
// 처음 지정된 색상을 기준으로
// 점점 밝아졌다가 어두워지고,
// 다시 밝아지는 방식으로 변화한다.
// ============================================================

void ChangeColor(RECTANGLE& rect)
{
    const float colorSpeed =
        0.00015f;


    rect.color.r +=
        colorSpeed *
        rect.colorDirection;

    rect.color.g +=
        colorSpeed *
        rect.colorDirection;

    rect.color.b +=
        colorSpeed *
        rect.colorDirection;


    // --------------------------------------------------------
    // 최대 밝기에 도달
    // --------------------------------------------------------

    if (rect.color.r >= 1.0f ||
        rect.color.g >= 1.0f ||
        rect.color.b >= 1.0f)
    {
        rect.color.r =
            min(rect.color.r, 1.0f);

        rect.color.g =
            min(rect.color.g, 1.0f);

        rect.color.b =
            min(rect.color.b, 1.0f);


        rect.colorDirection =
            -1.0f;
    }


    // --------------------------------------------------------
    // 최소 밝기에 도달
    // --------------------------------------------------------

    if (rect.color.r <= 0.0f ||
        rect.color.g <= 0.0f ||
        rect.color.b <= 0.0f)
    {
        rect.color.r =
            max(rect.color.r, 0.0f);

        rect.color.g =
            max(rect.color.g, 0.0f);

        rect.color.b =
            max(rect.color.b, 0.0f);


        rect.colorDirection =
            1.0f;
    }
}


// ============================================================
// 애니메이션 실행
//
// 모든 애니메이션 사각형을 검사한다.
//
// 각 사각형의 animation 값에 따라
// 서로 독립적으로 애니메이션을 실행한다.
// ============================================================

void Animation()
{
    for (int i = 0;
        i < rectangleCount;
        i++)
    {
        RECTANGLE& rect =
            rects[i];


        if (!rect.animated)
        {
            continue;
        }


        // ----------------------------------------------------
        // 사각형마다 자신의 애니메이션 실행
        // ----------------------------------------------------

        if (rect.animation == 1)
        {
            Animation1(rect);
        }
        else if (rect.animation == 2)
        {
            Animation2(rect);
        }
        else if (rect.animation == 3)
        {
            Animation3(rect);
        }
        else if (rect.animation == 4)
        {
            Animation4(rect);
        }


        // ----------------------------------------------------
        // 공통 크기 축소
        // ----------------------------------------------------

        ShrinkRect(rect);


        // ----------------------------------------------------
        // 공통 색상 변화
        // ----------------------------------------------------

        ChangeColor(rect);
    }


    // --------------------------------------------------------
    // 일정 크기 이하의 사각형 삭제
    // --------------------------------------------------------

    RemoveRect();
}


// ============================================================
// 일정 크기 이하의 사각형 삭제
// ============================================================

void RemoveRect()
{
    const float deleteSize =
        0.02f;


    for (int i = 0;
        i < rectangleCount;)
    {
        RECTANGLE& rect =
            rects[i];


        float width =
            rect.point.x2 -
            rect.point.x1;

        float heightSize =
            rect.point.y2 -
            rect.point.y1;


        if (width <= deleteSize ||
            heightSize <= deleteSize)
        {
            // 삭제된 사각형 뒤의
            // 데이터를 앞으로 이동
            for (int j = i;
                j < rectangleCount - 1;
                j++)
            {
                rects[j] =
                    rects[j + 1];
            }


            rectangleCount--;


            // 같은 i 위치에 새로운 사각형이 들어왔으므로
            // i를 증가시키지 않는다.
            continue;
        }


        i++;
    }
}


// ============================================================
// 키보드 Callback
//
// Q만 사용한다.
// ============================================================

void KeyCallback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods)
{
    if (action != GLFW_PRESS)
    {
        return;
    }


    if (key == GLFW_KEY_Q)
    {
        glfwSetWindowShouldClose(
            window,
            true
        );
    }
}


// ============================================================
// 마우스 Callback
//
// 좌클릭하면 사각형을 분할한다.
//
// 중요한 점:
//
// ①~③이 하나 선택되는 것이 아니라
// SplitRect4()에서 생성된 4개 각각이
// SetAnimation()을 호출하여
// 독립적으로 ①~④ 중 하나를 선택한다.
// ============================================================

void MouseButtonCallback(
    GLFWwindow* window,
    int button,
    int action,
    int mods)
{
    // 좌클릭만 사용
    if (button !=
        GLFW_MOUSE_BUTTON_LEFT)
    {
        return;
    }


    // 버튼을 누른 순간만 처리
    if (action != GLFW_PRESS)
    {
        return;
    }


    // 클릭한 사각형 찾기
    int selectedRect =
        SelectRect(window);


    // 사각형 외부 클릭
    if (selectedRect == -1)
    {
        return;
    }


    // --------------------------------------------------------
    // 기본 사각형은 4개로 사등분한다.
    //
    // 그리고 각각의 4개가 독립적으로
    // 애니메이션을 선택한다.
    // --------------------------------------------------------

    SplitRect4(selectedRect);
}


// ============================================================
// main
// ============================================================

int main()
{
    // --------------------------------------------------------
    // GLFW 초기화
    // --------------------------------------------------------

    if (!glfwInit())
    {
        cout <<
            "GLFW initialization failed."
            << endl;

        return -1;
    }


    // --------------------------------------------------------
    // OpenGL 버전 설정
    // --------------------------------------------------------

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3
    );

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        3
    );

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_COMPAT_PROFILE
    );


    // --------------------------------------------------------
    // 윈도우 생성
    // --------------------------------------------------------

    GLFWwindow* window =
        glfwCreateWindow(
            wide,
            height,
            "OpenGL Practice 6",
            nullptr,
            nullptr
        );


    if (!window)
    {
        cout <<
            "Window creation failed."
            << endl;

        glfwTerminate();

        return -1;
    }


    // --------------------------------------------------------
    // OpenGL Context 연결
    // --------------------------------------------------------

    glfwMakeContextCurrent(
        window
    );


    // --------------------------------------------------------
    // GLEW 초기화
    // --------------------------------------------------------

    glewExperimental =
        GL_TRUE;


    if (glewInit() != GLEW_OK)
    {
        cout <<
            "GLEW initialization failed."
            << endl;

        glfwDestroyWindow(
            window
        );

        glfwTerminate();

        return -1;
    }


    // --------------------------------------------------------
    // Viewport 설정
    // --------------------------------------------------------

    glViewport(
        0,
        0,
        wide,
        height
    );


    // --------------------------------------------------------
    // Callback 등록
    // --------------------------------------------------------

    glfwSetKeyCallback(
        window,
        KeyCallback
    );

    glfwSetMouseButtonCallback(
        window,
        MouseButtonCallback
    );


    // --------------------------------------------------------
    // 처음에 5~10개의 사각형 생성
    // --------------------------------------------------------

    uniform_int_distribution<int>
        initialCountDist(5, 10);


    int initialCount =
        initialCountDist(gen);


    for (int i = 0;
        i < initialCount;
        i++)
    {
        AddRect();
    }


    // --------------------------------------------------------
    // 메인 루프
    // --------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        // 자동 애니메이션
        Animation();


        // 화면 그리기
        DrawScene();


        // 화면 출력
        glfwSwapBuffers(window);


        // 이벤트 처리
        glfwPollEvents();
    }


    // --------------------------------------------------------
    // 종료
    // --------------------------------------------------------

    glfwDestroyWindow(
        window
    );

    glfwTerminate();

    return 0;
}
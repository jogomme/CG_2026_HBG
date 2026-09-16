#include <GL/glew.h>
#include <GL/glfw3.h>

#include <iostream>
#include <random>

// 랜덤
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> colorDist(0, 1.0f);
std::uniform_real_distribution<float> sizeDist(0, 0.9);

// 이렇게 하면 두 점이 주어짐으로 사각형을 그릴 수 있음
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

struct POSISTION
{
    float x;
    float y;
};

struct RECTANGLE
{
    POINT point;
    COLOR color;

    bool selected{ false };
};


// ------------------------------------------------------------
// 전역 변수
// ------------------------------------------------------------

// 필요한 변수들을 직접 만들어보세요.
// 예:
// 사각형 위치
// 사각형 색상

int wide{ 1600 };
int height{ 1200 };

RECTANGLE rects[4][5]{};
int rectangleCount[4]{};

int selectArea{ -1 };
int selectedRect{ -1 };

COLOR areaColor[4];

// ------------------------------------------------------------
// 함수 선언
// ------------------------------------------------------------

void InputProcess(GLFWwindow* window);

void DrawScene();

POSISTION SetPosToGL(float x, float y);

COLOR randColor();

void SetAreaColor();

void AddRect(int area);

void SelectRect(GLFWwindow* window);


void ChangeSize(float size);


void ChangeColor();


void Reset();


bool KeyPressed(GLFWwindow* window, int key);


bool MousePressed(GLFWwindow* window, int button);

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

    std::cout << "===== 명령어 목록 =====\n";
    std::cout << "c : 색깔바꾸기\n";
    std::cout << "+, - : 크기 바꾸기\n";
    std::cout << "1,2,3,4 : 사각형 추가하기\n";
    std::cout << "좌클릭 : 선택하기\n";
    std::cout << "r : 리셋하기\n";
    std::cout << "q : 프로그램 종료\n";
    std::cout << "=======================\n";


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
            "OpenGL Practice 2",
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

    // 4개의 영역 색상 설정
    SetAreaColor();

    // --------------------------------------------------------
    // 메인 루프
    // --------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        InputProcess(window);

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
// 키보드 입력
// ------------------------------------------------------------

void InputProcess(GLFWwindow* window)
{
    // ESC를 누르면 종료
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }


    // 1사분면
    if (KeyPressed(window, GLFW_KEY_1)) {
        AddRect(0);
    }

    // 2사분면
    if (KeyPressed(window, GLFW_KEY_2)) {
        AddRect(1);
    }

    // 3사분면
    if (KeyPressed(window, GLFW_KEY_3)) {
        AddRect(2);
    }

    // 4사분면
    if (KeyPressed(window, GLFW_KEY_4)) {
        AddRect(3);
    }
    
    // + -
    if (KeyPressed(window, GLFW_KEY_KP_ADD) || KeyPressed(window, GLFW_KEY_EQUAL)) {
        ChangeSize(GLFW_KEY_KP_ADD);
    }
    else if (KeyPressed(window, GLFW_KEY_MINUS)) {
        ChangeSize(GLFW_KEY_MINUS);
    }

    // 색깔 바꾸기
    if (KeyPressed(window, GLFW_KEY_C)) {
        ChangeColor();
    }

    // 리셋
    if (KeyPressed(window, GLFW_KEY_R)) {
        Reset();
    }

    // 마우스 선택
    if (MousePressed(window, GLFW_MOUSE_BUTTON_LEFT)) {
        SelectRect(window);
    }

}


// ------------------------------------------------------------
// 화면 그리기
// ------------------------------------------------------------

void DrawScene()
{
    // 배경색 설정
    glClearColor(
        1.0f,
        1.0f,
        1.0f,
        1.0f
    );

    glClear(GL_COLOR_BUFFER_BIT);


    // --------------------------------------------------------
    // 여기부터 직접 작성
    // --------------------------------------------------------

    // 1영역
    glColor3f(areaColor[0].r, areaColor[0].g, areaColor[0].b);
    glRectf(-1.0f, 1.0f, 0.0f, 0.0f);

    // 2영역
    glColor3f(areaColor[1].r, areaColor[1].g, areaColor[1].b);
    glRectf(0.0f, 1.0f, 1.0f, 0.0f);

    // 3영역
    glColor3f(areaColor[2].r, areaColor[2].g, areaColor[2].b);
    glRectf(-1.0f, 0.0f, 0.0f, -1.0f);

    // 4영역
    glColor3f(areaColor[3].r, areaColor[3].g, areaColor[3].b);
    glRectf(0.0f, 0.0f, 1.0f, -1.0f);

    // 각 분면 안에 작은 사각형 그리는 것
    for (int area = 0; area < 4; ++area) {
        for (int i = 0; i < rectangleCount[area]; ++i) {
            glColor3f(rects[area][i].color.r, rects[area][i].color.g, rects[area][i].color.b);
            POINT p{ rects[area][i].point };
            glRectf(p.x1, p.y1, p.x2, p.y2);

            // 선택 표시
            if (rects[area][i].selected)
            {
                glColor3f(1, 0, 0);

                glLineWidth(5.0f);

                glBegin(GL_LINE_LOOP);

                glVertex2f(p.x1, p.y1);
                glVertex2f(p.x2, p.y1);
                glVertex2f(p.x2, p.y2);
                glVertex2f(p.x1, p.y2);

                glEnd();
            }
        }
    }

    // --------------------------------------------------------
}


POSISTION SetPosToGL(float x, float y)
{
    POSISTION p{};

    p.x = (x / wide) * 2 - 1;

    p.y = 1 - (y / height) * 2;

    return p;
}

COLOR randColor()
{
    COLOR c;
    c.r = colorDist(gen);
    c.g = colorDist(gen);
    c.b = colorDist(gen);
    
    return c;
}

void SetAreaColor()
{
    for (int i = 0; i < 4; ++i) {
        areaColor[i] = randColor();
    }
}

POSISTION AreaPos(int area)
{
    float centerX{};
    float centerY{};

    if (area == 0)
    {
        centerX = 400;
        centerY = 300;
    }
    else if (area == 1)
    {
        centerX = 1200;
        centerY = 300;
    }
    else if (area == 2)
    {
        centerX = 400;
        centerY = 900;
    }
    else if (area == 3)
    {
        centerX = 1200;
        centerY = 900;
    }

    POSISTION p{ centerX,centerY };

    p = SetPosToGL(p.x, p.y);

    return p;
}

void AddRect(int area)
{
    if (rectangleCount[area] >= 5)
    {
        return;
    }



    RECTANGLE& rect = rects[area][rectangleCount[area]];

    POSISTION p{ AreaPos(area) };
    
    float sizeX = sizeDist(gen);
    float sizeY = sizeDist(gen);

    rect.point.x1 = p.x - sizeX / 2;
    rect.point.y1 = p.y + sizeY / 2;
    rect.point.x2 = p.x + sizeX / 2;
    rect.point.y2 = p.y - sizeY / 2;

    rect.color = randColor();

    rect.selected = false;

    ++rectangleCount[area];

}

void SelectRect(GLFWwindow* window)
{
    double mouseX{};
    double mouseY{};

    glfwGetCursorPos(window, &mouseX, &mouseY);

    POSISTION mouse{ SetPosToGL(mouseX, mouseY) };

    for (int area = 0; area < 4; ++area)
    {
        for (int i = 0; i < rectangleCount[area]; ++i)
        {
            rects[area][i].selected = false;
        }
    }

    selectArea = -1;
    selectedRect = -1;

    for (int area = 0; area < 4; ++area)
    {
        for (int i = rectangleCount[area] - 1; i >= 0; --i)
        {
            RECTANGLE& rect = rects[area][i];

            if (mouse.x >= rect.point.x1 &&
                mouse.x <= rect.point.x2 &&
                mouse.y <= rect.point.y1 &&
                mouse.y >= rect.point.y2)
            {
                selectArea = area;
                selectedRect = i;

                rect.selected = true;

                return;
            }
        }
    }
}


void ChangeSize(float size)
{
    for (int area = 0; area < 4; ++area) {
        for (int i = 0; i < rectangleCount[area]; ++i) {
            if (rects[area][i].selected) {
                if (size == GLFW_KEY_KP_ADD || size == GLFW_KEY_EQUAL) {
                    rects[area][i].point.x1 -= 0.01f;
                    rects[area][i].point.y1 += 0.01f;
                    rects[area][i].point.x2 += 0.01f;
                    rects[area][i].point.y2 -= 0.01f;

                    return;
                }
                else if (size == GLFW_KEY_MINUS) {
                    rects[area][i].point.x1 += 0.01f;
                    rects[area][i].point.y1 -= 0.01f;
                    rects[area][i].point.x2 -= 0.01f;
                    rects[area][i].point.y2 += 0.01f;

                    return;
                }
            }
        }
    }
}


void ChangeColor()
{
    for (int area = 0; area < 4; ++area) {
        for (int i = 0; i < rectangleCount[area]; ++i) {
            if (rects[area][i].selected) {
                rects[area][i].color = randColor();
                return;
            }
        }
    }
}


void Reset()
{
    SetAreaColor();
    for (int area = 0; area < 4; ++area) {
        for (int i = 0; i < rectangleCount[area]; ++i) {
            if (rects[area][i].selected) {
                rects[area][i].selected = false;
            }
            rects[area][i].point.x1 = -1;
            rects[area][i].point.y1 = -1;
            rects[area][i].point.x2 = -1;
            rects[area][i].point.y2 = -1;
        }
    }

    for (int i = 0; i < 4; ++i) {
        rectangleCount[i] = 0;
    }

    selectArea = -1;
    selectedRect = -1;

}


bool KeyPressed(GLFWwindow* window, int key)
{
    static bool previousKey[GLFW_KEY_LAST + 1]{};

    bool currentKey = glfwGetKey(window, key) == GLFW_PRESS;

    bool result = currentKey && !previousKey[key];

    previousKey[key] = currentKey;

    return result;
}


bool MousePressed(GLFWwindow* window, int button)
{
    static bool MouseDown[4]{ false };

    bool current = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    bool result = current && !MouseDown[button];

    MouseDown[button] = current;

    return result;

}
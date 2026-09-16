#include <GL/glew.h>
#include <GL/glfw3.h>

#include <iostream>
#include <random>

// 랜덤
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> colorDist(0, 1.0f);

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
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // 1사분면
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        AddRect(1);
    }

    // 2사분면
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        AddRect(2);
    }

    // 3사분면
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        AddRect(3);
    }

    // 4사분면
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        AddRect(4);
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

    // 1. 사각형의 색상을 설정


    // 2. 사각형을 그리기
    // glRectf(x1, y1, x2, y2);


    // --------------------------------------------------------
}


POSISTION SetPosToGL(float x, float y)
{
    POSISTION p{};
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

void AddRect(int area)
{

}

void SelectRect(GLFWwindow* window)
{

}


void ChangeSize(float size)
{

}


void ChangeColor()
{

}


void Reset()
{

}


bool KeyPressed(GLFWwindow* window, int key)
{
    return true;
}


bool MousePressed(GLFWwindow* window, int button)
{
    return true;
}
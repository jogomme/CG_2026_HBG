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
std::uniform_real_distribution<float> positionDist(-0.8f, 0.8f);
std::uniform_real_distribution<float> wideDist(1600 * 0.2, 1600 * 0.8);
std::uniform_real_distribution<float> heightDist(1200 * 0.2, 1200 * 0.8);

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
    POINT point;
    COLOR color;

    bool selected{ false };
};


// ------------------------------------------------------------
// 전역 변수
// ------------------------------------------------------------

int wide{ 1600 };
int height{ 1200 };


// 최대 20개
RECTANGLE rects[20]{};

int rectangleCount{ 0 };


// 현재 선택된 사각형
int selectedRect{ -1 };


// 드래그 상태
bool isDragging{ false };


// 드래그 시작 당시 마우스 위치
POSITION mouseStart{};


// 드래그 시작 당시 사각형 위치
POINT rectStart{};


// ------------------------------------------------------------
// 함수 선언
// ------------------------------------------------------------

POSITION SetPosToGL(float x, float y);

void DrawScene();


// 사각형 생성
void AddRect();


// 사각형 선택
void SelectRect(GLFWwindow* window);


// 사각형 이동
void MoveRect(GLFWwindow* window);


// 겹침 확인
bool IsOverlap(RECTANGLE& rect1, RECTANGLE& rect2);


// 사각형 합치기
void MergeRect(int rect1, int rect2);


// 사각형 분리
void SplitRect(int index);


// 랜덤 색상
COLOR randColor();


// 마우스 좌표
POSITION GetMousePosition(GLFWwindow* window);


// 리셋
void Reset();


// ------------------------------------------------------------
// Callback 함수
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
            "OpenGL Practice 3",
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
    // 메인 루프
    // --------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        // ----------------------------------------------------
        // Polling
        // ----------------------------------------------------

        if (isDragging)
        {
            MoveRect(window);
        }


        // 화면 그리기
        DrawScene();


        glfwSwapBuffers(window);


        // 이벤트 처리
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
    // 사각형 그리기
    // --------------------------------------------------------

    for (int i = 0; i < rectangleCount; ++i)
    {
        glColor3f(
            rects[i].color.r,
            rects[i].color.g,
            rects[i].color.b
        );


        glRectf(
            rects[i].point.x1,
            rects[i].point.y1,
            rects[i].point.x2,
            rects[i].point.y2
        );


        // 선택 표시
        if (rects[i].selected)
        {
            // 직접 작성
        }
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
// 사각형 생성
// ------------------------------------------------------------

void AddRect()
{
    // 최대 20개인지 확인
    if (rectangleCount >= 20) {
        return;
    }

    RECTANGLE& rect = rects[rectangleCount];

    // 랜덤 위치
    
    float x1 = wideDist(gen);
    float y1 = heightDist(gen);

    float x2 = wideDist(gen);
    float y2 = heightDist(gen);

    if (x1 > x2) {
        float tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    if (y1 > y2) {
        float tmp = y1;
        y1 = y2;
        y2 = tmp;
    }


    POSITION p1 = SetPosToGL(x1, y1);
    POSITION p2 = SetPosToGL(x2, y2);

    rect.point.x1 = p1.x;
    rect.point.y1 = p1.y;
    rect.point.x2 = p2.x;
    rect.point.y2 = p2.y;

    // 랜덤 색상
    rect.color = randColor();

    // 개수 증가
    rectangleCount++;
}


// ------------------------------------------------------------
// 사각형 선택
// ------------------------------------------------------------

void SelectRect(GLFWwindow* window)
{
    // 마우스 위치 가져오기
    POSITION p = GetMousePosition(window);

    // 기존 선택 해제
    if (selectedRect != -1) {
        rects[selectedRect].selected = false;
        selectedRect = -1;
    }



    // 가장 나중에 만들어진 사각형부터 검사
    for (int i = rectangleCount - 1; i >= 0; --i) {
        if (rects[i].point.x1 < p.x && p.x < rects[i].point.x2 &&
            rects[i].point.y1 >= p.y && p.y >= rects[i].point.y2
            ) {
            selectedRect = i;
            rects[i].selected = true;
            return;
        }
    }
}


// ------------------------------------------------------------
// 사각형 이동
// ------------------------------------------------------------

void MoveRect(GLFWwindow* window)
{
    // 선택된 사각형이 있는지 확인
    if (selectedRect == -1) {
        return;
    }

    // 현재 마우스 위치 가져오기
    POSITION mp = GetMousePosition(window);

    // 현재 위치와 이전 위치의 차이 계산
    double dx = mp.x - mouseStart.x;
    double dy = mp.y - mouseStart.y;

    // 선택된 사각형 이동
    RECTANGLE& r = rects[selectedRect];

    r.point.x1 = rectStart.x1 + dx;
    r.point.y1 = rectStart.y1 + dy;

    r.point.x2 = rectStart.x2 + dx;
    r.point.y2 = rectStart.y2 + dy;

    // 다른 사각형과 겹쳤는지 확인
    for (int i = 0; i < rectangleCount; ++i)
    {
        if (i == selectedRect)
        {
            continue;
        }

        if (IsOverlap(r, rects[i]))
        {
            MergeRect(selectedRect, i);

            mouseStart = mp;
            rectStart = rects[selectedRect].point;
            return;
        }
    }

}


// ------------------------------------------------------------
// 겹침 확인
// ------------------------------------------------------------

bool IsOverlap(RECTANGLE& rect1, RECTANGLE& rect2)
{
    // 두 사각형이 겹치는지 확인
    if (rect1.point.x2 <= rect2.point.x1 || rect2.point.x2 <= rect1.point.x1 ||
        rect1.point.y2 >= rect2.point.y1 || rect2.point.y2 >= rect1.point.y1
        ) {
        return false;
    }

    return true;
}


// ------------------------------------------------------------
// 사각형 합치기
// ------------------------------------------------------------

void MergeRect(int rect1, int rect2)
{
    RECTANGLE &r1 = rects[rect1];
    RECTANGLE &r2 = rects[rect2];

    POINT p{};

    RECTANGLE mr{};

    mr.selected = true;

    int keep;
    int remove;

    if (rect1 < rect2)
    {
        keep = rect1;
        remove = rect2;
    }
    else
    {
        keep = rect2;
        remove = rect1;
    }

    // x 최소값
    if (r1.point.x1 < r2.point.x1) {
        p.x1 = r1.point.x1;
    }
    else {
        p.x1 = r2.point.x1;
    }

    // y 최소값
    if (r1.point.y1 > r2.point.y1) {
        p.y1 = r1.point.y1;
    }
    else {
        p.y1 = r2.point.y1;
    }

    // x 최대값
    if (r1.point.x2 < r2.point.x2) {
        p.x2 = r2.point.x2;
    }
    else {
        p.x2 = r1.point.x2;
    }

    // y 최대값
    if (r1.point.y2 > r2.point.y2) {
        p.y2 = r2.point.y2;
    }
    else {
        p.y2 = r1.point.y2;
    }

    // 큰 사각형으로 변경
    mr.point = p;

    // 랜덤 색상
    mr.color = randColor();
    
    rects[keep] = mr;


    // 하나의 사각형 삭제
    for (int i = remove; i < rectangleCount - 1; ++i)
    {
        rects[i] = rects[i + 1];
    }

    // rectangleCount 감소
    rectangleCount--;

    // 선택 정보 수정
    selectedRect = keep;

}


// ------------------------------------------------------------
// 사각형 분리
// ------------------------------------------------------------

void SplitRect(int index)
{
    // 현재 개수가 20개인지 확인


    // 기존 사각형 정보 저장


    // 기존 사각형 삭제


    // 사각형 2개 생성


    // 랜덤 크기


    // 랜덤 색상


    // 개수 증가


    // 선택 정보 수정
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


    // 윈도우 좌표
    // → OpenGL 좌표 변환
    position = SetPosToGL(mouseX, mouseY);

    return position;
}


// ------------------------------------------------------------
// 리셋
// ------------------------------------------------------------

void Reset()
{
    // 전체 사각형 삭제


    // 개수 초기화


    // 선택 초기화


    // 드래그 초기화
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


    // Q
    if (key == GLFW_KEY_Q)
    {
        // 프로그램 종료
    }


    // A
    if (key == GLFW_KEY_A)
    {
        // 사각형 생성
        AddRect();
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
        // 마우스를 눌렀을 때
        if (action == GLFW_PRESS)
        {
            // 사각형 선택
            SelectRect(window);
            if (selectedRect != -1) {
                // 드래그 시작
                isDragging = true;

                // 마우스 시작 위치 저장
                mouseStart = GetMousePosition(window);

                // 선택된 사각형의 시작 위치 저장
                rectStart = rects[selectedRect].point;
            }
        }


        // 마우스를 뗐을 때
        else if (action == GLFW_RELEASE)
        {
            // 드래그 종료
            isDragging = false;
        }
    }


    // --------------------------------------------------------
    // 오른쪽 마우스
    // --------------------------------------------------------

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            // 사각형 선택


            // 선택된 사각형 분리
        }
    }
}

POSITION SetPosToGL(float x, float y)
{
    POSITION p{};

    p.x = (x / wide) * 2 - 1;
    p.y = 1 - (y / height) * 2;

    return p;
}
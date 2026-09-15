#include <GL/glew.h>
#include <GL/glfw3.h>

#include <iostream>
#include <random>

// ------------------------------------------------------------
// 랜덤 변수
// ------------------------------------------------------------
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dist(0, 1.0f);


// ------------------------------------------------------------
// 전역 변수
// ------------------------------------------------------------

// 현재 배경색
float rColor = 1.0f;
float gColor = 1.0f;
float bColor = 1.0f;

// 타이머를 사용하는 중인가?
bool timerOn = false;

// 마지막으로 색을 바꾼 시간
double lastTime = 0.0;

// 몇 초마다 색을 바꿀 것인지
double timer = 1.0;



// ------------------------------------------------------------
// 함수 선언
// ------------------------------------------------------------

void InputProcess(GLFWwindow* window);

void DrawScene();

void RandomColor();


// ------------------------------------------------------------
// main
// ------------------------------------------------------------

int main()
{
    // --------------------------------------------------------
    // GLFW 초기화
    // --------------------------------------------------------
    if (!glfwInit())
    {
        std::cerr << "GLFW 초기화 실패!" << std::endl;

        return -1;
    }

    std::cout << "===== 명령어 목록 =====\n"; 
    std::cout << "c : 청록색 (초록 + 파랑)\n"; 
    std::cout << "m : 자홍색 (빨강 + 파랑)\n"; 
    std::cout << "y : 노란색 (빨강 + 초록)\n"; 
    std::cout << "a : 랜덤 색상\n"; 
    std::cout << "g : 회색\n"; 
    std::cout << "k : 검정색\n"; 
    std::cout << "t : 타이머 설정 (특정 시간마다 랜덤 색상으로 변경)\n"; 
    std::cout << "s : 타이머 종료\n"; 
    std::cout << "ESC : 프로그램 종료\n"; 
    std::cout << "=======================\n";
      
        
        
     

    // --------------------------------------------------------
    // OpenGL 버전 설정
    // --------------------------------------------------------

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // 실습 1에서는 강의자료의 기본 도형/기본 OpenGL
    // 함수를 사용할 수 있도록 Compatibility Profile 사용
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);


    // --------------------------------------------------------
    // 윈도우 생성
    // --------------------------------------------------------

    GLFWwindow* window =
        glfwCreateWindow(
            800,
            600,
            "OpenGL Practice 1",
            nullptr,
            nullptr);


    if (!window)
    {
        std::cerr << "윈도우 생성 실패!" << std::endl;

        glfwTerminate();

        return -1;
    }


    // --------------------------------------------------------
    // OpenGL Context 활성화
    // --------------------------------------------------------

    glfwMakeContextCurrent(window);


    // --------------------------------------------------------
    // GLEW 초기화
    // --------------------------------------------------------

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "GLEW 초기화 실패!" << std::endl;

        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }


    // --------------------------------------------------------
    // 뷰포트 설정
    // --------------------------------------------------------

    glViewport(0, 0, 800, 600);


    // --------------------------------------------------------
    // 메인 루프
    // --------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        // 키보드 입력
        InputProcess(window);


        // 화면 그리기
        DrawScene();


        // 버퍼 교체
        glfwSwapBuffers(window);


        // 이벤트 처리
        glfwPollEvents();
    }


    // --------------------------------------------------------
    // 종료
    // --------------------------------------------------------

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}


// ------------------------------------------------------------
// 키보드 입력 처리
// ------------------------------------------------------------

void InputProcess(GLFWwindow* window)
{
    // ESC
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        // 윈도우 종료'
        glfwSetWindowShouldClose(window, true);
    }


    // 청록색
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        rColor = 0;
        gColor = 1;
        bColor = 1;
    }


    // 자홍색
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        rColor = 1;
        gColor = 0;
        bColor = 1;
    }


    // 노랑색
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        rColor = 1;
        gColor = 1;
        bColor = 0;
    }


    // 랜덤색
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        RandomColor();
    }


    // 회색
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        rColor = 0.5;
        gColor = 0.5;
        bColor = 0.5;
    }


    // 검정색
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    {
        rColor = 0;
        gColor = 0;
        bColor = 0;
    }


    // 타이머 시작
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        timerOn = true;
        // 마지막 시간 저장
        lastTime = glfwGetTime();
    }


    // 타이머 종료
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        timerOn = false;
    }
}


// ------------------------------------------------------------
// 화면 그리기
// ------------------------------------------------------------

void DrawScene()
{
    // 타이머가 켜져 있을 때
    // 일정 시간이 지났는지 확인
    if (timerOn)
    {
        double currentTime = glfwGetTime();

        if ((currentTime - lastTime) >= timer) {
            RandomColor();
            lastTime = currentTime;
        }
    }


    // 현재 설정된 색으로 화면 지우기
    glClearColor(
        rColor,
        gColor,
        bColor,
        1.0f
    );

    glClear(GL_COLOR_BUFFER_BIT);
}


// ------------------------------------------------------------
// 랜덤 색상
// ------------------------------------------------------------

void RandomColor()
{
    rColor = dist(gen);
    gColor = dist(gen);
    bColor = dist(gen);
}

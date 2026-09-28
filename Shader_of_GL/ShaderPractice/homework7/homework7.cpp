#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>



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
    GLFWwindow* window = glfwCreateWindow(
        1600, 1200,
        "OpenGL Practice 7",
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cout << "Window 생성 실패\n";
        glfwTerminate();
        return -1;
    }

    // OpenGL Context 생성
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

    // 화면 크기 설정
    glViewport(0, 0, 1600, 1200);

    // --------------------------------------------------
    // 여기부터 실습 7 구현
    // --------------------------------------------------

    float vertexs[] =
    {
         -0.5f, -0.5f, 0.0f,
          0.5f, -0.5f, 0.0f,
          0.0f,  0.5f, 0.0f,
    };

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenVertexArrays(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexs), vertexs, GL_STATIC_DRAW);

    // --------------------------------------------------
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // 메인 루프
    while (!glfwWindowShouldClose(window))
    {
        // 화면 지우기
        glClear(GL_COLOR_BUFFER_BIT);

        // --------------------------------------------------
        // 여기에서 네가 그리기 구현
        // --------------------------------------------------

        

        // --------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 종료
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include<random>

//------------------------------------------------------------------------------------------
// 랜덤 엔진
//------------------------------------------------------------------------------------------
std::random_device rd;
std::mt19937 gen(rd());

std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);


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

struct SHAPE_TYPE
{
    int type;

    POINT point[4];

    COLOR color;

    bool selected{ false };
};

//------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------

void MakeVertexData(int index);

//------------------------------------------------------------------------------------------
// 전역 변수 
//------------------------------------------------------------------------------------------

// 0 : 점, 1 : 선, 2 : 삼각형, 3 : 사각형
const int shape_index[] =
{
    1, 2, 3, 4
};

SHAPE_TYPE types[50]{};
int ShapeCount{ 0 };

int selectedRect{ -1 };
bool isDragging{ false };

GLuint VAO;
GLuint VBO;


//------------------------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------------------------
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

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexs), vertexs, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);

    const char* vertexShaderSource = R"(
    #version 330 core

    layout (location = 0) in vec3 aPos;

    void main()
    {
        gl_Position = vec4(aPos, 1.0);
    }
    
    )";

    const char* fragmentShaderSource = R"(
    #version 330 core
    
    in vec3 ourColor
    
    void main()
    {
        FragColor = vec4(ourColor, 1.0);
    }
    )";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);

    glCompileShader(vertexShader);



    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);

    glCompileShader(fragmentShader);


    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);

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

        glBindVertexArray(VAO);

        for (int i = 0; i < ShapeCount; ++i)
        {
            MakeVertexData(i);

            int type = types[i].type;
            int count = shape_index[type];

            if (type == 0)
            {
                glDrawArrays(GL_POINTS, 0, count);
            }
            else if (type == 1)
            {
                glDrawArrays(GL_LINES, 0, count);
            }
            else if (type == 2)
            {
                glDrawArrays(GL_TRIANGLES, 0, count);
            }
            else if (type == 3)
            {
                glDrawArrays(GL_TRIANGLE_FAN, 0, count);
            }
        }

        // --------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 종료
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

//------------------------------------------------------------------------------------------
// 함수 구현
//------------------------------------------------------------------------------------------

void MakeVertexData(int index)
{
    int type = types[index].type;
    int count = shape_index[type];

    float vertexData[24]{};

    for (int i = 0; i < count; ++i) {
        vertexData[i * 6] = types[index].point[i].x;
        vertexData[i * 6 + 1] = types[index].point[i].y;
        vertexData[i * 6 + 2] = types[index].point[i].z;

        vertexData[i * 6 + 3] = types[index].color.r;
        vertexData[i * 6 + 4] = types[index].color.g;
        vertexData[i * 6 + 5] = types[index].color.b;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        count * 6 * sizeof(float),
        vertexData,
        GL_DYNAMIC_DRAW
    );
}
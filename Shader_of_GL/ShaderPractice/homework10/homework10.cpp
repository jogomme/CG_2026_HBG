#include <GL/glew.h>
#include <GL/glfw3.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>


#include <iostream>
#include <random>

//------------------------------------------------------------------------------------------------------
// 랜덤 엔진
//------------------------------------------------------------------------------------------------------
std::random_device rd;
std::mt19937 gen(rd());

std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);
std::uniform_real_distribution<float> posDist(-1.0f, 1.0f);

//------------------------------------------------------------------------------------------------------
// 구조체 선언
//------------------------------------------------------------------------------------------------------

struct ShapeType {
    // 0 : 사각형
    // 1 : 삼각형
    // 2 : 직각 삼각형
    int type;
    
    // 오른쪽 타일이랑 왼쪽 내가 끌고가는게 같아야 함
    int MatchType;

    // vertexs 의 몇 번째 도형을 사용할 지에 관한 내용
    int vertexIndex;

    // 정점의 중점 데이터
    glm::vec2 position;

    // 정점의 color 데이터
    glm::vec3 color;

    // 선택 된 것인지 확인
    bool selected{ false };

    // 완료 된 것인지 확인
    bool completed{ false };

};


//------------------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------------------

void MouseButtonCallback(GLFWwindow* window,int button,int action, int mods );

void KeyCallback(GLFWwindow* window,int key,int scancode,int action,int mods );

// 시작 함수
void StartMap(ShapeType ls[], ShapeType rs[]);

glm::mat4 MakeModelMatrix(ShapeType& shape);

void DrawScenes(ShapeType& shape, GLuint modelLocation);

void MakeVertexData(ShapeType& shape);

//------------------------------------------------------------------------------------------------------
// 정점의 위치 데이터
//------------------------------------------------------------------------------------------------------

glm::vec2 vertexs[20][4] = {
    // ------------------------------------------------------------
    // 위쪽 모양판 : 직사각형 4개
    // ------------------------------------------------------------

    // 0
    {
        {-0.08f,  0.06f},
        { 0.08f,  0.06f},
        { 0.08f, -0.06f},
        {-0.08f, -0.06f}
    },

    // 1
    {
        {-0.10f,  0.07f},
        { 0.10f,  0.07f},
        { 0.10f, -0.07f},
        {-0.10f, -0.07f}
    },

    // 2
    {
        {-0.12f,  0.08f},
        { 0.12f,  0.08f},
        { 0.12f, -0.08f},
        {-0.12f, -0.08f}
    },

    // 3
    {
        {-0.14f,  0.09f},
        { 0.14f,  0.09f},
        { 0.14f, -0.09f},
        {-0.14f, -0.09f}
    },


    // ------------------------------------------------------------
    // 가운데 모양판 : 삼각형 4개 + 가운데 사각형
    // ------------------------------------------------------------

    // 4 : 위쪽 삼각형
    {
        {-0.10f, -0.02f},
        { 0.10f, -0.02f},
        { 0.00f,  0.15f},
        { 0.00f,  0.15f}
    },

    // 5 : 오른쪽 삼각형
    {
        {-0.02f,  0.10f},
        {-0.02f, -0.10f},
        { 0.15f,  0.00f},
        { 0.15f,  0.00f}
    },

    // 6 : 아래쪽 삼각형
    {
        {-0.10f,  0.02f},
        { 0.10f,  0.02f},
        { 0.00f, -0.15f},
        { 0.00f, -0.15f}
    },

    // 7 : 왼쪽 삼각형
    {
        { 0.02f,  0.10f},
        { 0.02f, -0.10f},
        {-0.15f,  0.00f},
        {-0.15f,  0.00f}
    },

    // 8 : 가운데 사각형
    {
        {-0.05f,  0.05f},
        { 0.05f,  0.05f},
        { 0.05f, -0.05f},
        {-0.05f, -0.05f}
    },


    // ------------------------------------------------------------
    // 아래쪽 모양판 : 직각삼각형 2개
    // ------------------------------------------------------------

    // 9 : 왼쪽 직각삼각형
    {
        {-0.12f,  0.20f},
        { 0.12f,  0.20f},
        {-0.12f, -0.20f},
        {-0.12f, -0.20f}
    },

    // 10 : 오른쪽 직각삼각형
    {
        { 0.12f,  0.20f},
        { 0.12f, -0.20f},
        {-0.12f, -0.20f},
        {-0.12f, -0.20f}
    },


    // ------------------------------------------------------------
    // 사용자 정의 모형
    // ------------------------------------------------------------

    // 11
    {
        {-0.08f,  0.20f},
        { 0.08f,  0.20f},
        { 0.08f, -0.20f},
        {-0.08f, -0.20f}
    },

    // 12
    {
        {-0.20f,  0.08f},
        { 0.20f,  0.08f},
        { 0.20f, -0.08f},
        {-0.20f, -0.08f}
    },

    // 13 : 정삼각형
    {
        { 0.00f,  0.20f},
        {-0.18f, -0.10f},
        { 0.18f, -0.10f},
        { 0.18f, -0.10f}
    },

    // 14 : 역삼각형
    {
        { 0.00f, -0.20f},
        {-0.18f,  0.10f},
        { 0.18f,  0.10f},
        { 0.18f,  0.10f}
    },

    // 15 : 오른쪽 삼각형
    {
        {-0.15f,  0.10f},
        {-0.15f, -0.10f},
        { 0.18f,  0.00f},
        { 0.18f,  0.00f}
    },

    // 16 : 왼쪽 삼각형
    {
        { 0.15f,  0.10f},
        { 0.15f, -0.10f},
        {-0.18f,  0.00f},
        {-0.18f,  0.00f}
    },

    // 17 : 마름모
    {
        { 0.00f,  0.18f},
        { 0.18f,  0.00f},
        { 0.00f, -0.18f},
        {-0.18f,  0.00f}
    },

    // 18 : 평행사변형
    {
        {-0.15f,  0.10f},
        { 0.10f,  0.10f},
        { 0.15f, -0.10f},
        {-0.10f, -0.10f}
    },

    // 19 : 사다리꼴
    {
        {-0.10f,  0.15f},
        { 0.10f,  0.15f},
        { 0.18f, -0.15f},
        {-0.18f, -0.15f}
    }
};

//------------------------------------------------------------------------------------------------------
// 전역 변수
//------------------------------------------------------------------------------------------------------

int wide = 1600;
int height = 1200;

int mid = wide / 2;

// 타입에 따른 정점 갯수
// 사각형 0 -> 4
// 삼각형 1 -> 3
// 직각 삼각형 2 -> 3
int vertexCount[3]{ 4, 3, 3 };

GLuint VAO;
GLuint VBO;

// 왼쪽 도형 12개
ShapeType LS[12];

// 오른쪽 도형 12개
ShapeType RS[12];

//------------------------------------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------------------------------------
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
        wide, height,
        "OpenGL Practice 10",
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
    glViewport(0, 0, wide, height);

    // 마우스, 키 콜 백 함수
    glfwSetMouseButtonCallback(window,MouseButtonCallback);

    glfwSetKeyCallback(window,KeyCallback);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * 6 * 4,
        nullptr,
        GL_DYNAMIC_DRAW
    );

    // 위치
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // 색상
    glVertexAttribPointer( 1, 3, GL_FLOAT,GL_FALSE,6 * sizeof(float),(void*)(3 * sizeof(float)) );

    glEnableVertexAttribArray(1);

    //------------------------------------------------------------------------------------------
    // Vertex Shader
    //------------------------------------------------------------------------------------------
    const char* vertexShaderSource = R"(
    #version 330 core
    
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    
    uniform mat4 model;
    
    out vec3 ourColor;
    
    void main()
    {
        gl_Position = model * vec4(aPos, 1.0);
        ourColor = aColor;
    }
    )";

    //------------------------------------------------------------------------------------------
    // Fragment Shader
    //------------------------------------------------------------------------------------------
    const char* fragmentShaderSource = R"(
    #version 330 core

    in vec3 ourColor;

    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(ourColor, 1.0);
    }
    )";

    //------------------------------------------------------------------------------------------
    // Vertex Shader 생성
    //------------------------------------------------------------------------------------------
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        vertexShader,
        1,
        &vertexShaderSource,
        nullptr
    );

    glCompileShader(vertexShader);

    //------------------------------------------------------------------------------------------
    // Fragment Shader 생성
    //------------------------------------------------------------------------------------------
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        fragmentShader,
        1,
        &fragmentShaderSource,
        nullptr
    );

    glCompileShader(fragmentShader);

    //------------------------------------------------------------------------------------------
    // Shader Program 생성
    //------------------------------------------------------------------------------------------
    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);

    glClearColor(
        1.0f,
        1.0f,
        1.0f,
        1.0f
    );

    GLuint modelLocation = glGetUniformLocation(shaderProgram, "model");





    StartMap(LS, RS);

    //------------------------------------------------------------------------------------------
    // 메인 루프
    //------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // 화면 지우기
        glClear(GL_COLOR_BUFFER_BIT);


        glBindVertexArray(VAO);

        DrawScenes(LS[0], modelLocation);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 종료
    glfwDestroyWindow(window);
    glfwTerminate();

}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{

}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

void StartMap(ShapeType ls[], ShapeType rs[])
{
    ls[0].type = 0;
    ls[0].vertexIndex = 0;
    ls[0].position = glm::vec2(-0.5f, 0.0f);
    ls[0].color.r = colorDist(gen);
    ls[0].color.g = colorDist(gen);
    ls[0].color.b = colorDist(gen);


}

glm::mat4 MakeModelMatrix(ShapeType& shape)
{
    glm::mat4 model(1.0f);

    model = glm::translate(model, glm::vec3(shape.position, 0.0f));

    return model;
}

void DrawScenes(ShapeType& shape, GLuint modelLocation)
{
    MakeVertexData(shape);

    glm::mat4 model = MakeModelMatrix(shape);

    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);

    glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount[shape.type]);
}

void MakeVertexData(ShapeType& shape)
{
    float vertexData[24]{};

    int count = vertexCount[shape.type];

    for (int i = 0; i < count; ++i)
    {
        vertexData[i * 6 + 0] = vertexs[shape.vertexIndex][i].x;
        vertexData[i * 6 + 1] = vertexs[shape.vertexIndex][i].y;
        vertexData[i * 6 + 2] = 0.0f;

        vertexData[i * 6 + 3] = shape.color.r;
        vertexData[i * 6 + 4] = shape.color.g;
        vertexData[i * 6 + 5] = shape.color.b;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(float) * 6 * count,
        vertexData
    );
}
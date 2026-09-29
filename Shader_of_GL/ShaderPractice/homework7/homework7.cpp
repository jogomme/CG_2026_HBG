#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <random>
#include <cmath>

//------------------------------------------------------------------------------------------
// 랜덤 엔진
//------------------------------------------------------------------------------------------
std::random_device rd;
std::mt19937 gen(rd());

std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);
std::uniform_real_distribution<float> positionDist(-0.8f, 0.8f);
std::uniform_real_distribution<float> sizeDist(0.15f, 0.35f);


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

void MakeVertexData(int index, bool isOutline);

void AddShape(int type);

POINT SetPosToGL(float x, float y);

POINT GetMousePosition(GLFWwindow* window);

bool isin(POINT mouse, SHAPE_TYPE& shape);

void MouseButtonCallback(
    GLFWwindow* window,
    int button,
    int action,
    int mods
);

void KeyCallback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods
);

POINT centerShape(int index);

//------------------------------------------------------------------------------------------
// 전역 변수
//------------------------------------------------------------------------------------------

int wide = 1600;
int height = 1200;

// 0 : 점, 1 : 선, 2 : 삼각형, 3 : 사각형
const int shape_index[] =
{
    1, 2, 3, 4
};

SHAPE_TYPE types[50]{};
int ShapeCount{ 0 };

int selectedRect{ -1 };

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
        wide, height,
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
    glViewport(0, 0, wide, height);

    // 마우스, 키 콜 백 함수
    glfwSetMouseButtonCallback(
        window,
        MouseButtonCallback
    );

    glfwSetKeyCallback(
        window,
        KeyCallback
    );

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

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertexs),
        vertexs,
        GL_STATIC_DRAW
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
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);

    //------------------------------------------------------------------------------------------
    // Vertex Shader
    //------------------------------------------------------------------------------------------
    const char* vertexShaderSource = R"(
    #version 330 core

    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;

    out vec3 ourColor;

    void main()
    {
        gl_Position = vec4(aPos, 1.0);
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

    // --------------------------------------------------

    glClearColor(
        1.0f,
        1.0f,
        1.0f,
        1.0f
    );

    // 테스트용 도형 생성
    AddShape(0);
    AddShape(1);
    AddShape(2);
    AddShape(3);

    // 테스트
    // types[0].selected = true;

    //------------------------------------------------------------------------------------------
    // 메인 루프
    //------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // 화면 지우기
        glClear(GL_COLOR_BUFFER_BIT);


        glBindVertexArray(VAO);

        // 도형 그리기
        for (int i = 0; i < ShapeCount; ++i)
        {
            int type = types[i].type;

            float speed = 0.001f;

            POINT oldP[4]{};

            for (int j = 0; j < shape_index[type]; ++j)
            {
                oldP[j] = types[i].point[j];
            }

            // 이동 - WASD, IJLK
            if (types[i].selected) {

                //W
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                    for (int j = 0; j < shape_index[type]; ++j) {
                        types[i].point[j].y += speed;
                    }
                }
                //A
                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                    for (int j = 0; j < shape_index[type]; ++j) {
                        types[i].point[j].x -= speed;
                    }
                }
                //S
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                    for (int j = 0; j < shape_index[type]; ++j) {
                        types[i].point[j].y -= speed;
                    }
                }
                //D
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                    for (int j = 0; j < shape_index[type]; ++j) {
                        types[i].point[j].x += speed;
                    }
                }
                // I
                if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
                {
                    for (int j = 0; j < shape_index[type]; ++j)
                    {
                        types[i].point[j].y += speed;
                        types[i].point[j].x -= speed;
                    }
                }
                // J
                if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
                {
                    for (int j = 0; j < shape_index[type]; ++j)
                    {
                        types[i].point[j].x += speed;
                        types[i].point[j].y += speed;
                    }
                }
                //K
                if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
                {
                    for (int j = 0; j < shape_index[type]; ++j)
                    {
                        types[i].point[j].y -= speed;
                        types[i].point[j].x -= speed;
                    }
                }
                //L
                if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
                {
                    for (int j = 0; j < shape_index[type]; ++j)
                    {
                        types[i].point[j].x += speed;
                        types[i].point[j].y -= speed;
                    }
                }
            }


            if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            {
                for (int j = 0; j < shape_index[type]; ++j)
                {
                    types[i].point[j].y += speed;
                }
            }

            if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            {
                for (int j = 0; j < shape_index[type]; ++j)
                {
                    types[i].point[j].x -= speed;
                }
            }

            if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
            {
                for (int j = 0; j < shape_index[type]; ++j)
                {
                    types[i].point[j].y -= speed;
                }
            }

            if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
            {
                for (int j = 0; j < shape_index[type]; ++j)
                {
                    types[i].point[j].x += speed;
                }
            }

            POINT pc = centerShape(i);

            if (pc.x > 1.0f || pc.x < -1.0f || pc.y > 1.0f || pc.y < -1.0f) {
                for (int j = 0; j < shape_index[type]; ++j)
                {
                    types[i].point[j] = oldP[j];
                }
            }

            // 도형 본체 그리기
            {
                //----------------------------------------------------------------------------------
                // 점
                //----------------------------------------------------------------------------------
                if (type == 0)
                {
                    // 윤곽선 색
                    MakeVertexData(i, true);

                    glPointSize(12.0f);
                    glDrawArrays(GL_POINTS, 0, 1);

                    // 본체 색
                    MakeVertexData(i, false);

                    glPointSize(7.0f);
                    glDrawArrays(GL_POINTS, 0, 1);
                }

                //----------------------------------------------------------------------------------
                // 선
                //----------------------------------------------------------------------------------
                else if (type == 1)
                {
                    // 윤곽선 색
                    MakeVertexData(i, true);

                    glLineWidth(7.0f);
                    glDrawArrays(GL_LINES, 0, 2);

                    // 본체 색
                    MakeVertexData(i, false);

                    glLineWidth(3.0f);
                    glDrawArrays(GL_LINES, 0, 2);
                }

                //----------------------------------------------------------------------------------
                // 삼각형
                //----------------------------------------------------------------------------------
                else if (type == 2)
                {
                    // 본체
                    MakeVertexData(i, false);

                    glDrawArrays(GL_TRIANGLES, 0, 3);

                    // 윤곽선
                    MakeVertexData(i, true);

                    glLineWidth(3.0f);
                    glDrawArrays(GL_LINE_LOOP, 0, 3);
                }

                //----------------------------------------------------------------------------------
                // 사각형
                //----------------------------------------------------------------------------------
                else if (type == 3)
                {
                    // 본체
                    MakeVertexData(i, false);

                    glDrawArrays(GL_TRIANGLES, 0, 6);

                    // 윤곽선
                    MakeVertexData(i, true);

                    // 바깥 테두리
                    glLineWidth(3.0f);
                    glDrawArrays(GL_LINE_LOOP, 0, 4);

                    // 대각선
                    glDrawArrays(GL_LINES, 4, 2);
                }
            }
        }

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

void MakeVertexData(int index, bool isOutline)
{
    int type = types[index].type;
    int count = shape_index[type];

    // 최대 6개의 정점
    // 정점 하나당 위치 3 + 색상 3
    float vertexData[36]{};

    //--------------------------------------------------------------------------------------
    // 사각형
    //--------------------------------------------------------------------------------------
    if (type == 3)
    {
        if (isOutline)
        {
            // 바깥 테두리용
            // 0 -> 1 -> 2 -> 3

            for (int i = 0; i < 4; ++i)
            {
                vertexData[i * 6] =
                    types[index].point[i].x;

                vertexData[i * 6 + 1] =
                    types[index].point[i].y;

                vertexData[i * 6 + 2] =
                    types[index].point[i].z;
            }

            // 대각선용
            // 0 -> 2
            vertexData[4 * 6] =
                types[index].point[0].x;

            vertexData[4 * 6 + 1] =
                types[index].point[0].y;

            vertexData[4 * 6 + 2] =
                types[index].point[0].z;

            vertexData[5 * 6] =
                types[index].point[2].x;

            vertexData[5 * 6 + 1] =
                types[index].point[2].y;

            vertexData[5 * 6 + 2] =
                types[index].point[2].z;

            count = 6;
        }
        else
        {
            // 사각형을 두 개의 삼각형으로 만들기

            int triangleIndex[] =
            {
                0, 1, 2,
                0, 2, 3
            };

            for (int i = 0; i < 6; ++i)
            {
                int pointIndex = triangleIndex[i];

                vertexData[i * 6] =
                    types[index].point[pointIndex].x;

                vertexData[i * 6 + 1] =
                    types[index].point[pointIndex].y;

                vertexData[i * 6 + 2] =
                    types[index].point[pointIndex].z;
            }

            count = 6;
        }
    }

    //--------------------------------------------------------------------------------------
    // 점, 선, 삼각형
    //--------------------------------------------------------------------------------------
    else
    {
        for (int i = 0; i < count; ++i)
        {
            vertexData[i * 6] =
                types[index].point[i].x;

            vertexData[i * 6 + 1] =
                types[index].point[i].y;

            vertexData[i * 6 + 2] =
                types[index].point[i].z;
        }
    }

    //--------------------------------------------------------------------------------------
    // 색상 설정
    //--------------------------------------------------------------------------------------

    for (int i = 0; i < count; ++i)
    {
        if (isOutline)
        {
            if (types[index].selected)
            {
                // 선택됨 -> 빨간색
                vertexData[i * 6 + 3] = 1.0f;
                vertexData[i * 6 + 4] = 0.0f;
                vertexData[i * 6 + 5] = 0.0f;
            }
            else
            {
                // 선택 안 됨 -> 검은색
                vertexData[i * 6 + 3] = 0.0f;
                vertexData[i * 6 + 4] = 0.0f;
                vertexData[i * 6 + 5] = 0.0f;
            }
        }
        else
        {
            // 본체 -> 원래 색
            vertexData[i * 6 + 3] =
                types[index].color.r;

            vertexData[i * 6 + 4] =
                types[index].color.g;

            vertexData[i * 6 + 5] =
                types[index].color.b;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        count * 6 * sizeof(float),
        vertexData,
        GL_DYNAMIC_DRAW
    );
}


//------------------------------------------------------------------------------------------

void AddShape(int type)
{
    if (ShapeCount >= 50)
    {
        std::cout << "갯수 초과 " << '\n';
        return;
    }

    SHAPE_TYPE& shape = types[ShapeCount];

    shape.type = type;

    // 랜덤 색상
    shape.color.r = colorDist(gen);
    shape.color.g = colorDist(gen);
    shape.color.b = colorDist(gen);

    //--------------------------------------------------------------------------------------
    // 도형별 정점 저장
    //--------------------------------------------------------------------------------------

    if (type == 0)
    {
        // 점
        shape.point[0].x =
            positionDist(gen);

        shape.point[0].y =
            positionDist(gen);
    }

    else if (type == 1)
    {
        // 선
        for (int i = 0; i < 2; ++i)
        {
            shape.point[i].x =
                positionDist(gen);

            shape.point[i].y =
                positionDist(gen);
        }
    }

    else if (type == 2)
    {
        // 삼각형
        float x = positionDist(gen);
        float y = positionDist(gen);

        float size = sizeDist(gen);

        POINT center;
        center.x = x;
        center.y = y;

        float angle = 3.14159265f / 2.0f;

        // 첫 번째 점
        shape.point[0].x =
            center.x + cos(angle) * size;

        shape.point[0].y =
            center.y + sin(angle) * size;

        // 두 번째 점
        angle += 2.0f * 3.14159265f / 3.0f;

        shape.point[1].x =
            center.x + cos(angle) * size;

        shape.point[1].y =
            center.y + sin(angle) * size;

        // 세 번째 점
        angle += 2.0f * 3.14159265f / 3.0f;

        shape.point[2].x =
            center.x + cos(angle) * size;

        shape.point[2].y =
            center.y + sin(angle) * size;
    }

    else if (type == 3)
    {
        // 사각형의 대각선 양 끝점
        float x1 = positionDist(gen);
        float y1 = positionDist(gen);

        float x2 = positionDist(gen);
        float y2 = positionDist(gen);

        // 좌표 순서 정리
        if (x1 > x2)
        {
            float temp = x1;
            x1 = x2;
            x2 = temp;
        }

        if (y1 > y2)
        {
            float temp = y1;
            y1 = y2;
            y2 = temp;
        }

        // 사각형의 네 꼭짓점
        shape.point[0] =
        { x1, y1, 0.0f };

        shape.point[1] =
        { x2, y1, 0.0f };

        shape.point[2] =
        { x2, y2, 0.0f };

        shape.point[3] =
        { x1, y2, 0.0f };
    }

    ShapeCount++;
}

POINT SetPosToGL(float x, float y)
{
    POINT p{};

    // x
    p.x = (x / wide) * 2 - 1;

    // y
    p.y = 1 - (y / height) * 2;

    return p;
}

POINT GetMousePosition(GLFWwindow* window)
{
    POINT position{};

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

void MouseButtonCallback( GLFWwindow* window, int button, int action, int mods )
{
    if (button == GLFW_MOUSE_BUTTON_LEFT &&
        action == GLFW_PRESS)
    {
        POINT mouse = GetMousePosition(window);

        for (int i = 0; i < ShapeCount; ++i)
        {
            if (isin(mouse, types[i]))
            {
                for (int j = 0; j < ShapeCount; ++j) {
                    types[j].selected = false;
                }
                types[i].selected = true;
            }
        }
        std::cout
            << "x : " << mouse.x
            << ", y : " << mouse.y
            << '\n';
    }
}

bool isin(POINT mouse, SHAPE_TYPE& shape)
{
    int type = shape.type;



    if (type == 0) {
        float x = shape.point[0].x;
        float y = shape.point[0].y;

        float offsetX = 10.0f / wide * 2.0f;
        float offsetY = 10.0f / height * 2.0f;

        if (mouse.x >= x - offsetX &&
            mouse.x <= x + offsetX &&
            mouse.y >= y - offsetY &&
            mouse.y <= y + offsetY)
        {
            return true;
        }
    }
    else if (type == 1) {
        float x1 = shape.point[0].x;
        float y1 = shape.point[0].y;

        float x2 = shape.point[1].x;
        float y2 = shape.point[1].y;

        float dx = x2 - x1;
        float dy = y2 - y1;

        float length = dx * dx + dy * dy;

        // 선의 길이가 0인 경우
        if (length == 0)
        {
            return false;
        }

        // 선 위에서 마우스와 가장 가까운 위치
        float t =
            ((mouse.x - x1) * dx +
                (mouse.y - y1) * dy) / length;

        // 선의 양 끝 바깥으로 나간 경우
        if (t < 0.0f)
        {
            t = 0.0f;
        }
        else if (t > 1.0f)
        {
            t = 1.0f;
        }

        // 가장 가까운 점
        float closeX = x1 + t * dx;
        float closeY = y1 + t * dy;

        // 마우스와 가장 가까운 점의 거리
        float distanceX = mouse.x - closeX;
        float distanceY = mouse.y - closeY;

        float distance =
            sqrt(distanceX * distanceX +
                distanceY * distanceY);

        // 이 정도 가까우면 선택
        float offset = 0.02f;

        if (distance <= offset)
        {
            return true;
        }
    }
    else if (type == 2) {

        POINT A = shape.point[0];
        POINT B = shape.point[1];
        POINT C = shape.point[2];

        // 세 변에 대해 마우스가 어느 쪽에 있는지 계산
        float d1 =
            (mouse.x - B.x) * (A.y - B.y) -
            (A.x - B.x) * (mouse.y - B.y);

        float d2 =
            (mouse.x - C.x) * (B.y - C.y) -
            (B.x - C.x) * (mouse.y - C.y);

        float d3 =
            (mouse.x - A.x) * (C.y - A.y) -
            (C.x - A.x) * (mouse.y - A.y);

        // 세 값의 부호가 모두 같으면 삼각형 내부
        if ((d1 >= 0 && d2 >= 0 && d3 >= 0) ||
            (d1 <= 0 && d2 <= 0 && d3 <= 0))
        {
            return true;
        }

    }
    else if (type == 3) {

        float minX = shape.point[0].x;
        float maxX = shape.point[2].x;

        float minY = shape.point[0].y;
        float maxY = shape.point[2].y;

        if (mouse.x >= minX &&
            mouse.x <= maxX &&
            mouse.y >= minY &&
            mouse.y <= maxY)
        {
            return true;
        }

    }

    return false;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
    {
        return;
    }

    // Q를 누르면 프로그램 종료
    if (key == GLFW_KEY_Q)
    {
        glfwSetWindowShouldClose(
            window,
            GLFW_TRUE
        );
    }
    else if (key == GLFW_KEY_P)
    {
        AddShape(0);
    }
    else if (key == GLFW_KEY_E)
    {
        AddShape(1);
    }
    else if (key == GLFW_KEY_T)
    {
        // T
        AddShape(2);
    }
    else if (key == GLFW_KEY_R)
    {
        // R
        AddShape(3);
    }
    else if (key == GLFW_KEY_C)
    {
        // C
        ShapeCount = 0;
    }
}

POINT centerShape(int index)
{
    SHAPE_TYPE& type = types[index];

    POINT p{};

    for (int i = 0; i < shape_index[type.type]; ++i) {
        p.x += type.point[i].x;
        p.y += type.point[i].y;
    }

    p.x = p.x / shape_index[type.type];
    p.y = p.y / shape_index[type.type];

    return p;

}
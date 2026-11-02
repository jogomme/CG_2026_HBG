// 읽는 순서: 아래의 main에서 시작 → 창을 준비 → UpdateScene과 DrawAll을 반복합니다.
// UpdateScene은 물체를 조금 움직이고, DrawAll은 움직인 모습을 화면에 그립니다.
// KeyCallback은 키보드의 버튼을 눌렀을 때 실행되는 함수입니다.
//
// 좌표는 물체의 주소입니다. x는 좌우, y는 위아래, z는 앞뒤 방향입니다.
// glm::vec3는 숫자 3개를 묶은 상자입니다. 위치(x, y, z)나 색(빨강, 초록, 파랑)을 담습니다.
// glm::mat4는 이동·회전·크기 변경을 모아 둔 '변신 설명서'인 행렬입니다.
// glm::mat4(1)은 아직 아무것도 바꾸지 않는 설명서입니다.
// translate는 옮기기, rotate는 돌리기, scale은 크기 바꾸기입니다.
// GLM의 이 함수들은 기존 행렬 오른쪽에 새 변환을 곱합니다.
// 그래서 점에 적용할 때는 나중에 붙인 변환부터 먼저 작용합니다. 코드 순서를 바꾸면 모습도 달라집니다.
// radians는 우리가 쓰는 도(degree)를 회전 함수가 받는 각도 단위인 라디안으로 바꿉니다.
// sin과 cos는 원 위의 위치나 부드러운 왕복 움직임을 계산할 때 쓰는 함수입니다.
//
// deltaTime은 이번 화면과 지난 화면 사이의 시간(초)입니다.
// '초당 속도 × deltaTime'만큼 움직이면 화면을 그리는 횟수가 달라도 속도를 맞출 수 있습니다.
// fmod는 나머지를 구합니다. 각도를 360으로 나누면 한 바퀴를 넘은 부분부터 다시 셉니다.
// clamp는 숫자가 정해진 최솟값과 최댓값 밖으로 나가지 못하게 하는 울타리입니다.
// bool은 켜짐(true)/꺼짐(false)을 담습니다. !는 그 상태를 반대로 뒤집습니다.
// 배열의 번호는 0부터 시작합니다. 예를 들어 물체 2개는 [0], [1]에 들어 있습니다.
//
// Common/PracticeScene.h는 여러 과제가 함께 쓰는 준비 도구입니다.
// DrawObj는 '도형, 변신 설명서, 색, 셰이더의 행렬 칸, 셰이더'를 받아 그립니다.
// 셰이더는 그래픽 카드에서 점의 위치와 화면의 색을 계산하는 작은 프로그램입니다.
// common::과 practice::는 어떤 도구 묶음에 속한 이름인지 알려주는 표시입니다.

#include "Common/PracticeScene.h"

//------------------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------------------
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void UpdateScene(float deltaTime);
void UpdateKeyboard(float deltaTime);
void DrawAll(common::Shader& shader, int width, int height);
void ResetScene();
void StartSwap(int mode);
void MakeWireObjects();
glm::mat4 MakeAxisModelMatrix();
glm::mat4 MakeModelMatrix(int obj);

//------------------------------------------------------------------------------------------------------
// 전역 변수와 플래그
//------------------------------------------------------------------------------------------------------
// 키를 누르면 true, 떼면 false가 됩니다. 키보드의 반복 입력 속도에 기대지 않습니다.
// 이전 과제의 isR/isL처럼 '누르고 있는 상태'를 기억하는 배열입니다.
bool keyDown[GLFW_KEY_LAST + 1]{};
// 두 물체의 위치입니다. 0번은 왼쪽, 1번은 오른쪽에서 시작합니다.
glm::vec3 position[2] = { {-0.55f, 0, 0}, {0.55f, 0, 0} };
// 자리 바꾸기 시작 순간의 위치를 따로 기억합니다. 이동 중인 position과 구분합니다.
glm::vec3 swapStart[2]{};
// 각 물체의 제자리 x축 회전 각도입니다.
float angleX[2]{};
// 각 물체의 제자리 y축 회전 각도입니다.
float angleY[2]{};
// 물체가 원점 둘레를 얼마나 돌았는지 나타내는 각도입니다.
float orbitAngle[2]{};
// 물체 자체의 크기 배율입니다. 1이면 원래 크기입니다.
float objScale[2] = {1, 1};
// 원점을 기준으로 늘리는 배율입니다. 크기와 원점까지의 거리가 함께 바뀝니다.
float originScale[2] = {1, 1};
// 1은 첫 물체, 2는 두 번째 물체, 3은 두 물체를 함께 고릅니다.
bool isMeshReady = false;
int selectedObj = 3;
// 0은 교환하지 않음, 1은 원점 경유, 2는 위아래 우회입니다.
int swapMode = 0;
// 교환 진행률입니다. 이름은 Time이지만 초가 아니라 0(시작)~1(완료)을 저장합니다.
float swapTime = 0.0f;
// 반복 애니메이션을 위한 각도입니다. 한 바퀴마다 같은 움직임이 반복됩니다.
float animationAngle = 0.0f;
// V 키 애니메이션을 켰는지 기억합니다.
bool isV = false;
// C 키로 선택한 도형 종류를 기억합니다.
bool isC = false;

//------------------------------------------------------------------------------------------------------
// 좌표축을 기울이는 설명서를 만듭니다. 돌린 행렬을 반환하므로 다른 물체도 같은 기울기를 쓸 수 있습니다.
//------------------------------------------------------------------------------------------------------
glm::mat4 MakeAxisModelMatrix()
{
    glm::mat4 model(1.0f);
    model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1, 0, 0));
    return glm::rotate(model, glm::radians(30.0f), glm::vec3(0, 1, 0));
}

//------------------------------------------------------------------------------------------------------
// obj 번호의 물체가 어디에, 얼마나 크게, 어떤 방향으로 보일지 정합니다.
// 실제 점에는 크기 변경 → 제자리 회전 → 이동 → 원점 확대 → 공전 → 축 기울기 순으로 작용합니다.
//------------------------------------------------------------------------------------------------------
glm::mat4 MakeModelMatrix(int obj)
{
    glm::mat4 model = MakeAxisModelMatrix();
    model = glm::rotate(model, glm::radians(orbitAngle[obj]), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(originScale[obj]));
    model = glm::translate(model, position[obj]);
    model = glm::rotate(model, glm::radians(angleX[obj]), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(angleY[obj]), glm::vec3(0, 1, 0));
    float size = objScale[obj];
    if (isV)
    {
        size *= 1.0f + (obj == 0 ? 0.4f : -0.4f) * sin(glm::radians(animationAngle));
    }
    return glm::scale(model, glm::vec3(0.35f * size));
}

//------------------------------------------------------------------------------------------------------
// 변수들을 처음 값으로 돌립니다. 장난감을 처음 놓았던 자리로 되돌리는 것과 같습니다.
//------------------------------------------------------------------------------------------------------
void ResetScene()
{
    // 초기화할 때 기억한 키도 놓아 줍니다. 다음에 다시 누르기 전까지는 멈춥니다.
    std::fill(std::begin(keyDown), std::end(keyDown), false);
    for (int i = 0; i < 2; ++i)
    {
        position[i] = glm::vec3(i == 0 ? -0.55f : 0.55f, 0, 0);
        orbitAngle[i] = 0.0f;
        angleY[i] = 0.0f;
        angleX[i] = 0.0f;
        originScale[i] = 1.0f;
        objScale[i] = 1.0f;
    }
    selectedObj = 3;
    swapMode = 0;
    animationAngle = 0.0f;
    swapTime = 0.0f;
    isC = false;
    isV = false;
}

//------------------------------------------------------------------------------------------------------
// 두 물체의 자리 바꾸기를 준비합니다. mode 1은 원점 경유, mode 2는 위아래 우회입니다.
// 지금의 공전과 원점 확대 결과를 위치·크기에 반영한 뒤, 그 위치를 출발점으로 기억합니다.
//------------------------------------------------------------------------------------------------------
void StartSwap(int mode)
{
    if (swapMode != 0)
    {
        return;
    }
    for (int i = 0; i < 2; ++i)
    {
        glm::mat4 orbit = glm::rotate(glm::mat4(1), glm::radians(orbitAngle[i]), glm::vec3(0, 1, 0));
        position[i] = glm::vec3(orbit * glm::vec4(position[i] * originScale[i], 1));
        orbitAngle[i] = 0;
        objScale[i] *= originScale[i];
        originScale[i] = 1;
        swapStart[i] = position[i];
    }
    isV = false;
    swapMode = mode;
    swapTime = 0;
}

//------------------------------------------------------------------------------------------------------
// 시간이 흐른 만큼 저장된 위치와 각도를 고칩니다. 여기서는 화면을 그리지 않습니다.
//------------------------------------------------------------------------------------------------------
// 누르고 있는 동안 '초당 속도 × 지난 시간'만큼 움직입니다.
// 예: 1초에 0.6만큼 가면 0.5초 동안에는 0.3만큼 갑니다.
void UpdateKeyboard(float deltaTime)
{
    if (swapMode != 0)
    {
        return;
    }

    bool shift = keyDown[GLFW_KEY_LEFT_SHIFT] || keyDown[GLFW_KEY_RIGHT_SHIFT];
    float direction = shift ? -1.0f : 1.0f;
    float rotation = direction * 90.0f * deltaTime;
    float movement = direction * 0.6f * deltaTime;
    // 크기 배율은 곱해서 바꿉니다. 누른 시간에 맞는 배율을 만들고, 축소는 그 역수를 씁니다.
    float scale = std::pow(1.8f, direction * deltaTime);

    for (int i = 0; i < 2; ++i)
    {
        if (selectedObj != 3 && selectedObj != i + 1)
        {
            continue;
        }
        if (keyDown[GLFW_KEY_X])
        {
            angleX[i] += rotation;
        }
        if (keyDown[GLFW_KEY_Y])
        {
            angleY[i] += rotation;
        }
        if (keyDown[GLFW_KEY_R])
        {
            orbitAngle[i] += rotation;
        }
        if (keyDown[GLFW_KEY_A])
        {
            objScale[i] = std::clamp(objScale[i] * scale, 0.1f, 4.0f);
        }
        if (keyDown[GLFW_KEY_B])
        {
            originScale[i] = std::clamp(originScale[i] * scale, 0.1f, 3.0f);
        }
        if (keyDown[GLFW_KEY_D])
        {
            position[i].x -= movement;
        }
        if (keyDown[GLFW_KEY_E])
        {
            position[i].y += movement;
        }
    }
}

void UpdateScene(float deltaTime)
{
    // 공통 코드가 도형을 준비한 다음, 17번에서만 PDF처럼 선 도형으로 바꿉니다.
    if (!isMeshReady)
    {
        MakeWireObjects();
        isMeshReady = true;
    }
    UpdateKeyboard(deltaTime);

    if (swapMode != 0)
    {
        // 초당 진행률 0.5만큼 나아갑니다. 약 2초에 완료하며 1을 넘지 않습니다.
        swapTime = std::min(1.0f, swapTime + deltaTime * 0.5f);
        for (int i = 0; i < 2; ++i)
        {
            // mix는 두 위치 사이를 이어 줍니다. 진행률 0이면 출발점, 1이면 목적지입니다.
            // 물체 번호가 0이면 1 - i는 1, 번호가 1이면 0이므로 서로의 출발점으로 갑니다.
            position[i] = glm::mix(swapStart[i], swapStart[1 - i], swapTime);
            if (swapMode == 1)
            {
                // 개별 이동 후에도 두 도형은 반드시 원점을 통과한다.
                // 절반 전에는 출발점에서 원점으로, 절반 뒤에는 원점에서 상대 출발점으로 갑니다.
                // 조건 ? 앞의 값 : 뒤의 값은 조건이 참이면 앞, 거짓이면 뒤를 고르는 표현입니다.
                position[i] = swapTime < 0.5f
                    ? glm::mix(swapStart[i], glm::vec3(0), swapTime * 2)
                    : glm::mix(glm::vec3(0), swapStart[1 - i], (swapTime - 0.5f) * 2);
            }
            // sin 값은 출발과 도착에서 0, 중간에서 1이 됩니다.
            // 첫 물체는 위쪽, 두 번째는 아래쪽으로 돌아서 다시 원래 높이에 도착합니다.
            if (swapMode == 2)
            {
                position[i].y += (i == 0 ? 0.45f : -0.45f) * sin(glm::radians(180.0f * swapTime));
            }
        }
        if (swapTime >= 1)
        {
            swapMode = 0;
        }
    }
    if (isV)
    {
        animationAngle = std::fmod(animationAngle + 90.0f * deltaTime, 360.0f);
        for (int i = 0; i < 2; ++i)
        {
            angleY[i] = std::fmod(angleY[i] + 90.0f * deltaTime, 360.0f);
            orbitAngle[i] = std::fmod(orbitAngle[i] + 45.0f * deltaTime, 360.0f);
        }
    }
}

//------------------------------------------------------------------------------------------------------
// 키를 누른 소식을 받습니다. key는 버튼 이름, action은 누름/반복/뗌 상태입니다.
// mods에는 Shift 같은 보조 키를 눌렀는지가 들어 있습니다. scancode는 여기서 사용하지 않습니다.
// PRESS는 처음 누름, REPEAT는 길게 눌러서 반복되는 입력입니다.
//------------------------------------------------------------------------------------------------------
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // 키를 뗐다는 소식도 먼저 기억해야 계속 움직이는 일이 생기지 않습니다.
    if (key >= 0 && key <= GLFW_KEY_LAST)
    {
        keyDown[key] = (action != GLFW_RELEASE);
    }

    // 종류 바꾸기나 시작/정지 버튼은 처음 누른 순간에만 한 번 실행합니다.
    // 길게 눌러 생기는 GLFW_REPEAT는 이동 계산에 쓰지 않습니다.
    if (action != GLFW_PRESS)
    {
        return;
    }

    if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_3)
    {
        selectedObj = key - GLFW_KEY_0;
    }
    else if (key == GLFW_KEY_S)
    {
        ResetScene();
    }
    else if (key == GLFW_KEY_T)
    {
        StartSwap(1);
    }
    else if (key == GLFW_KEY_U)
    {
        StartSwap(2);
    }
    else if (key == GLFW_KEY_C)
    {
        isC = !isC;
    }
    else if (key == GLFW_KEY_V && swapMode == 0)
    {
        isV = !isV;
    }
}

//------------------------------------------------------------------------------------------------------
// 저장된 위치와 각도로 이번 화면을 그립니다.
// model은 물체의 변환, view는 카메라의 시점, projection은 화면에 비추는 방법입니다.
// 원근 투영은 먼 물체를 작게 보이게 하고, 직각 투영은 거리에 따른 크기 감소가 없습니다.
//------------------------------------------------------------------------------------------------------
void DrawAll(common::Shader& shader, int width, int height)
{
    // 셰이더 안에서 model 행렬을 넣을 칸의 번호를 알아냅니다.
    GLint modelLocation = shader.uniformLocation("model");
    shader.setMatrix(
        shader.uniformLocation("view"),
        glm::lookAt(glm::vec3(0,0,3.5f), glm::vec3(0), glm::vec3(0,1,0))
    );
    shader.setMatrix(
        shader.uniformLocation("projection"),
        practice::MakeProjection(width, height, true, 1.5f)
    );
    practice::DrawObj(practice::axis, MakeAxisModelMatrix(), glm::vec3(1), modelLocation, shader);
    for (int i = 0; i < 2; ++i)
    {
        // isC가 꺼지면 0번은 원뿔/1번은 육면체, 켜지면 0번은 구/1번은 원뿔입니다.
        // &는 복사본 대신 이미 준비된 도형을 참조해서 쓴다는 뜻입니다.
        const practice::Mesh& mesh = isC
            ? (i == 0 ? practice::sphere : practice::cone) : (i == 0 ? practice::cone
            : practice::cube);
        glm::mat4 model = MakeModelMatrix(i);
        if (isC && i == 0)
        {
            model = glm::scale(model, glm::vec3(0.5f));
        }
        practice::DrawObj(mesh, model, i == 0
            ? glm::vec3(0.25f, 0.25f, 0.25f)
            : glm::vec3(0.15f, 0.45f, 0.9f), modelLocation, shader);
    }
}

//------------------------------------------------------------------------------------------------------
// 프로그램의 출발점입니다. RunScene에 키 입력·움직임·그리기 함수를 맡깁니다.
// RunScene 안에서 창 생성, 반복 그리기, 사용한 그래픽 자원 정리를 처리합니다.
//------------------------------------------------------------------------------------------------------

// PDF 예시는 면을 채운 덩어리 대신 모서리와 둥근 격자가 보입니다.
// glPolygonMode만 쓰면 삼각형을 나눈 대각선까지 보이므로 필요한 선을 직접 만듭니다.
void MakeWireObjects()
{
    for (practice::Mesh* mesh : { &practice::cube, &practice::cone, &practice::sphere })
    {
        glDeleteBuffers(1, &mesh->VBO);
        glDeleteVertexArrays(1, &mesh->VAO);
        *mesh = practice::Mesh{};
    }

    // 모서리는 두 끝점을 잇는 선입니다. 육면체에는 모서리가 12개 있습니다.
    const glm::vec3 corners[8] = {
        {-0.5f, -0.5f,  0.5f}, {0.5f, -0.5f,  0.5f},
        {-0.5f,  0.5f,  0.5f}, {0.5f,  0.5f,  0.5f},
        {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f},
        {-0.5f,  0.5f, -0.5f}, {0.5f,  0.5f, -0.5f}
    };
    const int edges[12][2] = {
        {0, 1}, {1, 3}, {3, 2}, {2, 0},
        {4, 5}, {5, 7}, {7, 6}, {6, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };
    std::vector<float> vertices;
    for (int i = 0; i < 12; ++i)
    {
        practice::MakeVertices(vertices, corners[edges[i][0]]);
        practice::MakeVertices(vertices, corners[edges[i][1]]);
    }
    practice::MakeMesh(practice::cube, vertices, GL_LINES);

    // 원뿔: 바닥 원과 꼭대기까지 잇는 선을 만듭니다.
    // 반지름과 높이는 공통 코드의 원뿔과 같으므로 기존 크기 계산을 그대로 씁니다.
    vertices.clear();
    const int segments = 32;
    for (int i = 0; i < segments; ++i)
    {
        float firstAngle = glm::radians(i * 360.0f / segments);
        float nextAngle = glm::radians((i + 1) * 360.0f / segments);
        glm::vec3 first(0.5f * cos(firstAngle), -0.5f, 0.5f * sin(firstAngle));
        glm::vec3 next(0.5f * cos(nextAngle), -0.5f, 0.5f * sin(nextAngle));
        practice::MakeVertices(vertices, first);
        practice::MakeVertices(vertices, next);
        practice::MakeVertices(vertices, first);
        practice::MakeVertices(vertices, glm::vec3(0.0f, 0.5f, 0.0f));
    }
    practice::MakeMesh(practice::cone, vertices, GL_LINES);

    // 구: 지구본의 가로선(위도)과 세로선(경도)을 작은 선분으로 이어 만듭니다.
    vertices.clear();
    for (int latitude = 1; latitude < 12; ++latitude)
    {
        float phi = glm::radians(-90.0f + latitude * 15.0f);
        for (int i = 0; i < segments; ++i)
        {
            float firstAngle = glm::radians(i * 360.0f / segments);
            float nextAngle = glm::radians((i + 1) * 360.0f / segments);
            practice::MakeVertices(vertices, glm::vec3(cos(phi) * cos(firstAngle), sin(phi), cos(phi) * sin(firstAngle)));
            practice::MakeVertices(vertices, glm::vec3(cos(phi) * cos(nextAngle), sin(phi), cos(phi) * sin(nextAngle)));
        }
    }
    for (int longitude = 0; longitude < 16; ++longitude)
    {
        float theta = glm::radians(longitude * 360.0f / 16);
        for (int i = 0; i < 24; ++i)
        {
            float firstAngle = glm::radians(-90.0f + i * 7.5f);
            float nextAngle = glm::radians(-90.0f + (i + 1) * 7.5f);
            practice::MakeVertices(vertices, glm::vec3(cos(firstAngle) * cos(theta), sin(firstAngle), cos(firstAngle) * sin(theta)));
            practice::MakeVertices(vertices, glm::vec3(cos(nextAngle) * cos(theta), sin(nextAngle), cos(nextAngle) * sin(theta)));
        }
    }
    practice::MakeMesh(practice::sphere, vertices, GL_LINES);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

int main()
{
    return practice::RunScene("homework17 - Modeling transforms", KeyCallback, UpdateScene, DrawAll);
}


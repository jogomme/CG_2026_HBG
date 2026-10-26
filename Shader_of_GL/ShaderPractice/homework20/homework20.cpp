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
void DrawAll(common::Shader& shader, int width, int height);
void ResetScene();
glm::mat4 MakeBeltModelMatrix(float distance);
void DrawNumber(int number, glm::mat4 model, GLint modelLocation, common::Shader& shader);

//------------------------------------------------------------------------------------------------------
// 전역 변수와 플래그
//------------------------------------------------------------------------------------------------------
// 벨트 윗면이 바닥에서 얼마나 높은지 정합니다.
const float beltHeight = 0.85f;
// 둥근 코너의 반지름입니다.
const float cornerRadius = 0.45f;
// 코너에 도착하기 전 직선 구간의 길이입니다.
const float firstLength = 4.05f;
// 둥근 길의 길이 = 라디안으로 나타낸 각도 × 반지름입니다.
const float cornerLength = glm::radians(90.0f) * cornerRadius;
// 코너를 지난 뒤 직선 구간의 길이입니다.
const float lastLength = 3.55f;
// 세 구간을 모두 더한 전체 이동 거리입니다.
const float pathLength = firstLength + cornerLength + lastLength;
// 벨트가 시작점에서 얼마나 움직였는지 기억합니다.
float beltDistance = 0;
// 카메라 위치를 만드는 거리 배율입니다. 작아지면 관찰 대상에 가까워집니다.
float cameraDistance = 10;
// 카메라가 관찰 대상 둘레에서 서 있는 방향의 각도입니다.
float cameraAngle = 40;
// B 키: 벨트와 상자를 움직일지 기억합니다.
bool isB = true;
// true면 원근 투영, false면 직각 투영을 사용합니다.
bool isPerspective = true;
const glm::vec3 boxColor[5] = {
    {0.95f, 0.4f, 0.2f}, {0.2f, 0.6f, 0.95f}, {0.85f, 0.7f, 0.2f}, 
    {0.45f, 0.75f, 0.35f}, {0.75f, 0.35f, 0.8f}
};

//------------------------------------------------------------------------------------------------------
// 변수들을 처음 값으로 돌립니다. 장난감을 처음 놓았던 자리로 되돌리는 것과 같습니다.
//------------------------------------------------------------------------------------------------------
void ResetScene()
{
    beltDistance = 0;
    cameraDistance = 10;
    cameraAngle = 40;
    isB = true;
    isPerspective = true;
}

//------------------------------------------------------------------------------------------------------
// 시간이 흐른 만큼 저장된 위치와 각도를 고칩니다. 여기서는 화면을 그리지 않습니다.
//------------------------------------------------------------------------------------------------------
void UpdateScene(float deltaTime)
{
    if (isB)
    {
        beltDistance = std::fmod(beltDistance + 0.85f * deltaTime, pathLength);
    }
}

//------------------------------------------------------------------------------------------------------
// 벨트를 따라 간 거리로 상자의 위치와 바라보는 방향을 구합니다.
// 길은 첫 직선 → 둥근 코너 → 마지막 직선의 세 구간입니다. 끝을 넘으면 시작부터 다시 셉니다.
//------------------------------------------------------------------------------------------------------
glm::mat4 MakeBeltModelMatrix(float distance)
{
    // 전체 길이로 나눈 나머지만 사용합니다. 음수면 아래에서 전체 길이를 더해 양수로 바꿉니다.
    distance = std::fmod(distance, pathLength);
    if (distance < 0)
    {
        distance += pathLength;
    }
    glm::vec3 position;
    float angle;
    if (distance < firstLength)
    {
        position = glm::vec3(-3 + distance, beltHeight, -1.5f);
        angle = 90;
    }
    else if (distance < firstLength + cornerLength)
    {
        // 코너의 원호를 따라 위치와 방향을 함께 바꾼다.
        // 코너 시작 각도 -90도에, 코너에서 이동한 거리 ÷ 반지름을 더합니다. 결과는 라디안입니다.
        float a = -glm::radians(90.0f) + (distance - firstLength) / cornerRadius;
        position = glm::vec3(1.05f + cornerRadius * cos(a), beltHeight, -1.05f + cornerRadius * sin(a));
        // 원에 나란히 따라가는 방향으로 상자의 앞쪽(+z)을 돌립니다. 위치와 방향이 함께 바뀝니다.
        angle = -glm::degrees(a);
    }
    else
    {
        position = glm::vec3(1.5f, beltHeight, -1.05f + distance - firstLength - cornerLength);
        angle = 0;
    }
    return glm::rotate(glm::translate(glm::mat4(1), position), glm::radians(angle), glm::vec3(0, 1, 0));
}

//------------------------------------------------------------------------------------------------------
// 키를 누른 소식을 받습니다. key는 버튼 이름, action은 누름/반복/뗌 상태입니다.
// mods에는 Shift 같은 보조 키를 눌렀는지가 들어 있습니다. scancode는 여기서 사용하지 않습니다.
// PRESS는 처음 누름, REPEAT는 길게 눌러서 반복되는 입력입니다.
//------------------------------------------------------------------------------------------------------
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // 키를 뗀 소식은 건너뜁니다. 처음 누름과 길게 누름만 아래에서 처리합니다.
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
    {
        return;
    }
    if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD)
    {
        cameraDistance = std::max(5.0f, cameraDistance - 0.3f);
    }
    else if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT)
    {
        cameraDistance = std::min(22.0f, cameraDistance + 0.3f);
    }
    else if (key == GLFW_KEY_LEFT)
    {
        cameraAngle -= 5;
    }
    else if (key == GLFW_KEY_RIGHT)
    {
        cameraAngle += 5;
    }
    else if (key == GLFW_KEY_P)
    {
        isPerspective = (mods & GLFW_MOD_SHIFT) == 0;
    }
    else if (key == GLFW_KEY_B && action == GLFW_PRESS)
    {
        isB = !isB;
    }
    else if (key == GLFW_KEY_R)
    {
        ResetScene();
    }
}

//------------------------------------------------------------------------------------------------------
// 작은 막대들을 붙여 상자 위에 숫자를 만듭니다. 전자시계의 숫자와 같은 방법입니다.
// 이 함수에 들어오는 number는 1~5입니다. 배열의 줄 번호는 0부터라서 number - 1을 씁니다.
//------------------------------------------------------------------------------------------------------
void DrawNumber(int number, glm::mat4 model, GLint modelLocation, common::Shader& shader)
{
    // 일곱 개의 가는 육면체로 상자 윗면에 1~5를 표시한다.
    // 한 줄이 숫자 하나입니다. 1인 막대만 그립니다.
    // 막대 순서: 위, 오른쪽 위, 오른쪽 아래, 아래, 왼쪽 아래, 왼쪽 위, 가운데입니다.
    const int segments[5][7] = {
        {0, 1, 1, 0, 0, 0, 0}, {1, 1, 0, 1, 1, 0, 1}, {1, 1, 1, 1, 0, 0, 1}, 
        {0, 1, 1, 0, 0, 1, 1}, {1, 0, 1, 1, 0, 1, 1}
    };
    // 숫자를 만드는 막대 7개의 중심 위치입니다. 상자 윗면보다 살짝 높게 둡니다.
    const glm::vec3 positions[7] = {
        {0, 0.526f, -0.16f}, {0.11f, 0.526f, -0.08f}, {0.11f, 0.526f, 0.08f}, 
        {0, 0.526f, 0.16f}, {-0.11f, 0.526f, 0.08f}, {-0.11f, 0.526f, -0.08f}, {0, 0.526f, 0}
    };
    for (int i = 0; i < 7; ++i)
    {
        if (!segments[number - 1][i])
        {
            continue;
        }
        // 위·아래·가운데 막대는 가로로 길게, 나머지는 세로로 길게 만듭니다.
        glm::vec3 size = (i == 0 || i == 3 || i == 6)
            ? glm::vec3(0.24f, 0.012f, 0.035f)
            : glm::vec3(0.035f, 0.012f, 0.15f);
        practice::DrawObj(
            practice::cube,
            practice::MakeModelMatrix(positions[i], size, model),
            glm::vec3(0.06f),
            modelLocation,
            shader
        );
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
    float angle = glm::radians(cameraAngle);
    glm::vec3 target(-0.5f, 0.4f, 0.3f);
    glm::vec3 camera = target + cameraDistance * glm::vec3(0.75f * sin(angle), 0.65f, 0.75f * cos(angle));
    shader.setMatrix(shader.uniformLocation("view"), glm::lookAt(camera, target, glm::vec3(0,1,0)));
    shader.setMatrix(
        shader.uniformLocation("projection"),
        practice::MakeProjection(width, height, isPerspective, cameraDistance * 0.42f)
    );
    practice::DrawObj(
        practice::plane,
        practice::MakeModelMatrix(glm::vec3(0), glm::vec3(10, 1, 9)),
        glm::vec3(0.78f, 0.82f, 0.87f),
        modelLocation,
        shader
    );
    practice::DrawObj(
        practice::plane,
        practice::MakeModelMatrix(glm::vec3(-0.75f, beltHeight, -1.5f), glm::vec3(5.5f, 1, 1)),
        glm::vec3(0.18f, 0.23f, 0.28f),
        modelLocation,
        shader
    );
    practice::DrawObj(
        practice::plane,
        practice::MakeModelMatrix(glm::vec3(1.5f, beltHeight, 0.5f), glm::vec3(1, 1, 5)),
        glm::vec3(0.18f, 0.23f, 0.28f),
        modelLocation,
        shader
    );
    const glm::vec3 legs[4] = {{-3, beltHeight / 2, -1.5f}, {-1, beltHeight / 2, -1.5f}, {1.5f, beltHeight / 2, -1.5f}, {1.5f, beltHeight / 2, 2.5f}};
    for (glm::vec3 position : legs)
    {
        practice::DrawObj(
            practice::cube,
            practice::MakeModelMatrix(position, glm::vec3(0.16f, beltHeight, 0.6f)),
            glm::vec3(0.4f, 0.45f, 0.5f),
            modelLocation,
            shader
        );
    }
    // 벨트 위 줄무늬 36개를 같은 간격으로 놓습니다. 모두 같은 beltDistance를 더해 함께 움직입니다.
    for (int i = 0; i < 36; ++i)
    {
        glm::mat4 model = MakeBeltModelMatrix(beltDistance + i * pathLength / 36);
        practice::DrawObj(
            practice::cube,
            practice::MakeModelMatrix(glm::vec3(0, 0.008f, 0), glm::vec3(0.96f, 0.016f, 0.045f), model),
            glm::vec3(0.55f, 0.6f, 0.65f),
            modelLocation,
            shader
        );
    }
    // 상자 5개를 전체 길이의 1/5씩 떨어뜨려 놓습니다. 각 상자에 숫자 1~5를 붙입니다.
    for (int i = 0; i < 5; ++i)
    {
        glm::mat4 model = MakeBeltModelMatrix(beltDistance - i * pathLength / 5);
        practice::DrawObj(
            practice::cube,
            practice::MakeModelMatrix(glm::vec3(0, 0.28f, 0), glm::vec3(0.52f, 0.52f, 0.52f), model),
            boxColor[i],
            modelLocation,
            shader
        );
        // 상자는 줄무늬의 윗면 위에 놓이며 숫자도 상자와 같은 변환을 받는다.
        DrawNumber(i + 1, glm::translate(model, glm::vec3(0, 0.02f, 0)), modelLocation, shader);
    }
}

//------------------------------------------------------------------------------------------------------
// 프로그램의 출발점입니다. RunScene에 키 입력·움직임·그리기 함수를 맡깁니다.
// RunScene 안에서 창 생성, 반복 그리기, 사용한 그래픽 자원 정리를 처리합니다.
//------------------------------------------------------------------------------------------------------
int main()
{
    return practice::RunScene("homework20 - Parcel conveyor", KeyCallback, UpdateScene, DrawAll);
}

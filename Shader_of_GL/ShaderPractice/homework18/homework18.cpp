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
glm::mat4 MakeOrbitMatrix(int planet);

//------------------------------------------------------------------------------------------------------
// 전역 변수와 플래그
//------------------------------------------------------------------------------------------------------
// 물체가 원점 둘레를 얼마나 돌았는지 나타내는 각도입니다.
float orbitAngle[3] = {0, 120, 240};
// 행성마다 다른 초당 공전 각도입니다. 숫자가 크면 더 빨리 돕니다.
float orbitSpeed[3] = {25, 40, 60};
// 행성의 궤도 자체를 기울일 각도입니다.
float orbitTilt[3] = {0, 45, -45};
// 각 달이 자기 행성 둘레에서 어디까지 돌았는지 기억합니다.
float moonAngle[3] = {0, 120, 240};
// 달의 궤도 틀에 추가로 적용할 회전 각도입니다.
float moonOrbitAngle = 0;
// 반복 애니메이션을 위한 각도입니다. 한 바퀴마다 같은 움직임이 반복됩니다.
float animationAngle = 0;
// 행성과 달의 궤도 반지름을 함께 늘릴 배율입니다.
float radiusScale = 1;
glm::vec3 position(0);
// true면 원근 투영, false면 직각 투영을 사용합니다.
bool isPerspective = false;
// true면 면을 채우지 않고 삼각형 테두리로 그립니다.
bool isW = false;
// 1 키: 궤도 반지름을 늘릴지 기억합니다. 꺼지면 줄어들어 기본 크기로 돌아갑니다.
bool is1 = false;
// 2 키: 중심 구의 크기와 색을 주기적으로 바꿀지 기억합니다.
bool is2 = false;
// 3 키: 달의 궤도 틀을 추가로 돌릴지 기억합니다.
bool is3 = false;
const glm::vec3 planetColor[3] = { {0.2f, 0.6f, 1}, {0.4f, 0.8f, 0.25f}, {0.9f, 0.25f, 0.4f} };
const glm::vec3 moonColor[3] = { {0.65f, 0.35f, 0.9f}, {0.15f, 0.75f, 0.7f}, {0.8f, 0.55f, 0.2f} };

//------------------------------------------------------------------------------------------------------
// 시간이 흐른 만큼 저장된 위치와 각도를 고칩니다. 여기서는 화면을 그리지 않습니다.
//------------------------------------------------------------------------------------------------------
void UpdateScene(float deltaTime)
{
    for (int i = 0; i < 3; ++i)
    {
        orbitAngle[i] = std::fmod(orbitAngle[i] + orbitSpeed[i] * deltaTime, 360.0f);
        moonAngle[i] = std::fmod(moonAngle[i] + (90 + i * 20) * deltaTime, 360.0f);
    }
    if (is1)
    {
        radiusScale += deltaTime * 0.3f;
    }
    else
    {
        radiusScale -= deltaTime * 0.3f;
    }
    radiusScale = std::clamp(radiusScale, 1.0f, 1.6f);
    if (is2)
    {
        animationAngle = std::fmod(animationAngle + 90 * deltaTime, 360.0f);
    }
    if (is3)
    {
        moonOrbitAngle = std::fmod(moonOrbitAngle + 60 * deltaTime, 360.0f);
    }
}

//------------------------------------------------------------------------------------------------------
// 행성이 도는 원의 기울기와 전체 위치를 정합니다.
// 행성의 개별 위치를 이 행렬로 옮기면 기울어진 궤도 위의 위치가 됩니다.
//------------------------------------------------------------------------------------------------------
glm::mat4 MakeOrbitMatrix(int planet)
{
    glm::mat4 model = glm::translate(glm::mat4(1), position);
    return glm::rotate(model, glm::radians(orbitTilt[planet]), glm::vec3(0, 0, 1));
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
    // Shift 키를 함께 눌렀는지 확인합니다. &는 해당 키의 표시가 있는지 검사합니다.
    bool shift = (mods & GLFW_MOD_SHIFT) != 0;
    if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (key == GLFW_KEY_P)
    {
        isPerspective = shift;
    }
    else if (key == GLFW_KEY_M)
    {
        isW = shift;
    }
    else if (key == GLFW_KEY_A)
    {
        position.x -= 0.15f;
    }
    else if (key == GLFW_KEY_D)
    {
        position.x += 0.15f;
    }
    else if (key == GLFW_KEY_W)
    {
        position.y += 0.15f;
    }
    else if (key == GLFW_KEY_S)
    {
        position.y -= 0.15f;
    }
    else if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD)
    {
        position.z += 0.15f;
    }
    else if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT)
    {
        position.z -= 0.15f;
    }
    else if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_1)
        {
            is1 = !is1;
        }
        else if (key == GLFW_KEY_2)
        {
            is2 = !is2;
        }
        else if (key == GLFW_KEY_3)
        {
            is3 = !is3;
        }
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
    glm::mat4 view = glm::lookAt(glm::vec3(0, 4.5f, 11), glm::vec3(0), glm::vec3(0, 1, 0));
    shader.setMatrix(shader.uniformLocation("view"), view);
    shader.setMatrix(
        shader.uniformLocation("projection"),
        practice::MakeProjection(width, height, isPerspective, 4.8f)
    );
    glPolygonMode(GL_FRONT_AND_BACK, isW ? GL_LINE : GL_FILL);
    // sin은 -1~1 사이를 부드럽게 오갑니다. 이 값으로 중심 구의 크기와 색을 함께 바꿉니다.
    float wave = sin(glm::radians(animationAngle));
    glm::vec3 centerColor(1.0f, 0.65f + 0.25f * wave, 0.15f + 0.1f * wave);
    practice::DrawObj(
        practice::sphere,
        practice::MakeModelMatrix(position, glm::vec3(0.5f + 0.12f * wave)),
        centerColor,
        modelLocation,
        shader
    );
    for (int i = 0; i < 3; ++i)
    {
        float radius = (1.5f + i * 0.75f) * radiusScale;
        glm::mat4 orbit = MakeOrbitMatrix(i);
        practice::DrawObj(
            practice::circle,
            glm::scale(orbit, glm::vec3(radius)),
            planetColor[i],
            modelLocation,
            shader
        );
        float angle = glm::radians(orbitAngle[i]);
        // cos로 원의 x, sin으로 원의 z를 구합니다. orbit을 곱해 기울기와 이동을 반영합니다.
        // vec4의 마지막 값 1은 위치를 뜻합니다. 0이면 이동의 영향을 받지 않는 방향입니다.
        glm::vec3 planetPosition = glm::vec3(orbit * glm::vec4(radius * cos(angle), 0, radius * sin(angle), 1));
        practice::DrawObj(
            practice::sphere,
            practice::MakeModelMatrix(planetPosition, glm::vec3(0.22f)),
            planetColor[i],
            modelLocation,
            shader
        );

        // 화면의 오른쪽/위쪽 방향을 사용하여 달의 궤도를 화면에 평행하게 유지한다.
        // 카메라 view의 역행렬에서 회전 부분만 가져와 달의 궤도 틀을 화면 방향에 맞춥니다.
        // mat3로 바꿀 때 위치 성분은 빠집니다. 달의 틀 중심은 행성 위치에 따로 놓습니다.
        glm::mat4 moonFrame = glm::translate(
            glm::mat4(1),
            planetPosition) * glm::mat4(glm::mat3(glm::inverse(view))
        );
        moonFrame = glm::rotate(moonFrame, glm::radians(moonOrbitAngle), glm::vec3(0, 0, 1));
        float moonRadius = 0.43f * radiusScale;
        // 공통 원은 xz 평면에 있습니다. x축으로 90도 돌려 달의 xy 궤도와 같은 평면에 놓습니다.
        glm::mat4 moonPath = glm::rotate(moonFrame, glm::radians(90.0f), glm::vec3(1, 0, 0));
        practice::DrawObj(
            practice::circle,
            glm::scale(moonPath, glm::vec3(moonRadius)),
            moonColor[i],
            modelLocation,
            shader
        );
        angle = glm::radians(moonAngle[i]);
        // 달은 자기 궤도의 xy 평면에서 돕니다. moonFrame을 곱해 행성 옆의 실제 위치로 옮깁니다.
        glm::vec3 moonPosition = glm::vec3(moonFrame * glm::vec4(moonRadius * cos(angle), moonRadius * sin(angle), 0, 1));
        practice::DrawObj(
            practice::sphere,
            practice::MakeModelMatrix(moonPosition, glm::vec3(0.085f)),
            moonColor[i],
            modelLocation,
            shader
        );
    }
    // 다 그린 뒤 기본 채우기 상태로 돌려놓습니다.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

//------------------------------------------------------------------------------------------------------
// 프로그램의 출발점입니다. RunScene에 키 입력·움직임·그리기 함수를 맡깁니다.
// RunScene 안에서 창 생성, 반복 그리기, 사용한 그래픽 자원 정리를 처리합니다.
//------------------------------------------------------------------------------------------------------
int main()
{
    return practice::RunScene("homework18 - Planets and moons", KeyCallback, UpdateScene, DrawAll);
}

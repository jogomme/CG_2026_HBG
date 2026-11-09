#pragma once

#include "Common/PracticeScene.h"
#include <random>

namespace tank {

//------------------------------------------------------------------------------------------------------
// 함수 선언
//------------------------------------------------------------------------------------------------------
inline void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
inline void UpdateScene(float deltaTime);
inline void UpdateKeyboard(float deltaTime);
inline void DrawAll(common::Shader& shader, int width, int height);
inline void ResetScene();
inline void ChangeColors();
//------------------------------------------------------------------------------------------------------
// 육면체 하나로 탱크 부품을 그립니다. model은 부품이 매달릴 몸체의 변환입니다.
// 부모 몸체의 설명서 뒤에 부품의 위치와 크기를 붙이므로 몸체가 움직이면 부품도 따라갑니다.
//------------------------------------------------------------------------------------------------------
inline void DrawCube(
    glm::mat4 model,
    glm::vec3 position,
    glm::vec3 size,
    int color,
    // 셰이더 안에서 model 행렬을 넣을 칸의 번호를 알아냅니다.
    GLint modelLocation,
    common::Shader& shader
);
inline glm::mat4 MakeViewMatrix();

//------------------------------------------------------------------------------------------------------
// 전역 변수와 플래그
//------------------------------------------------------------------------------------------------------
// 키를 누르면 true, 떼면 false가 됩니다. 키보드의 반복 입력 속도에 기대지 않습니다.
// 이전 과제의 isR/isL처럼 '누르고 있는 상태'를 기억하는 배열입니다.
inline bool keyDown[GLFW_KEY_LAST + 1]{};
inline float positionX = 0;
inline float positionZ = 0;
// 탱크 아래 몸체의 높이입니다.
inline float lowerHeight = 0.4f;
// 아래 몸체 높이의 부드러운 오르내림에 쓸 각도입니다.
inline float heightAngle = 0;
// 탱크 중앙 몸체의 회전 각도입니다.
inline float centerAngle = 0;
// 상부 몸체 두 개가 자리를 바꾸며 이동한 각도입니다.
inline float upperAngle = 0;
// 이번 자리 교환이 끝날 목표 각도입니다. 시작 각도에서 180도 더 갑니다.
inline float upperTarget = 0;
// 포신의 회전 각도입니다. 양쪽에 반대 부호를 주어 반대로 돌립니다.
inline float gunAngle = 0;
// 깃대의 회전 각도입니다.
inline float flagAngle = 0;
// 카메라가 제자리에서 좌우로 고개를 돌린 각도입니다.
inline float cameraYaw = 0;
// 카메라가 제자리에서 도는 초당 각도입니다. 0이면 멈춥니다.
inline float cameraSpinSpeed = 0;
// 카메라가 원점 둘레를 도는 초당 각도입니다.
inline float cameraOrbitSpeed = 0;
inline glm::vec3 cameraPosition(5, 4, 7);
// D 키: 아래 몸체 높이의 오르내림을 켤지 기억합니다.
inline bool isD = false;
// T 키: 중앙 몸체의 회전을 켤지 기억합니다.
inline bool isT = false;
// L 키: 상부 몸체의 자리 교환이 진행 중인지 기억합니다.
inline bool isL = false;
// G 키: 포신 회전을 켤지 기억합니다.
inline bool isG = false;
// P 키: 깃대 회전을 켤지 기억합니다.
inline bool isP = false;
inline glm::vec3 objColor[10] = {
    {0.25f, 0.32f, 0.38f}, {0.32f, 0.38f, 0.43f}, {0.35f, 0.55f, 0.32f},
    {0.55f, 0.8f, 0.25f}, {0.25f, 0.65f, 0.65f}, {0.55f, 0.6f, 0.2f},
    {0.2f, 0.45f, 0.35f}, {1, 0.65f, 0.15f}, {0.9f, 0.3f, 0.2f}, {0.35f, 0.75f, 0.9f}
};
inline std::mt19937 gen(std::random_device{}());

inline glm::vec3 cameraTarget(0, 0, 0);

//------------------------------------------------------------------------------------------------------
// 각 물체에 새 색을 골라 줍니다. 빨강·초록·파랑 성분을 각각 뽑습니다.
// 난수는 주사위처럼 값을 고르는 방법입니다. 여기서는 0.2~0.95 사이의 값을 사용합니다.
//------------------------------------------------------------------------------------------------------
inline void ChangeColors()
{
    std::uniform_real_distribution<float> colorDist(0.2f, 0.95f);
    for (glm::vec3& color : objColor)
    {
        color = glm::vec3(colorDist(gen), colorDist(gen), colorDist(gen));
    }
}

//------------------------------------------------------------------------------------------------------
// 변수들을 처음 값으로 돌립니다. 장난감을 처음 놓았던 자리로 되돌리는 것과 같습니다.
//------------------------------------------------------------------------------------------------------
inline void ResetScene()
{
    // 초기화할 때 기억한 키도 놓아 줍니다. 다음에 다시 누르기 전까지는 멈춥니다.
    std::fill(std::begin(keyDown), std::end(keyDown), false);
    positionZ = 0;
    positionX = 0;
    lowerHeight = 0.4f;
    flagAngle = 0;
    gunAngle = 0;
    upperTarget = 0;
    upperAngle = 0;
    centerAngle = 0;
    heightAngle = 0;
    cameraOrbitSpeed = 0;
    cameraSpinSpeed = 0;
    cameraYaw = 0;
    cameraPosition = glm::vec3(5, 4, 7);
    isP = false;
    isG = false;
    isL = false;
    isT = false;
    isD = false;
    const glm::vec3 initial[10] = {
        {0.25f, 0.32f, 0.38f}, {0.32f, 0.38f, 0.43f}, {0.35f, 0.55f, 0.32f},
        {0.55f, 0.8f, 0.25f}, {0.25f, 0.65f, 0.65f}, {0.55f, 0.6f, 0.2f},
        {0.2f, 0.45f, 0.35f}, {1, 0.65f, 0.15f}, {0.9f, 0.3f, 0.2f}, {0.35f, 0.75f, 0.9f}
    };
    for (int i = 0; i < 10; ++i)
    {
        objColor[i] = initial[i];
    }
}

//------------------------------------------------------------------------------------------------------
// 시간이 흐른 만큼 저장된 위치와 각도를 고칩니다. 여기서는 화면을 그리지 않습니다.
//------------------------------------------------------------------------------------------------------
// 누르고 있는 동안 '초당 속도 × 지난 시간'만큼 움직입니다.
// 예: 1초에 0.6만큼 가면 0.5초 동안에는 0.3만큼 갑니다.
inline void UpdateKeyboard(float deltaTime)
{
    float movement = 1.2f * deltaTime;
    bool shift = keyDown[GLFW_KEY_LEFT_SHIFT] || keyDown[GLFW_KEY_RIGHT_SHIFT];
    float cameraMovement = (shift ? -1.0f : 1.0f) * 1.6f * deltaTime;
    if (keyDown[GLFW_KEY_LEFT])
    {
        positionX -= movement;
    }
    if (keyDown[GLFW_KEY_RIGHT])
    {
        positionX += movement;
    }
    if (keyDown[GLFW_KEY_UP])
    {
        positionZ -= movement;
    }
    if (keyDown[GLFW_KEY_DOWN])
    {
        positionZ += movement;
    }
    if (keyDown[GLFW_KEY_X])
    {
        cameraPosition.x += cameraMovement;
        cameraTarget.x += cameraMovement;
    }

    if (keyDown[GLFW_KEY_Z])
    {
        cameraPosition.z += cameraMovement;
        cameraTarget.z += cameraMovement;
    }
    // 회전 중인 부품도 바닥 안에 남을 수 있도록 중심 위치에 여유를 둡니다.
    positionX = std::clamp(positionX, -2.5f, 2.5f);
    positionZ = std::clamp(positionZ, -2.5f, 2.5f);
}

inline void UpdateScene(float deltaTime)
{
    UpdateKeyboard(deltaTime);

    if (isD)
    {
        heightAngle = std::fmod(heightAngle + 90 * deltaTime, 360.0f);
        lowerHeight = 0.4f + 0.15f * sin(glm::radians(heightAngle));
    }
    if (isT)
    {
        centerAngle = std::fmod(centerAngle + 40 * deltaTime, 360.0f);
    }
    if (isL)
    {
        // 두 상부 몸체를 반원 경로로 이동시켜 서로 통과하지 않고 교환한다.
        upperAngle = std::min(upperTarget, upperAngle + 90 * deltaTime);
        if (upperAngle >= upperTarget)
        {
            isL = false;
            upperAngle = std::fmod(upperAngle, 360.0f);
        }
    }
    if (isG)
    {
        gunAngle = std::fmod(gunAngle + 45 * deltaTime, 360.0f);
    }
    if (isP)
    {
        flagAngle = std::fmod(flagAngle + 45 * deltaTime, 360.0f);
    }
    cameraYaw = std::fmod(cameraYaw + cameraSpinSpeed * deltaTime, 360.0f);
    glm::mat4 orbit = glm::rotate(
        glm::mat4(1),
        glm::radians(cameraOrbitSpeed * deltaTime),
        glm::vec3(0, 1, 0)
    );
    cameraPosition = glm::vec3(orbit * glm::vec4(cameraPosition, 1));
}

//------------------------------------------------------------------------------------------------------
// 키를 누른 소식을 받습니다. key는 버튼 이름, action은 누름/반복/뗌 상태입니다.
// mods에는 Shift 같은 보조 키를 눌렀는지가 들어 있습니다. scancode는 여기서 사용하지 않습니다.
// PRESS는 처음 누름, REPEAT는 길게 눌러서 반복되는 입력입니다.
//------------------------------------------------------------------------------------------------------
inline void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
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

    // Shift를 누르면 -1, 아니면 +1입니다. 같은 계산의 방향만 뒤집습니다.
    float direction = (mods & GLFW_MOD_SHIFT) ? -1.0f : 1.0f;
    if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_D)
        {
            isD = !isD;
        }
        else if (key == GLFW_KEY_T)
        {
            isT = !isT;
        }
        else if (key == GLFW_KEY_L && !isL)
        {
            upperTarget = upperAngle + 180;
            isL = true;
        }
        else if (key == GLFW_KEY_G)
        {
            isG = !isG;
        }
        else if (key == GLFW_KEY_P)
        {
            isP = !isP;
        }
        else if (key == GLFW_KEY_Y)
        {
            float speed = -direction * 35.0f;
             // 같은 방향으로 이미 돌고 있으면 멈추고, 아니면 요청한 방향으로 돌립니다.
            if (cameraSpinSpeed == speed)
            {
                cameraSpinSpeed = 0.0f;
            }
            else
            {
                cameraSpinSpeed = speed;
            }
        }
        else if (key == GLFW_KEY_R)
        {
            float speed = -direction * 25.0f;
             // 같은 방향으로 이미 돌고 있으면 멈추고, 아니면 요청한 방향으로 돌립니다.
            if (cameraOrbitSpeed == speed)
            {
                cameraOrbitSpeed = 0.0f;
            }
            else
            {
                cameraOrbitSpeed = speed;
            }
        }
        else if (key == GLFW_KEY_O)
        {
            ChangeColors();
        }
        else if (key == GLFW_KEY_C)
        {
            ResetScene();
        }
    }
    // 중앙 몸체가 회전해도 탱크 전체가 바닥 안에 머물도록 여유를 둔다.

}

//------------------------------------------------------------------------------------------------------
// 카메라가 서 있는 곳과 바라보는 방향으로 view 행렬을 만듭니다.
// 자전은 같은 자리에서 고개 돌리기, 공전은 원점 둘레를 걸어 다니기라고 생각하면 됩니다.
//------------------------------------------------------------------------------------------------------
inline glm::mat4 MakeViewMatrix()
{
    // 카메라 위치에서 원점으로 가는 방향입니다. normalize는 길이를 1로 맞춥니다.
    glm::vec3 forward = glm::normalize(cameraTarget - cameraPosition);
    // 위쪽 기준을 고릅니다. 보는 방향과 위쪽이 거의 나란하면 다른 기준을 써서 계산이 무너지지 않게 합니다.
    glm::vec3 worldUp = std::abs(forward.y) > 0.9999f ? glm::vec3(0, 0, -1) : glm::vec3(0, 1, 0);
    // cross는 두 방향에 수직인 방향을 구합니다. 여기서는 카메라의 오른쪽을 구합니다.
    glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
    // 오른쪽과 앞쪽에 수직인 방향을 카메라의 위쪽으로 삼습니다.
    glm::vec3 up = glm::normalize(glm::cross(right, forward));
    // 공전은 위치와 원점을 향하는 기준 방향을 함께 바꾸고, 자전은 위치를 유지한다.
    forward = glm::vec3(glm::rotate(glm::mat4(1), glm::radians(cameraYaw), up) * glm::vec4(forward, 0));
    return glm::lookAt(cameraPosition, cameraPosition + forward, up);
}

//------------------------------------------------------------------------------------------------------
// 육면체 하나로 탱크 부품을 그립니다. model은 부품이 매달릴 몸체의 변환입니다.
// 부모 몸체의 설명서 뒤에 부품의 위치와 크기를 붙이므로 몸체가 움직이면 부품도 따라갑니다.
//------------------------------------------------------------------------------------------------------
inline void DrawCube(glm::mat4 model, glm::vec3 position, glm::vec3 size, int color, GLint modelLocation, common::Shader& shader)
{
    practice::DrawObj(
        practice::cube,
        practice::MakeModelMatrix(position, size, model),
        objColor[color],
        modelLocation,
        shader
    );
}

//------------------------------------------------------------------------------------------------------
// 저장된 위치와 각도로 이번 화면을 그립니다.
// model은 물체의 변환, view는 카메라의 시점, projection은 화면에 비추는 방법입니다.
// 원근 투영은 먼 물체를 작게 보이게 하고, 직각 투영은 거리에 따른 크기 감소가 없습니다.
//------------------------------------------------------------------------------------------------------
// [기능] 탱크와 바닥만 그린다. [사용 상황] 같은 탱크를 다른 시점에서 출력할 때.
// [매개변수] shader: 현재 셰이더. [반환값] 없음.
// [사용 예시] tank::DrawWorld(shader); [주의사항] view/projection과 viewport를 먼저 설정한다.
// 장면 상태를 바꾸지 않으므로 여러 번 호출해도 탱크가 추가로 움직이지 않는다. [관련 실습] 19, 22번.
inline void DrawWorld(common::Shader& shader)
{
    GLint modelLocation = shader.uniformLocation("model");
    practice::DrawObj(
        practice::plane,
        practice::MakeModelMatrix(glm::vec3(0), glm::vec3(8, 1, 8)),
        glm::vec3(0.78f, 0.83f, 0.72f),
        modelLocation,
        shader
    );
    glm::mat4 base = glm::translate(glm::mat4(1), glm::vec3(positionX, 0, positionZ));
    for (int i = 0; i < 2; ++i)
    {
        DrawCube(base, glm::vec3(i == 0
            ? -0.62f
            : 0.62f, lowerHeight / 2, 0), glm::vec3(0.5f, lowerHeight, 1.7f), i, modelLocation, shader);
    }
    glm::mat4 center = glm::translate(base, glm::vec3(0, lowerHeight + 0.13f, 0));
    center = glm::rotate(center, glm::radians(centerAngle), glm::vec3(0, 1, 0));
    DrawCube(center, glm::vec3(0), glm::vec3(1.8f, 0.26f, 1.45f), 2, modelLocation, shader);
    for (int i = 0; i < 2; ++i)
    {
        // 첫 부품은 -1, 둘째는 +1입니다. 같은 식으로 양쪽에 놓거나 서로 반대로 돌릴 수 있습니다.
        float side = i == 0 ? -1.0f : 1.0f;
        // 상부 몸체 두 개를 원의 반대편에 놓습니다. 반 바퀴인 180도를 움직이면 서로의 자리가 됩니다.
        float a = glm::radians(upperAngle);
        glm::mat4 upper = glm::translate(
            center,
            glm::vec3(side * 0.48f * cos(a), 0.32f, side * 0.48f * sin(a))
        );
        DrawCube(upper, glm::vec3(0), i == 0
            ? glm::vec3(0.65f, 0.38f, 0.7f)
            : glm::vec3(0.6f, 0.32f, 0.65f), 3 + i, modelLocation, shader);
        glm::mat4 gun = glm::translate(upper, glm::vec3(0, 0, 0.32f));
        gun = glm::rotate(gun, glm::radians(side * gunAngle), glm::vec3(0, 1, 0));
        DrawCube(gun, glm::vec3(0, 0, 0.3f), glm::vec3(0.1f, 0.1f, 0.65f), 5 + i, modelLocation, shader);
        glm::mat4 flag = glm::translate(upper, glm::vec3(0, 0.2f, 0));
        flag = glm::rotate(flag, glm::radians(side * flagAngle), glm::vec3(1, 0, 0));
        DrawCube(
            flag,
            glm::vec3(0, 0.3f, 0),
            glm::vec3(0.055f, 0.6f, 0.055f),
            7 + i,
            modelLocation,
            shader
        );
        DrawCube(
            flag,
            glm::vec3(0.12f, 0.5f, 0),
            glm::vec3(0.24f, 0.16f, 0.025f),
            8 + i,
            modelLocation,
            shader
        );
    }
}

//------------------------------------------------------------------------------------------------------
// 프로그램의 출발점입니다. RunScene에 키 입력·움직임·그리기 함수를 맡깁니다.
// RunScene 안에서 창 생성, 반복 그리기, 사용한 그래픽 자원 정리를 처리합니다.
//------------------------------------------------------------------------------------------------------
inline void DrawAll(common::Shader& shader, int width, int height)
{
    shader.setMatrix(shader.uniformLocation("view"), MakeViewMatrix());
    shader.setMatrix(shader.uniformLocation("projection"), practice::MakeProjection(width, height, true, 5));
    DrawWorld(shader);
}
}

#include "Common/RoomScene.h"
#include <array>
#include <random>

constexpr float halfWidth = 3.6f;
constexpr float halfDepth = 3.3f;
constexpr float stageHeight = 3.6f;
constexpr float robotRadius = 0.28f;
struct Obstacle { glm::vec3 center; glm::vec3 size; glm::vec3 color; };
std::array<Obstacle,3> obstacles{};
std::mt19937 randomEngine(std::random_device{}());
glm::vec2 position(0.0f, 1.9f);
glm::vec2 direction(0.0f,-1.0f);
glm::vec3 cameraPosition(0.0f,4.7f,11.0f);
float feet = 0.0f;
float verticalSpeed = 0.0f;
float walkSpeed = 1.35f;
float stepAngle = 0.0f;
float doorOpen = 0.0f;
bool doorTarget = false;
bool grounded = true;
bool walking = true;
bool held[GLFW_KEY_LAST+1]{};

void ResetScene()
{
    position = {0,1.9f};
    direction = {0,-1};
    cameraPosition = {0,4.7f,11};
    feet = verticalSpeed = stepAngle = doorOpen = 0.0f;
    walkSpeed = 1.35f;
    walking = grounded = true;
    doorTarget = false;
    std::fill(std::begin(held),std::end(held),false);
    std::uniform_real_distribution<float> jitter(-0.25f,0.25f);
    // 세 구역에서 각각 뽑아 서로 겹치거나 시작점을 막지 않게 한다.
    obstacles = {{
        {{-2.0f+jitter(randomEngine),0.0f,0.4f+jitter(randomEngine)}, {1.10f,0.56f,0.85f}, {0.90f,0.55f,0.19f}},
        {{0.15f+jitter(randomEngine),0.0f,-0.8f+jitter(randomEngine)}, {0.82f,0.38f,0.95f}, {0.62f,0.65f,0.71f}},
        {{2.0f+jitter(randomEngine),0.0f,-1.55f+jitter(randomEngine)}, {1.02f,0.82f,1.10f}, {0.55f,0.35f,0.79f}}
    }};
}

// 로봇의 발 주변과 장애물의 xz 범위가 겹치는지 확인한다. 높이는 아래에서 따로 검사한다.
bool Overlaps(glm::vec2 point, const Obstacle& obstacle)
{
    return std::abs(point.x-obstacle.center.x) < obstacle.size.x/2+robotRadius
        && std::abs(point.y-obstacle.center.z) < obstacle.size.z/2+robotRadius;
}

float SupportHeight(glm::vec2 point, float limit)
{
    float support = 0.0f;
    for (const Obstacle& obstacle : obstacles)
        if (obstacle.size.y <= limit + 0.001f && Overlaps(point,obstacle))
            support = std::max(support,obstacle.size.y);
    return support;
}

void KeyCallback(GLFWwindow* window, int key, int, int action, int)
{
    if (key >= 0 && key <= GLFW_KEY_LAST && action != GLFW_REPEAT)
        held[key] = action == GLFW_PRESS;
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window,GLFW_TRUE);
    else if (key == GLFW_KEY_I) ResetScene();
    else if (key == GLFW_KEY_O) doorTarget = !doorTarget;
    else if (key == GLFW_KEY_J && grounded)
    {
        // 점프 자체는 제자리. 공중에서 방향 키를 누르면 장애물 위로 이동할 수 있다.
        verticalSpeed = 4.9f;
        grounded = walking = false;
    }
    else if (key == GLFW_KEY_W || key == GLFW_KEY_A || key == GLFW_KEY_S || key == GLFW_KEY_D)
    {
        if (key == GLFW_KEY_W) direction = {0,-1};
        if (key == GLFW_KEY_A) direction = {-1,0};
        if (key == GLFW_KEY_S) direction = {0,1};
        if (key == GLFW_KEY_D) direction = {1,0};
        walking = true;
    }
}

void UpdateScene(float dt)
{
    const bool plus = held[GLFW_KEY_EQUAL] || held[GLFW_KEY_KP_ADD];
    const bool minus = held[GLFW_KEY_MINUS] || held[GLFW_KEY_KP_SUBTRACT];
    walkSpeed = std::clamp(walkSpeed + (float(plus)-float(minus))*1.5f*dt, 0.45f, 2.8f);
    doorOpen = std::clamp(doorOpen + (doorTarget ? 2.0f : -2.0f)*dt, 0.0f, 1.0f);
    float sign = held[GLFW_KEY_LEFT_SHIFT] || held[GLFW_KEY_RIGHT_SHIFT] ? -1.0f : 1.0f;
    if (held[GLFW_KEY_X]) cameraPosition.x = std::clamp(cameraPosition.x + sign*2.5f*dt,-14.0f,14.0f);
    if (held[GLFW_KEY_Z]) cameraPosition.z = std::clamp(cameraPosition.z + sign*2.5f*dt,-14.0f,14.0f);
    if (held[GLFW_KEY_Y])
    {
        glm::mat4 orbit = glm::rotate(glm::mat4(1),glm::radians(sign*55.0f*dt),glm::vec3(0,1,0));
        cameraPosition = glm::vec3(orbit*glm::vec4(cameraPosition,1));
    }

    // 프레임이 길어도 장애물 모서리나 윗면을 건너뛰지 않게 짧게 나눠 검사한다.
    int steps = std::max(1,static_cast<int>(std::ceil(dt*120.0f)));
    float tick = dt/steps;
    for (int step = 0; step < steps; ++step)
    {
        if (walking)
        {
            glm::vec2 next = position + direction*walkSpeed*tick;
            bool blocked = std::abs(next.x) > halfWidth-robotRadius
                        || std::abs(next.y) > halfDepth-robotRadius;
            for (const Obstacle& obstacle : obstacles)
                if (feet < obstacle.size.y-0.001f && Overlaps(next,obstacle)) blocked = true;
            if (blocked) direction = -direction;
            else
            {
                position = next;
                stepAngle = std::fmod(stepAngle + walkSpeed*275.0f*tick,360.0f);
            }
        }

        float support = SupportHeight(position,feet);
        if (grounded && feet > support + 0.001f)
        {
            grounded = false; // 장애물의 끝을 지나면 발 아래 지지가 사라진다.
            verticalSpeed = 0;
        }
        if (!grounded)
        {
            verticalSpeed -= 9.8f*tick;
            feet += verticalSpeed*tick;
            if (verticalSpeed <= 0.0f && feet <= support)
            {
                feet = support;
                verticalSpeed = 0.0f;
                grounded = walking = true;
            }
        }
    }
}

void DrawRobot(common::Shader& shader, GLint location)
{
    glm::mat4 body = glm::translate(glm::mat4(1),glm::vec3(position.x,feet,position.y));
    body = glm::rotate(body,std::atan2(direction.x,direction.y),glm::vec3(0,1,0));
    auto part = [&](const glm::mat4& parent, glm::vec3 center, glm::vec3 size, glm::vec3 color)
    {
        practice::DrawObj(practice::cube,practice::MakeModelMatrix(center,size,parent),color,location,shader);
    };
    part(body,{0,0.93f,0},{0.48f,0.58f,0.32f},{0.23f,0.63f,0.74f});
    part(body,{0,1.43f,0},{0.37f,0.38f,0.32f},{0.95f,0.78f,0.45f});
    part(body,{0,1.42f,0.20f},{0.09f,0.09f,0.13f},{0.86f,0.38f,0.22f});
    for (float side : {-1.0f,1.0f})
    {
        part(body,{side*0.085f,1.49f,0.167f},{0.045f,0.045f,0.02f},{0.12f,0.12f,0.17f});
        float swing = walking ? std::sin(glm::radians(stepAngle))*walkSpeed*15.0f : 0.0f;
        glm::mat4 arm = glm::translate(body,glm::vec3(side*0.32f,1.16f,0));
        arm = glm::rotate(arm,glm::radians(-side*swing),glm::vec3(1,0,0));
        part(arm,{0,-0.24f,0},{0.14f,0.48f,0.15f},side<0 ? glm::vec3(0.88f,0.28f,0.25f) : glm::vec3(0.28f,0.42f,0.88f));
        glm::mat4 leg = glm::translate(body,glm::vec3(side*0.13f,0.64f,0));
        leg = glm::rotate(leg,glm::radians(side*swing),glm::vec3(1,0,0));
        part(leg,{0,-0.30f,0},{0.18f,0.60f,0.20f},side<0 ? glm::vec3(0.66f,0.25f,0.69f) : glm::vec3(0.26f,0.70f,0.42f));
        part(leg,{0,-0.60f,0.06f},{0.20f,0.08f,0.31f},{0.21f,0.24f,0.31f});
    }
}

void DrawAll(common::Shader& shader, int width, int height)
{
    GLint location = shader.uniformLocation("model");
    shader.setMatrix(shader.uniformLocation("view"),
        glm::lookAt(cameraPosition,glm::vec3(0,1.2f,0),glm::vec3(0,1,0)));
    shader.setMatrix(shader.uniformLocation("projection"),practice::MakeProjection(width,height,true,1));
    glm::mat4 identity(1);
    glEnable(GL_CULL_FACE);
    room::DrawWalls(identity,{halfWidth,halfDepth},stageHeight,location,shader);
    room::Panel(identity,{0,0,0},{2*halfWidth,2*halfDepth},0,{1,0,0},{0.45f,0.66f,0.48f},location,shader);
    glDisable(GL_CULL_FACE);

    for (const Obstacle& obstacle : obstacles)
        practice::DrawObj(practice::cube,
            practice::MakeModelMatrix(obstacle.center+glm::vec3(0,obstacle.size.y/2,0),obstacle.size),
            obstacle.color,location,shader);
    DrawRobot(shader,location);

    // 위쪽 끝은 고정하고 아랫부분이 위로 올라가도록 문 높이를 줄인다.
    // 문이 닫혀 있어도 로봇을 볼 수 있도록 마지막에 반투명하게 그린다.
    float doorHeight = stageHeight*(1.0f-doorOpen);
    if (doorHeight > 0.001f)
    {
        glEnable(GL_BLEND);
        glBlendColor(0,0,0,0.21f);
        glBlendFunc(GL_CONSTANT_ALPHA,GL_ONE_MINUS_CONSTANT_ALPHA);
        glDepthMask(GL_FALSE);
        room::Panel(identity,{0,stageHeight-doorHeight/2,halfDepth},
            {2*halfWidth,doorHeight},-90,{1,0,0},{0.85f,0.72f,0.39f},location,shader);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
}

int main()
{
    ResetScene();
    return practice::RunScene("homework23 - Robot stage",KeyCallback,UpdateScene,DrawAll);
}

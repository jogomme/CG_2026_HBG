#include "Common/RoomScene.h"
#include <array>

constexpr float halfWidth = 3.0f;
constexpr float halfDepth = 3.2f;
constexpr float roomHeight = 4.2f;
constexpr float radius = 0.19f;
const float cubeSize[] = {0.52f, 0.70f, 0.91f};
const float cubeZ[] = {-1.9f, 0.0f, 1.9f};

struct Ball
{
    glm::vec3 position{};
    glm::vec3 velocity{};
};
std::array<Ball,5> balls{};
int ballCount = 0;
float roomZ = 0.0f;
float roomYaw = 0.0f;
float roll = 0.0f;
float floorAngle = 0.0f;
float drop = 0.0f;
float dropSpeed = 0.0f;
bool floorOpen = false;
bool falling = false;
bool dragging = false;
double lastMouseX = 0.0;
bool held[GLFW_KEY_LAST+1]{};

// 공의 반지름만큼 벽 안쪽에서 중심을 반사한다. 넘어간 거리도 돌려주므로 벽에 붙지 않는다.
void Reflect(float& position, float& velocity, float minimum, float maximum)
{
    if (position < minimum)
    {
        position = 2.0f * minimum - position;
        velocity = std::abs(velocity);
    }
    else if (position > maximum)
    {
        position = 2.0f * maximum - position;
        velocity = -std::abs(velocity);
    }
}

void AddBall()
{
    if (ballCount >= static_cast<int>(balls.size()) || floorOpen || falling) return;
    float i = static_cast<float>(ballCount);
    balls[ballCount++] = {{-1.6f+0.70f*i,2.2f+0.16f*i,-0.8f+0.26f*i},
                         {1.2f+0.19f*i,0.8f+0.21f*i,1.4f-0.13f*i}};
}

void MouseButton(GLFWwindow* window, int button, int action, int)
{
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;
    dragging = action == GLFW_PRESS;
    double y = 0;
    glfwGetCursorPos(window, &lastMouseX, &y);
}

void MouseMove(GLFWwindow*, double x, double)
{
    if (dragging && !falling)
    {
        // 오른쪽으로 드래그하면 z축 기준 시계 방향으로 굴린다.
        roll = std::clamp(roll + static_cast<float>(x-lastMouseX)*0.65f, -220.0f, 220.0f);
    }
    lastMouseX = x;
}

void KeyCallback(GLFWwindow* window, int key, int, int action, int)
{
    if (key >= 0 && key <= GLFW_KEY_LAST && action != GLFW_REPEAT)
        held[key] = action == GLFW_PRESS;
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GLFW_TRUE);
    else if (key == GLFW_KEY_B) AddBall();
    else if (key == GLFW_KEY_O) floorOpen = !floorOpen;
    else if (key == GLFW_KEY_R)
    {
        floorOpen = falling = dragging = false;
        floorAngle = drop = dropSpeed = roll = roomZ = roomYaw = 0.0f;
        ballCount = 0;
        AddBall();
    }
}

void UpdateScene(float dt)
{
    static bool ready = false;
    if (!ready)
    {
        GLFWwindow* window = glfwGetCurrentContext();
        glfwSetMouseButtonCallback(window, MouseButton);
        glfwSetCursorPosCallback(window, MouseMove);
        AddBall();
        ready = true;
    }

    float sign = held[GLFW_KEY_LEFT_SHIFT] || held[GLFW_KEY_RIGHT_SHIFT] ? -1.0f : 1.0f;
    if (held[GLFW_KEY_Z]) roomZ = std::clamp(roomZ + sign*2.0f*dt, -6.0f, 3.0f);
    if (held[GLFW_KEY_Y]) roomYaw = std::fmod(roomYaw - sign*65.0f*dt, 360.0f);
    floorAngle = std::clamp(floorAngle + (floorOpen ? 220.0f : -220.0f)*dt, 0.0f, 90.0f);
    if (floorAngle > 70.0f) falling = true;
    if (!floorOpen && floorAngle == 0.0f && falling)
    {
        falling = false;
        drop = dropSpeed = 0.0f;
        ballCount = 0;
        AddBall();
    }

    if (falling)
    {
        dropSpeed += 9.8f * dt;
        drop -= dropSpeed * dt;
    }

    for (int i = 0; i < ballCount; ++i)
    {
        Ball& ball = balls[i];
        if (falling) ball.velocity.y -= 9.8f * dt;
        ball.position += ball.velocity * dt;
        // 앞면은 그리지 않지만 공은 직육면체의 앞쪽 경계에서도 반사된다.
        Reflect(ball.position.x, ball.velocity.x, -halfWidth+radius, halfWidth-radius);
        Reflect(ball.position.z, ball.velocity.z, -halfDepth+radius, halfDepth-radius);
        if (!falling) Reflect(ball.position.y, ball.velocity.y, radius, roomHeight-radius);
    }
}

// 90도마다 다음 모서리로 지지점이 바뀐다. 모서리를 중심으로 굴러 중심이 들렸다 내려온다.
glm::vec2 RollCenter(float degrees, float size)
{
    float angle = std::abs(degrees);
    float quarter = std::floor(angle/90.0f);
    float a = glm::radians(angle - quarter*90.0f);
    float x = size*(quarter + 0.5f + 0.5f*(std::sin(a)-std::cos(a)));
    float y = size*0.5f*(std::cos(a)+std::sin(a));
    return {degrees < 0.0f ? -x : x, y};
}

void DrawAll(common::Shader& shader, int width, int height)
{
    GLint modelLocation = shader.uniformLocation("model");
    shader.setMatrix(shader.uniformLocation("view"),
        glm::lookAt(glm::vec3(0,3.5f,11.5f), glm::vec3(0,1.9f,0), glm::vec3(0,1,0)));
    shader.setMatrix(shader.uniformLocation("projection"), practice::MakeProjection(width,height,true,1));
    glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(0,0,roomZ));
    model = glm::translate(model, glm::vec3(0,roomHeight/2,0));
    model = glm::rotate(model, glm::radians(roomYaw), glm::vec3(0,1,0));
    model = glm::translate(model, glm::vec3(0,-roomHeight/2,0));

    glEnable(GL_CULL_FACE);
    room::DrawWalls(model, {halfWidth,halfDepth}, roomHeight, modelLocation, shader);
    for (float side : {-1.0f,1.0f})
    {
        // 양쪽 바깥 모서리에 경첩을 둬 바닥 두 장이 아래로 열린다.
        glm::mat4 hinge = glm::translate(model, glm::vec3(side*halfWidth,0,0));
        hinge = glm::rotate(hinge, glm::radians(side*floorAngle), glm::vec3(0,0,1));
        room::Panel(hinge, {-side*halfWidth/2,0,0}, {halfWidth,2*halfDepth},
                    0, {1,0,0}, {0.76f,0.68f,0.46f}, modelLocation, shader);
    }
    glDisable(GL_CULL_FACE);

    for (int i = 0; i < 3; ++i)
    {
        glm::vec2 center = RollCenter(roll, cubeSize[i]);
        glm::mat4 cubeModel = glm::translate(model, glm::vec3(center.x,center.y+drop,cubeZ[i]));
        cubeModel = glm::rotate(cubeModel, glm::radians(-roll), glm::vec3(0,0,1));
        cubeModel = glm::scale(cubeModel, glm::vec3(cubeSize[i]));
        practice::DrawObj(practice::cube, cubeModel, {0.89f,0.25f+0.12f*i,0.15f}, modelLocation, shader);
    }
    for (int i = 0; i < ballCount; ++i)
        practice::DrawObj(practice::sphere,
            practice::MakeModelMatrix(balls[i].position, glm::vec3(radius), model),
            {0.18f+0.12f*i,0.40f,0.95f-0.10f*i}, modelLocation, shader);
}

int main()
{
    return practice::RunScene("homework21 - Bouncing balls", KeyCallback, UpdateScene, DrawAll);
}

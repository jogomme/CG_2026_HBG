#include "../homework19/TankScene.h"

// 화면을 나누어도 탱크는 하나다. UpdateScene은 한 번만 호출하고 DrawWorld만 세 번 호출한다.
void DrawViewport(common::Shader& shader, int x, int y, int width, int height,
                  const glm::mat4& view, bool perspective, const glm::vec3& background)
{
    if (width <= 0 || height <= 0) return;
    glViewport(x,y,width,height);
    // viewport만 바꾸면 glClear는 전체 화면에 영향을 준다. scissor로 지울 영역을 제한한다.
    glScissor(x,y,width,height);
    glClearColor(background.r, background.g, background.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.setMatrix(shader.uniformLocation("view"), view);
    float aspect = static_cast<float>(width)/height;
    shader.setMatrix(shader.uniformLocation("projection"),
        practice::MakeProjection(width,height,perspective,4.7f/std::min(aspect,1.0f)));
    tank::DrawWorld(shader);
}

void DrawAll(common::Shader& shader, int width, int height)
{
    const int left = width*3/5;
    const int bottom = height/2;
    glEnable(GL_SCISSOR_TEST);
    DrawViewport(shader, 0, 0, left-1, height, tank::MakeViewMatrix(), true, {0.87f,0.92f,0.98f});
    // 위에서 xz를 볼 때 위쪽 기준은 -z, 앞에서 xy를 볼 때 위쪽 기준은 +y다.
    DrawViewport(shader, left+1, bottom+1, width-left-1, height-bottom-1,
        glm::lookAt(glm::vec3(0,12,0), glm::vec3(0), glm::vec3(0,0,-1)), false, {0.86f,0.94f,0.89f});
    DrawViewport(shader, left+1, 0, width-left-1, bottom-1,
        glm::lookAt(glm::vec3(0,1,12), glm::vec3(0,1,0), glm::vec3(0,1,0)), false, {0.97f,0.90f,0.86f});
    glDisable(GL_SCISSOR_TEST);
    glViewport(0,0,width,height);
    glClearColor(0.20f,0.23f,0.28f,1.0f);
}

int main()
{
    return practice::RunScene("homework22 - Three viewports", tank::KeyCallback, tank::UpdateScene, DrawAll);
}

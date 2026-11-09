#pragma once
#include "PracticeScene.h"

namespace room {
/*
 * [기능] 한쪽에서만 보이는 사각형을 그린다.
 * [사용 상황] 상자의 안쪽 벽, 움직이는 바닥이나 앞문을 그릴 때 사용한다.
 * [매개변수] parent: 방의 모델 행렬. center: 방 좌표의 중심. size: 가로/깊이 길이.
 * degrees: 회전각(도). axis: 회전축. color: RGB(0~1).
 * location: model uniform 위치. shader: 현재 사용 중인 셰이더.
 * [반환값] 없음. [사용 예시] Panel(glm::mat4(1), {0,0,0}, {6,6}, 0, {1,0,0}, {0,1,0}, location, shader);
 * [주의사항] 평면은 xz 평면, 앞면 법선은 +y. 정점에는 S→R→T→parent 순으로 작용한다.
 * 앞면만 그리려면 호출 전에 GL_CULL_FACE를 켠다.
 * [관련 실습] 21, 23번.
 */
inline void Panel(const glm::mat4& parent, glm::vec3 center, glm::vec2 size,
                  float degrees, glm::vec3 axis, glm::vec3 color, GLint location, common::Shader& shader)
{
    glm::mat4 model = glm::translate(parent, center);
    model = glm::rotate(model, glm::radians(degrees), axis);
    model = glm::scale(model, glm::vec3(size.x, 1.0f, size.y));
    practice::DrawObj(practice::plane, model, color, location, shader);
}

/*
 * [기능] 방의 좌우·뒤쪽·위쪽 벽을 안쪽 방향으로 그린다.
 * [사용 상황] 카메라가 방 바깥에 있어도 가까운 벽의 뒷면이 내부를 가리지 않게 한다.
 * [매개변수] parent: 방 변환. half: x/z의 반폭·반깊이. height: 방 높이(장면 단위).
 * location: model uniform 위치. shader: 현재 사용 중인 셰이더.
 * [반환값] 없음. [사용 예시] DrawWalls(glm::mat4(1), {3,3}, 4, location, shader);
 * [주의사항] 바닥과 앞문은 따로 그린다. GL_CULL_FACE를 켜고 사용한다.
 * [관련 실습] 21, 23번.
 */
inline void DrawWalls(const glm::mat4& parent, glm::vec2 half, float height,
                      GLint location, common::Shader& shader)
{
    Panel(parent, {-half.x,height/2,0}, {height,2*half.y}, -90, {0,0,1}, {0.32f,0.63f,0.49f}, location, shader);
    Panel(parent, {half.x,height/2,0}, {height,2*half.y}, 90, {0,0,1}, {0.35f,0.48f,0.70f}, location, shader);
    Panel(parent, {0,height/2,-half.y}, {2*half.x,height}, 90, {1,0,0}, {0.68f,0.43f,0.39f}, location, shader);
    Panel(parent, {0,height,0}, {2*half.x,2*half.y}, 180, {1,0,0}, {0.63f,0.58f,0.68f}, location, shader);
}
}

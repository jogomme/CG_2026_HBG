#pragma once

#include "PracticeScene.h"
#include "RotationMotion.h"

#include <algorithm>
#include <cmath>

namespace tank
{
// 부모 기준으로 부품의 관절 위치를 정하고 그 자리에서 돌린다.
// 반환값에는 부품 크기가 없으므로 다음 부품의 부모로 사용해도 된다.
inline glm::mat4 PartFrame(const glm::mat4& parent, const glm::vec3& offset,
                           float degrees = 0.0f, const glm::vec3& axis = glm::vec3(0, 1, 0))
{
    return glm::translate(parent, offset) * motion::RotationMatrix(degrees, axis);
}

// 관절에서 meshOffset만큼 떨어진 위치에 실제 크기로 육면체를 그린다.
inline void DrawCube(common::Shader& shader, GLint modelLocation, const glm::mat4& frame,
                     const glm::vec3& meshOffset, const glm::vec3& size, const glm::vec3& color)
{
    const glm::mat4 model = practice::MakeModelMatrix(meshOffset, size, frame);
    practice::DrawObj(practice::cube, model, color, modelLocation, shader);
}

// side에 -1과 +1을 넣으면 반대편 두 위치가 나온다.
// 19번과 같은 방향으로 회전해 90도에서 +X 쪽 부품은 +Z 쪽에 놓인다.
inline glm::vec3 PairedOrbitOffset(float side, float radius, float height, float degrees)
{
    return motion::OrbitPosition(glm::vec3(0.0f, height, 0.0f),
                                 glm::vec3(side * radius, 0.0f, 0.0f),
                                 glm::vec3(0, -1, 0), degrees);
}

// 현재 각도를 목표까지 이동시킨다. 목표에 닿으면 true를 반환한다.
// 자리 교환 중에는 360도로 나누지 않고 목표 도착 후에만 각도를 정리한다.
inline bool AdvanceAngleTo(float& degrees, float target, float degreesPerSecond, float dt)
{
    const float step = std::abs(degreesPerSecond) * std::max(dt, 0.0f);
    if (degrees < target)
        degrees = std::min(degrees + step, target);
    else if (degrees > target)
        degrees = std::max(degrees - step, target);
    return degrees == target;
}

// 카메라의 위치를 두고, target을 향하는 방향에서 좌우로 고개를 돌린다.
inline glm::mat4 MakeYawView(const glm::vec3& eye, const glm::vec3& target, float yawDegrees)
{
    const glm::vec3 toTarget = target - eye;
    const glm::vec3 forward = glm::dot(toTarget, toTarget) > 0.0f
        ? glm::normalize(toTarget) : glm::vec3(0, 0, -1);
    const glm::vec3 worldUp = std::abs(forward.y) > 0.9999f
        ? glm::vec3(0, 0, -1) : glm::vec3(0, 1, 0);
    const glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
    const glm::vec3 up = glm::cross(right, forward);
    const glm::vec3 direction = glm::mat3(motion::RotationMatrix(yawDegrees, up)) * forward;
    return glm::lookAt(eye, eye + direction, up);
}
}

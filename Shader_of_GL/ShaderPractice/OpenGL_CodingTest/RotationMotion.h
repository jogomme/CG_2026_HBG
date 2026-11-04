#pragma once

#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>

namespace motion
{
// 각도는 도 단위. 길이가 0인 축은 회전 없이 처리한다.
inline glm::mat4 RotationMatrix(float degrees, const glm::vec3& axis)
{
    const glm::mat4 identity(1.0f);
    if (glm::dot(axis, axis) == 0.0f)
        return identity;
    return glm::rotate(identity, glm::radians(degrees), axis);
}

// offset은 공전각 0도에서 중심으로부터 물체까지의 상대 위치다.
inline glm::vec3 OrbitPosition(const glm::vec3& center, const glm::vec3& offset,
                              const glm::vec3& axis, float degrees)
{
    return center + glm::mat3(RotationMatrix(degrees, axis)) * offset;
}

// 모델의 localPivot이 월드의 position에 오도록 배치하고 그 점을 중심으로 자전한다.
inline glm::mat4 SpinModel(const glm::vec3& position, const glm::vec3& axis,
                           float degrees, const glm::vec3& size,
                           const glm::vec3& localPivot = glm::vec3(0.0f))
{
    const glm::mat4 identity(1.0f);
    const glm::mat4 placement = glm::translate(identity, position);
    const glm::mat4 spin = RotationMatrix(degrees, axis);
    const glm::mat4 scaling = glm::scale(identity, size);
    const glm::mat4 pivot = glm::translate(identity, -localPivot);
    return placement * spin * scaling * pivot;
}
}

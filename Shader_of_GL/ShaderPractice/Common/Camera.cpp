#include "Camera.h"
#include <GL/glm/gtc/matrix_transform.hpp>
namespace common {
Camera::Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
    : position_(position), target_(target), up_(up) {}
glm::mat4 Camera::view() const { return glm::lookAt(position_, target_, up_); }
glm::mat4 Camera::projection(int width, int height) const {
    return glm::perspective(glm::radians(45.0f), static_cast<float>(width) / height, 0.1f, 100.0f);
}
void Camera::setPose(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) {
    position_ = position; target_ = target; up_ = up;
}
}

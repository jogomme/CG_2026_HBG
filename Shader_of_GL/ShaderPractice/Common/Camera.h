#pragma once
#include <GL/glm/glm.hpp>
namespace common {
class Camera {
public:
    Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up);
    glm::mat4 view() const;
    // Requires positive framebuffer dimensions; skip minimized frames first.
    glm::mat4 projection(int width, int height) const;
    void setPose(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up);
private:
    glm::vec3 position_, target_, up_;
};
}

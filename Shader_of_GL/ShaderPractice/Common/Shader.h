#pragma once
#include "../GLCommon.h"
#include <GL/glm/glm.hpp>
namespace common {
// Reset/destroy while the owning OpenGL context is current.
class Shader {
public:
    Shader(const char* vertex, const char* fragment);
    ~Shader();
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    bool valid() const { return program_ != 0; }
    void use() const;
    GLint uniformLocation(const char* name) const;
    // Requires this program to be bound with use().
    void setMatrix(GLint location, const glm::mat4& value) const;
    void reset();
private:
    GLuint program_ = 0;
};
}

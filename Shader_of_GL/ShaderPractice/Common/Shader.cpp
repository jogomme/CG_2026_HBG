#include "Shader.h"
namespace common {
Shader::Shader(const char* vertex, const char* fragment) : program_(CreateProgram(vertex, fragment)) {}
Shader::~Shader() { reset(); }
void Shader::use() const { glUseProgram(program_); }
GLint Shader::uniformLocation(const char* name) const { return glGetUniformLocation(program_, name); }
void Shader::setMatrix(GLint location, const glm::mat4& value) const {
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}
void Shader::reset() {
    if (program_) { glDeleteProgram(program_); program_ = 0; }
}
}

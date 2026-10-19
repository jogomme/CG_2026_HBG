#pragma once

// Optional defaults. Custom GLSL sources can be passed to Shader unchanged.
// Layout matches configurePositionColorAttributes(): xyz/rgb at locations 0/1.
namespace common {
namespace shader_sources {
inline constexpr const char* positionColorVertex = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 ourColor;
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
}
)glsl";
inline constexpr const char* positionColorFragment = R"glsl(
#version 330 core
in vec3 ourColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(ourColor, 1.0);
}
)glsl";
}
}

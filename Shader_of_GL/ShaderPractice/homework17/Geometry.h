#pragma once
#include "Scene.h"
#include <GL/glm/gtc/constants.hpp>
#include <vector>
#include <cmath>

namespace homework17 {
// GLUtils가 기대하는 배열 배치는 위치 xyz, 색상 rgb의 float 6개입니다.
// 두 정점마다 선분 하나가 완성되므로 모든 도형을 GL_LINES로 그릴 수 있습니다.
using LineVertices = std::vector<float>;

inline void addLine(LineVertices& data, const glm::vec3& from,
                    const glm::vec3& to, const glm::vec3& color) {
    for (const auto& point : {from, to}) {
        data.insert(data.end(), {point.x, point.y, point.z, color.r, color.g, color.b});
    }
}

inline LineVertices makeAxes() {
    LineVertices data;
    addLine(data, {-2.4f, 0, 0}, {2.4f, 0, 0}, {0.85f, 0.1f, 0.1f});
    addLine(data, {0, -1.7f, 0}, {0, 1.7f, 0}, {0.1f, 0.65f, 0.1f});
    addLine(data, {0, 0, -1.7f}, {0, 0, 1.7f}, {0.1f, 0.25f, 0.9f});
    return data;
}

inline LineVertices makeCube() {
    LineVertices data;
    constexpr float h = 0.42f;
    // 다른 두 좌표를 고정하고 한 좌표의 부호만 바꾸면 육면체 모서리가 됩니다.
    // 축마다 4개씩 총 12개 모서리를 만들며 삼각형 대각선은 생성하지 않습니다.
    for (int a : {-1, 1}) for (int b : {-1, 1}) {
        const float u = h * a, v = h * b;
        addLine(data, {-h, u, v}, {h, u, v}, {0.85f, 0.1f, 0.1f});
        addLine(data, {u, -h, v}, {u, h, v}, {0.1f, 0.65f, 0.1f});
        addLine(data, {u, v, -h}, {u, v, h}, {0.1f, 0.25f, 0.9f});
    }
    return data;
}

inline LineVertices makeCone() {
    LineVertices data;
    constexpr int segments = 24, rings = 5;
    constexpr float radius = 0.45f, height = 0.9f;
    const glm::vec3 color(0.15f);
    // 원뿔 꼭짓점은 +y 방향입니다. 위로 올라갈수록 반지름이 선형으로 줄어듭니다.
    // 수평 원과 모선을 별도로 만들면 원뿔의 3차원 구조를 철망처럼 볼 수 있습니다.
    for (int ring = 0; ring < rings; ++ring) {
        const float t = static_cast<float>(ring) / rings;
        const float y = -height * 0.5f + height * t;
        const float r = radius * (1.0f - t);
        for (int i = 0; i < segments; ++i) {
            const float a = glm::two_pi<float>() * i / segments;
            const float b = glm::two_pi<float>() * (i + 1) / segments;
            addLine(data, {r * std::cos(a), y, r * std::sin(a)},
                          {r * std::cos(b), y, r * std::sin(b)}, color);
        }
    }
    for (int i = 0; i < segments; ++i) {
        const float a = glm::two_pi<float>() * i / segments;
        addLine(data, {radius * std::cos(a), -height * 0.5f, radius * std::sin(a)},
                      {0, height * 0.5f, 0}, color);
    }
    return data;
}

inline LineVertices makeSphere() {
    LineVertices data;
    constexpr int slices = 24, stacks = 12;
    const glm::vec3 color(0.2f, 0.2f, 0.65f);
    // 위도/경도 매개변수로 구 위의 점을 만들고 이웃한 점을 연결합니다.
    const auto point = [](int row, int column) {
        const float latitude = -glm::half_pi<float>() + glm::pi<float>() * row / stacks;
        const float longitude = glm::two_pi<float>() * column / slices;
        return 0.48f * glm::vec3(std::cos(latitude) * std::cos(longitude),
                                std::sin(latitude), std::cos(latitude) * std::sin(longitude));
    };
    for (int row = 0; row <= stacks; ++row) for (int col = 0; col < slices; ++col) {
        if (row > 0 && row < stacks) addLine(data, point(row, col), point(row, col + 1), color);
        if (row < stacks) addLine(data, point(row, col), point(row + 1, col), color);
    }
    return data;
}

inline LineVertices makePyramid() {
    LineVertices data;
    const glm::vec3 base[] = {{-0.42f, -0.42f, -0.42f}, {0.42f, -0.42f, -0.42f},
                              {0.42f, -0.42f, 0.42f}, {-0.42f, -0.42f, 0.42f}};
    for (int i = 0; i < 4; ++i) {
        addLine(data, base[i], base[(i + 1) % 4], {0.65f, 0.2f, 0.1f});
        addLine(data, base[i], {0, 0.48f, 0}, {0.65f, 0.2f, 0.1f});
    }
    return data;
}
}

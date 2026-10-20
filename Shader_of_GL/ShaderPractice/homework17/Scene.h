#pragma once
#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>
#include <array>

namespace homework17 {
enum class Shape { Cone, Cube, Sphere, Pyramid };

// 이 상태는 과제 17의 장면에만 필요하므로 Common으로 옮기지 않습니다.
// 위치와 자세를 분리하면 제자리 자전과 원점 주위 공전의 차이를 직접 표현할 수 있습니다.
struct Object {
    glm::vec3 position{0.0f};
    glm::mat4 orientation{1.0f};
    float scale = 1.0f;
    Shape shape = Shape::Cone;
};

struct Scene {
    std::array<Object, 2> objects;

    Scene() { reset(); }
    void reset() {
        objects = {};
        objects[0].position = glm::vec3(-1.25f, 0.0f, 0.0f);
        objects[1].position = glm::vec3( 1.25f, 0.0f, 0.0f);
        objects[0].shape = Shape::Cone;
        objects[1].shape = Shape::Cube;
    }
};

inline glm::mat4 sceneTilt() {
    // 열벡터에 행렬을 곱할 때 오른쪽 변환부터 적용됩니다.
    // 따라서 Rx(30) * Ry(30)는 y축 30도 회전 후 x축 30도 회전입니다.
    // 축과 도형에 같은 행렬을 적용해야 도형이 기울어진 좌표계 위에 놓입니다.
    return glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(1, 0, 0))
         * glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0, 1, 0));
}

inline glm::mat4 objectModel(const Object& object) {
    // 모델 좌표 → 크기 → 자체 자세 → 장면 내 위치 → 공통 기울기 순서입니다.
    // T가 R보다 왼쪽에 있으므로 자전해도 도형 중심의 위치는 바뀌지 않습니다.
    return sceneTilt()
         * glm::translate(glm::mat4(1.0f), object.position)
         * object.orientation
         * glm::scale(glm::mat4(1.0f), glm::vec3(object.scale));
}
}

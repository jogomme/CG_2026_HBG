#pragma once
#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>
#include <array>
#include <algorithm>

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

// GLFW 키 번호와 분리하여 행렬 계산을 OpenGL 창 없이도 검사할 수 있게 합니다.
enum class Transform { SpinX, SpinY, OrbitY, LocalScale, OriginScale, MoveX, MoveY };

struct Scene {
    std::array<Object, 2> objects;
    int selected = 3; // 1: 왼쪽, 2: 오른쪽, 3: 두 도형
    bool alternateShapes = false;

    Scene() { reset(); }
    void reset() {
        selected = 3;
        alternateShapes = false;
        objects = {};
        objects[0].position = glm::vec3(-1.25f, 0.0f, 0.0f);
        objects[1].position = glm::vec3( 1.25f, 0.0f, 0.0f);
        objects[0].shape = Shape::Cone;
        objects[1].shape = Shape::Cube;
    }

    void select(int number) {
        if (number >= 1 && number <= 3) selected = number;
    }

    void changeShapes() {
        // 도형 데이터만 바꾸므로 현재 위치, 크기, 자세는 그대로 유지됩니다.
        // 선택 번호와 관계없이 두 도형을 함께 원뿔/육면체 ↔ 구/사각뿔로 바꿉니다.
        alternateShapes = !alternateShapes;
        objects[0].shape = alternateShapes ? Shape::Sphere : Shape::Cone;
        objects[1].shape = alternateShapes ? Shape::Pyramid : Shape::Cube;
    }

    void transform(Transform command, bool uppercase) {
        const float direction = uppercase ? -1.0f : 1.0f;
        const float angle = glm::radians(5.0f) * direction;
        const float factor = uppercase ? 1.0f / 1.05f : 1.05f;
        for (std::size_t i = 0; i < objects.size(); ++i) {
            if (selected != 3 && selected != static_cast<int>(i) + 1) continue;
            auto& object = objects[i];
            switch (command) {
            case Transform::SpinX:
            case Transform::SpinY: {
                const glm::vec3 axis = command == Transform::SpinX
                    ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
                // 자세 행렬의 오른쪽에 회전을 곱하면 도형 자신의 로컬 축 기준 자전입니다.
                // 위치는 그대로 두므로 중심이 원점 주위를 도는 공전과 구별됩니다.
                object.orientation = glm::rotate(object.orientation, angle, axis);
                break;
            }
            case Transform::OrbitY: {
                // 원점 기준 회전은 위치와 자세 양쪽에 같은 Ry를 왼쪽에서 적용합니다.
                // R * T(p) = T(Rp) * R 관계를 이용해 T/R로 분리된 상태를 갱신합니다.
                const glm::mat4 rotation = glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0));
                object.position = glm::vec3(rotation * glm::vec4(object.position, 1));
                object.orientation = rotation * object.orientation;
                break;
            }
            case Transform::LocalScale:
            case Transform::OriginScale: {
                // 0 또는 음수 크기는 피하고, 반복 입력으로 무한히 커지는 것도 제한합니다.
                const float next = std::clamp(object.scale * factor, 0.15f, 3.0f);
                if (command == Transform::OriginScale) {
                    // b/B는 S * T(p) = T(sp) * S이므로 중심 위치도 배율만큼 변합니다.
                    // 제한에 걸렸을 때도 실제 적용된 배율을 사용하여 크기와 위치를 일치시킵니다.
                    object.position *= next / object.scale;
                }
                object.scale = next;
                break;
            }
            case Transform::MoveX:
                // PDF의 d/D 순서에 맞춰 소문자는 왼쪽, 대문자는 오른쪽입니다.
                object.position.x -= 0.08f * direction;
                break;
            case Transform::MoveY:
                object.position.y += 0.08f * direction;
                break;
            }
        }
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

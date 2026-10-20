#include "../Scene.h"
#include "../Geometry.h"
#include <cmath>
#include <cstdlib>
#include <iostream>

namespace {
int checks = 0;
void require(bool condition, const char* message) {
    ++checks;
    if (!condition) { std::cerr << "실패: " << message << '\n'; std::exit(1); }
}
bool near(float a, float b) { return std::abs(a - b) < 0.0001f; }
bool near(const glm::vec3& a, const glm::vec3& b) { return glm::length(a - b) < 0.0001f; }
bool near(const glm::mat4& a, const glm::mat4& b) {
    for (int c = 0; c < 4; ++c) for (int r = 0; r < 4; ++r)
        if (!near(a[c][r], b[c][r])) return false;
    return true;
}
}

int main() {
    using namespace homework17;
    Scene scene;
    const auto initial = scene.objects;
    scene.select(1);
    scene.transform(Transform::SpinX, false);
    require(near(scene.objects[0].position, initial[0].position), "자전은 중심을 이동시키지 않는다");
    require(!near(scene.objects[0].orientation, initial[0].orientation), "선택한 도형은 자전한다");
    require(near(scene.objects[1].orientation, initial[1].orientation), "비선택 도형은 변하지 않는다");
    scene.transform(Transform::SpinX, true);
    require(near(scene.objects[0].orientation, initial[0].orientation), "x와 X는 반대 회전이다");
    scene.select(2);
    scene.transform(Transform::SpinY, false);
    scene.transform(Transform::SpinY, true);
    require(near(scene.objects[1].orientation, initial[1].orientation), "y와 Y는 반대 회전이다");
    scene.select(3);
    scene.transform(Transform::OrbitY, false);
    require(near(glm::length(scene.objects[0].position), 1.25f), "공전은 원점까지 거리를 유지한다");
    require(!near(scene.objects[0].position, initial[0].position), "공전은 중심 위치를 바꾼다");
    scene.transform(Transform::OrbitY, true);
    require(near(scene.objects[0].position, initial[0].position), "r/R 공전은 서로 상쇄된다");
    scene.reset();
    scene.transform(Transform::LocalScale, false);
    require(near(scene.objects[0].position, initial[0].position), "a는 제자리 확대이다");
    require(near(scene.objects[0].scale, 1.05f), "a는 도형을 확대한다");
    scene.transform(Transform::LocalScale, true);
    require(near(scene.objects[0].scale, 1.0f), "A는 반대 배율이다");
    scene.transform(Transform::OriginScale, false);
    require(near(scene.objects[0].position, initial[0].position * 1.05f), "b는 위치도 확대한다");
    scene.transform(Transform::OriginScale, true);
    require(near(scene.objects[0].position, initial[0].position), "B는 원점 기준 축소이다");
    scene.transform(Transform::MoveX, false);
    require(scene.objects[0].position.x < initial[0].position.x, "d는 왼쪽 이동이다");
    scene.transform(Transform::MoveX, true);
    scene.transform(Transform::MoveY, false);
    require(scene.objects[0].position.y > 0, "e는 위로 이동한다");
    scene.transform(Transform::MoveY, true);
    require(near(scene.objects[0].position, initial[0].position), "반대 방향 이동은 상쇄된다");
    const auto pose = scene.objects[0];
    scene.changeShapes();
    require(scene.objects[0].shape == Shape::Sphere && scene.objects[1].shape == Shape::Pyramid,
            "c는 선택과 관계없이 두 도형을 바꾼다");
    require(near(scene.objects[0].position, pose.position), "도형 변경은 자세를 보존한다");
    for (int i = 0; i < 200; ++i) scene.transform(Transform::OriginScale, true);
    require(scene.objects[0].scale >= 0.15f, "축소해도 크기가 0이 되지 않는다");
    scene.reset();
    require(scene.selected == 3 && scene.objects[0].shape == Shape::Cone &&
            near(scene.objects[0].scale, 1), "s 초기 상태 복구");
    require(near(glm::vec3(objectModel(scene.objects[0]) * glm::vec4(0, 0, 0, 1)),
                 glm::vec3(sceneTilt() * glm::vec4(initial[0].position, 1))), "공통 기울기와 모델 중심");
    for (const auto& data : {makeAxes(), makeCube(), makeCone(), makeSphere(), makePyramid()}) {
        require(!data.empty() && data.size() % 12 == 0, "선분은 xyz/rgb 정점 두 개로 구성된다");
        for (float value : data) require(std::isfinite(value), "도형 데이터에 NaN/무한대가 없다");
    }
    std::cout << "장면/기하 검사 통과: " << checks << '\n';
}

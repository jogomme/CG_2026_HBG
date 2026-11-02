#include "Camera.h"
#include <GL/glm/gtc/matrix_transform.hpp>
namespace common {
/*
 * [기능]
 * 카메라 위치, 바라볼 지점, 위쪽 방향을 기억하는 Camera 객체를 만든다.
 *
 * [사용 상황]
 * 고정 시점·카메라 이동·카메라 공전을 구현할 때 사용할 기준 시점을 저장한다.
 *
 * [매개변수]
 * position: 카메라의 월드 좌표(x,y,z, 장면과 같은 거리 단위).
 * target: 바라볼 지점의 월드 좌표. 방향 벡터가 아니라 지점이다.
 * up: 카메라 위쪽의 기준 방향. 단위 없음. 보통 (0,1,0).
 *
 * [반환값]
 * 생성자이므로 반환값 없음. view()와 projection()을 제공하는 객체를 만든다.
 *
 * [사용 예시]
 * common::Camera camera(
 *     glm::vec3(0.0f, 2.0f, 5.0f),
 *     glm::vec3(0.0f, 0.0f, 0.0f),
 *     glm::vec3(0.0f, 1.0f, 0.0f)
 * );
 * glm::mat4 view = camera.view();
 *
 * [주의사항]
 * position과 target이 같으면 바라보는 방향을 정할 수 없다. up도 영벡터이거나 보는 방향과 나란하면 안 된다.
 * 이 클래스는 GPU 호출 없이 숫자만 저장한다. 창 생성 전에도 객체를 만들 수 있다.
 * 생성만으로 화면이 바뀌지 않는다. view()/projection() 결과를 셰이더에 전달해야 한다.
 *
 * [관련 실습]
 * 16번 직접 사용, 19번 카메라 원리에 응용 가능(19번은 직접 lookAt 사용). 21번 템플릿 사용.
 *
 * [함께 사용할 헤더·함수]
 * Common/Camera.h
 * view, projection, setPose, Shader::setMatrix
 *
 */
Camera::Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
    : position_(position), target_(target), up_(up) {}
/*
 * [기능]
 * 저장한 카메라 시점으로 월드 좌표를 카메라 좌표로 옮기는 view 행렬을 만든다.
 *
 * [사용 상황]
 * 카메라 이동·자전·공전 이후 화면을 카메라 시점으로 그릴 때 사용한다.
 *
 * [매개변수]
 * 인자 없음. 생성자나 setPose로 저장한 position_, target_, up_을 사용한다.
 *
 * [반환값]
 * glm::lookAt(position_, target_, up_)로 만든 glm::mat4. 카메라 물체의 모델 행렬이 아니라 장면을 보는 변환이다.
 *
 * [사용 예시]
 * glm::mat4 view = camera.view();
 * shader.use();
 * shader.setMatrix(shader.uniformLocation("view"), view);
 *
 * [주의사항]
 * 위치가 이동했다면 setPose로 먼저 갱신한다. target을 고정하면 카메라가 이동할 때 계속 그 지점을 향해 고개도 바뀐다.
 * 평행 이동하면서 보는 방향을 유지하려면 position과 target에 같은 이동량을 더한다.
 * 기본 OpenGL/GLM view에서 카메라는 자신의 -z 방향을 바라본다. 월드 +z가 항상 화면 위쪽이라는 뜻은 아니다.
 *
 * [관련 실습]
 * 16번, 19번 카메라 원리에 응용. 21번 템플릿.
 *
 * [함께 사용할 헤더·함수]
 * Common/Camera.h, Common/Shader.h
 * setPose, Shader::setMatrix
 *
 */
glm::mat4 Camera::view() const { return glm::lookAt(position_, target_, up_); }
/*
 * [기능]
 * 화면 비율에 맞는 고정 설정의 원근 projection 행렬을 만든다.
 *
 * [사용 상황]
 * 창 크기가 변해도 물체가 옆으로 늘어나지 않는 원근 화면을 만들 때 사용한다.
 *
 * [매개변수]
 * width: 현재 framebuffer 너비(픽셀, 양수).
 * height: 현재 framebuffer 높이(픽셀, 양수). 창 크기가 아니라 glfwGetFramebufferSize 결과를 쓴다.
 *
 * [반환값]
 * 시야각 45도, 비율 width/height, near 0.1, far 100.0인 원근 glm::mat4. near/far는 장면 거리 단위이다.
 *
 * [사용 예시]
 * int width = 0;
 * int height = 0;
 * glfwGetFramebufferSize(window, &width, &height);
 * if (width > 0 && height > 0)
 * {
 *     shader.use();
 *     shader.setMatrix(shader.uniformLocation("projection"), camera.projection(width, height));
 * }
 *
 * [주의사항]
 * 최소화하면 width나 height가 0일 수 있다. 함수 내부에 방어 코드가 없으므로 호출 전에 건너뛴다.
 * 직각 투영 기능은 없다. 직각/원근 전환에는 practice::MakeProjection을 사용한다.
 * 카메라 위치를 바꾸는 함수가 아니다. 원근 투영에서 화면 크기는 카메라 깊이에도 영향을 받는다.
 *
 * [관련 실습]
 * 16번, 21번 템플릿. 18·20번은 MakeProjection으로 투영 전환.
 *
 * [함께 사용할 헤더·함수]
 * Common/Camera.h, Common/Shader.h
 * glfwGetFramebufferSize, view, Shader::setMatrix
 *
 */
glm::mat4 Camera::projection(int width, int height) const {
    return glm::perspective(glm::radians(45.0f), static_cast<float>(width) / height, 0.1f, 100.0f);
}
/*
 * [기능]
 * 카메라 위치·바라볼 지점·위쪽 방향을 새 값으로 저장한다.
 *
 * [사용 상황]
 * 키보드 카메라 이동, 대상 추적, 원점 기준 카메라 공전, 제자리 카메라 자전의 결과를 반영할 때 사용한다.
 *
 * [매개변수]
 * position: 새 카메라 월드 위치(장면 거리 단위).
 * target: 새로 바라볼 월드 지점(같은 단위).
 * up: 새 위쪽 방향(단위 없음, 영벡터 또는 시선과 평행이면 안 됨).
 *
 * [반환값]
 * 반환값 없음(void). 이후 view()가 새 시점을 계산한다.
 *
 * [사용 예시]
 * glm::vec3 eye(0.0f, 2.0f, 5.0f);
 * glm::vec3 target(0.0f, 0.0f, 0.0f);
 * glm::vec3 movement(1.0f, 0.0f, 0.0f);
 * camera.setPose(eye + movement, target + movement, glm::vec3(0.0f, 1.0f, 0.0f));
 * glm::mat4 view = camera.view();
 *
 * [주의사항]
 * 이동량을 더하는 함수가 아니라 전달한 절대값으로 덮어쓰는 함수이다. 기존 위치는 사용자가 별도 변수로 관리한다.
 * 같은 자리에서 자전하려면 position을 고정하고 target을 position + 회전시킨 앞쪽 방향으로 만든다.
 * 원점 공전은 position을 원점 둘레로 돌리고 target을 원점에 유지한다. view()를 셰이더에 다시 보내야 화면에 반영된다.
 *
 * [관련 실습]
 * 16번 Camera 도입과 연관, 19번 카메라 이동·자전·공전 응용. 기존 19번은 직접 lookAt으로 구현.
 *
 * [함께 사용할 헤더·함수]
 * Common/Camera.h
 * view, glm::rotate, Shader::setMatrix
 *
 */
void Camera::setPose(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) {
    position_ = position; target_ = target; up_ = up;
}
}


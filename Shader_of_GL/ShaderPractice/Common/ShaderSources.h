#pragma once

// 기본 GLSL 문자열입니다. 직접 작성한 GLSL 문자열을 Shader에 전달해도 됩니다.
// configurePositionColorAttributes와 짝입니다. 입력 0번은 xyz, 1번은 rgb입니다.
namespace common {
namespace shader_sources {
/*
 * [기능] positionColorVertex: GLSL vertex main의 소스 문자열입니다. C++ 호출 함수가 아닙니다.
 * [사용 상황] 위치·정점 색을 사용하고 model/view/projection으로 3D 변환하는 실습.
 * [매개변수] C++ 함수 인자 없음. GPU 입력 0번 aPos=xyz, 1번 aColor=rgb.
 * uniform model/view/projection은 4×4 행렬이며 정점에는 model → view → projection 순으로 작용합니다.
 * [반환값] GLSL main은 void. gl_Position에 clip 좌표, ourColor에 다음 단계로 보낼 색을 기록합니다.
 * [사용 예시] common::Shader shader(common::shader_sources::positionColorVertex,
 *                                 common::shader_sources::positionColorFragment);
 * [주의사항] gl_Position은 아직 픽셀 좌표가 아닙니다. 이후 GPU가 w로 나누고 viewport로 옮깁니다.
 * 세 uniform을 설정하지 않으면 기본값이 0 행렬이라 안 보일 수 있습니다. CPU에서 직접 main()을 호출하지 않습니다.
 * [관련 실습] 16~20번, 21번 템플릿. [함께 사용] Shader.h, ShaderSources.h, configurePositionColorAttributes.
 */
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
/*
 * [기능] positionColorFragment: 보간된 정점 색을 화면 색으로 쓰는 GLSL fragment main 문자열입니다.
 * [사용 상황] 정점마다 넣은 RGB를 그대로 사용하는 실습. 서로 다른 정점 색은 면 안에서 부드럽게 섞입니다.
 * [매개변수] C++ 함수 인자 없음. GPU 입력 ourColor=보간된 RGB(보통 0~1).
 * [반환값] GLSL main은 void. FragColor=vec4(ourColor,1.0)으로 불투명 RGBA를 출력합니다.
 * [사용 예시] common::Shader shader(common::shader_sources::positionColorVertex,
 *                                 common::shader_sources::positionColorFragment);
 * [주의사항] objectColor uniform은 없습니다. practice::DrawObj의 색 변경에는 RunScene의 별도 fragment를 사용합니다.
 * 이 코드는 조명·텍스처·충돌 계산을 하지 않습니다. CPU에서 직접 호출하지 않습니다.
 * [관련 실습] 16번, 21번 템플릿. 17~20번은 RunScene 내부의 색 곱셈 fragment 사용.
 * [함께 사용] Shader.h, ShaderSources.h, positionColorVertex.
 */
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


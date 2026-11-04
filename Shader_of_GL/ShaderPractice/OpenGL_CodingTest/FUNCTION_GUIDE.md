# OpenGL_CodingTest 함수 사용 안내

19번 탱크 문제에서 쓸 함수부터 정리했다. 현재 예제처럼 탱크 함수와 자전·공전 함수를 함께 쓸 때는 소스 위에 다음 줄을 추가한다.

    #include "TankPractice.h"

각도는 도 단위, 속도는 도/초, dt는 지난 프레임부터 흐른 시간(초)이다. 함수가 필요한 위치는 키 입력을 받는 KeyCallback, 값을 갱신하는 UpdateScene, 화면을 그리는 DrawAll로 나뉜다.

## 19번 탱크 함수

### tank::PartFrame(parent, offset, degrees, axis)

부모 기준에서 offset만큼 떨어진 곳에 관절을 놓고 그 자리에서 회전시킨다. parent는 부모의 행렬, degrees는 각도, axis는 회전축이다. 각도와 축을 생략하면 0도와 Y축을 사용한다.

부품의 기준 행렬을 반환한다. 내부 계산은 부모 × 이동 × 회전이다. 반환한 행렬을 다음 부품의 parent에 넘기면 부모의 움직임을 따라간다. 실제 크기는 DrawCube에서 준다.

### tank::DrawCube(shader, modelLocation, frame, meshOffset, size, color)

DrawAll에서 육면체 하나를 그린다. shader는 현재 셰이더, modelLocation은 model 행렬을 보낼 위치다. frame은 관절 행렬, meshOffset은 관절에서 큐브 중심까지의 상대 위치, size는 XYZ 배율, color는 RGB 값이다.

frame × 이동 × 신축으로 모델 행렬을 만든 뒤 큐브를 그린다. 반환값은 없다. 포신은 관절에서 앞쪽으로 meshOffset을 줘야 관절을 기준으로 회전한다.

### tank::PairedOrbitOffset(side, radius, height, degrees)

공전 중인 부품의 부모 기준 위치를 vec3로 반환한다. side는 양쪽에 각각 -1과 +1, radius는 반지름, height는 Y 높이, degrees는 공전 각도다. -Y축 기준으로 돌기 때문에 +X 쪽 부품은 90도에서 +Z, 180도에서 -X로 간다. DrawAll에서 결과를 PartFrame의 offset으로 넘긴다.

### tank::AdvanceAngleTo(degrees, target, degreesPerSecond, dt)

UpdateScene에서 현재 각도 변수 degrees를 직접 갱신한다. 매 프레임 속도 × dt만큼 target에 가까워지고, 한 번에 지나칠 거리라면 target에서 멈춘다. 도착하면 true를 반환한다. 자리 교환 도중에는 누적 각도를 그대로 유지한다. 예를 들어 270도에서 반 바퀴를 더 돌면 목표는 450도다.

### tank::MakeYawView(eye, target, yawDegrees)

DrawAll에서 카메라의 view 행렬을 만든다. eye는 카메라 위치, target은 기준으로 바라볼 점, yawDegrees는 좌우 회전 각도다. eye에서 target을 향하는 방향을 구해 위쪽 축을 중심으로 회전한 후 glm::lookAt을 호출한다. 공전이 필요하면 motion::OrbitPosition으로 eye를 먼저 구한다.

## 19번에서 조합하는 순서

L 키를 처음 누른 순간에 목표각을 잡고, 교환 중에는 같은 키 입력을 건너뛴다. 아래 변수는 KeyCallback, UpdateScene, DrawAll이 함께 사용하는 상태다.

    float upperAngle = 0.0f;
    float upperTarget = 0.0f;
    bool swapping = false;

KeyCallback에서 처리한다.

    if (key == GLFW_KEY_L && action == GLFW_PRESS && !swapping) {
        upperTarget = upperAngle + 180.0f;
        swapping = true;
    }

UpdateScene에서 처리한다.

    if (swapping && tank::AdvanceAngleTo(upperAngle, upperTarget, 90.0f, dt)) {
        swapping = false;
        upperAngle = std::fmod(upperAngle, 360.0f);
    }

DrawAll에서 본체 행렬을 만든 뒤 상부 부품과 포신을 연결한다. positionX, positionZ, lowerHeight, centerAngle, gunAngle은 움직임을 위해 저장한 값이다.

    const GLint modelLocation = shader.uniformLocation("model");
    const auto base = tank::PartFrame(glm::mat4(1.0f), {positionX, 0.0f, positionZ});
    const auto center = tank::PartFrame(base, {0.0f, lowerHeight + 0.13f, 0.0f}, centerAngle);
    tank::DrawCube(shader, modelLocation, center,
                   glm::vec3(0.0f), {1.8f, 0.26f, 1.45f}, {0.35f, 0.55f, 0.32f});

    for (float side : {-1.0f, 1.0f}) {
        const auto upper = tank::PartFrame(
            center, tank::PairedOrbitOffset(side, 0.48f, 0.32f, upperAngle));
        tank::DrawCube(shader, modelLocation, upper,
                       glm::vec3(0.0f), {0.65f, 0.38f, 0.7f}, {0.55f, 0.8f, 0.25f});
        const auto gun = tank::PartFrame(upper, {0.0f, 0.0f, 0.32f}, side * gunAngle);
        tank::DrawCube(shader, modelLocation, gun,
                       {0.0f, 0.0f, 0.30f}, {0.1f, 0.1f, 0.65f}, {0.25f, 0.65f, 0.65f});
    }

깃대도 upper에 붙인다. X축으로 돌리고 싶으면 PartFrame의 마지막 인자를 glm::vec3(1, 0, 0)으로 지정한다.

## 자전·공전 함수

- motion::RotationMatrix(degrees, axis)는 각도를 라디안으로 바꾸고 축에 대한 회전행렬을 반환한다. 축이 0이면 단위행렬을 반환한다.
- motion::OrbitPosition(center, offset, axis, degrees)는 시작 위치의 상대 좌표 offset을 축 주위로 회전한 다음 중심 좌표를 더한다. 결과는 vec3 위치다. 예를 들어 원점 기준 (2, 0, 0)을 +Y축 주위로 90도 돌리면 약 (0, 0, -2)다.
- motion::SpinModel(position, axis, degrees, size, localPivot)은 물체를 놓고 자전시키는 모델 행렬을 반환한다. 모델의 localPivot이 월드의 position에 놓인다. localPivot의 기본값은 (0, 0, 0)이다. 점에 적용되는 순서는 중심 빼기 → 신축 → 회전 → 배치다.

## 공용 함수 빠른 목록

- practice::RunScene(title, keyCallback, updateScene, drawAll)은 창과 셰이더를 준비하고 프레임마다 입력, 갱신, 그리기를 실행한다. main에서 호출하며, 정상 종료는 0, 초기화 실패는 -1이다.
- practice::MakeModelMatrix(position, size, parent)는 parent × 이동 × 신축 행렬을 반환한다. parent를 생략하면 단위행렬을 사용한다.
- practice::DrawObj(mesh, model, color, modelLocation, shader)는 선택한 도형을 지정한 모델 행렬과 RGB 색으로 그린다. cube, sphere, cone, circle, axis, plane을 RunScene이 미리 준비한다.
- practice::MakeProjection(width, height, perspective, extent)는 투영 행렬을 반환한다. perspective가 true이면 원근 투영, false이면 직각 투영이다. extent는 직각 투영의 세로 반범위다. width와 height는 프레임 버퍼 크기다.
- practice::MakeVertices(vertices, position, shade)는 배열에 정점의 XYZ와 RGB를 추가한다. practice::MakeMesh(mesh, vertices, mode)는 배열을 GPU에 올리고 그리기 모드를 저장한다. mode의 기본값은 GL_TRIANGLES다. practice::MakeObjects()는 기본 도형을 생성하며 RunScene 안에서 호출된다.
- shader.uniformLocation(name)은 셰이더 변수 위치를 반환한다. shader.setMatrix(location, value)는 해당 위치에 행렬을 보낸다. 예를 들어 view와 projection은 DrawAll에서 정하고, model은 도형마다 바꾼다. Shader 생성자는 GLSL 소스로 프로그램을 만들고, valid()는 준비 여부를 확인하며, use()는 프로그램을 활성화한다.
- common::Camera(position, target, up)는 카메라 위치와 목표점을 저장한다. view()와 projection(width, height)은 행렬을 만들고 setPose(position, target, up)은 카메라 설정을 갱신한다.
- CreateGLWindow(width, height, title)는 OpenGL 창을 만들고 GLFWwindow 포인터를 반환한다. CheckShader(shader)와 CheckProgram(program)은 각각 컴파일과 링크 성공 여부를 확인한다. CreateProgram(vertexSource, fragmentSource)은 GLSL 프로그램 번호를 반환한다. common::configurePositionColorAttributes()는 위치 3개와 RGB 3개로 저장한 정점 배열의 읽는 방식을 설정한다. RunScene을 쓰면 창과 기본 도형 준비를 맡길 수 있다.

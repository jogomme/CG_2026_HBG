# OpenGL_CodingTest

이 폴더 전체를 복사하거나 압축합니다. Visual Studio 2026에서 OpenGL_CodingTest.slnx를 열고 x64를 선택한 다음 F5 키를 누릅니다. C++ 데스크톱 개발 도구와 Windows SDK 10.0.26100.0이 필요합니다.

Common에는 공용 소스, .cg-deps에는 GLM 헤더와 GLEW·GLFW의 x64 헤더, 라이브러리, DLL이 들어 있습니다. OpenGL.Local.props가 폴더 안의 라이브러리를 연결하고 빌드할 때 DLL을 실행 파일 옆으로 복사합니다. 대상 컴퓨터의 그래픽 드라이버는 OpenGL 3.3을 지원해야 합니다.

소스와 설정만 옮길 때 .vs, x64, *.vcxproj.user 및 컴파일 중간 출력 폴더는 제외해도 됩니다. .cg-deps는 반드시 포함합니다.

## 자전과 공전 재사용

- 공전: motion::OrbitPosition(center, offset, axis, degrees). offset은 중심에서 본 시작 위치입니다. 축에 수직인 offset 성분의 길이가 공전 반지름입니다.
- 자전: motion::SpinModel(position, axis, degrees, size, localPivot). 모델의 localPivot이 월드의 position에 오도록 배치하고 그 점을 중심으로 회전합니다. localPivot의 기본값은 모델 원점입니다.
- 자전만 하려면 고정된 position을 SpinModel에 넘깁니다. 공전만 하려면 OrbitPosition으로 위치를 구하고 SpinModel에 각도 0을 넘깁니다. 여러 물체는 물체마다 두 각도를 따로 저장합니다.
- 공전의 중심·축과 자전축은 모델을 그리는 좌표계에 맞춰 지정합니다. 기본 예제에서는 월드 좌표계를 사용하며 공전은 위치만 바꿉니다. UpdateScene의 속도는 도/초 단위입니다.

## 19번 탱크 문제용 함수

TankPractice.h를 포함합니다. 본체와 부품의 기준 좌표를 PartFrame으로 만든 다음 DrawCube에서만 크기를 지정합니다.

    auto base = tank::PartFrame(glm::mat4(1), glm::vec3(x, 0, z));
    auto center = tank::PartFrame(base, glm::vec3(0, height, 0), centerAngle);
    auto upper = tank::PartFrame(center, tank::PairedOrbitOffset(side, 0.48f, 0.32f, upperAngle));
    auto gun = tank::PartFrame(upper, glm::vec3(0, 0, 0.32f), side * gunAngle);
    tank::DrawCube(shader, modelLocation, gun,
                   glm::vec3(0, 0, 0.30f), glm::vec3(0.1f, 0.1f, 0.65f), color);

side는 -1과 +1입니다. 자리 교환을 시작할 때 target = upperAngle + 180을 저장하고, UpdateScene에서 tank::AdvanceAngleTo(upperAngle, target, 90.0f, dt)가 true이면 자리 교환을 끝냅니다. 진행 도중 다시 시작하지 않도록 플래그로 막습니다.

카메라를 제자리에서 돌릴 때 tank::MakeYawView(eye, target, yaw)를 사용합니다. 카메라를 공전시키려면 기존 motion::OrbitPosition으로 eye를 구해서 넘깁니다.

# ShaderPractice 공통 코드

## 변경 이유와 선택
실제 솔루션은 이미 과제별 실행 프로젝트로 분리되어 있다. 따라서 단일 프로젝트의 cpp 전환이나 별도 정적 라이브러리 프로젝트를 도입하지 않았다. 전자는 main 중복 관리가 필요하고, 후자는 현재 규모에서 빌드 설정과 학습 부담을 늘린다. 각 실행 프로젝트가 Common의 세 cpp를 한 번씩 컴파일하는 방식으로 유지한다. 서로 다른 실행 파일에서 각각 컴파일하는 것은 중복 링크가 아니다.

기존 GLCommon.h는 이미 창 생성, GLEW 초기화, 셰이더 컴파일/링크 및 오류 로그를 제공한다. 이 검증된 헤더와 이전 과제의 호환성을 보존하고 Shader에서 이를 재사용한다. homework16의 반복된 xyz/rgb 정점 속성 설정과 행렬 계산만 추가 분리했다.

## 책임과 API
- Shader.h/.cpp: 프로그램 단독 소유, valid(), use(), uniformLocation(), setMatrix(), reset(). 복사는 금지한다. use() 후 uniform을 설정한다. 컴파일/링크 실패는 valid()==false로 확인하며 GLCommon.h의 로그를 사용한다.
- Camera.h/.cpp: 위치/주시점/up과 view(), projection(width,height), setPose(). 기존 45도, near 0.1, far 100 투영을 유지한다. 양수 framebuffer 크기를 전달한다. 이동/회전 입력은 과제에서 계산한 pose를 전달한다. homework16에는 카메라 이동 입력이 없으므로 새 입력을 추가하지 않았다.
- GLUtils.h/.cpp: configurePositionColorAttributes(). 먼저 해당 VAO와 VBO를 바인딩한다. 정점은 위치 xyz + 색 rgb의 float 여섯 개이며 attribute 0/1을 사용한다. 다른 형식까지 일반화하지 않았다.
- GLCommon.h: 기존 창 생성 및 셰이더 컴파일/링크 보조 함수. 다른 과제는 계속 기존 방식으로 사용할 수 있다.
- homework16.cpp: 도형 데이터, 색상, 배치, 변환, 입력 상태, 모든 애니메이션과 그리기 순서를 소유한다. GPU geometry 해제도 해당 과제가 담당한다.

Shader는 OpenGL context가 살아 있을 때 해제해야 한다. homework16은 창을 닫기 전에 shader.reset()을 호출한다. 이후 소멸자는 0 핸들이므로 OpenGL을 호출하지 않는다. geometry는 정상 종료와 셰이더 실패 모두에서 해제하며 기존 누락된 사각뿔 VAO/VBO/EBO도 포함한다.

## Visual Studio 빌드 및 실행
1. ShaderPractice.slnx를 Visual Studio 2026(v145, Windows SDK 10.0.26100.0)에서 연다.
2. homework16을 시작 프로젝트로 설정한다. cpp의 main을 주석 처리하거나 파일을 옮길 필요가 없다.
3. Debug / x64 또는 Release / x64에서 homework16 프로젝트를 빌드하고 실행한다.
4. 이전 과제 실행은 해당 프로젝트를 시작 프로젝트로 설정한다.

기존 C++20, /utf-8, include 상속, opengl32.lib/glew32.lib/glfw3dll.lib 설정은 보존했다. GL 및 GLM 헤더, lib, 런타임 DLL은 기존 환경에 의존한다. 새 PC에서는 기존 GL.props 설명과 설치 경로를 확인해야 한다. Visual Studio 2022는 v145를 기본 제공하지 않으므로 검증된 도구 버전과 구별한다.

개발자 명령 프롬프트에서:
```bat
msbuild homework16\homework16.vcxproj /p:Configuration=Debug /p:Platform=x64
msbuild homework16\homework16.vcxproj /p:Configuration=Release /p:Platform=x64
```

## homework17 추가 절차
1. 솔루션에 빈 C++ 실행 프로젝트 homework17을 추가하고 ShaderPractice/homework17에 저장한다. homework16 전체 cpp를 복사하지 않는다.
2. Debug/Release x64에 기존 외부 include/lib 경로를 상속시키고 C++20 및 /utf-8을 설정한다. 기존 GL.props를 속성 시트로 가져오면 공통 언어/include/링크 설정을 재사용할 수 있다. 도구 집합/SDK 및 외부 SDK 경로는 기존 homework16과 맞춘다.
3. 기존 항목 추가에서 ../Common/Shader.cpp, Camera.cpp, GLUtils.cpp와 대응 헤더를 선택한다. 복사본을 만들지 않는다. 각 cpp는 프로젝트에 한 번만 등록하고 cpp 파일을 #include하지 않는다. Visual Studio가 filters를 함께 관리하게 한다.
4. homework17.cpp 하나에 main을 만든다. Common/Shader.h, Common/Camera.h, Common/GLUtils.h를 include한다.
5. CreateGLWindow로 창을 만들고 실패를 확인한다. 살아 있는 context 내부 scope에 Shader를 만들고 valid()를 확인한다. VAO/VBO 및 과제 도형을 생성한 뒤 configurePositionColorAttributes를 호출한다.
6. shader.use() 후 과제 셰이더의 uniform 위치를 얻고 Camera의 view/projection을 setMatrix로 전달한다. framebuffer가 0인 프레임은 건너뛴다. 과제별 입력, 업데이트, 렌더링을 구현한다.
7. geometry와 Shader를 context가 살아 있을 때 해제한 뒤 glfwDestroyWindow와 glfwTerminate를 호출한다. Debug/Release에서 빌드하고 homework17을 시작 프로젝트로 선택한다.

장면, 셰이더 소스, 도형 생성 알고리즘은 과제마다 작성한다. 공통 창 초기화/행렬/셰이더/정점 설정을 복사할 필요가 없다. 여러 과제에서 실제 동일한 기능이 확인될 때만 Common을 확장한다. 빌드 시간이 커지거나 공통 cpp 설정 관리가 반복되면 공통 props 또는 정적 라이브러리를 검토한다.

## 검증 및 수동 확인
입력 콜백부터 파일 끝까지와 메인 루프의 애니메이션/그리기 구간은 원본과 문자열 비교하여 동일함을 확인했다. 따라서 키 반복, 토글, 6~9번 상태 전환 로직을 변경하지 않았다. 창/화면의 실제 시각적 동등성은 이 비교만으로 보장할 수 없다.

수동 확인: 창 생성, 셰이더 오류 로그 유무, 축/육면체/사각뿔 색상과 변환, 창 크기 조절 및 최소화 복원, 기존 키 PRESS/REPEAT/RELEASE, 특히 6~9번 시작/정지/전환, 정상 종료를 확인한다. 실제 GUI 상호작용 검증은 수행하지 않았다.

실제 수행 결과(2026-10-17): Visual Studio 2026 MSBuild의 Debug x64 및 Release x64 컴파일/링크 성공. 기존 애니메이션 코드의 C4244 변환 경고 15건씩 남음. 프로젝트 XML, filters 일치, cpp 중복 등록, 파일 존재, git diff --check 통과. 커밋 및 Push 없음.

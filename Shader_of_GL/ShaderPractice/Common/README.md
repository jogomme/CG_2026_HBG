# OpenGL 실습 템플릿과 공통 코드

## 기존 항목 조사 결과
현재 ShaderPractice.slnx에는 homework 실행 프로젝트들이 등록되어 있으며 `OpenGL 실습`이라는 솔루션 폴더나 라이브러리 프로젝트는 없다. 이름이 일치하는 실제 항목은 ProjectTemplates/VS/OpenGL33Shader/OpenGL33Shader.vstemplate의 `OpenGL 3.3 실습` 프로젝트 템플릿이다. Type="Project"이고 생성되는 vcxproj의 Debug/Release ConfigurationType은 Application이다. 정적 라이브러리(StaticLibrary)가 아니다.

Visual Studio 18 사용자 Templates/ProjectTemplates/OpenGL33Shader 및 Visual C++/OpenGL33Shader에도 설치 사본이 존재한다. 저장소의 템플릿 원본과 설치 사본을 구분해야 한다. 최초 통합 때에는 설치 사본을 유지했으나, homework17 연결 수정 작업에서 두 설치본을 백업 후 최신 저장소 원본으로 갱신했다.

## 선택한 구조와 이유
템플릿은 과제 고유의 main.cpp와 실행 프로젝트 설정만 생성한다. 공통 구현을 템플릿에 넣어 복제하면 과제마다 수정해야 하므로 Common은 공유 경로에 둔다. 별도 정적 라이브러리는 현재 규모에서 필요하지 않아 만들지 않았다.

```
ShaderPractice/
  GLCommon.h                 기존 include 경로를 유지하는 호환 헤더
  Common/
    Common.props             공유 소스/헤더 등록과 include 경로
    GLCommon.h               창 초기화, 셰이더 컴파일/링크의 유일한 구현
    Shader.h/.cpp            프로그램 소유권과 uniform API
    Camera.h/.cpp            view/projection, pose 관리
    GLUtils.h/.cpp           xyz/rgb 정점 속성 설정
  ProjectTemplates/
    New-Homework.ps1          기존 VS 템플릿으로 새 실행 프로젝트 생성
    VS/OpenGL33Shader/        OpenGL 3.3 실습 템플릿 원본
  homework16/                과제 도형, 입력, 애니메이션과 렌더링
```

기존 GLCommon.h의 구현을 Common/GLCommon.h로 그대로 이동했다. 기존 위치는 이를 include하기만 하므로 컴파일/링크 알고리즘의 중복 구현이 없다. Shader는 CreateProgram을 재사용하고 프로그램 삭제만 소유한다. Camera와 GLUtils의 동작은 유지했다. homework16.cpp는 이번 변경에서 수정하지 않았다.

Common.props는 자신의 위치(MSBuildThisFileDirectory)를 기준으로 소스를 등록한다. 각 실행 파일은 공통 cpp 3개를 각각 한 번 컴파일한다. 헤더는 pragma once, 기존 함수는 inline이므로 기존 호환 헤더와 새 헤더를 함께 include해도 중복 정의가 발생하지 않는다. Common.props import와 공통 cpp 수동 등록을 동시에 사용하지 않는다. cpp를 #include하지 않는다. props의 Link 메타데이터로 공유 항목 이름을 제공한다. homework17과 새 템플릿의 filters에는 OpenGLCommonDirectory 속성을 사용한 Common 그룹을 등록하여 공유 파일을 표시한다.

## 빌드와 homework17 생성
Visual Studio 2026 / v145 / Windows SDK 10.0.26100.0 / C++20 / x64 설정과 외부 라이브러리 링크 설정을 유지했다. 기존 GL.props도 유지하며 Common.props와 역할을 구분한다. GL.props는 언어/링커 기본값이고 Common.props는 공유 소스 등록이다.

ShaderPractice에서:
```powershell
.\ProjectTemplates\New-Homework.ps1 -Name homework17 -Title 'OpenGL Practice 17' -Build
```
스크립트는 VS/OpenGL33Shader를 사용하고 main과 프로젝트만 생성한다. Common은 복사하지 않는다. 솔루션에는 x64 매핑으로 등록한다. 기존 폴더나 같은 프로젝트 등록이 있으면 중단한다. 이름 검증 및 창 제목 문자열 이스케이프를 적용한다. 생성된 프로젝트를 시작 프로젝트로 선택한 뒤 과제 도형과 입력을 구현한다. Shader와 GLUtils 사용 예시는 생성된 main에 들어 있다. 3D 과제는 Camera.h를 include하고 view()/projection()을 셰이더에 전달한다.

```bat
msbuild homework16\homework16.vcxproj /p:Configuration=Debug /p:Platform=x64
msbuild homework16\homework16.vcxproj /p:Configuration=Release /p:Platform=x64
```

VS의 새 프로젝트 대화상자에 설치된 `OpenGL 3.3 실습`을 계속 사용하려면 사용자 템플릿 폴더의 OpenGL33Shader 설치본(두 위치)을 저장소 ProjectTemplates/VS/OpenGL33Shader 원본으로 갱신하고 VS를 다시 연다. 이전 설치본은 새 Common.props를 참조하지 않는다. 설치본 갱신 전에도 위 스크립트는 수정된 원본을 직접 사용하므로 즉시 사용할 수 있다. 템플릿 프로젝트는 기본적으로 프로젝트 상위의 Common/Common.props를 찾는다.

## 다른 C++ OpenGL 프로젝트에서 재사용
Common 디렉터리 전체를 하나의 공유 위치에 두고 소비 프로젝트에서 Common.props를 한 번 import한다. 소비 프로젝트는 Shader.h, Camera.h, GLUtils.h, GLCommon.h를 직접 include할 수 있다. Common 내부에서는 ShaderPractice 부모 경로에 의존하지 않는다.

```xml
<!-- Microsoft.Cpp.props 및 프로젝트 설정 뒤, Microsoft.Cpp.targets 앞 -->
<Import Project="..\SharedOpenGL\Common\Common.props" />
```

템플릿으로 만든 프로젝트를 외부 경로에 둘 때는 OpenGLCommonProps 속성을 실제 Common.props 경로로 지정한다(Import 이전 속성 그룹 또는 MSBuild /p:OpenGLCommonProps=... 사용). VS 프로젝트 속성 파일을 외부 PC에 그대로 복사하면 SDK 설치 경로와 v145 유무는 별도 확인이 필요하다.

이 공통 코드는 현재 Windows의 windows.h, GLEW, GLFW, GLM 설치 형식(<GL/...>)에 의존한다. 소비 프로젝트가 C++ 표준/SDK/include/lib 경로와 opengl32.lib, glew32.lib, glfw3dll.lib 및 런타임 DLL을 제공한다. Common.props는 사용자 컴파일/링크 정책을 덮어쓰지 않는다. 플랫폼 독립 라이브러리라고 주장하지 않는다. 여러 솔루션에서 바이너리 공유가 필요해지면 이 단일 소스 집합을 정적 라이브러리로 묶되 소비 프로젝트의 cpp 직접 컴파일을 제거한다.

## API와 수명
- Shader: valid(), use(), uniformLocation(), setMatrix(), reset(). use() 후 uniform을 설정하며 복사는 금지한다. 살아 있는 context에서 reset 또는 소멸한다.
- Camera: view(), projection(width,height), setPose(). 현재 45도, near 0.1, far 100. 양수 framebuffer 크기만 전달한다. 키 매핑과 이동량은 과제에서 정한다.
- GLUtils: configurePositionColorAttributes(). 대상 VAO/VBO를 먼저 바인딩한다. 정점은 xyz/rgb의 float 6개이다.
- GLCommon: CreateGLWindow(), CreateProgram(), CheckShader(), CheckProgram(). 기존 실패 반환과 로그를 유지한다.

## 검증
- homework16 Debug x64, Release x64 실제 MSBuild 컴파일/링크 성공. 기존 C4244 변환 경고가 남는다.
- 이전 GLCommon 경로를 사용하는 homework15 Debug x64 빌드 성공.
- 저장소 밖 임시 복사본에서 생성 스크립트로 새 프로젝트를 만들고 Debug/Release x64 빌드 검증.
- 공통 GLCommon 구현과 HEAD의 기존 구현이 동일함을 비교. homework16.cpp가 HEAD와 동일함을 확인. XML 파싱 및 git diff --check 통과.
- GUI 실행, 화면 비교 및 6~9번 키 상호작용은 미검증. Visual Studio의 시작 프로젝트, x64 구성, 도형/입력/애니메이션/정상 종료를 수동 확인한다.
- 현재 refactor/shader-practice-common에서 작업. 이번 작업은 커밋, main 병합, Push를 수행하지 않았다.
`n검증 환경 참고: 제한된 실행 환경에서 생성 프로젝트의 최초 Debug 빌드는 MSPDB140.DLL 버전 오류(LNK1101)가 발생했다. 동일 프로젝트를 일반 로컬 MSBuild 접근으로 Debug Rebuild 및 Release 빌드하여 성공을 확인했다.


## homework17 및 설치 템플릿 수정 결과
실제 위치: ShaderPractice/homework17/homework17.vcxproj. 구형 템플릿으로 생성되어 Common.props import가 없던 문제를 수정했다. homework17.cpp 및 vcxproj.user는 SHA-256 비교로 변경 없음을 확인했다. 공통 파일은 복사하지 않았으며 Common.cpp 3개는 각각 한 번만 컴파일되고 헤더 4개가 등록된다.

Visual Studio 18 사용자 템플릿 두 위치(OpenGL33Shader, Visual C++/OpenGL33Shader)를 outputs/template-backup-20261017-192357에 백업한 뒤 갱신했다. manifest.json에 이전 비교와 사용자 파일 해시를 기록했다. 설치본 파일 4개 모두 저장소 템플릿과 일치한다.

homework17 Debug/Release x64와 갱신된 설치본에서 확장한 임시 프로젝트 Debug/Release x64 모두 MSBuild 컴파일/링크 성공. 실제 VS 새 프로젝트 대화상자 조작 및 화면 표시는 미검증이다. 열려 있는 homework17을 다시 로드하거나 솔루션을 다시 열고, 템플릿 목록 갱신에는 Visual Studio를 재시작한다. 새 과제는 ShaderPractice 바로 아래에 생성하면 ../Common/Common.props를 자동 참조한다. 외부 위치에서는 OpenGLCommonProps를 지정한다. 파일 삭제, 커밋, main 병합, Push는 수행하지 않았다.

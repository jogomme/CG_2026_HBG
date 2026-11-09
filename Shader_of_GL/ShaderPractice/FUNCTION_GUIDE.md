# 공통 함수 사용 설명서 — 시험장 검색용

대상: `Shader_of_GL/ShaderPractice/Common`의 모든 C++ 공통 함수·멤버 함수 25개. 삭제된 복사 생성자·복사 대입도 포함합니다. GLSL의 main은 C++ 함수와 구분해 설명합니다. 과제별 cpp의 KeyCallback·UpdateKeyboard·MakeWireObjects·DrawNumber 등은 이 공통 API의 일부가 아닙니다.

## Ctrl + F 검색 안내

| 찾을 말 | 바로 찾을 항목 |
|---|---|
| 정점 추가 | practice::MakeVertices, 정점 두 개로 선 만들기 |
| 정점 연결 / VAO / VBO | MakeMesh, configurePositionColorAttributes |
| 도형 그리기 / 색상 | MakeObjects, DrawObj, objectColor |
| 자전 / 제자리 회전 | 조합 예제: 자전 T * R * S |
| 공전 / 행성 | 조합 예제: 공전 R * T * S |
| 부모 행렬 / 포신 / 계층 | MakeModelMatrix, 부모-자식 조합 |
| 기준점 / 모서리 / 펼치기 | 조합 예제: T(pivot) * R * T(-pivot) |
| 충돌 / 겹침 | 충돌 항목: 공통 충돌 함수 없음, 회전 없는 AABB 예제 |
| 카메라 이동 / 추적 | Camera::setPose, Camera::view |
| 카메라 자전 / 카메라 공전 | 조합 예제의 카메라 항목 |
| 원근 / 직각 / 줌 | Camera::projection, MakeProjection |
| 셰이더 / uniform / 화면 안 보임 | Shader, uniformLocation, setMatrix, 점검 순서 |
| 창 생성 / 반복문 / 초기화 | CreateGLWindow, RunScene |
| 삭제 / 해제 / 수명 / 복사 금지 | Shader::~Shader, reset, Mesh 주의사항 |

## 먼저 알아둘 약속

- `Common/...` include는 `ShaderPractice`를 include 경로로 둔 현재 과제 프로젝트 기준입니다. Common.props가 있는 프로젝트를 사용하세요. `Shader.cpp`, `Camera.cpp`, `GLUtils.cpp`도 함께 컴파일해야 합니다. 파일 하나만 복사하면 링크 오류가 날 수 있습니다.
- 코드에 `shader`, `camera`, `window`가 나오면 이미 준비한 해당 객체를 사용합니다. 각 예제의 실행 위치를 확인하세요. 아래 최소 실행 예제는 하나의 완성된 cpp로 사용할 수 있습니다.
- 거리에는 미터·픽셀 같은 고정 단위가 없습니다. 장면 전체에서 같은 좌표 단위를 정해 사용합니다. framebuffer 크기만 픽셀입니다. 회전 숫자는 예제에서 도(degree)로 쓰며 `glm::radians`로 바꿉니다.
- GLM은 기본적으로 열벡터를 사용합니다. `A * B * 점`은 B부터 작용합니다. `glm::translate(model,p)`는 `model * T(p)`입니다. 코드에 먼저 쓴 변환이 점에 먼저 적용되는 것은 아닙니다.
- 함수의 반환형이 void이면 값을 반환하지 않습니다. 참조 `&`로 전달한 배열·객체나 OpenGL 상태가 바뀔 수 있습니다.

| 용어 | 쉬운 뜻 |
|---|---|
| 정점 | 선이나 삼각형의 위치를 정하는 점. 여기서는 위치와 색을 한 묶음으로 저장 |
| VBO | 정점 데이터를 담는 GPU 버퍼 |
| VAO | 어떤 VBO를 어떤 간격으로 읽을지 기억하는 설정 묶음 |
| uniform | 그리는 동안 셰이더에서 함께 사용하는 값. 행렬·물체 색 등을 CPU에서 전달 |
| model | 물체를 장면의 어디에, 어떤 크기와 방향으로 놓을지 정하는 행렬 |
| view | 장면을 카메라의 눈으로 바라보게 옮기는 행렬 |
| projection | 원근 또는 직각 방식으로 화면에 비추는 행렬 |

## 바로 실행할 최소 예제

17~20번 프로젝트처럼 Common.props가 연결된 프로젝트의 cpp 전체로 사용합니다. `RunScene`이 창, shader, 기본 Mesh, objectColor 위치와 자원 정리를 맡습니다. 별도 `MakeObjects()`를 중복 호출하지 마세요.

```cpp
#include "Common/PracticeScene.h"

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS && (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void UpdateScene(float deltaTime)
{
    // 애니메이션이 없으므로 여기서는 바꾸지 않습니다.
}

void DrawAll(common::Shader& shader, int width, int height)
{
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 2.0f, 5.0f),
        glm::vec3(0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    shader.setMatrix(shader.uniformLocation("view"), view);
    shader.setMatrix(shader.uniformLocation("projection"),
        practice::MakeProjection(width, height, true, 3.0f));
    glm::mat4 model = practice::MakeModelMatrix(glm::vec3(0.0f), glm::vec3(1.0f));
    practice::DrawObj(practice::cube, model, glm::vec3(1.0f, 0.3f, 0.2f),
        shader.uniformLocation("model"), shader);
}

int main()
{
    return practice::RunScene("공통 함수 연습", KeyCallback, UpdateScene, DrawAll);
}
```

## 함수 목록의 예제를 넣는 위치

`return -1`이 들어 있는 초기화 예제는 `int main()` 또는 int를 반환하는 초기화 함수 안에 넣습니다. `shader.use`, `setMatrix`, `DrawObj` 예제는 유효한 OpenGL 컨텍스트와 shader가 필요합니다. `MakeModelMatrix`, Camera의 숫자 계산 예제는 컨텍스트 없이도 계산할 수 있습니다. GPU 도형을 직접 만든 예제는 실제 그리기를 끝낸 후 정리해야 합니다. 예제의 삭제 명령을 그리기 전에 붙이지 마세요.


## 1. CreateGLWindow

GLFW 창과 OpenGL 3.3 Core 컨텍스트를 만들고 GLEW를 준비한다.

**헤더:** `Common/GLCommon.h`  
**관련 실습:** 14~16번 직접 사용, 17~20번 RunScene 내부 사용. 21번 템플릿도 사용.  
**함께 사용할 함수:** glfwGetFramebufferSize, common::Shader, glfwDestroyWindow, glfwTerminate

**사용 상황**

직접 main과 반복문을 작성하는 실습의 출발점. 창 생성, 그래픽 카드 함수 준비, 첫 viewport 설정에 사용한다.

**매개변수**

- width: 창 내용 영역의 가로 크기(화면 좌표 단위, 일반 배율에서 픽셀과 같음).
- height: 창 내용 영역의 세로 크기(같은 단위). 둘 다 양수.
- title: 창 제목인 널 종료 문자열. 한글은 프로젝트 /utf-8 설정을 사용한다.

**반환값**

성공하면 GLFWwindow*. 실패하면 nullptr. 실패 원인은 한국어 콘솔 메시지로 출력한다.

**복사 예제**

main에서 처음 호출합니다. 반환값 확인 전에는 다른 OpenGL 함수를 부르지 않습니다.

```cpp
GLFWwindow* window = CreateGLWindow(1200, 900, "그래픽스 실습");
if (!window)
{
    return -1;
}
// 여기부터 OpenGL 함수와 common::Shader를 사용할 수 있습니다.
```

**주의사항**

- 윈도우 크기와 framebuffer 픽셀 수는 고해상도 화면에서 다를 수 있다. 그릴 때는 glfwGetFramebufferSize로 크기를 다시 받는다.
- 이 함수는 반복 그리기를 시작하지 않는다. glfwPollEvents, glClear, 그리기, glfwSwapBuffers는 직접 작성하거나 RunScene에 맡긴다.
- 종료할 때 GPU 자원을 먼저 지우고 glfwDestroyWindow(window), glfwTerminate() 순서로 정리한다.
- glEnable(GL_DEPTH_TEST)는 여기서 켜지지 않는다. 3D의 은면 제거는 호출자가 켠다. Windows 전용 코드이다.

**정의·선언 위치:** [Common/GLCommon.h](Common/GLCommon.h)

## 2. CheckShader

셰이더 하나의 컴파일 성공 여부를 검사하고 실패 로그를 출력한다.

**헤더:** `Common/GLCommon.h, Common/ShaderSources.h`  
**관련 실습:** 14~16번 CreateProgram 경유, 17~20번 Shader 생성 경유. 직접 검사 방식은 11·13번에도 등장.  
**함께 사용할 함수:** glCreateShader, glShaderSource, glCompileShader, CheckProgram

**사용 상황**

직접 GLSL을 컴파일하거나 셰이더 문법 오류를 찾을 때 사용한다. CreateProgram 내부에서도 호출한다.

**매개변수**

- shader: glCreateShader로 만든 셰이더 객체 번호(GLuint, 단위 없음). 먼저 glCompileShader를 호출해야 한다.

**반환값**

컴파일 성공은 true, 실패는 false. 실패 로그를 최대 1024바이트 버퍼로 받아 콘솔에 출력한다.

**복사 예제**

OpenGL 컨텍스트를 만든 뒤 실행합니다.

```cpp
GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(vertexShader, 1, &common::shader_sources::positionColorVertex, nullptr);
glCompileShader(vertexShader);
bool compiled = CheckShader(vertexShader);
glDeleteShader(vertexShader); // 확인용 셰이더이므로 여기서 정리합니다.
```

**주의사항**

- 프로그램 번호가 아니라 셰이더 번호를 넣는다. 검사만 하고 객체를 삭제하지 않는다.
- 컴파일 성공이어도 vertex/fragment 사이의 연결이 실패할 수 있으므로 링크는 CheckProgram으로 별도 검사한다.

**정의·선언 위치:** [Common/GLCommon.h](Common/GLCommon.h)

## 3. CheckProgram

셰이더 프로그램의 링크 성공 여부를 검사하고 실패 로그를 출력한다.

**헤더:** `Common/GLCommon.h, Common/ShaderSources.h`  
**관련 실습:** 14~16번 CreateProgram 경유, 17~20번 Shader 생성 경유. 직접 검사 방식은 11·13번에도 등장.  
**함께 사용할 함수:** CreateProgram, glLinkProgram, glUseProgram

**사용 상황**

vertex/fragment 셰이더를 묶은 프로그램이 사용 가능한지 확인할 때 사용한다.

**매개변수**

- program: glCreateProgram으로 만든 프로그램 번호(GLuint, 단위 없음). 먼저 glLinkProgram을 호출한다.

**반환값**

링크 성공은 true, 실패는 false. 실패 로그를 최대 1024바이트 버퍼로 받아 콘솔에 출력한다.

**복사 예제**

OpenGL 컨텍스트를 만든 뒤 실행합니다.

```cpp
GLuint program = CreateProgram(
    common::shader_sources::positionColorVertex,
    common::shader_sources::positionColorFragment
);
if (program != 0)
{
    bool linked = CheckProgram(program); // CreateProgram 내부에서도 이미 확인합니다.
    glDeleteProgram(program);
}
```

**주의사항**

- 셰이더 번호와 프로그램 번호는 서로 다른 종류이다. CheckShader와 혼동하지 않는다.
- 이 함수는 glUseProgram을 호출하거나 프로그램을 삭제하지 않는다. 링크 성공 후에만 사용한다.

**정의·선언 위치:** [Common/GLCommon.h](Common/GLCommon.h)

## 4. CreateProgram

vertex/fragment GLSL을 컴파일하고 링크한 프로그램 번호를 만든다.

**헤더:** `Common/GLCommon.h, Common/ShaderSources.h`  
**관련 실습:** 14·15번 직접 사용, 16~20번 Shader 생성자 내부에서 사용.  
**함께 사용할 함수:** CheckShader, CheckProgram, glUseProgram, glDeleteProgram

**사용 상황**

클래스 없이 GLuint로 셰이더 프로그램을 관리하거나, Shader 생성자가 내부적으로 프로그램을 만들 때 사용한다.

**매개변수**

- vertexSource: vertex 셰이더 GLSL 전체 문자열(const char*, 널 종료).
- fragmentSource: fragment 셰이더 GLSL 전체 문자열(const char*, 널 종료). 파일 경로가 아니다.

**반환값**

성공하면 0이 아닌 프로그램 번호(GLuint), 컴파일/링크 실패면 0. 중간 셰이더 객체는 내부에서 삭제한다.

**복사 예제**

OpenGL 컨텍스트를 만든 뒤 실행합니다.

```cpp
GLuint program = CreateProgram(
    common::shader_sources::positionColorVertex,
    common::shader_sources::positionColorFragment
);
if (program == 0)
{
    return -1;
}
glUseProgram(program);
// 모든 그리기를 끝낸 뒤, 컨텍스트가 살아 있을 때 호출합니다.
glDeleteProgram(program);
```

**주의사항**

- 반환값 0을 검사한다. 성공한 프로그램은 호출자가 glDeleteProgram으로 정리해야 한다.
- common::Shader가 소유한 프로그램을 직접 삭제하면 안 된다. 수동 관리와 Shader 관리 중 한 방식을 선택한다.
- 기본 vertex 셰이더를 쓰면 model/view/projection 세 uniform 행렬을 설정해야 한다.
- CreateProgram 자체는 프로그램을 현재 사용 상태로 바꾸지 않는다. glUseProgram 또는 Shader::use가 필요하다.

**정의·선언 위치:** [Common/GLCommon.h](Common/GLCommon.h)

## 5. common::configurePositionColorAttributes

VBO의 위치 xyz와 색 rgb를 셰이더의 입력 0번과 1번에 연결한다.

**헤더:** `Common/GLUtils.h`  
**관련 실습:** 16번 직접 사용, 17~20번 MakeMesh 내부 사용. 21번 템플릿도 사용.  
**함께 사용할 함수:** glBindVertexArray, glBindBuffer, glBufferData; practice::MakeMesh가 내부에서 호출

**사용 상황**

정점을 x,y,z,r,g,b 순서로 저장하는 실습에서 VAO/VBO를 준비한 직후 사용한다.

**매개변수**

- 인자 없음. 현재 바인딩된 VAO와 GL_ARRAY_BUFFER의 VBO에 설정을 기록한다.
- 정점 하나는 float 6개: 위치 3개 + 색 3개. stride는 6*sizeof(float)바이트, 색 시작점은 3*sizeof(float)바이트.

**반환값**

반환값 없음(void). 현재 VAO의 attribute 0·1을 설정하고 활성화한다.

**복사 예제**

OpenGL 컨텍스트를 만든 뒤 실행합니다.

```cpp
float vertex[] = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
GLuint vao = 0;
GLuint vbo = 0;
glGenVertexArrays(1, &vao);
glBindVertexArray(vao);
glGenBuffers(1, &vbo);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
common::configurePositionColorAttributes();
// 이후 그릴 때 glBindVertexArray(vao)를 사용합니다.
glDeleteBuffers(1, &vbo);
glDeleteVertexArrays(1, &vao);
```

**주의사항**

- 먼저 대상 VAO와 VBO를 바인딩해야 한다. 잘못된 VAO에 설정하면 나중에 그려지지 않는다.
- 법선·UV가 추가된 정점에는 그대로 쓰면 안 된다. attribute 번호, stride와 offset을 실제 배열 형식에 맞춰 바꿔야 한다.
- Common.props를 통해 GLUtils.cpp도 컴파일해야 한다. 헤더 include만으로 구현이 링크되지는 않는다.

**정의·선언 위치:** [Common/GLUtils.h](Common/GLUtils.h)

## 6. common::Shader::Shader

GLSL 두 개로 프로그램을 만들고 Shader 객체가 그 번호를 관리한다.

**헤더:** `Common/Shader.h, Common/ShaderSources.h`  
**관련 실습:** 16~20번, 21번 템플릿.  
**함께 사용할 함수:** CreateGLWindow, CreateProgram, valid, use

**사용 상황**

직접 glDeleteProgram을 빠뜨리지 않도록 프로그램의 생성·정리를 한 객체에 맡길 때 사용한다.

**매개변수**

- vertex: vertex 셰이더 전체 GLSL 문자열.
- fragment: fragment 셰이더 전체 GLSL 문자열. 둘 다 const char*, 파일 경로가 아니다.

**반환값**

생성자는 반환값이 없다. 만들어진 객체의 valid()로 성공 여부를 확인한다.

**복사 예제**

OpenGL 컨텍스트를 만든 뒤 실행합니다.

```cpp
common::Shader shader(
    common::shader_sources::positionColorVertex,
    common::shader_sources::positionColorFragment
);
if (!shader.valid())
{
    return -1;
}
shader.use();
```

**주의사항**

- CreateGLWindow 이후에 만든다. 생성 실패가 예외로 전달되는 것이 아니라 valid()==false가 된다.
- 이 객체는 복사할 수 없다. 함수에 넘길 때 common::Shader&로 전달한다.
- 소멸자가 glDeleteProgram을 호출하므로 창을 파괴하기 전에 객체의 수명이 끝나야 한다.

**정의·선언 위치:** [Common/Shader.h](Common/Shader.h)

## 7. common::Shader::~Shader

Shader 객체의 수명이 끝날 때 소유한 프로그램을 자동으로 정리한다.

**헤더:** `Common/Shader.h, Common/ShaderSources.h`  
**관련 실습:** 16~20번, 21번 템플릿.  
**함께 사용할 함수:** reset, glfwDestroyWindow, glfwTerminate

**사용 상황**

함수를 끝내거나 중괄호 블록을 벗어날 때 GPU 프로그램 번호를 돌려줄 때 사용된다.

**매개변수**

- 인자 없음. 객체가 기억한 program_을 reset()으로 정리한다.

**반환값**

반환값 없음. 소멸자는 반환형을 쓰지 않는다.

**복사 예제**

OpenGL 컨텍스트를 만든 뒤 실행합니다.

```cpp
{
    common::Shader shader(
        common::shader_sources::positionColorVertex,
        common::shader_sources::positionColorFragment
    );
    // 이 블록 안에서 shader를 사용합니다.
} // 여기서 소멸자가 자동 호출됩니다.
// glfwDestroyWindow와 glfwTerminate는 이 블록 뒤에서 호출합니다.
```

**주의사항**

- shader.~Shader()를 직접 호출하지 않는다. 같은 객체를 다시 소멸시키는 문제가 생긴다.
- 소유한 프로그램의 OpenGL 컨텍스트가 현재 컨텍스트로 살아 있어야 한다. 전역 Shader 객체로 만들지 않는다.
- reset()을 먼저 호출했어도 program_이 0이므로 중복 삭제를 막는다.

**정의·선언 위치:** [Common/Shader.h](Common/Shader.h)

## 8. common::Shader::Shader(복사 금지)

다른 Shader 객체를 복사해서 새 객체를 만드는 것을 컴파일 단계에서 금지한다.

**헤더:** `Common/Shader.h, Common/ShaderSources.h`  
**관련 실습:** 16~20번, 21번 템플릿의 Shader 수명 관리.  
**함께 사용할 함수:** Shader 생성자, common::Shader&

**사용 상황**

GPU 프로그램 하나를 두 객체가 중복 소유하고 두 번 삭제하는 실수를 막는다.

**매개변수**

- const Shader&: 원래 복사 대상으로 받을 객체 참조. 선언이 = delete이므로 실제 호출할 수 없다.

**반환값**

호출 불가. 반환되는 객체도 없고 복사를 시도하면 컴파일 오류가 난다.

**복사 예제**

복사 금지 선언은 실행되지 않습니다. 예제의 정상 생성 부분은 OpenGL 컨텍스트가 필요합니다.

```cpp
common::Shader shader(
    common::shader_sources::positionColorVertex,
    common::shader_sources::positionColorFragment
);
common::Shader& sameShader = shader; // 복사 대신 참조를 사용합니다.
// common::Shader copy = shader; // 금지된 예: 주석을 풀면 컴파일 오류입니다.
```

**주의사항**

- 사용 예제에서 &를 빼면 복사가 된다. DrawAll 같은 함수의 매개변수도 common::Shader&를 사용한다.
- 기본 move 생성자도 제공하지 않는다. 벡터에 값으로 넣거나 std::move로 옮길 수 있다고 가정하지 않는다.

**정의·선언 위치:** [Common/Shader.h](Common/Shader.h)

## 9. common::Shader::operator=(복사 대입 금지)

이미 있는 Shader 객체에 다른 Shader를 복사 대입하는 것을 금지한다.

**헤더:** `Common/Shader.h, Common/ShaderSources.h`  
**관련 실습:** 16~20번, 21번 템플릿의 Shader 수명 관리.  
**함께 사용할 함수:** Shader 생성자, use, reset

**사용 상황**

program_ 번호를 덮어써서 기존 자원이 누출되거나 두 객체가 같은 프로그램을 삭제하는 실수를 막는다.

**매개변수**

- const Shader&: 복사 대입하려던 대상. = delete이므로 호출할 수 없다.

**반환값**

선언상 Shader&지만 호출 자체가 금지되어 실제 반환값은 없다.

**복사 예제**

OpenGL 컨텍스트를 만든 뒤 실행합니다.

```cpp
common::Shader shader(
    common::shader_sources::positionColorVertex,
    common::shader_sources::positionColorFragment
);
common::Shader& drawShader = shader;
// drawShader = shader; // 참조여도 대입은 금지된 연산자를 호출합니다.
drawShader.use(); // 같은 객체를 참조해서 사용하는 것은 가능합니다.
```

**주의사항**

- 참조 변수에 대한 대입도 참조 대상을 바꾸는 것이 아니라 객체 대입이다. 호출 인수에 참조를 전달해 사용한다.
- 프로그램을 새로 만들려면 별도의 새 Shader 객체를 적절한 수명 범위에 만든다.

**정의·선언 위치:** [Common/Shader.h](Common/Shader.h)

## 10. common::Shader::valid

프로그램 생성에 성공한 번호를 현재 가지고 있는지 확인한다.

**헤더:** `Common/Shader.h, Common/ShaderSources.h`  
**관련 실습:** 16~20번, 21번 템플릿.  
**함께 사용할 함수:** Shader 생성자, use

**사용 상황**

Shader 생성 직후 실패를 확인하거나 reset 후 상태를 확인할 때 사용한다.

**매개변수**

- 인자 없음. 객체의 program_이 0이 아닌지 검사한다. const이므로 상태를 바꾸지 않는다.

**반환값**

program_ != 0이면 true, 0이면 false. 그리기 결과 전체를 검증하는 함수는 아니다.

**복사 예제**

OpenGL 컨텍스트를 만든 뒤 실행합니다.

```cpp
common::Shader shader(
    common::shader_sources::positionColorVertex,
    common::shader_sources::positionColorFragment
);
if (!shader.valid())
{
    return -1;
}
```

**주의사항**

- 정점 데이터, uniform 행렬, VAO 설정의 정상 여부까지 확인하지 않는다.
- 이 클래스에서 성공한 프로그램은 생성 시 링크까지 확인하지만, valid()가 매번 다시 링크를 검사하는 것은 아니다.

**정의·선언 위치:** [Common/Shader.h](Common/Shader.h)

## 11. common::Shader::use

이 Shader의 프로그램을 이후 OpenGL 명령이 사용할 프로그램으로 선택한다.

**헤더:** `Common/Shader.h, Common/ShaderSources.h`  
**관련 실습:** 16~20번, 21번 템플릿.  
**함께 사용할 함수:** valid, uniformLocation, setMatrix

**사용 상황**

그리기 직전 또는 다른 프로그램을 쓰다가 이 프로그램으로 돌아올 때 사용한다.

**매개변수**

- 인자 없음. 객체가 기억한 program_ 번호를 glUseProgram에 전달한다.

**반환값**

반환값 없음(void). OpenGL의 현재 프로그램 상태가 바뀐다.

**복사 예제**

OpenGL 컨텍스트를 만든 뒤 실행합니다.

```cpp
shader.use();
GLint modelLocation = shader.uniformLocation("model");
shader.setMatrix(modelLocation, glm::mat4(1.0f));
```

**주의사항**

- shader.valid()가 true인 객체로 호출한다.
- setMatrix를 부르기 전에 이 프로그램을 use해야 한다. 다른 프로그램에서 찾은 uniform 위치는 섞어 쓰지 않는다.
- 여러 프로그램을 번갈아 쓰면 필요한 지점마다 다시 호출한다.

**정의·선언 위치:** [Common/Shader.h](Common/Shader.h)

## 12. common::Shader::uniformLocation

셰이더 안에서 uniform 값을 넣을 칸의 위치 번호를 찾는다.

**헤더:** `Common/Shader.h, Common/ShaderSources.h`  
**관련 실습:** 16~20번, 21번 템플릿.  
**함께 사용할 함수:** use, setMatrix, glUniform3fv

**사용 상황**

model/view/projection 행렬이나 objectColor 같은 전역 셰이더 값을 보낼 때 사용한다.

**매개변수**

- name: GLSL uniform 이름(const char*, 대소문자 구분, 단위 없음). 예: "model".

**반환값**

GLint 위치 번호. 이름이 없거나 최적화로 사용되지 않으면 -1. 0도 유효한 위치이므로 실패로 보지 않는다.

**복사 예제**

OpenGL 컨텍스트를 만든 뒤 실행합니다.

```cpp
GLint modelLocation = shader.uniformLocation("model");
if (modelLocation == -1)
{
    std::cerr << "model 이름과 셰이더 사용 여부를 확인하세요." << std::endl;
}
shader.use();
shader.setMatrix(modelLocation, glm::mat4(1.0f));
```

**주의사항**

- 성공적으로 링크된 프로그램의 이름을 조회한다. 조회 자체에는 use()가 필수는 아니지만 값을 보낼 때는 필요하다.
- 위치 번호는 프로그램마다 다르다. 다시 링크하거나 새 프로그램을 만들면 위치를 다시 조회한다.
- setMatrix에 -1을 넘기면 OpenGL은 조용히 무시한다. 화면이 안 나오면 위치부터 확인한다.

**정의·선언 위치:** [Common/Shader.h](Common/Shader.h)

## 13. common::Shader::setMatrix

GLM 4×4 행렬 하나를 현재 사용 중인 셰이더의 uniform에 보낸다.

**헤더:** `Common/Shader.h, Common/ShaderSources.h, <GL/glm/gtc/matrix_transform.hpp>`  
**관련 실습:** 16~20번, 21번 템플릿.  
**함께 사용할 함수:** use, uniformLocation, glm::translate/rotate/scale

**사용 상황**

물체의 이동·자전·공전, 카메라 view, 투영 projection을 화면에 반영할 때 사용한다.

**매개변수**

- location: 이 프로그램에서 찾은 uniform 위치(GLint, 단위 없음).
- value: 보낼 glm::mat4 참조. 위치는 모델 좌표 단위, 회전값은 이미 행렬로 만들어져 있다.

**반환값**

반환값 없음(void). glUniformMatrix4fv로 행렬 1개를 전송한다.

**복사 예제**

OpenGL 컨텍스트를 만든 뒤 실행합니다.

```cpp
shader.use();
glm::mat4 model(1.0f);
model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
shader.setMatrix(shader.uniformLocation("model"), model);
```

**주의사항**

- 반드시 이 shader.use()를 먼저 호출한다. 이 메서드는 자동으로 프로그램을 바인딩하지 않는다.
- GLM의 기본 열 우선 저장에 맞춰 GL_FALSE로 보낸다. 임의로 transpose하거나 GL_TRUE로 바꾸지 않는다.
- 기본 vertex 셰이더는 projection * view * model * 정점 순서이다. 실제 정점에는 오른쪽 model부터 적용된다.

**정의·선언 위치:** [Common/Shader.h](Common/Shader.h)

## 14. common::Shader::reset

현재 소유한 프로그램을 삭제하고 저장 번호를 0으로 바꾼다.

**헤더:** `Common/Shader.h, Common/ShaderSources.h`  
**관련 실습:** 16~20번 Shader 소멸 경유. 명시적 호출은 자원 관리 응용.  
**함께 사용할 함수:** valid, Shader 소멸자, glUseProgram(0)

**사용 상황**

컨텍스트를 닫기 전에 GPU 프로그램을 명시적으로 정리하거나 Shader를 빈 상태로 만들 때 사용한다.

**매개변수**

- 인자 없음. 객체가 기억한 프로그램을 정리한다.

**반환값**

반환값 없음(void). 이후 valid()는 false이며, 이 함수가 셰이더를 다시 만들지는 않는다.

**복사 예제**

OpenGL 컨텍스트를 만든 뒤 실행합니다.

```cpp
shader.reset();
bool nowEmpty = !shader.valid();
// 필요하면 glUseProgram(0)으로 현재 프로그램도 해제합니다.
glUseProgram(0);
```

**주의사항**

- 이 프로그램을 만든 OpenGL 컨텍스트가 살아 있고 현재 컨텍스트여야 한다.
- 현재 사용 중인 프로그램은 glDeleteProgram 후 사용 해제 시점까지 삭제가 보류될 수 있다. reset은 상태 저장 번호만 즉시 0으로 만든다.
- 여러 번 호출해도 0인 번호를 다시 삭제하지 않는다. 초기 화면 복원이나 객체 위치 초기화 함수가 아니다.

**정의·선언 위치:** [Common/Shader.h](Common/Shader.h)

## 15. common::Camera::Camera

카메라 위치, 바라볼 지점, 위쪽 방향을 기억하는 Camera 객체를 만든다.

**헤더:** `Common/Camera.h`  
**관련 실습:** 16번 직접 사용, 19번 카메라 원리에 응용 가능(19번은 직접 lookAt 사용). 21번 템플릿 사용.  
**함께 사용할 함수:** view, projection, setPose, Shader::setMatrix

**사용 상황**

고정 시점·카메라 이동·카메라 공전을 구현할 때 사용할 기준 시점을 저장한다.

**매개변수**

- position: 카메라의 월드 좌표(x,y,z, 장면과 같은 거리 단위).
- target: 바라볼 지점의 월드 좌표. 방향 벡터가 아니라 지점이다.
- up: 카메라 위쪽의 기준 방향. 단위 없음. 보통 (0,1,0).

**반환값**

생성자이므로 반환값 없음. view()와 projection()을 제공하는 객체를 만든다.

**복사 예제**

이 함수 자체는 OpenGL 컨텍스트 없이도 사용할 수 있습니다.

```cpp
common::Camera camera(
    glm::vec3(0.0f, 2.0f, 5.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
);
glm::mat4 view = camera.view();
```

**주의사항**

- position과 target이 같으면 바라보는 방향을 정할 수 없다. up도 영벡터이거나 보는 방향과 나란하면 안 된다.
- 이 클래스는 GPU 호출 없이 숫자만 저장한다. 창 생성 전에도 객체를 만들 수 있다.
- 생성만으로 화면이 바뀌지 않는다. view()/projection() 결과를 셰이더에 전달해야 한다.

**정의·선언 위치:** [Common/Camera.h](Common/Camera.h)

## 16. common::Camera::view

저장한 카메라 시점으로 월드 좌표를 카메라 좌표로 옮기는 view 행렬을 만든다.

**헤더:** `Common/Camera.h, Common/Shader.h`  
**관련 실습:** 16번, 19번 카메라 원리에 응용. 21번 템플릿.  
**함께 사용할 함수:** setPose, Shader::setMatrix

**사용 상황**

카메라 이동·자전·공전 이후 화면을 카메라 시점으로 그릴 때 사용한다.

**매개변수**

- 인자 없음. 생성자나 setPose로 저장한 position_, target_, up_을 사용한다.

**반환값**

glm::lookAt(position_, target_, up_)로 만든 glm::mat4. 카메라 물체의 모델 행렬이 아니라 장면을 보는 변환이다.

**복사 예제**

행렬 계산은 창 없이 가능. 예제의 setMatrix는 유효한 shader와 OpenGL 컨텍스트가 필요합니다.

```cpp
glm::mat4 view = camera.view();
shader.use();
shader.setMatrix(shader.uniformLocation("view"), view);
```

**주의사항**

- 위치가 이동했다면 setPose로 먼저 갱신한다. target을 고정하면 카메라가 이동할 때 계속 그 지점을 향해 고개도 바뀐다.
- 평행 이동하면서 보는 방향을 유지하려면 position과 target에 같은 이동량을 더한다.
- 기본 OpenGL/GLM view에서 카메라는 자신의 -z 방향을 바라본다. 월드 +z가 항상 화면 위쪽이라는 뜻은 아니다.

**정의·선언 위치:** [Common/Camera.h](Common/Camera.h)

## 17. common::Camera::projection

화면 비율에 맞는 고정 설정의 원근 projection 행렬을 만든다.

**헤더:** `Common/Camera.h, Common/Shader.h`  
**관련 실습:** 16번, 21번 템플릿. 18·20번은 MakeProjection으로 투영 전환.  
**함께 사용할 함수:** glfwGetFramebufferSize, view, Shader::setMatrix

**사용 상황**

창 크기가 변해도 물체가 옆으로 늘어나지 않는 원근 화면을 만들 때 사용한다.

**매개변수**

- width: 현재 framebuffer 너비(픽셀, 양수).
- height: 현재 framebuffer 높이(픽셀, 양수). 창 크기가 아니라 glfwGetFramebufferSize 결과를 쓴다.

**반환값**

시야각 45도, 비율 width/height, near 0.1, far 100.0인 원근 glm::mat4. near/far는 장면 거리 단위이다.

**복사 예제**

행렬 계산은 창 없이 가능. 예제는 기존 window, shader, camera가 준비된 반복문 안에서 실행합니다.

```cpp
int width = 0;
int height = 0;
glfwGetFramebufferSize(window, &width, &height);
if (width > 0 && height > 0)
{
    shader.use();
    shader.setMatrix(shader.uniformLocation("projection"), camera.projection(width, height));
}
```

**주의사항**

- 최소화하면 width나 height가 0일 수 있다. 함수 내부에 방어 코드가 없으므로 호출 전에 건너뛴다.
- 직각 투영 기능은 없다. 직각/원근 전환에는 practice::MakeProjection을 사용한다.
- 카메라 위치를 바꾸는 함수가 아니다. 원근 투영에서 화면 크기는 카메라 깊이에도 영향을 받는다.

**정의·선언 위치:** [Common/Camera.h](Common/Camera.h)

## 18. common::Camera::setPose

카메라 위치·바라볼 지점·위쪽 방향을 새 값으로 저장한다.

**헤더:** `Common/Camera.h`  
**관련 실습:** 16번 Camera 도입과 연관, 19번 카메라 이동·자전·공전 응용. 기존 19번은 직접 lookAt으로 구현.  
**함께 사용할 함수:** view, glm::rotate, Shader::setMatrix

**사용 상황**

키보드 카메라 이동, 대상 추적, 원점 기준 카메라 공전, 제자리 카메라 자전의 결과를 반영할 때 사용한다.

**매개변수**

- position: 새 카메라 월드 위치(장면 거리 단위).
- target: 새로 바라볼 월드 지점(같은 단위).
- up: 새 위쪽 방향(단위 없음, 영벡터 또는 시선과 평행이면 안 됨).

**반환값**

반환값 없음(void). 이후 view()가 새 시점을 계산한다.

**복사 예제**

이 함수 자체는 OpenGL 컨텍스트 없이도 사용할 수 있습니다.

```cpp
glm::vec3 eye(0.0f, 2.0f, 5.0f);
glm::vec3 target(0.0f, 0.0f, 0.0f);
glm::vec3 movement(1.0f, 0.0f, 0.0f);
camera.setPose(eye + movement, target + movement, glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 view = camera.view();
```

**주의사항**

- 이동량을 더하는 함수가 아니라 전달한 절대값으로 덮어쓰는 함수이다. 기존 위치는 사용자가 별도 변수로 관리한다.
- 같은 자리에서 자전하려면 position을 고정하고 target을 position + 회전시킨 앞쪽 방향으로 만든다.
- 원점 공전은 position을 원점 둘레로 돌리고 target을 원점에 유지한다. view()를 셰이더에 다시 보내야 화면에 반영된다.

**정의·선언 위치:** [Common/Camera.h](Common/Camera.h)

## 19. practice::MakeVertices

CPU의 vector 끝에 위치 xyz와 밝기 rgb로 이루어진 정점 하나를 추가한다.

**헤더:** `Common/PracticeScene.h`  
**관련 실습:** 17번 선 도형 직접 사용, 17~20번 MakeObjects 내부 사용.  
**함께 사용할 함수:** MakeMesh, DrawObj

**사용 상황**

정점 추가, 선분·삼각형·원·구·원뿔 데이터를 직접 만들 때 사용한다.

**매개변수**

- vertices: float 배열을 담은 std::vector<float>&. 끝에 float 6개를 추가하므로 내용이 변한다.
- position: 도형 자체의 모델 좌표(x,y,z, 장면 거리 단위).
- shade: r,g,b에 똑같이 넣을 밝기(float, 보통 0~1). 생략하면 1.0f. 각 RGB 색을 따로 받는 함수는 아니다.

**반환값**

반환값 없음(void). vertices.size()가 6 증가한다. GPU로 보내지는 않는다.

**복사 예제**

OpenGL 컨텍스트를 만든 뒤 실행합니다.

```cpp
std::vector<float> vertices;
practice::MakeVertices(vertices, glm::vec3(-0.5f, 0.0f, 0.0f), 1.0f);
practice::MakeVertices(vertices, glm::vec3( 0.5f, 0.0f, 0.0f), 1.0f);
// 이제 정점 2개 = float 12개입니다. GL_LINES로 선분 하나를 만들 수 있습니다.
```

**주의사항**

- 정점 개수와 float 개수를 혼동하지 않는다. 정점 개수는 size()/6이다.
- vector를 수정해도 이미 GPU로 보낸 VBO는 자동 갱신되지 않는다. MakeMesh 또는 별도 버퍼 갱신이 필요하다.
- 17~20번의 fragment 셰이더에서는 이 밝기와 DrawObj의 objectColor가 곱해진다. 충돌 검사나 인덱스 추가는 하지 않는다.

**정의·선언 위치:** [Common/PracticeScene.h](Common/PracticeScene.h)

## 20. practice::MakeMesh

위치·색 정점 배열을 GPU VBO에 보내고 VAO를 만들어 Mesh에 저장한다.

**헤더:** `Common/PracticeScene.h`  
**관련 실습:** 17번 선 도형, 17~20번 공통 도형 준비.  
**함께 사용할 함수:** MakeVertices, configurePositionColorAttributes, DrawObj

**사용 상황**

직접 만든 선·삼각형·궤도·도형을 GPU에서 그릴 준비를 할 때 사용한다.

**매개변수**

- mesh: 결과 VAO/VBO/count/mode를 저장할 practice::Mesh&. 사용하지 않은 빈 Mesh를 전달한다.
- vertices: xyzrgb 반복 형식의 const std::vector<float>&. 길이는 6의 배수여야 한다.
- mode: 그리는 방식(GLenum). 생략하면 GL_TRIANGLES. 선분은 GL_LINES, 닫힌 궤도는 GL_LINE_LOOP.

**반환값**

반환값 없음(void). mesh.VAO, mesh.VBO를 새로 만들고 count=size()/6, mode를 저장한다.

**복사 예제**

OpenGL 컨텍스트를 만든 뒤 실행합니다.

```cpp
std::vector<float> vertices;
practice::MakeVertices(vertices, glm::vec3(-0.5f, 0.0f, 0.0f));
practice::MakeVertices(vertices, glm::vec3( 0.5f, 0.0f, 0.0f));
practice::Mesh line;
practice::MakeMesh(line, vertices, GL_LINES);
// 그리기가 끝나고 컨텍스트가 살아 있을 때 정리합니다.
glDeleteBuffers(1, &line.VBO);
glDeleteVertexArrays(1, &line.VAO);
```

**주의사항**

- 같은 mesh에 반복 호출하면 이전 VAO/VBO를 자동 삭제하지 않아 자원이 누출된다. 이전 번호를 먼저 정리한다.
- EBO 없이 glDrawArrays로 그리는 형식이다. GL_LINES는 정점 2개씩, GL_TRIANGLES는 3개씩 묶인다.
- 함수가 끝나도 대상 VAO와 VBO가 바인딩된 상태이다. 사용자 정의 Mesh는 RunScene이 자동 정리하지 않는다.

**정의·선언 위치:** [Common/PracticeScene.h](Common/PracticeScene.h)

## 21. practice::MakeObjects

공통 육면체·구·원뿔·궤도 원·좌표축·바닥 Mesh 6개를 준비한다.

**헤더:** `Common/PracticeScene.h`  
**관련 실습:** 17~20번. 17번은 초기 공통 도형을 선 도형으로 교체.  
**함께 사용할 함수:** MakeVertices, MakeMesh, RunScene, DrawObj

**사용 상황**

17~20번에서 미리 만들어 둔 도형을 바로 그리거나, 직접 반복문을 구성할 때 초기 준비에 사용한다.

**매개변수**

- 인자 없음. practice::cube, sphere, cone, circle, axis, plane 전역 Mesh에 결과를 저장한다.

**반환값**

반환값 없음(void). GPU VAO/VBO가 도형마다 생성된다.

**복사 예제**

OpenGL 컨텍스트를 만든 뒤 실행합니다.

```cpp
practice::MakeObjects();
// 이 뒤부터 practice::cube, practice::sphere 등을 DrawObj에 전달할 수 있습니다.
// RunScene을 쓴다면 내부에서 이미 호출하므로 다시 호출하지 않습니다.
```

**주의사항**

- cube는 중심 원점, 한 변 1; sphere는 반지름 1; cone은 y축 높이 1, 바닥 반지름 0.5이다.
- plane은 y=0인 xz 평면의 한 변 1 정사각형; circle은 y=0인 xz 평면의 반지름 1 원이다. axis는 각 축 -1~1이다.
- 한 번만 호출한다. 이전 Mesh를 정리하지 않고 다시 호출하면 누출된다.
- 17번은 공통 도형 준비 후 자체 MakeWireObjects에서 일부를 선 도형으로 교체한다. RunScene 내부 호출과 구분한다.

**정의·선언 위치:** [Common/PracticeScene.h](Common/PracticeScene.h)

## 22. practice::MakeModelMatrix

부모 행렬 뒤에 위치 이동과 크기 변경을 붙인 모델 행렬을 만든다.

**헤더:** `Common/PracticeScene.h`  
**관련 실습:** 17번 변환 원리, 18번 행성·달, 19번 부모-자식 탱크, 20번 벨트·상자·숫자.  
**함께 사용할 함수:** glm::translate/rotate/scale, DrawObj

**사용 상황**

물체 배치·크기 변경, 부모-자식 부품, 탱크 몸체와 포신, 벨트 위 상자·숫자, 공전 배치에 사용한다.

**매개변수**

- position: 부모 좌표계에서의 위치(glm::vec3, 장면 거리 단위). 부모 생략 시 월드 위치.
- size: x/y/z 크기 배율(glm::vec3, 단위 없음). (1,1,1)이 기본 크기. 도형 원래 크기에 곱해진다.
- parent: 부모의 glm::mat4. 생략하면 단위행렬 glm::mat4(1.0f). 부모가 없으면 명시적으로 전달하지 않아도 된다.

**반환값**

parent * T(position) * S(size) 형태의 glm::mat4. 실제 정점에는 S → T → parent 순서로 적용된다.

**복사 예제**

행렬 계산만 하므로 OpenGL 컨텍스트 없이도 사용할 수 있습니다.

```cpp
glm::mat4 parent(1.0f);
parent = glm::rotate(parent, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 model = practice::MakeModelMatrix(
    glm::vec3(2.0f, 0.0f, 0.0f),
    glm::vec3(0.5f),
    parent
); // 원점에서 2만큼 떨어진 자리를 부모가 45도 돌립니다. 공전 배치입니다.
```

**주의사항**

- 함수 자체에는 angle 인자가 없으며 제자리 회전을 자동으로 만들지 않는다. 자전은 T * R * S로 따로 합성한다.
- parent에 회전을 넣으면 위치도 돌아 공전이 된다. 부모에 크기 변경이 있으면 자식의 위치와 크기도 영향을 받는다.
- size.x=2는 항상 가로 길이 2가 아니다. sphere 원래 지름은 2이므로 size=0.5일 때 지름 1이다.
- 원점 확대와 제자리 확대는 행렬 순서가 다르다. 설명서의 자전/공전/기준점 예제를 비교한다.

**정의·선언 위치:** [Common/PracticeScene.h](Common/PracticeScene.h)

## 23. practice::DrawObj

준비된 Mesh 하나를 모델 행렬과 색으로 그린다.

**헤더:** `Common/PracticeScene.h`  
**관련 실습:** 17~20번.  
**함께 사용할 함수:** MakeObjects/MakeMesh, MakeModelMatrix, Shader::use/setMatrix, RunScene

**사용 상황**

행성·탱크 부품·상자·숫자·축·궤도 등 같은 도형을 다른 위치와 색으로 여러 번 그릴 때 사용한다.

**매개변수**

- mesh: MakeMesh/MakeObjects로 준비한 Mesh의 const 참조. count는 정점 개수, mode는 그리기 방식.
- model: 물체의 glm::mat4 모델 행렬. model 좌표를 월드로 옮긴다.
- color: objectColor로 보낼 RGB(glm::vec3, 보통 각 성분 0~1). 밝기 정점 색과 곱해진다.
- modelLocation: 같은 shader의 "model" uniform 위치(GLint).
- shader: 현재 use()한 common::Shader&. 복사하지 않는다.

**반환값**

반환값 없음(void). 모델/색 uniform을 전송하고 VAO를 바인딩하여 glDrawArrays로 그린다.

**복사 예제**

OpenGL 컨텍스트를 만든 뒤 실행합니다.

```cpp
shader.use();
GLint modelLocation = shader.uniformLocation("model");
glm::mat4 model = practice::MakeModelMatrix(glm::vec3(0.0f), glm::vec3(0.5f));
practice::DrawObj(practice::cube, model, glm::vec3(1.0f, 0.2f, 0.2f), modelLocation, shader);
```

**주의사항**

- view/projection은 여기서 보내지 않는다. 한 화면을 그리기 전에 호출자가 설정한다.
- practice::colorLocation에 같은 프로그램의 "objectColor" 위치가 설정되어 있어야 한다. RunScene은 준비하지만 기본 positionColorFragment에는 이 uniform이 없다.
- 기본 fragment 셰이더를 직접 쓸 때는 색을 바꾸려면 별도의 대응 셰이더가 필요하다. 설명서의 최소 실행 예제는 RunScene을 사용해 이 조건을 맞춘다.
- 충돌을 검사하거나 물체 상태를 갱신하지 않는다. 은면 제거·wireframe·viewport도 호출자가 정한다.

**정의·선언 위치:** [Common/PracticeScene.h](Common/PracticeScene.h)

## 24. practice::MakeProjection

원근 투영 또는 직각 투영의 projection 행렬을 만든다.

**헤더:** `Common/PracticeScene.h`  
**관련 실습:** 17~20번, 특히 18·20번의 투영 전환.  
**함께 사용할 함수:** glfwGetFramebufferSize, Shader::setMatrix, glm::lookAt

**사용 상황**

18번 p/P, 20번 p/P처럼 직각/원근 화면을 전환하거나 창 크기 변화에 대응할 때 사용한다.

**매개변수**

- width,height: 현재 framebuffer 너비·높이(픽셀, 둘 다 양수).
- perspective: true면 원근 투영, false면 직각 투영.
- extent: 직각 투영에서 화면 중심부터 위/아래 끝까지의 거리(장면 거리 단위, 양수). 원근 투영에서는 이 인자를 사용하지 않는다.

**반환값**

glm::mat4 projection. 원근은 시야각 45도·near 0.1·far 100. 직각은 y범위 ±extent, x범위 ±extent*(width/height), near 0.1·far 100.

**복사 예제**

행렬 계산만 하므로 OpenGL 컨텍스트 없이도 사용할 수 있습니다.

```cpp
glm::mat4 perspective = practice::MakeProjection(1200, 900, true, 4.8f);
glm::mat4 orthographic = practice::MakeProjection(1200, 900, false, 4.8f);
// 실제 반복문에서는 위 숫자 대신 양수인지 확인한 framebuffer 크기를 넣습니다.
```

**주의사항**

- height=0이면 나눗셈이 깨진다. RunScene은 최소화 시 건너뛰지만 직접 호출할 때는 검사한다.
- 원근에서는 같은 크기의 물체가 카메라 깊이에 따라 다르게 보인다. 직각에서는 깊이가 달라도 크기가 같다.
- 원근 화면의 확대를 extent로 조절할 수 없다. 카메라 거리 또는 별도 시야각 설정이 필요하다.
- 물체 이동은 model, 카메라 시점은 view, 투영 방식은 projection이다. 서로 대신 쓰지 않는다.

**정의·선언 위치:** [Common/PracticeScene.h](Common/PracticeScene.h)

## 25. practice::RunScene

창·셰이더·공통 도형을 준비하고 입력·갱신·그리기 반복과 종료 정리를 맡는다.

**헤더:** `Common/PracticeScene.h`  
**관련 실습:** 17~20번.  
**함께 사용할 함수:** KeyCallback, UpdateScene, DrawAll, MakeObjects, DrawObj

**사용 상황**

17~20번처럼 과제별 키 입력·애니메이션·그리기만 작성하여 실행할 때 사용한다.

**매개변수**

- title: 창 제목 문자열(const char*). 창은 1200×900 화면 좌표 크기로 생성된다.
- keyCallback: GLFWkeyfun. void(GLFWwindow*,int key,int scancode,int action,int mods) 형식의 함수 주소.
- UpdateScene: void(float) 함수 주소. 매 프레임 deltaTime(초)을 받는다.
- DrawAll: void(common::Shader&,int width,int height) 함수 주소. width/height는 framebuffer 픽셀 수.

**반환값**

정상 종료 0, 창 또는 셰이더 초기화 실패 -1. main에서 return practice::RunScene(...);으로 사용한다.

**복사 예제**

main에서 사용합니다. 호출 전에 창이나 기본 Mesh를 별도로 만들지 않습니다.

```cpp
// 아래 함수들은 전역 범위에 정의합니다. 전체 실행 예제는 FUNCTION_GUIDE.md의 첫 예제입니다.
// void KeyCallback(GLFWwindow*, int, int, int, int);
// void UpdateScene(float);
// void DrawAll(common::Shader&, int, int);
return practice::RunScene("그래픽스 실습", KeyCallback, UpdateScene, DrawAll);
```

**주의사항**

- 호출한 자리에서 종료까지 반복하므로 다른 게임 반복문 안에서 호출하지 않는다. callback은 null이 아닌 유효한 함수를 전달한다.
- UpdateScene은 초 단위 시간을 받으며 함수 내부에서 최대 0.05초로 제한한다. 긴 정지나 매우 낮은 FPS에서는 실제 벽시계 시간보다 애니메이션이 느려질 수 있다.
- 실행 순서는 이벤트 처리 → 시간·화면 크기 확인 → UpdateScene → 화면 지우기 → DrawAll → 화면 교환이다.
- model/view/projection 행렬은 DrawAll에서 설정한다. 셰이더와 objectColor, 6개 기본 Mesh, depth test는 내부에서 준비한다.
- 자동 정리 대상은 6개 기본 Mesh와 내부 Shader이다. 추가 Mesh의 생성·삭제는 직접 처리한다.
- 17번의 선 도형 교체는 UpdateScene 첫 호출에서 수행한다. GLFW 컨텍스트가 생기기 전 전역 초기화로 GPU 함수를 호출하지 않는다.

**정의·선언 위치:** [Common/PracticeScene.h](Common/PracticeScene.h)

## 검색 조합 예제 — 기존 함수와 GLM을 함께 쓰기

아래 코드는 새로운 공통 함수를 추가한 것이 아닙니다. `MakeOrbitMatrix` 같은 이름의 공통 함수가 있다고 가정하지 마세요. 기존 과제별 동명 함수는 파일에 따라 인자와 동작이 다릅니다. 예제의 `model`을 `DrawObj`에 전달하거나 셰이더의 model uniform에 보내면 됩니다.

### 자전 — T * R * S

관련 실습: 17번. 헤더: `Common/PracticeScene.h`. 위치는 그대로 두고 자기 중심에서 y축으로 45도 회전합니다.

```cpp
glm::mat4 model(1.0f);
model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
model = glm::scale(model, glm::vec3(0.5f));
// 실제 점에는 크기 변경 → 회전 → 이동 순서로 작용합니다.
```

### 공전 — R * T * S

관련 실습: 17·18번. 헤더: `Common/PracticeScene.h`. 부모 회전이 이동 벡터도 돌리므로 원점 둘레로 배치됩니다. 같은 각도를 시간에 따라 바꾸면 공전 애니메이션입니다. 회전축은 0이 아닌 방향 벡터이며 예제에서는 y축입니다.

```cpp
float angle = 45.0f; // 도 단위입니다. 매 프레임 속도(도/초) * deltaTime을 더할 수 있습니다.
glm::mat4 orbit(1.0f);
orbit = glm::rotate(orbit, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 model = practice::MakeModelMatrix(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.5f), orbit);
// 위치만 공전하고 원래 방향을 유지하려면 이 결과의 회전까지 그대로 사용하지 말고,
// 회전시킨 위치를 따로 구한 뒤 T * 자전R * S로 조립합니다.
```

### 기준점·모서리를 중심으로 돌리기 — T(pivot) * R * T(-pivot)

관련 실습: 15·16번 면 펼치기. 헤더: `Common/PracticeScene.h`. 물체 꼭짓점이 어떤 좌표로 정의되었는지와 pivot의 좌표계가 같아야 합니다.

```cpp
glm::vec3 pivot(0.0f, -0.5f, 0.5f);
glm::mat4 model(1.0f);
model = glm::translate(model, pivot);
model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
model = glm::translate(model, -pivot);
// 점을 -pivot으로 옮김 → 원점에서 회전 → pivot으로 되돌림 순서입니다.
```

### 부모 행렬 — 몸체를 따라오는 포신·상자 위 숫자

관련 실습: 19·20번. 헤더: `Common/PracticeScene.h`. 부모를 전달하지 않으면 자식은 월드에 홀로 놓입니다. 부모 크기 변경까지 전달하면 자식의 상대 거리도 늘어나므로 필요하면 크기 변경 전 부모 행렬을 전달합니다.

```cpp
glm::mat4 body = practice::MakeModelMatrix(glm::vec3(1.0f, 0.5f, 0.0f), glm::vec3(1.0f));
glm::mat4 gun = practice::MakeModelMatrix(
    glm::vec3(0.0f, 0.0f, 0.6f),
    glm::vec3(0.1f, 0.1f, 0.8f),
    body
);
```

### 카메라 이동 — position과 target을 함께 옮기기

관련 실습: 19번 카메라 이동 응용. 헤더: `Common/Camera.h`. 이 예제에서 Camera 변수를 새로 만들지만, 실제 반복문에서는 전역/지속 변수의 위치를 갱신하고 기존 camera.setPose를 호출합니다.

```cpp
glm::vec3 eye(0.0f, 2.0f, 5.0f);
glm::vec3 target(0.0f);
glm::vec3 movement(0.2f, 0.0f, 0.0f);
common::Camera camera(eye, target, glm::vec3(0.0f, 1.0f, 0.0f));
eye += movement;
target += movement;
camera.setPose(eye, target, glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 view = camera.view();
```

### 카메라 자전 — 제자리에서 고개 돌리기

관련 실습: 19번. 헤더: `Common/Camera.h`, `<GL/glm/gtc/matrix_transform.hpp>`. 여기서는 카메라 자신의 up 방향을 회전축으로 씁니다. 물체 이동이 아닙니다.

```cpp
glm::vec3 eye(0.0f, 2.0f, 5.0f);
glm::vec3 forward = glm::normalize(-eye);
glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
glm::vec3 up = glm::normalize(glm::cross(right, forward));
glm::mat4 turn = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), up);
glm::vec3 newForward = glm::vec3(turn * glm::vec4(forward, 0.0f));
common::Camera camera(eye, eye + newForward, up);
glm::mat4 view = camera.view();
// 방향 벡터의 마지막 값은 0이라 이동의 영향을 받지 않습니다. 위치라면 1입니다.
```

### 카메라 공전 — 원점 주위를 이동하며 원점 보기

관련 실습: 19·20번. 헤더: `Common/Camera.h`, `<GL/glm/gtc/matrix_transform.hpp>`. 기존 시점과 새 시점의 거리를 유지하며 월드 y축 주위를 돕니다.

```cpp
glm::vec3 eye(0.0f, 2.0f, 5.0f);
glm::mat4 orbit = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
glm::vec3 newEye = glm::vec3(orbit * glm::vec4(eye, 1.0f));
common::Camera camera(newEye, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 view = camera.view();
```

### 충돌·겹침 — 공통 충돌 함수는 없음

`DrawObj`는 그리기만 하고, `MakeModelMatrix`는 행렬만 만듭니다. 어느 쪽도 충돌을 검사하지 않습니다. 19번의 `std::clamp`는 탱크 중심의 바닥 범위를 제한하는 것이며 일반 물체 충돌 함수가 아닙니다.

다음은 참고용으로 직접 작성하는 **회전 없는 축 정렬 상자(AABB) 겹침 검사**입니다. 헤더: `<GL/glm/glm.hpp>`. 관련 실습: 19번 범위 제한에서 확장할 수 있으나 기존 공통 구현은 아닙니다. center는 월드 중심 위치, halfSize는 양수인 가로·세로·깊이의 절반입니다. 경계만 닿아도 true가 됩니다. 회전한 상자에는 이 크기를 그대로 사용할 수 없고 회전 후 월드 경계나 다른 충돌 방식을 계산해야 합니다.

```cpp
glm::vec3 firstCenter(0.0f, 0.5f, 0.0f);
glm::vec3 secondCenter(0.8f, 0.5f, 0.0f);
glm::vec3 firstHalfSize(0.5f);
glm::vec3 secondHalfSize(0.5f);
glm::vec3 distance = glm::abs(firstCenter - secondCenter);
glm::vec3 limit = firstHalfSize + secondHalfSize;
bool overlaps = distance.x <= limit.x && distance.y <= limit.y && distance.z <= limit.z;
// true면 세 축의 범위가 모두 겹칩니다. 실제 위치를 밀어내는 처리는 별도로 작성합니다.
```

### 화면이 안 보일 때 점검 순서

1. 창 반환값이 nullptr인지, shader.valid()가 false인지 확인합니다.
2. 올바른 VAO/VBO, xyzrgb 순서와 정점 개수(count)를 확인합니다.
3. shader.use() 후 같은 shader에서 구한 model/view/projection 위치에 값을 보냈는지 확인합니다.
4. 단위행렬은 `glm::mat4(1.0f)`입니다. 초기화하지 않은 행렬이나 기본 uniform의 0행렬을 쓰지 않습니다.
5. 카메라의 position과 target이 다른지, up이 시선과 나란하지 않은지 확인합니다.
6. framebuffer 크기가 양수인지, glViewport와 glClear, glfwSwapBuffers를 호출했는지 확인합니다.
7. 물체가 near/far 범위와 화면 밖으로 나갔는지, GL_DEPTH_TEST가 필요한 상황인지 확인합니다.
8. practice::DrawObj라면 RunScene의 objectColor uniform 준비가 되어 있는지 확인합니다.

### 자원 수명 — 준비와 정리 순서

`CreateGLWindow → Shader/VAO/VBO 준비 → 반복 갱신·그리기 → VAO/VBO·Shader 정리 → glfwDestroyWindow → glfwTerminate`입니다. `RunScene`을 사용하면 기본 자원의 이 과정을 내부에서 수행합니다. 셰이더 프로그램 번호와 uniform 위치는 같은 프로그램 안에서만 의미가 있습니다.

### GLSL 진입점과 셰이더 문자열

- `common::shader_sources::positionColorVertex`: 위치 aPos와 색 aColor를 받아 `projection * view * model`로 gl_Position을 계산합니다. 변환은 model부터 적용됩니다. `ourColor`를 다음 단계로 전달합니다. 헤더: `Common/ShaderSources.h`. 관련: 16~20번, 21번 템플릿.
- `common::shader_sources::positionColorFragment`: 보간된 ourColor를 불투명 RGBA로 출력합니다. objectColor는 없습니다. 관련: 16번, 21번 템플릿.
- `practice::RunScene` 내부 fragment: `ourColor * objectColor`로 색을 만듭니다. `practice::DrawObj`와 함께 쓰는 17~20번용 셰이더입니다.
- 이 문자열 안의 `void main()`은 GPU에서 실행됩니다. C++의 main과 다르며 CPU에서 함수로 호출하지 않습니다. 사용 예는 Shader 생성자의 예제를 참고하세요.

## 실습 21~24 — 방과 여러 뷰포트

[실행·조작 안내](Practice21-24.md)

- **방 벽 / 안쪽 컬링 — `room::DrawWalls`**: 좌우·뒤·위 네 면을 안쪽을 향하게 그린다. `Common/RoomScene.h`, `room::Panel`, `glEnable(GL_CULL_FACE)`와 함께 사용한다. 관련 실습: 21·23번.
- **정점 순서 / 바닥 / 문 — `room::Panel`**: 기존 xz 평면을 이동·회전·확대해서 방의 면 하나를 그린다. `Common/RoomScene.h`, `practice::MakeObjects`, `practice::DrawObj`와 함께 사용한다. 관련 실습: 21·23번.
- **뷰포트 / 같은 장면 반복 출력 — `tank::DrawWorld`**: 19번과 22번의 탱크 및 바닥을 그린다. `homework19/TankScene.h`를 포함하고 `view`, `projection`, `glViewport`를 설정한 후 사용한다. `tank::UpdateScene`은 프레임마다 한 번만 호출한다.

```cpp
// practice::RunScene의 DrawAll 안에서 사용합니다.
GLint modelLocation = shader.uniformLocation("model");
glm::mat4 roomModel(1.0f);
glEnable(GL_CULL_FACE);
room::DrawWalls(roomModel, glm::vec2(3.0f, 3.0f), 4.0f, modelLocation, shader);
room::Panel(roomModel, glm::vec3(0.0f), glm::vec2(6.0f), 0.0f,
    glm::vec3(1, 0, 0), glm::vec3(0.4f, 0.6f, 0.4f), modelLocation, shader);
glDisable(GL_CULL_FACE);
```

### 충돌 — 21번 공, 23번 로봇

21번의 `Reflect`는 공 중심이 반지름을 고려한 방의 경계를 넘으면 초과 거리와 속도 방향을 반사한다. 23번의 `Overlaps`는 xz 평면에서 로봇과 장애물의 겹침을, `SupportHeight`는 발 아래의 지지 높이를 계산한다. 모두 해당 과제의 상태와 좌표에 맞춘 함수다. 범용 공통 충돌 함수로 등록하지 않았다.

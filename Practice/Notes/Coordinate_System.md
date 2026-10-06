# OpenGL 좌표계 완전 정리

## 1. 세 가지 좌표

실습에서 반드시 구분한다.

1. GLFW 윈도우/마우스 픽셀 좌표
2. OpenGL NDC 좌표
3. 객체의 로컬 좌표

## 2. GLFW 마우스 좌표

왼쪽 위가 `(0,0)`이고 오른쪽으로 x, 아래로 y가 증가한다.

## 3. OpenGL NDC

화면 중앙이 `(0,0)`이다. 기본적으로 화면에 표시되는 NDC 범위를 `[-1,1]`로 생각할 수 있다.

x축은 왼쪽에서 오른쪽으로 증가하고, y축은 아래에서 위로 증가한다.

## 4. 마우스 좌표를 OpenGL 좌표로 변환

창 크기가 `width × height`라면:

~~~cpp
float xGL =
    (x / width) * 2.0f - 1.0f;

float yGL =
    1.0f -
    (y / height) * 2.0f;
~~~

예를 들어 1600×1200 창에서 `(0,0)`은 `(-1,1)`, `(800,600)`은 `(0,0)`, `(1600,1200)`은 `(1,-1)`이 된다.

y에 `1.0f -`가 필요한 이유는 윈도우와 OpenGL의 y축 방향이 반대이기 때문이다.

## 5. 로컬 좌표

도형 자체의 모양은 로컬 좌표로 정의하고, 실제 화면에서의 위치는 Model Matrix로 따로 처리하는 것이 편하다.

예를 들어 중심이 `(0,0)`인 사각형은:

~~~text
(-0.2,0.2)       (0.2,0.2)
      +-----------+
      |   (0,0)   |
      +-----------+
(-0.2,-0.2)      (0.2,-0.2)
~~~

처럼 정의할 수 있다.

## 6. 좌표계 혼동 방지

픽셀 좌표와 OpenGL 좌표를 직접 더하면 안 된다.

~~~text
mouse pixel
    ↓
OpenGL coordinate
    ↓
object position
    ↓
model matrix
    ↓
local vertex
~~~

## 7. 디버깅

좌표 문제는 좌상단, 중앙, 우하단 세 점을 먼저 확인한다.

~~~cpp
std::cout << "mouse = "
          << mouseX << ", "
          << mouseY << '\\n';

std::cout << "GL = "
          << glX << ", "
          << glY << '\\n';
~~~

## 8. 전체 흐름

~~~text
마우스 픽셀 좌표
      ↓
OpenGL 좌표 변환
      ↓
객체 위치 / 로컬 좌표
      ↓
Model Matrix
      ↓
Vertex Shader
      ↓
NDC
      ↓
Viewport
      ↓
Framebuffer
      ↓
화면
~~~

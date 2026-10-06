# Practice 12 — 마우스 이동으로 좌표 추적하기

현재 저장소의 Practice 11 이후 학습 흐름과 OpenGL 기초 강의자료의 마우스 이동 Callback 내용을 연결하여 정리한 실습 문서다.

## 1. 목표

GLFW의 마우스 이동 Callback을 사용하여 마우스가 움직일 때마다 현재 위치를 받고, 필요하면 윈도우 좌표를 OpenGL 좌표로 변환한다.

## 2. Callback 선언

~~~cpp
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
~~~

## 3. Callback 등록

윈도우 생성 및 Context 활성화 후 등록한다.

~~~cpp
glfwSetCursorPosCallback(window, CursorPosCallback);
~~~

## 4. 가장 먼저 확인할 구현

~~~cpp
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    std::cout << xpos << ", " << ypos << '\\n';
}
~~~

마우스를 움직였을 때 좌표가 계속 출력되면 Callback 연결은 성공한 것이다.

## 5. 윈도우 좌표 → OpenGL 좌표

GLFW 마우스 좌표는 일반적으로 왼쪽 위가 (0,0)이고 아래로 갈수록 y가 증가한다. OpenGL의 기본 NDC는 중앙이 (0,0)이고 위로 갈수록 y가 증가한다.

창 크기를 wide, height라고 하면:

~~~cpp
float glX =
    (static_cast<float>(xpos) / wide) * 2.0f - 1.0f;

float glY =
    1.0f -
    (static_cast<float>(ypos) / height) * 2.0f;
~~~

1600×1200 창에서는 (0,0) → (-1,1), (800,600) → (0,0), (1600,1200) → (1,-1)이 된다.

## 6. Polling과 Callback

마우스 버튼 클릭은 glfwSetMouseButtonCallback(), 마우스 위치 이동은 glfwSetCursorPosCallback()을 사용한다.

드래그에서는 보통 버튼 Callback으로 dragging 상태를 관리하고, 마우스 이동을 받아 선택된 도형의 위치를 변경한다.

## 7. 중요한 주의점

Callback을 등록해도 glfwPollEvents()가 필요하다.

~~~cpp
glfwPollEvents();
~~~

를 메인 루프에서 계속 호출해야 GLFW가 OS 이벤트를 처리하고 등록된 Callback을 실행할 수 있다.

## 8. 완료 기준

- [ ] 마우스 이동 시 Callback이 호출된다.
- [ ] xpos, ypos를 출력할 수 있다.
- [ ] 윈도우 좌표와 OpenGL 좌표의 원점 위치 차이를 설명할 수 있다.
- [ ] x 좌표를 [-1,1] 범위로 변환할 수 있다.
- [ ] y 좌표를 [-1,1] 범위로 변환할 수 있다.
- [ ] y축 변환 방향이 반대인 이유를 설명할 수 있다.
- [ ] Mouse Button Callback과 Cursor Position Callback의 역할을 구분할 수 있다.

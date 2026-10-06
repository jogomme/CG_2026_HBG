# GLFW 입력과 시간 정리

## 1. Polling

Polling은 프로그램이 현재 상태를 직접 확인하는 방식이다.

~~~cpp
if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
{
    // 현재 A가 눌려 있음
}
~~~

키를 누르는 동안 계속 이동시키는 기능 등에 편하다.

## 2. Callback

Callback은 이벤트가 발생하면 GLFW가 등록된 함수를 호출한다.

~~~cpp
glfwSetKeyCallback(window, KeyCallback);
~~~

## 3. Key Callback

~~~cpp
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
}
~~~

`action`은 `GLFW_PRESS`, `GLFW_RELEASE`, `GLFW_REPEAT` 등을 사용한다.

한 번 눌렀을 때만 처리하려면:

~~~cpp
if (action != GLFW_PRESS)
{
    return;
}
~~~

## 4. Mouse Button Callback

~~~cpp
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
}
~~~

왼쪽은 `GLFW_MOUSE_BUTTON_LEFT`, 오른쪽은 `GLFW_MOUSE_BUTTON_RIGHT`다.

## 5. Mouse Move Callback

~~~cpp
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
}
~~~

마우스 이동과 드래그 추적에 사용한다.

## 6. Scroll Callback

~~~cpp
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
}
~~~

줌이나 크기 조절 등에 사용할 수 있다.

## 7. Char Callback

~~~cpp
void CharCallback(GLFWwindow* window, unsigned int codepoint)
{
}
~~~

게임 조작보다 실제 문자 입력에 적합하다.

## 8. glfwPollEvents

Callback을 사용해도 메인 루프에서 다음을 호출해야 한다.

~~~cpp
glfwPollEvents();
~~~

이 함수가 GLFW가 처리해야 할 OS 이벤트를 처리하는 중요한 지점이다.

## 9. 시간

~~~cpp
double currentTime = glfwGetTime();
~~~

프로그램 시작 이후 경과한 시간을 초 단위로 얻는다.

## 10. Delta Time

~~~cpp
double lastTime = glfwGetTime();

while (...)
{
    double currentTime = glfwGetTime();
    double deltaTime = currentTime - lastTime;
    lastTime = currentTime;
}
~~~

`deltaTime`은 프레임 간 경과 시간이다.

FPS에 독립적인 이동:

~~~cpp
position += speed * static_cast<float>(deltaTime);
~~~

## 11. 선택 기준

| 목적 | 방법 |
|---|---|
| 키를 누르는 동안 계속 이동 | Polling |
| 키를 눌렀을 때 명령 실행 | Key Callback |
| 마우스 클릭 | Mouse Button Callback |
| 마우스 이동/드래그 | Cursor Position Callback |
| 휠 | Scroll Callback |
| 텍스트 입력 | Char Callback |
| 애니메이션 시간 | `glfwGetTime()` |
| FPS 독립 이동 | Delta Time |

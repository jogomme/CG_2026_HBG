# Practice 13 — 마우스 휠과 문자 입력 Callback

현재 OpenGL 기초 강의자료의 Scroll Callback과 Char Callback 내용을 실습 형태로 정리했다.

## 1. 목표

GLFW Callback으로 마우스 휠과 실제 문자 입력을 받는다.

## 2. 마우스 휠 Callback

선언:

~~~cpp
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
~~~

등록:

~~~cpp
glfwSetScrollCallback(window, ScrollCallback);
~~~

구현:

~~~cpp
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    std::cout << "Scroll : " << xoffset << ", " << yoffset << '\\n';
}
~~~

일반적인 수직 스크롤에서는 yoffset > 0이면 위, yoffset < 0이면 아래로 해석할 수 있다.

## 3. 휠 입력으로 값 변경

예를 들어 도형 크기를 조절한다.

~~~cpp
float size = 1.0f;

if (yoffset > 0)
{
    size += 0.1f;
}
else if (yoffset < 0)
{
    size -= 0.1f;
}

if (size < 0.1f)
{
    size = 0.1f;
}
~~~

## 4. 문자 입력 Callback

문자 자체를 받고 싶을 때 사용한다.

~~~cpp
void CharCallback(GLFWwindow* window, unsigned int codepoint);
~~~

등록:

~~~cpp
glfwSetCharCallback(window, CharCallback);
~~~

구현:

~~~cpp
void CharCallback(GLFWwindow* window, unsigned int codepoint)
{
    std::cout << "Unicode = " << codepoint << '\\n';
    std::cout << "Character = " << static_cast<char>(codepoint) << '\\n';
}
~~~

## 5. Key Callback과 Char Callback

Key Callback은 "어떤 키를 눌렀는가"를 처리한다. 게임 조작에 적합하다.

Char Callback은 "어떤 문자가 입력되었는가"를 처리한다. 텍스트 입력에 적합하다.

~~~text
게임 조작 → Key Callback
문자 입력 → Char Callback
~~~

## 6. 완료 기준

- [ ] 휠 Callback을 등록할 수 있다.
- [ ] yoffset의 방향을 이해한다.
- [ ] 휠로 변수 값을 증가/감소시킬 수 있다.
- [ ] Char Callback을 등록할 수 있다.
- [ ] codepoint의 의미를 이해한다.
- [ ] Key Callback과 Char Callback을 구분할 수 있다.

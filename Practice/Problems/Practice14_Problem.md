# Practice 14 — 창 크기 변경과 Viewport 관리

현재 OpenGL 기초 강의자료의 Framebuffer Size Callback, Window Size Callback, glViewport 내용을 실습 형태로 정리했다.

## 1. 목표

창의 크기가 변경될 때 실제 렌더링 영역을 새 크기에 맞게 업데이트한다.

## 2. Framebuffer Size Callback

선언:

~~~cpp
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
~~~

등록:

~~~cpp
glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
~~~

구현:

~~~cpp
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
~~~

동작은 다음과 같다.

~~~text
Framebuffer 크기 변경
        ↓
Callback 호출
        ↓
새 width / height 전달
        ↓
glViewport() 재설정
        ↓
새 렌더링 영역 사용
~~~

## 3. Window Size와 Framebuffer Size

Window Size는 논리적인 창 크기다.

Framebuffer Size는 실제 OpenGL이 렌더링하는 픽셀 영역의 크기다.

일반적인 환경에서는 같아 보일 수 있지만 HiDPI 환경에서는 다를 수 있다. 따라서 OpenGL 렌더링 영역은 Framebuffer Size를 기준으로 설정하는 것이 중요하다.

## 4. Aspect Ratio

~~~cpp
if (height != 0)
{
    float aspectRatio =
        static_cast<float>(width) /
        static_cast<float>(height);
}
~~~

height가 0인 상태에서 나누면 안 된다.

## 5. glViewport와 NDC

기본 NDC는 대략 (-1,1) ~ (1,-1)의 영역이다. glViewport()는 NDC를 실제 framebuffer 픽셀 영역에 매핑하는 데 사용된다.

~~~text
NDC
[-1,1]
   ↓
Viewport
   ↓
Framebuffer
[0,width] × [0,height]
~~~

## 6. 완료 기준

- [ ] Framebuffer Size Callback을 등록할 수 있다.
- [ ] 창 크기가 변경되면 glViewport()를 다시 설정한다.
- [ ] Window Size와 Framebuffer Size의 차이를 설명할 수 있다.
- [ ] Aspect Ratio를 계산할 수 있다.
- [ ] height == 0 상황을 처리할 수 있다.
- [ ] Viewport의 역할을 설명할 수 있다.

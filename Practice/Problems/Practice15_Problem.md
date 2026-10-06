# Practice 15 — 시간과 Delta Time으로 애니메이션 만들기

현재 OpenGL 기초 강의자료와 최신 저장소의 Practice 11.1에서 사용된 GLM 회전 구조를 바탕으로 정리했다.

## 1. 목표

`glfwGetTime()`으로 시간에 따라 도형을 움직이거나 회전시킨다. 또한 Delta Time을 사용하여 FPS에 관계없이 일정한 속도로 움직이는 방법을 익힌다.

## 2. glfwGetTime()

~~~cpp
double time = glfwGetTime();
~~~

프로그램 시작 이후 경과한 시간을 초 단위로 얻는다.

예:

~~~cpp
float x = static_cast<float>(std::sin(time));
~~~

시간이 변하면서 x도 계속 변한다.

## 3. 회전 애니메이션

~~~cpp
glm::mat4 model(1.0f);

model = glm::rotate(
    model,
    angle,
    glm::vec3(0.0f, 0.0f, 1.0f)
);
~~~

시간을 각도로 만들 수 있다.

~~~cpp
float angle =
    static_cast<float>(glfwGetTime()) *
    glm::radians(90.0f);
~~~

약 1초마다 90도씩 회전하는 효과가 된다.

## 4. Delta Time이 필요한 이유

다음처럼 프레임마다 이동하면 FPS에 따라 실제 이동 속도가 달라진다.

~~~cpp
position += 0.01f;
~~~

따라서 프레임 수가 아니라 실제 경과 시간을 이용한다.

## 5. Delta Time 계산

~~~cpp
double lastTime = glfwGetTime();

while (!glfwWindowShouldClose(window))
{
    double currentTime = glfwGetTime();
    double deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // ...
}
~~~

`deltaTime`은 이전 프레임부터 현재 프레임까지 걸린 시간이다.

## 6. 속도 기반 이동

초당 이동 속도를 정한다.

~~~cpp
float speed = 1.0f;

position +=
    speed * static_cast<float>(deltaTime);
~~~

이렇게 하면 FPS가 달라도 실제 시간 기준 속도가 일정해진다.

## 7. 메인 루프 구조

~~~cpp
double lastTime = glfwGetTime();

while (!glfwWindowShouldClose(window))
{
    double currentTime = glfwGetTime();
    double deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    glfwPollEvents();

    // 상태 업데이트
    // position += speed * deltaTime;

    glClear(GL_COLOR_BUFFER_BIT);

    // Model Matrix 생성
    // Uniform 전달
    // Draw

    glfwSwapBuffers(window);
}
~~~

## 8. 전체 흐름

~~~text
glfwGetTime()
      ↓
time / deltaTime
      ↓
position / angle
      ↓
glm::translate / glm::rotate
      ↓
model matrix
      ↓
glUniformMatrix4fv()
      ↓
Vertex Shader
      ↓
화면
~~~

## 9. 주의점

GLM의 회전 함수는 라디안 단위를 사용하므로 `glm::radians(90.0f)`처럼 변환한다.

`glfwGetTime()`은 누적 시간 계산에 편하고, Delta Time은 이동 속도와 게임 로직을 FPS에서 분리할 때 편하다.

## 10. 완료 기준

- [ ] glfwGetTime()을 사용할 수 있다.
- [ ] 시간에 따라 도형을 회전시킬 수 있다.
- [ ] 시간에 따라 도형을 이동시킬 수 있다.
- [ ] Delta Time을 계산할 수 있다.
- [ ] speed * deltaTime의 의미를 설명할 수 있다.
- [ ] FPS가 달라도 속도가 일정해지는 이유를 설명할 수 있다.
- [ ] GLM 회전과 Uniform 전달 과정을 연결해서 설명할 수 있다.

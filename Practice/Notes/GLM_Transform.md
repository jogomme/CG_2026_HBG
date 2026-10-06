# GLM 변환과 Model Matrix

## 1. GLM의 역할

GLM은 그래픽스 수학을 위한 C++ 라이브러리다.

현재 실습에서는 vec2, vec3, vec4, mat4와 translate, rotate, scale이 특히 중요하다.

## 2. Model Matrix

객체의 로컬 좌표를 이동, 회전, 크기 변경하여 장면에 배치하는 행렬로 생각하면 된다.

~~~cpp
glm::mat4 model(1.0f);
~~~

## 3. Translation

~~~cpp
model = glm::translate(model, glm::vec3(x, y, z));
~~~

객체를 이동시킨다.

## 4. Rotation

~~~cpp
model = glm::rotate(
    model,
    angle,
    glm::vec3(0.0f, 0.0f, 1.0f)
);
~~~

2D에서는 보통 Z축을 기준으로 회전한다. GLM 회전 함수에는 라디안 단위를 사용하므로 `glm::radians(90.0f)`처럼 변환한다.

## 5. Scale

~~~cpp
model = glm::scale(
    model,
    glm::vec3(scaleX, scaleY, scaleZ)
);
~~~

객체 크기를 변경한다.

## 6. 여러 변환

~~~cpp
glm::mat4 model(1.0f);

model = glm::translate(model, position);
model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
model = glm::scale(model, scale);
~~~

변환 순서는 중요하다. `Translate → Rotate`와 `Rotate → Translate`는 일반적으로 다른 결과를 만든다.

## 7. 자기 중심 회전

도형의 로컬 정점을 중심 `(0,0)` 기준으로 정의하면 자기 중심 회전이 쉬워진다.

예를 들어 사각형 정점을 중심 기준으로 만들고 Z축 회전을 적용하면 회전 중심도 자연스럽게 도형의 중심이 된다.

## 8. Uniform 전달

GLSL:

~~~glsl
uniform mat4 model;

void main()
{
    gl_Position = model * vec4(aPos, 1.0);
}
~~~

C++:

~~~cpp
GLuint modelLocation =
    glGetUniformLocation(shaderProgram, "model");

glUniformMatrix4fv(
    modelLocation,
    1,
    GL_FALSE,
    &model[0][0]
);
~~~

## 9. 시간과 회전 연결

~~~cpp
double time = glfwGetTime();

float angle =
    static_cast<float>(time) * glm::radians(90.0f);

glm::mat4 model(1.0f);

model = glm::rotate(
    model,
    angle,
    glm::vec3(0.0f, 0.0f, 1.0f)
);
~~~

이 Model Matrix를 Uniform으로 전달하면 시간에 따라 회전하는 도형을 만들 수 있다.

## 10. 디버깅 포인트

- 도형이 너무 멀리 이동한다 → Translation 확인
- 예상과 다른 중심으로 회전한다 → 로컬 좌표와 변환 순서 확인
- 갑자기 커지거나 작아진다 → Scale 확인
- 회전 방향이 이상하다 → 각도 단위와 회전축 확인
- 아무것도 안 보인다 → 최종 Model Matrix 적용 후 좌표가 화면 밖으로 나가지 않는지 확인

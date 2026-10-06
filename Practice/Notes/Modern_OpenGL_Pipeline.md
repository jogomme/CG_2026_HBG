# Modern OpenGL 핵심 구조

## 1. 전체 흐름

~~~text
C++ 프로그램
   ↓
Vertex Data
   ↓
VBO
   ↓
VAO
   ↓
Vertex Shader
   ↓
Primitive Assembly
   ↓
Rasterization
   ↓
Fragment Shader
   ↓
Framebuffer
   ↓
화면
~~~

## 2. VBO

Vertex Buffer Object는 정점 데이터를 GPU에 저장한다.

~~~cpp
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
~~~

## 3. VAO

Vertex Array Object는 정점 속성 연결 상태를 저장한다.

~~~cpp
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);
~~~

일반적인 초기화 순서는:

~~~text
VAO 생성/Bind
    ↓
VBO 생성/Bind
    ↓
glBufferData
    ↓
glVertexAttribPointer
    ↓
glEnableVertexAttribArray
~~~

## 4. 정점 속성

정점 데이터가 `x y z r g b`라면 정점 하나당 6개의 float다.

위치:

~~~cpp
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
~~~

색상:

~~~cpp
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
~~~

`0`과 `1`은 GLSL의 `layout(location = ...)`와 연결된다.

## 5. Vertex Shader

~~~glsl
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 model;
out vec3 ourColor;

void main()
{
    gl_Position = model * vec4(aPos, 1.0);
    ourColor = aColor;
}
~~~

## 6. Fragment Shader

~~~glsl
#version 330 core

in vec3 ourColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}
~~~

## 7. Shader Program

Vertex Shader와 Fragment Shader를 Program으로 link해야 GPU에서 사용할 수 있다.

~~~cpp
GLuint shaderProgram = glCreateProgram();
glAttachShader(shaderProgram, vertexShader);
glAttachShader(shaderProgram, fragmentShader);
glLinkProgram(shaderProgram);
glUseProgram(shaderProgram);
~~~

## 8. Uniform 전달

GLSL:

~~~glsl
uniform mat4 model;
~~~

C++:

~~~cpp
GLuint modelLocation = glGetUniformLocation(shaderProgram, "model");
glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
~~~

## 9. GLM 변환

단위 행렬:

~~~cpp
glm::mat4 model(1.0f);
~~~

이동:

~~~cpp
model = glm::translate(model, glm::vec3(x, y, z));
~~~

회전:

~~~cpp
model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
~~~

크기:

~~~cpp
model = glm::scale(model, glm::vec3(sx, sy, sz));
~~~

## 10. 변환 순서

`Translate → Rotate`와 `Rotate → Translate`는 같은 결과가 아니다.

특히 자기 중심 회전과 원점을 중심으로 한 공전을 구분할 때 변환 순서가 중요하다.

## 11. 그리기

~~~cpp
glBindVertexArray(VAO);
glDrawArrays(GL_TRIANGLES, 0, 3);
~~~

`GL_TRIANGLES`에서는 3개의 정점마다 하나의 삼각형을 만든다.

## 12. 화면에 아무것도 안 보일 때 확인 순서

1. GLFW 초기화
2. Window 생성
3. Context 활성화
4. GLEW 초기화
5. Viewport
6. VAO
7. VBO
8. Buffer Data
9. Vertex Attribute
10. Shader Compile
11. Program Link
12. `glUseProgram`
13. Uniform
14. 좌표
15. `glDrawArrays`
16. `glfwSwapBuffers`

한 번에 여러 부분을 수정하지 말고 이 순서대로 하나씩 확인한다.

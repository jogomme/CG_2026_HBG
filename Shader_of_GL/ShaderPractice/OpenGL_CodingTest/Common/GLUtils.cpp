#include "GLUtils.h"
namespace common {
/*
 * [기능]
 * VBO의 위치 xyz와 색 rgb를 셰이더의 입력 0번과 1번에 연결한다.
 *
 * [사용 상황]
 * 정점을 x,y,z,r,g,b 순서로 저장하는 실습에서 VAO/VBO를 준비한 직후 사용한다.
 *
 * [매개변수]
 * 인자 없음. 현재 바인딩된 VAO와 GL_ARRAY_BUFFER의 VBO에 설정을 기록한다.
 * 정점 하나는 float 6개: 위치 3개 + 색 3개. stride는 6*sizeof(float)바이트, 색 시작점은 3*sizeof(float)바이트.
 *
 * [반환값]
 * 반환값 없음(void). 현재 VAO의 attribute 0·1을 설정하고 활성화한다.
 *
 * [사용 예시]
 * float vertex[] = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
 * GLuint vao = 0;
 * GLuint vbo = 0;
 * glGenVertexArrays(1, &vao);
 * glBindVertexArray(vao);
 * glGenBuffers(1, &vbo);
 * glBindBuffer(GL_ARRAY_BUFFER, vbo);
 * glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
 * common::configurePositionColorAttributes();
 * // 이후 그릴 때 glBindVertexArray(vao)를 사용합니다.
 * glDeleteBuffers(1, &vbo);
 * glDeleteVertexArrays(1, &vao);
 *
 * [주의사항]
 * 먼저 대상 VAO와 VBO를 바인딩해야 한다. 잘못된 VAO에 설정하면 나중에 그려지지 않는다.
 * 법선·UV가 추가된 정점에는 그대로 쓰면 안 된다. attribute 번호, stride와 offset을 실제 배열 형식에 맞춰 바꿔야 한다.
 * Common.props를 통해 GLUtils.cpp도 컴파일해야 한다. 헤더 include만으로 구현이 링크되지는 않는다.
 *
 * [관련 실습]
 * 16번 직접 사용, 17~20번 MakeMesh 내부 사용. 21번 템플릿도 사용.
 *
 * [함께 사용할 헤더·함수]
 * Common/GLUtils.h
 * glBindVertexArray, glBindBuffer, glBufferData; practice::MakeMesh가 내부에서 호출
 *
 */
void configurePositionColorAttributes() {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}
}


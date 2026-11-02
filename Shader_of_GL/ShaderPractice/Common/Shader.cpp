#include "Shader.h"
namespace common {
/*
 * [기능]
 * GLSL 두 개로 프로그램을 만들고 Shader 객체가 그 번호를 관리한다.
 *
 * [사용 상황]
 * 직접 glDeleteProgram을 빠뜨리지 않도록 프로그램의 생성·정리를 한 객체에 맡길 때 사용한다.
 *
 * [매개변수]
 * vertex: vertex 셰이더 전체 GLSL 문자열.
 * fragment: fragment 셰이더 전체 GLSL 문자열. 둘 다 const char*, 파일 경로가 아니다.
 *
 * [반환값]
 * 생성자는 반환값이 없다. 만들어진 객체의 valid()로 성공 여부를 확인한다.
 *
 * [사용 예시]
 * common::Shader shader(
 *     common::shader_sources::positionColorVertex,
 *     common::shader_sources::positionColorFragment
 * );
 * if (!shader.valid())
 * {
 *     return -1;
 * }
 * shader.use();
 *
 * [주의사항]
 * CreateGLWindow 이후에 만든다. 생성 실패가 예외로 전달되는 것이 아니라 valid()==false가 된다.
 * 이 객체는 복사할 수 없다. 함수에 넘길 때 common::Shader&로 전달한다.
 * 소멸자가 glDeleteProgram을 호출하므로 창을 파괴하기 전에 객체의 수명이 끝나야 한다.
 *
 * [관련 실습]
 * 16~20번, 21번 템플릿.
 *
 * [함께 사용할 헤더·함수]
 * Common/Shader.h, Common/ShaderSources.h
 * CreateGLWindow, CreateProgram, valid, use
 *
 */
Shader::Shader(const char* vertex, const char* fragment) : program_(CreateProgram(vertex, fragment)) {}
/*
 * [기능]
 * Shader 객체의 수명이 끝날 때 소유한 프로그램을 자동으로 정리한다.
 *
 * [사용 상황]
 * 함수를 끝내거나 중괄호 블록을 벗어날 때 GPU 프로그램 번호를 돌려줄 때 사용된다.
 *
 * [매개변수]
 * 인자 없음. 객체가 기억한 program_을 reset()으로 정리한다.
 *
 * [반환값]
 * 반환값 없음. 소멸자는 반환형을 쓰지 않는다.
 *
 * [사용 예시]
 * {
 *     common::Shader shader(
 *         common::shader_sources::positionColorVertex,
 *         common::shader_sources::positionColorFragment
 *     );
 *     // 이 블록 안에서 shader를 사용합니다.
 * } // 여기서 소멸자가 자동 호출됩니다.
 * // glfwDestroyWindow와 glfwTerminate는 이 블록 뒤에서 호출합니다.
 *
 * [주의사항]
 * shader.~Shader()를 직접 호출하지 않는다. 같은 객체를 다시 소멸시키는 문제가 생긴다.
 * 소유한 프로그램의 OpenGL 컨텍스트가 현재 컨텍스트로 살아 있어야 한다. 전역 Shader 객체로 만들지 않는다.
 * reset()을 먼저 호출했어도 program_이 0이므로 중복 삭제를 막는다.
 *
 * [관련 실습]
 * 16~20번, 21번 템플릿.
 *
 * [함께 사용할 헤더·함수]
 * Common/Shader.h, Common/ShaderSources.h
 * reset, glfwDestroyWindow, glfwTerminate
 *
 */
Shader::~Shader() { reset(); }
/*
 * [기능]
 * 이 Shader의 프로그램을 이후 OpenGL 명령이 사용할 프로그램으로 선택한다.
 *
 * [사용 상황]
 * 그리기 직전 또는 다른 프로그램을 쓰다가 이 프로그램으로 돌아올 때 사용한다.
 *
 * [매개변수]
 * 인자 없음. 객체가 기억한 program_ 번호를 glUseProgram에 전달한다.
 *
 * [반환값]
 * 반환값 없음(void). OpenGL의 현재 프로그램 상태가 바뀐다.
 *
 * [사용 예시]
 * shader.use();
 * GLint modelLocation = shader.uniformLocation("model");
 * shader.setMatrix(modelLocation, glm::mat4(1.0f));
 *
 * [주의사항]
 * shader.valid()가 true인 객체로 호출한다.
 * setMatrix를 부르기 전에 이 프로그램을 use해야 한다. 다른 프로그램에서 찾은 uniform 위치는 섞어 쓰지 않는다.
 * 여러 프로그램을 번갈아 쓰면 필요한 지점마다 다시 호출한다.
 *
 * [관련 실습]
 * 16~20번, 21번 템플릿.
 *
 * [함께 사용할 헤더·함수]
 * Common/Shader.h, Common/ShaderSources.h
 * valid, uniformLocation, setMatrix
 *
 */
void Shader::use() const { glUseProgram(program_); }
/*
 * [기능]
 * 셰이더 안에서 uniform 값을 넣을 칸의 위치 번호를 찾는다.
 *
 * [사용 상황]
 * model/view/projection 행렬이나 objectColor 같은 전역 셰이더 값을 보낼 때 사용한다.
 *
 * [매개변수]
 * name: GLSL uniform 이름(const char*, 대소문자 구분, 단위 없음). 예: "model".
 *
 * [반환값]
 * GLint 위치 번호. 이름이 없거나 최적화로 사용되지 않으면 -1. 0도 유효한 위치이므로 실패로 보지 않는다.
 *
 * [사용 예시]
 * GLint modelLocation = shader.uniformLocation("model");
 * if (modelLocation == -1)
 * {
 *     std::cerr << "model 이름과 셰이더 사용 여부를 확인하세요." << std::endl;
 * }
 * shader.use();
 * shader.setMatrix(modelLocation, glm::mat4(1.0f));
 *
 * [주의사항]
 * 성공적으로 링크된 프로그램의 이름을 조회한다. 조회 자체에는 use()가 필수는 아니지만 값을 보낼 때는 필요하다.
 * 위치 번호는 프로그램마다 다르다. 다시 링크하거나 새 프로그램을 만들면 위치를 다시 조회한다.
 * setMatrix에 -1을 넘기면 OpenGL은 조용히 무시한다. 화면이 안 나오면 위치부터 확인한다.
 *
 * [관련 실습]
 * 16~20번, 21번 템플릿.
 *
 * [함께 사용할 헤더·함수]
 * Common/Shader.h, Common/ShaderSources.h
 * use, setMatrix, glUniform3fv
 *
 */
GLint Shader::uniformLocation(const char* name) const { return glGetUniformLocation(program_, name); }
/*
 * [기능]
 * GLM 4×4 행렬 하나를 현재 사용 중인 셰이더의 uniform에 보낸다.
 *
 * [사용 상황]
 * 물체의 이동·자전·공전, 카메라 view, 투영 projection을 화면에 반영할 때 사용한다.
 *
 * [매개변수]
 * location: 이 프로그램에서 찾은 uniform 위치(GLint, 단위 없음).
 * value: 보낼 glm::mat4 참조. 위치는 모델 좌표 단위, 회전값은 이미 행렬로 만들어져 있다.
 *
 * [반환값]
 * 반환값 없음(void). glUniformMatrix4fv로 행렬 1개를 전송한다.
 *
 * [사용 예시]
 * shader.use();
 * glm::mat4 model(1.0f);
 * model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
 * shader.setMatrix(shader.uniformLocation("model"), model);
 *
 * [주의사항]
 * 반드시 이 shader.use()를 먼저 호출한다. 이 메서드는 자동으로 프로그램을 바인딩하지 않는다.
 * GLM의 기본 열 우선 저장에 맞춰 GL_FALSE로 보낸다. 임의로 transpose하거나 GL_TRUE로 바꾸지 않는다.
 * 기본 vertex 셰이더는 projection * view * model * 정점 순서이다. 실제 정점에는 오른쪽 model부터 적용된다.
 *
 * [관련 실습]
 * 16~20번, 21번 템플릿.
 *
 * [함께 사용할 헤더·함수]
 * Common/Shader.h, Common/ShaderSources.h, <GL/glm/gtc/matrix_transform.hpp>
 * use, uniformLocation, glm::translate/rotate/scale
 *
 */
void Shader::setMatrix(GLint location, const glm::mat4& value) const {
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}
/*
 * [기능]
 * 현재 소유한 프로그램을 삭제하고 저장 번호를 0으로 바꾼다.
 *
 * [사용 상황]
 * 컨텍스트를 닫기 전에 GPU 프로그램을 명시적으로 정리하거나 Shader를 빈 상태로 만들 때 사용한다.
 *
 * [매개변수]
 * 인자 없음. 객체가 기억한 프로그램을 정리한다.
 *
 * [반환값]
 * 반환값 없음(void). 이후 valid()는 false이며, 이 함수가 셰이더를 다시 만들지는 않는다.
 *
 * [사용 예시]
 * shader.reset();
 * bool nowEmpty = !shader.valid();
 * // 필요하면 glUseProgram(0)으로 현재 프로그램도 해제합니다.
 * glUseProgram(0);
 *
 * [주의사항]
 * 이 프로그램을 만든 OpenGL 컨텍스트가 살아 있고 현재 컨텍스트여야 한다.
 * 현재 사용 중인 프로그램은 glDeleteProgram 후 사용 해제 시점까지 삭제가 보류될 수 있다. reset은 상태 저장 번호만 즉시 0으로 만든다.
 * 여러 번 호출해도 0인 번호를 다시 삭제하지 않는다. 초기 화면 복원이나 객체 위치 초기화 함수가 아니다.
 *
 * [관련 실습]
 * 16~20번 Shader 소멸 경유. 명시적 호출은 자원 관리 응용.
 *
 * [함께 사용할 헤더·함수]
 * Common/Shader.h, Common/ShaderSources.h
 * valid, Shader 소멸자, glUseProgram(0)
 *
 */
void Shader::reset() {
    if (program_) { glDeleteProgram(program_); program_ = 0; }
}
}


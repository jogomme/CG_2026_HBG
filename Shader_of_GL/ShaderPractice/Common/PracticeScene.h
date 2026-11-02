#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "Shader.h"
#include "ShaderSources.h"
#include "GLUtils.h"
#include <GL/glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>
#include <vector>

// 실습 17~20의 정점 생성과 그리기만 공유한다. 변환과 입력은 각 cpp에 둔다.
namespace practice {
/*
 * [자료형] practice::Mesh: GPU의 도형 준비 결과를 기억하는 묶음입니다.
 * VAO: 정점 읽는 방법을 기억한 객체 번호. VBO: 실제 정점 데이터를 담은 버퍼 번호.
 * count: float 개수가 아니라 정점 개수. mode: GL_TRIANGLES/GL_LINES/GL_LINE_LOOP 등.
 * 함수가 아니므로 생성 호출은 practice::Mesh mesh;처럼 합니다. 처음 번호와 개수는 0입니다.
 * 복사해도 GPU 데이터가 복제되지 않습니다. 같은 번호를 두 번 삭제하지 않도록 소유자를 정합니다.
 * 소멸자가 GPU 자원을 자동 삭제하지 않습니다. 사용자 정의 Mesh는 직접 정리합니다.
 * 관련 실습: 17~20번. 함께 사용: MakeVertices → MakeMesh → DrawObj.
 */
struct Mesh
{
    GLuint VAO{}, VBO{};
    GLsizei count{};
    GLenum mode = GL_TRIANGLES;
};

inline Mesh cube, sphere, cone, circle, axis, plane;
inline GLint colorLocation{};

/*
 * [기능]
 * CPU의 vector 끝에 위치 xyz와 밝기 rgb로 이루어진 정점 하나를 추가한다.
 *
 * [사용 상황]
 * 정점 추가, 선분·삼각형·원·구·원뿔 데이터를 직접 만들 때 사용한다.
 *
 * [매개변수]
 * vertices: float 배열을 담은 std::vector<float>&. 끝에 float 6개를 추가하므로 내용이 변한다.
 * position: 도형 자체의 모델 좌표(x,y,z, 장면 거리 단위).
 * shade: r,g,b에 똑같이 넣을 밝기(float, 보통 0~1). 생략하면 1.0f. 각 RGB 색을 따로 받는 함수는 아니다.
 *
 * [반환값]
 * 반환값 없음(void). vertices.size()가 6 증가한다. GPU로 보내지는 않는다.
 *
 * [사용 예시]
 * std::vector<float> vertices;
 * practice::MakeVertices(vertices, glm::vec3(-0.5f, 0.0f, 0.0f), 1.0f);
 * practice::MakeVertices(vertices, glm::vec3( 0.5f, 0.0f, 0.0f), 1.0f);
 * // 이제 정점 2개 = float 12개입니다. GL_LINES로 선분 하나를 만들 수 있습니다.
 *
 * [주의사항]
 * 정점 개수와 float 개수를 혼동하지 않는다. 정점 개수는 size()/6이다.
 * vector를 수정해도 이미 GPU로 보낸 VBO는 자동 갱신되지 않는다. MakeMesh 또는 별도 버퍼 갱신이 필요하다.
 * 17~20번의 fragment 셰이더에서는 이 밝기와 DrawObj의 objectColor가 곱해진다. 충돌 검사나 인덱스 추가는 하지 않는다.
 *
 * [관련 실습]
 * 17번 선 도형 직접 사용, 17~20번 MakeObjects 내부 사용.
 *
 * [함께 사용할 헤더·함수]
 * Common/PracticeScene.h
 * MakeMesh, DrawObj
 *
 */
inline void MakeVertices(std::vector<float>& vertices, glm::vec3 position, float shade = 1.0f)
{
    vertices.insert(vertices.end(), { position.x, position.y, position.z, shade, shade, shade });
}

/*
 * [기능]
 * 위치·색 정점 배열을 GPU VBO에 보내고 VAO를 만들어 Mesh에 저장한다.
 *
 * [사용 상황]
 * 직접 만든 선·삼각형·궤도·도형을 GPU에서 그릴 준비를 할 때 사용한다.
 *
 * [매개변수]
 * mesh: 결과 VAO/VBO/count/mode를 저장할 practice::Mesh&. 사용하지 않은 빈 Mesh를 전달한다.
 * vertices: xyzrgb 반복 형식의 const std::vector<float>&. 길이는 6의 배수여야 한다.
 * mode: 그리는 방식(GLenum). 생략하면 GL_TRIANGLES. 선분은 GL_LINES, 닫힌 궤도는 GL_LINE_LOOP.
 *
 * [반환값]
 * 반환값 없음(void). mesh.VAO, mesh.VBO를 새로 만들고 count=size()/6, mode를 저장한다.
 *
 * [사용 예시]
 * std::vector<float> vertices;
 * practice::MakeVertices(vertices, glm::vec3(-0.5f, 0.0f, 0.0f));
 * practice::MakeVertices(vertices, glm::vec3( 0.5f, 0.0f, 0.0f));
 * practice::Mesh line;
 * practice::MakeMesh(line, vertices, GL_LINES);
 * // 그리기가 끝나고 컨텍스트가 살아 있을 때 정리합니다.
 * glDeleteBuffers(1, &line.VBO);
 * glDeleteVertexArrays(1, &line.VAO);
 *
 * [주의사항]
 * 같은 mesh에 반복 호출하면 이전 VAO/VBO를 자동 삭제하지 않아 자원이 누출된다. 이전 번호를 먼저 정리한다.
 * EBO 없이 glDrawArrays로 그리는 형식이다. GL_LINES는 정점 2개씩, GL_TRIANGLES는 3개씩 묶인다.
 * 함수가 끝나도 대상 VAO와 VBO가 바인딩된 상태이다. 사용자 정의 Mesh는 RunScene이 자동 정리하지 않는다.
 *
 * [관련 실습]
 * 17번 선 도형, 17~20번 공통 도형 준비.
 *
 * [함께 사용할 헤더·함수]
 * Common/PracticeScene.h
 * MakeVertices, configurePositionColorAttributes, DrawObj
 *
 */
inline void MakeMesh(Mesh& mesh, const std::vector<float>& vertices, GLenum mode = GL_TRIANGLES)
{
    mesh.mode = mode;
    mesh.count = static_cast<GLsizei>(vertices.size() / 6);
    glGenVertexArrays(1, &mesh.VAO);
    glBindVertexArray(mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    common::configurePositionColorAttributes();
}

/*
 * [기능]
 * 공통 육면체·구·원뿔·궤도 원·좌표축·바닥 Mesh 6개를 준비한다.
 *
 * [사용 상황]
 * 17~20번에서 미리 만들어 둔 도형을 바로 그리거나, 직접 반복문을 구성할 때 초기 준비에 사용한다.
 *
 * [매개변수]
 * 인자 없음. practice::cube, sphere, cone, circle, axis, plane 전역 Mesh에 결과를 저장한다.
 *
 * [반환값]
 * 반환값 없음(void). GPU VAO/VBO가 도형마다 생성된다.
 *
 * [사용 예시]
 * practice::MakeObjects();
 * // 이 뒤부터 practice::cube, practice::sphere 등을 DrawObj에 전달할 수 있습니다.
 * // RunScene을 쓴다면 내부에서 이미 호출하므로 다시 호출하지 않습니다.
 *
 * [주의사항]
 * cube는 중심 원점, 한 변 1; sphere는 반지름 1; cone은 y축 높이 1, 바닥 반지름 0.5이다.
 * plane은 y=0인 xz 평면의 한 변 1 정사각형; circle은 y=0인 xz 평면의 반지름 1 원이다. axis는 각 축 -1~1이다.
 * 한 번만 호출한다. 이전 Mesh를 정리하지 않고 다시 호출하면 누출된다.
 * 17번은 공통 도형 준비 후 자체 MakeWireObjects에서 일부를 선 도형으로 교체한다. RunScene 내부 호출과 구분한다.
 *
 * [관련 실습]
 * 17~20번. 17번은 초기 공통 도형을 선 도형으로 교체.
 *
 * [함께 사용할 헤더·함수]
 * Common/PracticeScene.h
 * MakeVertices, MakeMesh, RunScene, DrawObj
 *
 */
inline void MakeObjects()
{
    const glm::vec3 corners[8] = {
        {-0.5f,-0.5f,0.5f}, {0.5f,-0.5f,0.5f}, {-0.5f,0.5f,0.5f}, {0.5f,0.5f,0.5f},
        {-0.5f,-0.5f,-0.5f}, {0.5f,-0.5f,-0.5f}, {-0.5f,0.5f,-0.5f}, {0.5f,0.5f,-0.5f}
    };
    const int faces[6][4] = { {0,1,2,3}, {5,4,7,6}, {1,5,3,7}, {4,0,6,2}, {2,3,6,7}, {4,5,0,1} };
    const int order[6] = {0,1,2,2,1,3};
    std::vector<float> vertices;
    for (int face = 0; face < 6; ++face)
        for (int i : order) MakeVertices(vertices, corners[faces[face][i]], 1.0f - face * 0.08f);
    MakeMesh(cube, vertices);

    vertices.clear();
    for (int y = 0; y < 20; ++y)
    {
        for (int x = 0; x < 32; ++x)
        {
            glm::vec3 points[4];
            for (int i = 0; i < 4; ++i)
            {
                float latitude = glm::radians(-90.0f + (y + i / 2) * 9.0f);
                float longitude = glm::radians((x + i % 2) * 11.25f);
                points[i] = {cos(latitude) * cos(longitude), sin(latitude), cos(latitude) * sin(longitude)};
            }
            for (int i : order) MakeVertices(vertices, points[i], 0.65f + 0.35f * (points[i].y + 1.0f) * 0.5f);
        }
    }
    MakeMesh(sphere, vertices);

    vertices.clear();
    for (int i = 0; i < 32; ++i)
    {
        float a = glm::radians(i * 11.25f), b = glm::radians((i + 1) * 11.25f);
        glm::vec3 first(0.5f * cos(a), -0.5f, 0.5f * sin(a));
        glm::vec3 second(0.5f * cos(b), -0.5f, 0.5f * sin(b));
        float shade = 0.65f + 0.3f * (cos(a) + 1.0f) * 0.5f;
        MakeVertices(vertices, first, shade); MakeVertices(vertices, second, shade);
        MakeVertices(vertices, {0,0.5f,0}, shade);
        MakeVertices(vertices, first, 0.6f); MakeVertices(vertices, {0,-0.5f,0}, 0.6f);
        MakeVertices(vertices, second, 0.6f);
    }
    MakeMesh(cone, vertices);

    vertices.clear();
    for (int i = 0; i < 128; ++i)
    {
        float a = glm::radians(i * 360.0f / 128);
        MakeVertices(vertices, {cos(a), 0, sin(a)});
    }
    MakeMesh(circle, vertices, GL_LINE_LOOP);
    MakeMesh(axis, { -1,0,0, 1,0,0, 1,0,0, 1,0,0,
                     0,-1,0, 0,1,0, 0,1,0, 0,1,0,
                     0,0,-1, 0,0,1, 0,0,1, 0,0,1 }, GL_LINES);
    vertices.clear();
    for (int i : order) MakeVertices(vertices, glm::vec3(corners[faces[4][i]].x, 0, corners[faces[4][i]].z));
    MakeMesh(plane, vertices);
}

/*
 * [기능]
 * 부모 행렬 뒤에 위치 이동과 크기 변경을 붙인 모델 행렬을 만든다.
 *
 * [사용 상황]
 * 물체 배치·크기 변경, 부모-자식 부품, 탱크 몸체와 포신, 벨트 위 상자·숫자, 공전 배치에 사용한다.
 *
 * [매개변수]
 * position: 부모 좌표계에서의 위치(glm::vec3, 장면 거리 단위). 부모 생략 시 월드 위치.
 * size: x/y/z 크기 배율(glm::vec3, 단위 없음). (1,1,1)이 기본 크기. 도형 원래 크기에 곱해진다.
 * parent: 부모의 glm::mat4. 생략하면 단위행렬 glm::mat4(1.0f). 부모가 없으면 명시적으로 전달하지 않아도 된다.
 *
 * [반환값]
 * parent * T(position) * S(size) 형태의 glm::mat4. 실제 정점에는 S → T → parent 순서로 적용된다.
 *
 * [사용 예시]
 * glm::mat4 parent(1.0f);
 * parent = glm::rotate(parent, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
 * glm::mat4 model = practice::MakeModelMatrix(
 *     glm::vec3(2.0f, 0.0f, 0.0f),
 *     glm::vec3(0.5f),
 *     parent
 * ); // 원점에서 2만큼 떨어진 자리를 부모가 45도 돌립니다. 공전 배치입니다.
 *
 * [주의사항]
 * 함수 자체에는 angle 인자가 없으며 제자리 회전을 자동으로 만들지 않는다. 자전은 T * R * S로 따로 합성한다.
 * parent에 회전을 넣으면 위치도 돌아 공전이 된다. 부모에 크기 변경이 있으면 자식의 위치와 크기도 영향을 받는다.
 * size.x=2는 항상 가로 길이 2가 아니다. sphere 원래 지름은 2이므로 size=0.5일 때 지름 1이다.
 * 원점 확대와 제자리 확대는 행렬 순서가 다르다. 설명서의 자전/공전/기준점 예제를 비교한다.
 *
 * [관련 실습]
 * 17번 변환 원리, 18번 행성·달, 19번 부모-자식 탱크, 20번 벨트·상자·숫자.
 *
 * [함께 사용할 헤더·함수]
 * Common/PracticeScene.h
 * glm::translate/rotate/scale, DrawObj
 *
 */
inline glm::mat4 MakeModelMatrix(glm::vec3 position, glm::vec3 size, glm::mat4 parent = glm::mat4(1.0f))
{
    return glm::scale(glm::translate(parent, position), size);
}

/*
 * [기능]
 * 준비된 Mesh 하나를 모델 행렬과 색으로 그린다.
 *
 * [사용 상황]
 * 행성·탱크 부품·상자·숫자·축·궤도 등 같은 도형을 다른 위치와 색으로 여러 번 그릴 때 사용한다.
 *
 * [매개변수]
 * mesh: MakeMesh/MakeObjects로 준비한 Mesh의 const 참조. count는 정점 개수, mode는 그리기 방식.
 * model: 물체의 glm::mat4 모델 행렬. model 좌표를 월드로 옮긴다.
 * color: objectColor로 보낼 RGB(glm::vec3, 보통 각 성분 0~1). 밝기 정점 색과 곱해진다.
 * modelLocation: 같은 shader의 "model" uniform 위치(GLint).
 * shader: 현재 use()한 common::Shader&. 복사하지 않는다.
 *
 * [반환값]
 * 반환값 없음(void). 모델/색 uniform을 전송하고 VAO를 바인딩하여 glDrawArrays로 그린다.
 *
 * [사용 예시]
 * shader.use();
 * GLint modelLocation = shader.uniformLocation("model");
 * glm::mat4 model = practice::MakeModelMatrix(glm::vec3(0.0f), glm::vec3(0.5f));
 * practice::DrawObj(practice::cube, model, glm::vec3(1.0f, 0.2f, 0.2f), modelLocation, shader);
 *
 * [주의사항]
 * view/projection은 여기서 보내지 않는다. 한 화면을 그리기 전에 호출자가 설정한다.
 * practice::colorLocation에 같은 프로그램의 "objectColor" 위치가 설정되어 있어야 한다. RunScene은 준비하지만 기본 positionColorFragment에는 이 uniform이 없다.
 * 기본 fragment 셰이더를 직접 쓸 때는 색을 바꾸려면 별도의 대응 셰이더가 필요하다. 설명서의 최소 실행 예제는 RunScene을 사용해 이 조건을 맞춘다.
 * 충돌을 검사하거나 물체 상태를 갱신하지 않는다. 은면 제거·wireframe·viewport도 호출자가 정한다.
 *
 * [관련 실습]
 * 17~20번.
 *
 * [함께 사용할 헤더·함수]
 * Common/PracticeScene.h
 * MakeObjects/MakeMesh, MakeModelMatrix, Shader::use/setMatrix, RunScene
 *
 */
inline void DrawObj(const Mesh& mesh, const glm::mat4& model, glm::vec3 color,
                    GLint modelLocation, common::Shader& shader)
{
    shader.setMatrix(modelLocation, model);
    glUniform3fv(colorLocation, 1, &color[0]);
    glBindVertexArray(mesh.VAO);
    glDrawArrays(mesh.mode, 0, mesh.count);
}

/*
 * [기능]
 * 원근 투영 또는 직각 투영의 projection 행렬을 만든다.
 *
 * [사용 상황]
 * 18번 p/P, 20번 p/P처럼 직각/원근 화면을 전환하거나 창 크기 변화에 대응할 때 사용한다.
 *
 * [매개변수]
 * width,height: 현재 framebuffer 너비·높이(픽셀, 둘 다 양수).
 * perspective: true면 원근 투영, false면 직각 투영.
 * extent: 직각 투영에서 화면 중심부터 위/아래 끝까지의 거리(장면 거리 단위, 양수). 원근 투영에서는 이 인자를 사용하지 않는다.
 *
 * [반환값]
 * glm::mat4 projection. 원근은 시야각 45도·near 0.1·far 100. 직각은 y범위 ±extent, x범위 ±extent*(width/height), near 0.1·far 100.
 *
 * [사용 예시]
 * glm::mat4 perspective = practice::MakeProjection(1200, 900, true, 4.8f);
 * glm::mat4 orthographic = practice::MakeProjection(1200, 900, false, 4.8f);
 * // 실제 반복문에서는 위 숫자 대신 양수인지 확인한 framebuffer 크기를 넣습니다.
 *
 * [주의사항]
 * height=0이면 나눗셈이 깨진다. RunScene은 최소화 시 건너뛰지만 직접 호출할 때는 검사한다.
 * 원근에서는 같은 크기의 물체가 카메라 깊이에 따라 다르게 보인다. 직각에서는 깊이가 달라도 크기가 같다.
 * 원근 화면의 확대를 extent로 조절할 수 없다. 카메라 거리 또는 별도 시야각 설정이 필요하다.
 * 물체 이동은 model, 카메라 시점은 view, 투영 방식은 projection이다. 서로 대신 쓰지 않는다.
 *
 * [관련 실습]
 * 17~20번, 특히 18·20번의 투영 전환.
 *
 * [함께 사용할 헤더·함수]
 * Common/PracticeScene.h
 * glfwGetFramebufferSize, Shader::setMatrix, glm::lookAt
 *
 */
inline glm::mat4 MakeProjection(int width, int height, bool perspective, float extent)
{
    float aspect = static_cast<float>(width) / height;
    if (perspective) return glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    return glm::ortho(-extent * aspect, extent * aspect, -extent, extent, 0.1f, 100.0f);
}

/*
 * [기능]
 * 창·셰이더·공통 도형을 준비하고 입력·갱신·그리기 반복과 종료 정리를 맡는다.
 *
 * [사용 상황]
 * 17~20번처럼 과제별 키 입력·애니메이션·그리기만 작성하여 실행할 때 사용한다.
 *
 * [매개변수]
 * title: 창 제목 문자열(const char*). 창은 1200×900 화면 좌표 크기로 생성된다.
 * keyCallback: GLFWkeyfun. void(GLFWwindow*,int key,int scancode,int action,int mods) 형식의 함수 주소.
 * UpdateScene: void(float) 함수 주소. 매 프레임 deltaTime(초)을 받는다.
 * DrawAll: void(common::Shader&,int width,int height) 함수 주소. width/height는 framebuffer 픽셀 수.
 *
 * [반환값]
 * 정상 종료 0, 창 또는 셰이더 초기화 실패 -1. main에서 return practice::RunScene(...);으로 사용한다.
 *
 * [사용 예시]
 * // 아래 함수들은 전역 범위에 정의합니다. 전체 실행 예제는 FUNCTION_GUIDE.md의 첫 예제입니다.
 * // void KeyCallback(GLFWwindow*, int, int, int, int);
 * // void UpdateScene(float);
 * // void DrawAll(common::Shader&, int, int);
 * return practice::RunScene("그래픽스 실습", KeyCallback, UpdateScene, DrawAll);
 *
 * [주의사항]
 * 호출한 자리에서 종료까지 반복하므로 다른 게임 반복문 안에서 호출하지 않는다. callback은 null이 아닌 유효한 함수를 전달한다.
 * UpdateScene은 초 단위 시간을 받으며 함수 내부에서 최대 0.05초로 제한한다. 긴 정지나 매우 낮은 FPS에서는 실제 벽시계 시간보다 애니메이션이 느려질 수 있다.
 * 실행 순서는 이벤트 처리 → 시간·화면 크기 확인 → UpdateScene → 화면 지우기 → DrawAll → 화면 교환이다.
 * model/view/projection 행렬은 DrawAll에서 설정한다. 셰이더와 objectColor, 6개 기본 Mesh, depth test는 내부에서 준비한다.
 * 자동 정리 대상은 6개 기본 Mesh와 내부 Shader이다. 추가 Mesh의 생성·삭제는 직접 처리한다.
 * 17번의 선 도형 교체는 UpdateScene 첫 호출에서 수행한다. GLFW 컨텍스트가 생기기 전 전역 초기화로 GPU 함수를 호출하지 않는다.
 *
 * [관련 실습]
 * 17~20번.
 *
 * [함께 사용할 헤더·함수]
 * Common/PracticeScene.h
 * KeyCallback, UpdateScene, DrawAll, MakeObjects, DrawObj
 *
 */
inline int RunScene(const char* title, GLFWkeyfun keyCallback, void (*UpdateScene)(float),
                    void (*DrawAll)(common::Shader&, int, int))
{
    GLFWwindow* window = CreateGLWindow(1200, 900, title);
    if (!window) return -1;
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);
    int result = 0;
    {
        /*
         * [기능] 아래 GLSL fragment main은 ourColor와 objectColor를 곱해 최종 색을 냅니다.
         * [사용 상황] 17~20번처럼 같은 Mesh를 여러 색으로 그릴 때 RunScene이 사용합니다.
         * [매개변수] C++ 함수 인자 없음. GPU 입력 ourColor=보간된 RGB,
         * uniform objectColor=DrawObj의 RGB(각 성분 보통 0~1)를 사용합니다.
         * [반환값] GLSL main은 void. FragColor에 불투명 RGBA를 기록합니다.
         * [사용 예시] return practice::RunScene("실습", KeyCallback, UpdateScene, DrawAll);
         * [주의사항] CPU에서 main을 호출하지 않습니다. RunScene이 컴파일·연결합니다.
         * 기본 positionColorFragment와 달리 objectColor uniform이 필요합니다.
         * [관련 실습] 17~20번. [함께 사용할 헤더·함수] PracticeScene.h, DrawObj, RunScene.
         */
        const char* fragment = R"glsl(
            #version 330 core
            in vec3 ourColor;
            uniform vec3 objectColor;
            out vec4 FragColor;
            void main() { FragColor = vec4(ourColor * objectColor, 1.0); }
        )glsl";
        common::Shader shader(common::shader_sources::positionColorVertex, fragment);
        if (!shader.valid()) result = -1;
        else
        {
            shader.use();
            colorLocation = shader.uniformLocation("objectColor");
            MakeObjects();
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.95f, 0.96f, 0.98f, 1.0f);
            double previousTime = glfwGetTime();
            while (!glfwWindowShouldClose(window))
            {
                glfwPollEvents();
                if (glfwWindowShouldClose(window)) break;
                double currentTime = glfwGetTime();
                float deltaTime = static_cast<float>(std::min(currentTime - previousTime, 0.05));
                previousTime = currentTime;
                int width, height;
                glfwGetFramebufferSize(window, &width, &height);
                if (width <= 0 || height <= 0) continue;
                UpdateScene(deltaTime);
                glViewport(0, 0, width, height);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                DrawAll(shader, width, height);
                glfwSwapBuffers(window);
            }
            for (Mesh* mesh : {&cube, &sphere, &cone, &circle, &axis, &plane})
            {
                glDeleteBuffers(1, &mesh->VBO);
                glDeleteVertexArrays(1, &mesh->VAO);
                *mesh = Mesh{};
            }
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return result;
}
}


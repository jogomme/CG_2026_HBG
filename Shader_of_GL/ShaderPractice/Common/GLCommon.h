//------------------------------------------------------------------------------------------------------
// OpenGL 3.3 실습 공통 모듈 ( header only )
//
// 과제마다 반복되는 코드를 모아 둡니다.
//   CreateGLWindow()         창 생성 + GLEW 초기화 + viewport + 콘솔 UTF-8
//   CreateProgram( vs, fs )  셰이더 컴파일 + 링크 + 오류 로그 출력
//   CheckShader()            셰이더 컴파일 결과 확인
//   CheckProgram()           프로그램 링크 결과 확인
//
// 사용법 ( 과제 .cpp 상단 )
//   #include "GLCommon.h"
//
// include 경로는 vcxproj 가 한 단계 위( ShaderPractice )로 잡아 줍니다.
//
// <windows.h> 를 GL 헤더보다 먼저 include 해야 APIENTRY 재정의 경고가 없습니다.
//------------------------------------------------------------------------------------------------------

#pragma once

#include <windows.h>

#include <GL/glew.h>
#include <GL/glfw3.h>

#include <iostream>

//------------------------------------------------------------------------------------------------------
// 창 생성 + OpenGL 3.3 Context + GLEW + viewport
//   실패하면 nullptr 을 돌려줍니다.
//------------------------------------------------------------------------------------------------------
/*
 * [기능]
 * GLFW 창과 OpenGL 3.3 Core 컨텍스트를 만들고 GLEW를 준비한다.
 *
 * [사용 상황]
 * 직접 main과 반복문을 작성하는 실습의 출발점. 창 생성, 그래픽 카드 함수 준비, 첫 viewport 설정에 사용한다.
 *
 * [매개변수]
 * width: 창 내용 영역의 가로 크기(화면 좌표 단위, 일반 배율에서 픽셀과 같음).
 * height: 창 내용 영역의 세로 크기(같은 단위). 둘 다 양수.
 * title: 창 제목인 널 종료 문자열. 한글은 프로젝트 /utf-8 설정을 사용한다.
 *
 * [반환값]
 * 성공하면 GLFWwindow*. 실패하면 nullptr. 실패 원인은 한국어 콘솔 메시지로 출력한다.
 *
 * [사용 예시]
 * GLFWwindow* window = CreateGLWindow(1200, 900, "그래픽스 실습");
 * if (!window)
 * {
 *     return -1;
 * }
 * // 여기부터 OpenGL 함수와 common::Shader를 사용할 수 있습니다.
 *
 * [주의사항]
 * 윈도우 크기와 framebuffer 픽셀 수는 고해상도 화면에서 다를 수 있다. 그릴 때는 glfwGetFramebufferSize로 크기를 다시 받는다.
 * 이 함수는 반복 그리기를 시작하지 않는다. glfwPollEvents, glClear, 그리기, glfwSwapBuffers는 직접 작성하거나 RunScene에 맡긴다.
 * 종료할 때 GPU 자원을 먼저 지우고 glfwDestroyWindow(window), glfwTerminate() 순서로 정리한다.
 * glEnable(GL_DEPTH_TEST)는 여기서 켜지지 않는다. 3D의 은면 제거는 호출자가 켠다. Windows 전용 코드이다.
 *
 * [관련 실습]
 * 14~16번 직접 사용, 17~20번 RunScene 내부 사용. 21번 템플릿도 사용.
 *
 * [함께 사용할 헤더·함수]
 * Common/GLCommon.h
 * glfwGetFramebufferSize, common::Shader, glfwDestroyWindow, glfwTerminate
 *
 */
inline GLFWwindow* CreateGLWindow(int width, int height, const char* title)
{
    // 콘솔 출력 코드페이지를 UTF-8 로 설정
    //   /utf-8 로 문자열은 UTF-8 로 나가는데 콘솔 기본 코드페이지는 949 라서 그대로 두면 깨집니다.
    SetConsoleOutputCP(CP_UTF8);

    if (!glfwInit())
    {
        std::cout << "GLFW 초기화 실패\n";
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    GLFWwindow* window = glfwCreateWindow(
        width,
        height,
        title,
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cout << "Window 생성 실패\n";
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW 초기화 실패\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    glViewport(0, 0, width, height);

    return window;
}

//------------------------------------------------------------------------------------------------------
// 셰이더 컴파일 결과 확인
//------------------------------------------------------------------------------------------------------
/*
 * [기능]
 * 셰이더 하나의 컴파일 성공 여부를 검사하고 실패 로그를 출력한다.
 *
 * [사용 상황]
 * 직접 GLSL을 컴파일하거나 셰이더 문법 오류를 찾을 때 사용한다. CreateProgram 내부에서도 호출한다.
 *
 * [매개변수]
 * shader: glCreateShader로 만든 셰이더 객체 번호(GLuint, 단위 없음). 먼저 glCompileShader를 호출해야 한다.
 *
 * [반환값]
 * 컴파일 성공은 true, 실패는 false. 실패 로그를 최대 1024바이트 버퍼로 받아 콘솔에 출력한다.
 *
 * [사용 예시]
 * GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
 * glShaderSource(vertexShader, 1, &common::shader_sources::positionColorVertex, nullptr);
 * glCompileShader(vertexShader);
 * bool compiled = CheckShader(vertexShader);
 * glDeleteShader(vertexShader); // 확인용 셰이더이므로 여기서 정리합니다.
 *
 * [주의사항]
 * 프로그램 번호가 아니라 셰이더 번호를 넣는다. 검사만 하고 객체를 삭제하지 않는다.
 * 컴파일 성공이어도 vertex/fragment 사이의 연결이 실패할 수 있으므로 링크는 CheckProgram으로 별도 검사한다.
 *
 * [관련 실습]
 * 14~16번 CreateProgram 경유, 17~20번 Shader 생성 경유. 직접 검사 방식은 11·13번에도 등장.
 *
 * [함께 사용할 헤더·함수]
 * Common/GLCommon.h, Common/ShaderSources.h
 * glCreateShader, glShaderSource, glCompileShader, CheckProgram
 *
 */
inline bool CheckShader(GLuint shader)
{
    int success = 0;

    glGetShaderiv(
        shader,
        GL_COMPILE_STATUS,
        &success
    );

    if (success == GL_FALSE)
    {
        char log[1024];

        glGetShaderInfoLog(
            shader,
            sizeof(log),
            nullptr,
            log
        );

        std::cout << "셰이더 컴파일 실패\n";
        std::cout << log << "\n";

        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------------------
// 프로그램 링크 결과 확인
//------------------------------------------------------------------------------------------------------
/*
 * [기능]
 * 셰이더 프로그램의 링크 성공 여부를 검사하고 실패 로그를 출력한다.
 *
 * [사용 상황]
 * vertex/fragment 셰이더를 묶은 프로그램이 사용 가능한지 확인할 때 사용한다.
 *
 * [매개변수]
 * program: glCreateProgram으로 만든 프로그램 번호(GLuint, 단위 없음). 먼저 glLinkProgram을 호출한다.
 *
 * [반환값]
 * 링크 성공은 true, 실패는 false. 실패 로그를 최대 1024바이트 버퍼로 받아 콘솔에 출력한다.
 *
 * [사용 예시]
 * GLuint program = CreateProgram(
 *     common::shader_sources::positionColorVertex,
 *     common::shader_sources::positionColorFragment
 * );
 * if (program != 0)
 * {
 *     bool linked = CheckProgram(program); // CreateProgram 내부에서도 이미 확인합니다.
 *     glDeleteProgram(program);
 * }
 *
 * [주의사항]
 * 셰이더 번호와 프로그램 번호는 서로 다른 종류이다. CheckShader와 혼동하지 않는다.
 * 이 함수는 glUseProgram을 호출하거나 프로그램을 삭제하지 않는다. 링크 성공 후에만 사용한다.
 *
 * [관련 실습]
 * 14~16번 CreateProgram 경유, 17~20번 Shader 생성 경유. 직접 검사 방식은 11·13번에도 등장.
 *
 * [함께 사용할 헤더·함수]
 * Common/GLCommon.h, Common/ShaderSources.h
 * CreateProgram, glLinkProgram, glUseProgram
 *
 */
inline bool CheckProgram(GLuint program)
{
    int success = 0;

    glGetProgramiv(
        program,
        GL_LINK_STATUS,
        &success
    );

    if (success == GL_FALSE)
    {
        char log[1024];

        glGetProgramInfoLog(
            program,
            sizeof(log),
            nullptr,
            log
        );

        std::cout << "Program 링크 실패\n";
        std::cout << log << "\n";

        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------------------
// 정점 / 프래그먼트 셰이더 소스로 프로그램 생성 ( 컴파일 + 링크 )
//   실패하면 0 을 돌려줍니다.
//------------------------------------------------------------------------------------------------------
/*
 * [기능]
 * vertex/fragment GLSL을 컴파일하고 링크한 프로그램 번호를 만든다.
 *
 * [사용 상황]
 * 클래스 없이 GLuint로 셰이더 프로그램을 관리하거나, Shader 생성자가 내부적으로 프로그램을 만들 때 사용한다.
 *
 * [매개변수]
 * vertexSource: vertex 셰이더 GLSL 전체 문자열(const char*, 널 종료).
 * fragmentSource: fragment 셰이더 GLSL 전체 문자열(const char*, 널 종료). 파일 경로가 아니다.
 *
 * [반환값]
 * 성공하면 0이 아닌 프로그램 번호(GLuint), 컴파일/링크 실패면 0. 중간 셰이더 객체는 내부에서 삭제한다.
 *
 * [사용 예시]
 * GLuint program = CreateProgram(
 *     common::shader_sources::positionColorVertex,
 *     common::shader_sources::positionColorFragment
 * );
 * if (program == 0)
 * {
 *     return -1;
 * }
 * glUseProgram(program);
 * // 모든 그리기를 끝낸 뒤, 컨텍스트가 살아 있을 때 호출합니다.
 * glDeleteProgram(program);
 *
 * [주의사항]
 * 반환값 0을 검사한다. 성공한 프로그램은 호출자가 glDeleteProgram으로 정리해야 한다.
 * common::Shader가 소유한 프로그램을 직접 삭제하면 안 된다. 수동 관리와 Shader 관리 중 한 방식을 선택한다.
 * 기본 vertex 셰이더를 쓰면 model/view/projection 세 uniform 행렬을 설정해야 한다.
 * CreateProgram 자체는 프로그램을 현재 사용 상태로 바꾸지 않는다. glUseProgram 또는 Shader::use가 필요하다.
 *
 * [관련 실습]
 * 14·15번 직접 사용, 16~20번 Shader 생성자 내부에서 사용.
 *
 * [함께 사용할 헤더·함수]
 * Common/GLCommon.h, Common/ShaderSources.h
 * CheckShader, CheckProgram, glUseProgram, glDeleteProgram
 *
 */
inline GLuint CreateProgram(const char* vertexSource, const char* fragmentSource)
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        vertexShader,
        1,
        &vertexSource,
        nullptr
    );

    glCompileShader(vertexShader);

    if (!CheckShader(vertexShader))
    {
        glDeleteShader(vertexShader);
        return 0;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        fragmentShader,
        1,
        &fragmentSource,
        nullptr
    );

    glCompileShader(fragmentShader);

    if (!CheckShader(fragmentShader))
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    // 링크가 끝나면 셰이더 오브젝트는 더 이상 필요 없습니다.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!CheckProgram(program))
    {
        glDeleteProgram(program);
        return 0;
    }

    return program;
}


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

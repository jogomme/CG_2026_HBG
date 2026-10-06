# Practice 학습 정리

현재 컴퓨터 그래픽스 실습에서 구현에 직접 사용할 수 있도록 문제 문서와 공통 개념 문서를 분리한다.

## 실습 문서

- Practice 12 — 마우스 이동 Callback과 좌표 변환
- Practice 13 — 마우스 휠 / 문자 입력 Callback
- Practice 14 — Framebuffer Size / Viewport
- Practice 15 — 시간 / Delta Time 애니메이션

## 공통 학습 문서

- Coordinate_System.md — 윈도우 좌표, OpenGL NDC, 로컬 좌표, 좌표 변환
- GLFW_Input_Time.md — Polling, Callback, 마우스/키보드 이벤트, 시간, Delta Time
- Modern_OpenGL_Pipeline.md — VAO, VBO, Shader, Uniform, Draw Call, 디버깅 순서
- GLM_Transform.md — Model Matrix, Translate, Rotate, Scale, 변환 순서

## 문서 작성 기준

각 Practice 문서는 문제만 읽어도 구현 순서를 잡을 수 있도록 다음 내용을 포함한다.

1. 목표
2. 사용할 GLFW/OpenGL 함수
3. 함수 선언과 등록 방법
4. 핵심 구현 코드 구조
5. 좌표/수학 규칙
6. 주의할 점
7. 완료 체크리스트

공통 Notes 문서는 특정 과제에 종속되지 않고 이후 실습에서도 반복해서 사용할 개념을 정리한다.

## 중요한 주의

현재 저장소에서 공식적으로 확인되는 최신 Practice는 11.1이다. Practice 12~15에 대한 별도의 공식 문제 파일은 현재 저장소에서 확인되지 않아, 이번 12~15 문서는 현재 제공된 OpenGL 기초 강의자료와 최신 코드 흐름을 기반으로 후속 학습 실습 형태로 정리했다.

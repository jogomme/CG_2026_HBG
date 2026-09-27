# OpenGL 실습 7 — Modern OpenGL

## 목표
Legacy 방식이 아니라 Vertex Shader + Fragment Shader를 사용하는 Modern OpenGL 방식으로 구현한다.

## 도형
- 점
- 선
- 삼각형
- 사각형

## 그래픽 파이프라인
CPU 정점 데이터 → VBO → VAO 설정 → Vertex Shader → Rasterization → Fragment Shader → 화면

## Shader
### Vertex Shader
- 정점 위치를 입력받는다.
- 정점의 위치를 출력한다.

### Fragment Shader
- 프래그먼트의 최종 색상을 결정한다.

### Program
1. Vertex Shader 생성/컴파일.
2. Fragment Shader 생성/컴파일.
3. 두 Shader를 Program으로 Link.
4. 렌더링 전에 Program 사용.

## 버퍼
- VBO에 정점 데이터를 저장.
- VAO에 정점 속성 설정.
- Draw 명령으로 원하는 도형을 렌더링.

## 입력
- 마우스로 도형 선택.
- 선택된 도형 이동.
- 마우스 좌표를 OpenGL 좌표로 변환.
- 전체 도형 최대 50개.

## 상태
- 도형 종류
- 정점/색상 데이터
- 위치
- 선택 도형
- 드래그 상태
- 도형 개수
- VAO/VBO
- Shader Program

## 핵심
- glBegin/glEnd, glColor3f, glRectf 방식 대신 버퍼와 Shader를 사용한다.
- CPU에서 정점 데이터를 준비하고 GPU 버퍼로 전달한다.
- Shader가 정점과 색상을 처리한다.

## 구현 순서
1. OpenGL context 생성.
2. Shader 작성/컴파일/Link.
3. VAO/VBO 생성.
4. 정점 데이터 전달.
5. Vertex Attribute 연결.
6. Draw 명령으로 점/선/삼각형/사각형 출력.
7. 마우스 선택/이동 추가.
8. 최대 50개 제한.
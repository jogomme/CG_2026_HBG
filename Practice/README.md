# Practice 학습 정리

실습 명세서는 원본 과제 자료에서 요구사항을 추출하고, 구현에 필요한 상태/수학/테스트를 보충한다. 원문에 없는 항목은 임의로 필수 조건처럼 쓰지 않고 미확정 사항으로 표시한다.

## 실습 명세서

- [Practice 13](Problems/Practice13_Problem.md) — 키보드 명령으로 육면체/사각뿔의 면 선택 렌더링, 면별 색상, 깊이 테스트
- [Practice 14](Problems/Practice14_Problem.md) — 꼭짓점별 색상, 고정 좌표축, 와이어/솔리드, 이동 및 회전
- [Practice 15](Problems/Practice15_Problem.md) — 육면체의 개별 면/두 면/네 면 애니메이션
- [Practice 16](Problems/Practice16_Problem.md) — 사각뿔 옆면의 힌지 회전, 꼭짓점 기준 회전, 면 펼치기
- [Practice 17](Problems/Practice17_Problem.md) — 두 3D 객체의 자전/공전/스케일/이동 및 복합 애니메이션

## 공통 학습 문서

- Coordinate_System.md — 윈도우 좌표, OpenGL NDC, 로컬 좌표, 좌표 변환
- GLFW_Input_Time.md — Polling, Callback, 마우스/키보드 이벤트, 시간, Delta Time
- Modern_OpenGL_Pipeline.md — VAO, VBO, Shader, Uniform, Draw Call, 디버깅 순서
- GLM_Transform.md — Model Matrix, Translate, Rotate, Scale, 변환 순서

## 명세서 구성 기준

각 실습 문서는 다음을 포함한다.

1. 과제 텍스트 전문 추출(OCR)
2. 그래픽스 기본 환경 분석
3. 키보드 상호작용 및 내부 상태 설계
4. 단계별 코딩 구현 계획
5. 테스트 체크리스트
6. 오류 해결과 원문에서 정하지 않은 사항

## 자료 해석 시 주의

창 크기, 배경 RGB, 정확한 정점 좌표, 회전 속도, 투영 방식 등 원문에 적혀 있지 않은 항목은 확정 명세로 취급하지 않는다. 이런 부분은 구현자가 선택할 수 있는 사항으로 분리한다.

실습 13~15는 「지엘3 좌표계 변환 (실습 131415).pdf」, 실습 16~17은 「지엘3 좌표계 변환 (실습 1617).pdf」의 내용을 바탕으로 작성했다. 이미지/슬라이드에서 직접 확인되는 문구와 구현을 위한 합리적 추론은 문서 안에서 구분한다.

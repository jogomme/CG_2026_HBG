@echo off
REM ---------------------------------------------------------------------------------------
REM  새 실습 과제 만들기
REM
REM  실행 정책 때문에 .ps1 을 직접 실행할 수 없을 때 이 파일을 더블클릭 하세요.
REM  New-Homework.ps1 을 ExecutionPolicy Bypass 로 감싸서 실행합니다.
REM ---------------------------------------------------------------------------------------

setlocal

set "SCRIPT_DIR=%~dp0"
set "PROJECT_NAME=%~1"

if "%PROJECT_NAME%"=="" (
    set /p PROJECT_NAME=새 과제 이름을 입력하세요 ( 예: homework11 ):
)

if "%PROJECT_NAME%"=="" (
    echo.
    echo 이름이 입력되지 않아 취소했습니다.
    pause
    exit /b 1
)

powershell -NoProfile -ExecutionPolicy Bypass -File "%SCRIPT_DIR%New-Homework.ps1" -Name "%PROJECT_NAME%"

echo.
pause

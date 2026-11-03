@echo off
REM ============================================================
REM  build.bat - double-click wrapper for build.ps1
REM
REM  Usage:
REM    1) Double-click  -> compile the newest .cpp and run it
REM    2) Drag a .cpp file onto this .bat -> compile that file
REM    3) From cmd:  build.bat hanshuxvexi
REM ============================================================

chcp 65001 >NUL
cd /d "%~dp0"

if not exist "%~dp0build.ps1" (
    echo [ERROR] build.ps1 not found next to build.bat
    pause
    exit /b 1
)

set "TARGET=%~1"

if "%TARGET%"=="" (
    echo [INFO] No file given - compiling the newest .cpp
) else (
    echo [INFO] Target: %TARGET%
)

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build.ps1" "%TARGET%"

echo.
pause
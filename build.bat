@echo off
setlocal enabledelayedexpansion

echo.
echo ============================================================
echo    DDoS Detection Simulation - Build Script
echo    Visual Studio 2022 + CMake + ImGui
echo ============================================================
echo.

set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
set "VS_CMAKE=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"

if not exist "%VCVARS%" (
    echo [ERROR] Visual Studio 2022 not found!
    pause
    exit /b 1
)

echo [1/4] Setting up Visual Studio environment...
call "%VCVARS%" x64 >nul 2>&1

echo [2/4] Configuring CMake...
if not exist "build" mkdir build
"%VS_CMAKE%" -S . -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake configuration failed!
    pause
    exit /b 1
)

echo.
echo [3/4] Building project...
"%VS_CMAKE%" --build build --config Release
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

echo.
echo [4/4] Build successful!
echo ============================================================
echo    Executable: build\DDoSDetectionSimulation.exe
echo ============================================================
echo.
echo Press any key to run...
pause >nul
start "" "build\DDoSDetectionSimulation.exe"

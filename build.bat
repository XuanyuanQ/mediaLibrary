@echo off
setlocal

REM ============================================================================
REM  Build Media Library and start it. Output: build\Release\appMediaLibrary.exe
REM
REM  Settings: edit the three lines below, or set them as environment variables.
REM    QT_DIR        Qt folder for MSVC 64-bit, e.g. C:/Qt/6.8.0/msvc2022_64
REM    VS_GENERATOR  Visual Studio version:
REM                    "Visual Studio 18 2026"  or  "Visual Studio 17 2022"
REM    CMAKE_EXE     cmake.exe to use. Keep "cmake" if cmake is on PATH.
REM                  Otherwise use the full path, e.g. the CMake inside Visual Studio:
REM                  <VS folder>\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe
REM ============================================================================
if not defined QT_DIR       set "QT_DIR=C:/QtSS/6.12.0/msvc2022_64"
if not defined VS_GENERATOR set "VS_GENERATOR=Visual Studio 18 2026"
if not defined CMAKE_EXE    set "CMAKE_EXE=cmake"

REM Run from the script folder
cd /d "%~dp0"

REM Generate the Visual Studio project
"%CMAKE_EXE%" -S . -B build -G "%VS_GENERATOR%" -A x64 -DCMAKE_PREFIX_PATH="%QT_DIR%"
if errorlevel 1 (
    echo CMake generate failed
    pause
    exit /b 1
)

REM Build Release (also copies the Qt DLLs next to the exe)
"%CMAKE_EXE%" --build build --config Release
if errorlevel 1 (
    echo Build failed
    pause
    exit /b 1
)

echo Build Success! build\Release\appMediaLibrary.exe
@REM  start "" "build\Release\appMediaLibrary.exe"

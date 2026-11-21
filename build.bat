@echo off
setlocal

REM ============================================================================
REM  Build and run Media Library (Visual Studio 2026 + MSVC Qt)
REM
REM  Usage:
REM    double-click               -> build Release and start the app
REM    build.bat Debug            -> build Debug and start the app
REM    build.bat Release norun    -> build only
REM
REM  If Qt is installed elsewhere, change QT_DIR below or run "set QT_DIR=..." first.
REM ============================================================================

if not defined QT_DIR set "QT_DIR=C:/QtSS/6.12.0/msvc2022_64"
set "GENERATOR=Visual Studio 18 2026"
set "BUILD_DIR=build"

REM Arg 1: build type (default Release). Arg 2: "norun" skips starting the app.
set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Release"
set "RUN=1"
if /i "%~2"=="norun" set "RUN=0"

REM Run from the script folder so CMakeLists.txt is found from anywhere
pushd "%~dp0"

REM ---- 1. Check tools ----
where cmake >nul 2>nul
if errorlevel 1 (
    echo [ERROR] cmake not found. Install CMake and add it to PATH.
    goto :fail
)
if not exist "%QT_DIR%\lib\cmake\Qt6\Qt6Config.cmake" (
    echo [ERROR] Qt6 not found in "%QT_DIR%".
    echo         Set QT_DIR in build.bat to the msvc2022_64 Qt folder.
    goto :fail
)

echo ==== Qt:     %QT_DIR%
echo ==== Config: %CONFIG%
echo.

REM ---- 2. Configure (only the first time, or after deleting the build folder) ----
if not exist "%BUILD_DIR%\CMakeCache.txt" (
    echo ==== [1/2] Configuring ...
    cmake -S . -B "%BUILD_DIR%" -G "%GENERATOR%" -A x64 -DCMAKE_PREFIX_PATH="%QT_DIR%"
    if errorlevel 1 goto :fail
) else (
    echo ==== [1/2] Already configured, skipped. Delete %BUILD_DIR% to configure again.
)

REM ---- 3. Build ----
echo.
echo ==== [2/2] Building %CONFIG% ...
cmake --build "%BUILD_DIR%" --config %CONFIG% -j 8
if errorlevel 1 goto :fail

set "EXE=%BUILD_DIR%\%CONFIG%\appMediaLibrary.exe"
echo.
echo ==== Build succeeded: %CD%\%EXE%

REM ---- 4. Start the app in its own window ----
if "%RUN%"=="1" start "" "%EXE%"

popd
endlocal
exit /b 0

:fail
echo.
echo ==== Build failed. See the errors above.
popd
endlocal
REM Keep the window open when started by double-click
pause
exit /b 1

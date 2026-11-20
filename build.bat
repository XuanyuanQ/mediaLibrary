@echo off
REM 注意：本文件以 GBK 编码保存（中文 Windows 的 cmd 默认编码），否则中文会乱码甚至导致脚本出错
setlocal

REM ============================================================================
REM  一键编译并运行 Media Library（Visual Studio 2026 + MSVC 版 Qt）
REM
REM  用法：
REM    双击运行                   -> 编译 Release 版并启动程序
REM    build.bat Debug            -> 编译 Debug 版并启动程序
REM    build.bat Release norun    -> 只编译，不启动
REM
REM  Qt 装在别的位置时，修改下面的 QT_DIR，或者先在命令行里 set QT_DIR=... 再运行
REM ============================================================================

if not defined QT_DIR set "QT_DIR=C:/QtSS/6.12.0/msvc2022_64"
set "GENERATOR=Visual Studio 18 2026"
set "BUILD_DIR=build"

REM 第一个参数：构建类型（默认 Release）；第二个参数为 norun 时不启动程序
set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Release"
set "RUN=1"
if /i "%~2"=="norun" set "RUN=0"

REM 切换到脚本所在目录，这样双击或在任意目录调用都能找到 CMakeLists.txt
pushd "%~dp0"

REM ---- 1. 检查环境 ----
where cmake >nul 2>nul
if errorlevel 1 (
    echo [错误] 找不到 cmake，请先安装 CMake 并把它加入 PATH。
    goto :fail
)
if not exist "%QT_DIR%\lib\cmake\Qt6\Qt6Config.cmake" (
    echo [错误] 在 "%QT_DIR%" 下没有找到 Qt6。
    echo        请修改 build.bat 里的 QT_DIR，指向 msvc2022_64 版本的 Qt 目录。
    goto :fail
)

echo ==== Qt:     %QT_DIR%
echo ==== Config: %CONFIG%
echo.

REM ---- 2. 配置（生成 VS 工程）----
REM 已经配置过就跳过，只在第一次或删除 build 目录后执行，节省时间
if not exist "%BUILD_DIR%\CMakeCache.txt" (
    echo ==== [1/2] 配置项目 ...
    cmake -S . -B "%BUILD_DIR%" -G "%GENERATOR%" -A x64 -DCMAKE_PREFIX_PATH="%QT_DIR%"
    if errorlevel 1 goto :fail
) else (
    echo ==== [1/2] 已配置过，跳过（需要重新配置时删除 %BUILD_DIR% 目录即可）
)

REM ---- 3. 编译 ----
echo.
echo ==== [2/2] 编译 %CONFIG% ...
cmake --build "%BUILD_DIR%" --config %CONFIG% -j 8
if errorlevel 1 goto :fail

set "EXE=%BUILD_DIR%\%CONFIG%\appMediaLibrary.exe"
echo.
echo ==== 编译成功：%CD%\%EXE%

REM ---- 4. 启动程序 ----
REM start 会让程序在独立窗口运行，脚本窗口随即关闭
if "%RUN%"=="1" start "" "%EXE%"

popd
endlocal
exit /b 0

:fail
echo.
echo ==== 编译失败，请查看上面的错误信息。
popd
endlocal
REM 双击运行时窗口会立刻关闭，这里暂停一下方便看错误
pause
exit /b 1

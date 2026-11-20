# Media Library

Qt 6 / QML 媒体库浏览器，MVVM 架构。架构说明见 [ARCHITECTURE.md](ARCHITECTURE.md)（浏览器打开 [ARCHITECTURE.html](ARCHITECTURE.html) 可以看到渲染好的图）。

> **当前状态：架构骨架。** 目录、构建、各层接口和对象组装都已就绪，界面是标注了组件名和绑定关系的占位框；具体逻辑（代码里标 `TODO` 的地方）尚未实现。

## 构建和运行

需要 Qt 6.5 以上（Quick、QuickControls2、LinguistTools、Test 模块）和 CMake 3.21+。

**Windows，双击 `build.bat`**：用 Visual Studio 2026 + MSVC 版 Qt 编译并启动。Qt 路径默认是 `C:/QtSS/6.12.0/msvc2022_64`，可以在脚本开头修改。

**命令行（MinGW 预设）**：

```bash
cmake --preset mingw
```

```bash
cmake --build --preset mingw
```

```bash
ctest --preset mingw
```

```bash
build\mingw\appMediaLibrary.exe
```

**命令行（Visual Studio 2026）**：

```bash
cmake -S . -B build -G "Visual Studio 18 2026" -A x64 -DCMAKE_PREFIX_PATH=C:/QtSS/6.12.0/msvc2022_64
```

```bash
cmake --build build --config Release -j 8
```

## 目录

| 目录 | 内容 |
|---|---|
| `src/model/` | Model：`MediaItem`、`MediaJsonLoader`、`MediaLibraryModel` |
| `src/viewmodel/` | ViewModel：`LibraryViewModel`、`MediaFilterModel` |
| `src/services/` | Service：`LanguageManager` |
| `src/main.cpp` | 创建并组装所有对象，注入 QML |
| `qml/` | View：`Main.qml`、`Theme.qml`、`layouts/`、`components/` |
| `data/` | mock 数据 |
| `i18n/` | 翻译源文件（`.ts`） |
| `tests/` | Qt Test 单元测试（只链接 C++ 静态库 `mediacore`） |
"# mediaLibrary" 

# Media Library

Qt 6 / QML 媒体库浏览器，MVVM 架构。架构说明见 [ARCHITECTURE.md](ARCHITECTURE.md)（浏览器打开 [ARCHITECTURE.html](ARCHITECTURE.html) 可以看到渲染好的图）。

> **当前状态：** 列表显示、按标题或创作者搜索、类型筛选、点击查看详情、无结果提示、语言切换（英文 / 瑞典语）已完成。

## 构建和运行

需要 Qt 6.5 以上（Quick、QuickControls2、LinguistTools 模块）和 CMake 3.21+。

**Windows，双击 `build.bat`**：用 Visual Studio 2026 + MSVC 版 Qt 编译并启动。Qt 路径默认是 `C:/QtSS/6.12.0/msvc2022_64`，可以在脚本开头修改。

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
| `src/interface/` | 接口层，QML 只能看到这一层：`FromUi.h`（上发消息名）、`UiEvents.h`（上发入口）、`ToUi.h`（下发数据名）、`UiDataBase`（统一下发入口）、`UiProperty.h`（属性宏）、`LibraryData` / `LanguageData`（下发数据） |
| `src/logic/` | 逻辑层，QML 看不到：`LibraryManager`（搜索、筛选、选中）、`LanguageManager`（切换语言） |
| `src/model/` | Model：`MediaItem`、`MediaJsonLoader`、`MediaLibraryModel`、`MediaFilterModel` |
| `src/main.cpp` | 创建并组装所有对象，把接口对象注册成 QML 单例（`UiEvents`、`LibraryData`、`LanguageData`） |
| `qml/` | View：`Main.qml`、`Theme.qml`、`layouts/`、`components/` |
| `data/` | mock 数据 |
| `i18n/` | 翻译源文件：`medialibrary_sv.ts`（瑞典语）；英文是源语言，不需要翻译文件 |

## 后续改进

**单元测试（时间充分时优先补上）**

题目没有要求单元测试，这一版没有写。架构上已经为测试做好了准备：逻辑全部在 C++ 静态库 `mediacore` 里，不依赖 QML，测试程序直接链接这个库就能运行，不需要启动界面。计划用 Qt Test 覆盖：

| 测试对象 | 覆盖点 |
|---|---|
| `MediaJsonLoader` | 正常加载；文件不存在；JSON 格式错误；缺少 ID 或标题的记录被跳过并给出警告 |
| `MediaFilterModel` | 空搜索返回全部；按标题、按创作者搜索；不区分大小写；多关键字同时满足；类型筛选；`Others` 分类；搜索和筛选组合 |
| `LibraryManager` | 发 `UiEvents` 消息后 `LibraryData` 的值是否正确：搜索防抖（连续输入只过滤一次）；选中后详情的格式化文字；`emptyState` 三种情况；计数 |
| `UiDataBase` | `publish()` 类型不对、发错数据类时丢弃并警告；值没变时不发信号 |

**后台加载数据（数据量变大时）**

这一版没有用线程：数据只有 24 条，而且编在程序资源里，加载和过滤都是微秒级，不会卡住界面，加线程只会增加复杂度。

如果以后媒体库变大（几万条），或者数据改成从磁盘扫描、网络、数据库读取，就把加载放到后台线程：

- 用 `QtConcurrent::run` 在后台调用 `MediaJsonLoader::load()`，再用 `QFutureWatcher` 在加载完成时通知主线程；也可以用 worker 对象加 `moveToThread`，通过信号把结果发回来
- 结果回到主线程后，再调用 `MediaLibraryModel::setItems()`。`QAbstractItemModel` 被 ListView 使用，只能在 GUI 线程里修改，后台线程只负责读取和解析，不碰模型
- 加载期间可以下发一个“加载中”的状态给界面显示

现在的结构已经为此留好了位置：读取（`MediaJsonLoader`）和模型（`MediaLibraryModel`）是分开的，改成异步加载只需要改 `main.cpp` 里加载数据的那一段，模型、逻辑和界面都不用动。

**其他**：播放、收藏和文件夹、数据持久化、在线数据源，这一版刻意不做，控制范围，专注题目要求。

# Media Library —— Qt 6 / QML 媒体库浏览器（MVVM）
# 架构说明见 ARCHITECTURE.md
#
# 构建产物：
#   mediacore        静态库：Model + ViewModel + Service（C++ 逻辑），同时是 QML 模块 MediaLibrary.Core
#   appMediaLibrary  可执行文件：main.cpp + QML 界面（QML 模块 MediaLibrary）
#   tests/           单元测试，只链接 mediacore，不启动界面
cmake_minimum_required(VERSION 3.21)

project(MediaLibrary VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 LinguistTools)
qt_standard_project_setup(REQUIRES 6.5)

# QML 文件放在子目录（layouts/、components/）里：启用新行为，为子目录生成 qmldir
if(QT_KNOWN_POLICY_QTP0004)
    qt_policy(SET QTP0004 NEW)
endif()

option(MEDIALIBRARY_BUILD_TESTS "Build unit tests" ON)

# 和旧项目一样：Visual Studio 编译器必须搭配 MSVC 版 Qt，提前给出明确提示
if(MSVC AND EXISTS "${QT6_INSTALL_PREFIX}/lib/libQt6Core.a")
    message(FATAL_ERROR
        "当前使用 Visual Studio (MSVC) 编译，但 CMAKE_PREFIX_PATH 指向的是 MinGW 版 Qt：\n"
        "  ${QT6_INSTALL_PREFIX}\n"
        "请传入 -DCMAKE_PREFIX_PATH=<Qt目录>/6.x.x/msvc2022_64")
endif()

# =============================================================================
# 1. mediacore：C++ 逻辑静态库（Model / ViewModel / Service）
# =============================================================================
# 做成 QML 模块 "MediaLibrary.Core"，这样 QML 可以写
#   required property LibraryViewModel libraryVm
# 这种带类型的声明，也能访问 C++ 枚举（LibraryViewModel.NoMatch）。
# 对象本身都在 main.cpp 里创建，QML 不能自己创建它们（QML_UNCREATABLE）。
qt_add_library(mediacore STATIC)

qt_add_qml_module(mediacore
    URI MediaLibrary.Core
    VERSION 1.0
    # QML 模块的输出目录要和 URI 对应（MediaLibrary.Core -> MediaLibrary/Core），qmllint 等工具才能找到
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/MediaLibrary/Core
    SOURCES
        src/model/mediaitem.h
        src/model/mediajsonloader.h        src/model/mediajsonloader.cpp
        src/model/medialibrarymodel.h      src/model/medialibrarymodel.cpp
        src/viewmodel/mediafiltermodel.h   src/viewmodel/mediafiltermodel.cpp
        src/viewmodel/libraryviewmodel.h   src/viewmodel/libraryviewmodel.cpp
        src/services/languagemanager.h     src/services/languagemanager.cpp
)

# 头文件按层分目录；生成的 QML 类型注册代码按文件名包含头文件，所以每层目录都要加进来
target_include_directories(mediacore PUBLIC
    src
    src/model
    src/viewmodel
    src/services
)

target_link_libraries(mediacore PUBLIC Qt6::Core Qt6::Qml)

# =============================================================================
# 2. appMediaLibrary：应用程序（main.cpp + QML 界面）
# =============================================================================
qt_add_executable(appMediaLibrary src/main.cpp)

# Theme.qml 是单例：其他 QML 文件直接写 Theme.xxx 使用
set_source_files_properties(qml/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(appMediaLibrary
    URI MediaLibrary
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/Theme.qml
        qml/MediaUtils.js
        qml/layouts/WideLayout.qml
        qml/layouts/NarrowLayout.qml
        qml/components/LibraryPane.qml
        qml/components/DetailView.qml
        qml/components/SearchField.qml
        qml/components/TypeFilter.qml
        qml/components/LanguageSwitch.qml
        qml/components/MediaList.qml
        qml/components/MediaRow.qml
        qml/components/EmptyState.qml
        qml/components/CoverArt.qml
        qml/components/Placeholder.qml   # 骨架阶段的占位框，实现完成后删除
    RESOURCES
        data/media.json
)

# mediacoreplugin：mediacore 的 QML 插件（静态），main.cpp 里用 Q_IMPORT_QML_PLUGIN 导入
target_link_libraries(appMediaLibrary PRIVATE
    mediacore
    mediacoreplugin
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
)

set_target_properties(appMediaLibrary PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# 翻译：构建时自动 lrelease，把 .qm 打包到资源 :/i18n/ 下。
# 更新翻译源文件（扫描 qsTr/tr）：cmake --build <build目录> --target update_translations，
# 然后用 Qt Linguist 打开 i18n/medialibrary_zh_CN.ts 填写译文。
qt_add_translations(appMediaLibrary
    TS_FILES i18n/medialibrary_zh_CN.ts
    RESOURCE_PREFIX /i18n
)

# Windows：编译完成后复制 Qt 运行库到 exe 旁边，双击即可运行
option(DEPLOY_QT_RUNTIME "Copy Qt runtime next to the executable after build (Windows)" ON)
if(WIN32 AND DEPLOY_QT_RUNTIME)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT6_INSTALL_PREFIX}/${QT6_INSTALL_BINS}")
    if(WINDEPLOYQT_EXECUTABLE)
        set(_deploy_runtime_flag "")
        if(MSVC)
            set(_deploy_runtime_flag --no-compiler-runtime)
        endif()
        add_custom_command(TARGET appMediaLibrary POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                    --qmldir "${CMAKE_CURRENT_SOURCE_DIR}/qml"
                    --no-translations
                    ${_deploy_runtime_flag}
                    --verbose 0
                    "$<TARGET_FILE:appMediaLibrary>"
            COMMENT "Deploying Qt runtime next to appMediaLibrary.exe")
    endif()
endif()

# =============================================================================
# 3. 单元测试
# =============================================================================
if(MEDIALIBRARY_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

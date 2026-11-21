# Media Library —— Qt 6 / QML 媒体库浏览器（MVVM）
# 架构说明见 ARCHITECTURE.md
#
# 构建产物：
#   mediacore        静态库：Model + ViewModel + Service（C++ 逻辑）
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
# 普通静态库，主程序和测试都链接它。
# 交给 QML 的方式：main.cpp 里用 qmlRegisterSingletonInstance 把对象注册成单例
# （模块名 MediaLibrary.Core），QML 里 import MediaLibrary.Core 之后直接写 LibraryViewModel.xxx。
qt_add_library(mediacore STATIC
    src/model/MediaItem.h
    src/model/MediaJsonLoader.h        src/model/MediaJsonLoader.cpp
    src/model/MediaLibraryModel.h      src/model/MediaLibraryModel.cpp
    src/viewmodel/MediaFilterModel.h   src/viewmodel/MediaFilterModel.cpp
    src/viewmodel/LibraryViewModel.h   src/viewmodel/LibraryViewModel.cpp
    src/services/LanguageManager.h     src/services/LanguageManager.cpp
)

# 头文件按层分目录，include 时直接写文件名即可（例如 #include "LibraryViewModel.h"）
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

target_link_libraries(appMediaLibrary PRIVATE
    mediacore
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

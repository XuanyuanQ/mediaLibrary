# Media Library —— Qt 6 / QML 媒体库浏览器（MVVM）
# 架构说明见 ARCHITECTURE.md
#
# 构建产物：
#   mediacore        静态库：Model + ViewModel + Service（C++ 逻辑）
#   appMediaLibrary  可执行文件：main.cpp + QML 界面（QML 模块 MediaLibrary）
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

# =============================================================================
# 1. mediacore：C++ 静态库（接口层 / 逻辑层 / Model），目录和 ARCHITECTURE.md 的分层图一一对应
# =============================================================================
# 普通静态库，逻辑和界面分开编译（以后补单元测试时可以直接链接它，不用启动界面）。
# 交给 QML 的方式：main.cpp 里用 qmlRegisterSingletonInstance 把接口层对象注册成单例
# （模块名 MediaLibrary.Core），QML 里写 UiEvents.post(...)、LibraryData.xxx。
qt_add_library(mediacore STATIC
    # 接口层：QML 只能看到这一层（上发 UiEvents，下发 LibraryData / LanguageData）
    src/interface/FromUi.h             # 上发消息表：全部上发消息的名字（UI 发出）
    src/interface/UiEvents.h           # 上发统一入口 post 信号
    src/interface/ToUi.h               # 下发消息表：全部下发数据的名字（发往 UI）
    src/interface/UiDataBase.h         src/interface/UiDataBase.cpp   # 下发统一入口 publish()
    src/interface/UiProperty.h         # 下发属性宏 UI_PROPERTY
    src/interface/LibraryData.h        # 媒体库下发数据（QML 只读）
    src/interface/LanguageData.h       # 语言下发数据（QML 只读）

    # 逻辑层：QML 看不到；处理上发消息，计算后 publish 下发
    src/logic/LibraryManager.h       src/logic/LibraryManager.cpp
    src/logic/LanguageManager.h        src/logic/LanguageManager.cpp

    # Model：数据本身
    src/model/MediaItem.h
    src/model/MediaJsonLoader.h        src/model/MediaJsonLoader.cpp
    src/model/MediaLibraryModel.h      src/model/MediaLibraryModel.cpp
    src/model/MediaFilterModel.h       src/model/MediaFilterModel.cpp
)

# 头文件按层分目录，include 时直接写文件名即可（例如 #include "LibraryManager.h"）
target_include_directories(mediacore PUBLIC
    src
    src/interface
    src/logic
    src/model
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
# 然后用 Qt Linguist 打开 i18n/*.ts 填写译文。英文是源语言，不需要 .ts。
# SOURCE_TARGETS：除了界面（QML、main.cpp），还要扫描 mediacore 里 C++ 的 tr()
qt_add_translations(appMediaLibrary
    SOURCE_TARGETS appMediaLibrary mediacore
    TS_FILES
        i18n/medialibrary_sv.ts      # 瑞典语
    RESOURCE_PREFIX /i18n
)

# Windows：编译完成后复制 Qt 运行库到 exe 旁边，双击即可运行
option(DEPLOY_QT_RUNTIME "Copy Qt runtime next to the executable after build (Windows)" ON)
if(WIN32 AND DEPLOY_QT_RUNTIME)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT6_INSTALL_PREFIX}/${QT6_INSTALL_BINS}")
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET appMediaLibrary POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                    --qmldir "${CMAKE_CURRENT_SOURCE_DIR}/qml"
                    --no-translations
                    --no-compiler-runtime   # VC 运行库不复制（装了 Visual Studio 的机器上已有）
                    --verbose 0
                    "$<TARGET_FILE:appMediaLibrary>"
            COMMENT "Deploying Qt runtime next to appMediaLibrary.exe")
    endif()
endif()

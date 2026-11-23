# Media Library - Qt 6 / QML media library browser. See architecture.svg.
#
#   mediacore        static library: interface, logic and model (C++)
#   appMediaLibrary  executable: main.cpp + QML UI
cmake_minimum_required(VERSION 3.21)

project(MediaLibrary VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 LinguistTools)
qt_standard_project_setup(REQUIRES 6.5)

# Generate qmldir files for QML in subfolders (layouts/, components/)
if(QT_KNOWN_POLICY_QTP0004)
    qt_policy(SET QTP0004 NEW)
endif()

# =============================================================================
# 1. mediacore: C++ code, folders match the layers in architecture.svg
# =============================================================================
qt_add_library(mediacore STATIC
    # Interface: the only C++ QML can see
    src/interface/FromUi.h             # messages from the UI
    src/interface/UiEvents.h           # post() signal for messages from the UI
    src/interface/ToUi.h               # data sent to the UI
    src/interface/UiDataBase.h         src/interface/UiDataBase.cpp   # publish()
    src/interface/UiProperty.h         # UI_PROPERTY macro
    src/interface/LibraryData.h
    src/interface/LanguageData.h

    # Logic: hidden from QML
    src/logic/LibraryManager.h         src/logic/LibraryManager.cpp
    src/logic/LanguageManager.h        src/logic/LanguageManager.cpp

    # Model
    src/model/MediaItem.h
    src/model/MediaJsonLoader.h        src/model/MediaJsonLoader.cpp
    src/model/MediaLibraryModel.h      src/model/MediaLibraryModel.cpp
    src/model/MediaFilterModel.h       src/model/MediaFilterModel.cpp
)

# Include by file name only, e.g. #include "LibraryManager.h"
target_include_directories(mediacore PUBLIC
    src
    src/interface
    src/logic
    src/model
)

target_link_libraries(mediacore PUBLIC Qt6::Core Qt6::Qml)

# =============================================================================
# 2. appMediaLibrary: main.cpp + QML UI
# =============================================================================
qt_add_executable(appMediaLibrary src/main.cpp)

# Theme.qml is a singleton (used as Theme.xxx)
set_source_files_properties(qml/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(appMediaLibrary
    URI MediaLibrary
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/Theme.qml
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

# Translations: .qm files are built and embedded under :/i18n/.
# Update .ts files with: cmake --build <build dir> --target update_translations
# All translated texts are qsTr() in QML. English is the source language.
qt_add_translations(appMediaLibrary
    SOURCE_TARGETS appMediaLibrary
    TS_FILES
        i18n/medialibrary_sv.ts      # Swedish
    RESOURCE_PREFIX /i18n
)

# Windows: copy the Qt runtime next to the exe after building
option(DEPLOY_QT_RUNTIME "Copy Qt runtime next to the executable after build (Windows)" ON)
if(WIN32 AND DEPLOY_QT_RUNTIME)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT6_INSTALL_PREFIX}/${QT6_INSTALL_BINS}")
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET appMediaLibrary POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                    --qmldir "${CMAKE_CURRENT_SOURCE_DIR}/qml"
                    --no-translations
                    --no-compiler-runtime   # VC runtime is already on machines with Visual Studio
                    --verbose 0
                    "$<TARGET_FILE:appMediaLibrary>"
            COMMENT "Deploying Qt runtime next to appMediaLibrary.exe")
    endif()
endif()

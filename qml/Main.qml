// 主窗口：接收 main.cpp 注入的对象，按窗口宽度选择布局。
// 这里不保存任何业务状态（搜索、筛选、选中都在 libraryVm 里）。
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

ApplicationWindow {
    id: window

    // 由 main.cpp 通过 setInitialProperties 注入
    required property LibraryViewModel libraryVm
    required property LanguageManager language

    readonly property bool wide: width >= Theme.wideBreakpoint

    width: 1280
    height: 720
    minimumWidth: 360
    minimumHeight: 480
    visible: true
    title: qsTr("Media Library")
    color: Theme.background

    // 分辨率适配：根据窗口尺寸计算缩放系数（宽屏基准 1280×720，窄屏基准 390×844）
    // TODO: 按 ARCHITECTURE.md 第 8 节实现 clamp 计算
    Binding {
        target: Theme
        property: "scale"
        value: 1.0
    }

    // 宽窄布局都绑定同一个 libraryVm，切换时状态不会丢
    Loader {
        anchors.fill: parent
        sourceComponent: window.wide ? wideLayout : narrowLayout
    }

    Component {
        id: wideLayout
        WideLayout {
            libraryVm: window.libraryVm
            language: window.language
        }
    }

    Component {
        id: narrowLayout
        NarrowLayout {
            libraryVm: window.libraryVm
            language: window.language
        }
    }
}

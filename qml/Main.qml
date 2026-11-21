// 主窗口：按窗口宽度选择布局。
// 不保存任何业务状态（搜索、筛选、选中都在 LibraryViewModel 单例里），
// 所以宽窄布局切换时状态不会丢。
import QtQuick
import QtQuick.Controls
import MediaLibrary

ApplicationWindow {
    id: window

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

    Loader {
        anchors.fill: parent
        sourceComponent: window.wide ? wideLayout : narrowLayout
    }

    Component {
        id: wideLayout
        WideLayout { }
    }

    Component {
        id: narrowLayout
        NarrowLayout { }
    }
}

// 主窗口：只做横屏，内容就是 WideLayout（左列表 + 右详情）。
// 不保存任何业务状态（搜索、筛选、选中都在 C++ 里，QML 通过 LibraryData 单例读取）。
import QtQuick
import QtQuick.Controls
import MediaLibrary

ApplicationWindow {
    id: window

    width: 1280
    height: 720
    minimumWidth: 900      // 再窄左右两栏就挤不下了
    minimumHeight: 480
    visible: true
    title: qsTr("Media Library")
    color: Theme.background

    // 分辨率适配：根据窗口尺寸计算缩放系数（基准 1280×720）
    // TODO: 按 ARCHITECTURE.md 第 8 节实现 clamp 计算
    Binding {
        target: Theme
        property: "scale"
        value: 1.0
    }

    WideLayout {
        anchors.fill: parent
    }
}

// 【骨架专用】占位框：显示组件名和它绑定的 ViewModel 属性，方便检查架构。
// 各组件实现完成后删除本文件。
import QtQuick
import QtQuick.Controls
import MediaLibrary

Rectangle {
    id: root

    property string name        // 组件名
    property string hint        // 绑定 / 调用说明
    default property alias content: inner.data   // 可以在占位框里再放子组件

    // 宽度按文字撑开，但最多 220，避免把整行撑出窗口
    implicitWidth: Math.min(Math.max(label.implicitWidth, hintLabel.implicitWidth) + Theme.dp(16),
                            Theme.dp(220))
    implicitHeight: Theme.dp(48)
    radius: Theme.dp(6)
    color: Theme.surface
    border.width: 1
    border.color: Theme.border

    Column {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: Theme.dp(8)
        spacing: Theme.dp(2)

        width: root.width - Theme.dp(16)

        Label {
            id: label
            width: parent.width
            elide: Text.ElideRight
            text: root.name
            color: Theme.accent
            font.family: Theme.monoFont
            font.pixelSize: Theme.sp(12)
        }
        Label {
            id: hintLabel
            width: parent.width
            elide: Text.ElideRight
            text: root.hint
            visible: text.length > 0
            color: Theme.textSecondary
            font.pixelSize: Theme.sp(11)
        }
    }

    Item {
        id: inner
        anchors.fill: parent
    }
}

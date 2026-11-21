// 列表中的一行：标题 + 创作者，右侧年份
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MediaLibrary
import MediaLibrary.Core

ItemDelegate {
    id: root

    // 这一行在列表中的位置（从 0 开始），由 ListView 自动赋值
    required property int index

    // 这一行的数据：由 ListView 根据模型 roleNames() 自动赋值。
    // 名字必须和 C++ roleNames() 里的一致（mediaId、title …），写错会在启动时报
    // “Required property xxx was not initialized”
    required property int mediaId
    required property string title
    required property string creator
    required property string type
    required property int year

    readonly property bool selected: LibraryViewModel.selectedId === root.mediaId

    padding: Theme.dp(12)

    background: Rectangle {
        radius: Theme.dp(8)
        color: root.selected || root.down ? Theme.surfaceRaised : Theme.surface
        border.width: 1
        border.color: root.selected ? Theme.accent : Theme.border
    }

    contentItem: RowLayout {
        spacing: Theme.dp(12)

        // 序号：index 从 0 开始，所以显示 index + 1。
        // 固定宽度并右对齐，两位数和一位数的序号也能上下对齐
        Label {
            Layout.preferredWidth: Theme.dp(24)
            text: root.index + 1
            horizontalAlignment: Text.AlignRight
            color: Theme.textSecondary
            font.family: Theme.monoFont
            font.pixelSize: Theme.sp(13)
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: Theme.dp(2)

            Label {
                Layout.fillWidth: true
                text: root.title
                textFormat: Text.PlainText    // 按纯文本显示，标题里的 "<Code>" 不会被当成标签
                elide: Text.ElideRight
                color: Theme.textPrimary
                font.pixelSize: Theme.sp(15)
                font.bold: true
            }
            Label {
                Layout.fillWidth: true
                text: root.creator
                textFormat: Text.PlainText
                elide: Text.ElideRight
                color: Theme.textSecondary
                font.pixelSize: Theme.sp(13)
            }
        }

        Label {
            text: root.year > 0 ? root.year : ""
            color: Theme.textSecondary
            font.pixelSize: Theme.sp(13)
        }
    }

    // 上发：点击后发一条 ItemClicked 消息，C++ 的 LibraryViewModel::handleUiEvent() 交给 select()
    onClicked: UiEvents.post(UiEvents.ItemClicked, root.mediaId)

    Accessible.name: root.title + ", " + root.creator
}

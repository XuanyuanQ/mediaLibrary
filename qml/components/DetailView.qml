// 详情：类型、标题、创作者、年份 · 时长（或页数）、简介。
// 只绑定 LibraryData.selectedItem，显示文字都已在 C++（LibraryManager::publishSelectedItem）里准备好。
//
// 数据流：点击列表某一行 → MediaRow 上发 UiEvents.post(ItemClicked, id)
//        → C++ 的 LibraryManager::handleUiEvent() → select(id)
//        → publishSelectedItem() 生成详情，publish(UiData::SelectedItem, …) 下发，发出 selectedItemChanged
//        → 这里所有用到 selectedItem 的绑定重新读取 → 显示新内容
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MediaLibrary
import MediaLibrary.Core

Item {
    id: root

    // selectedItem 是 C++ 的 QVariantMap，到 QML 里变成 JS 对象，用 .title 等读取字段
    readonly property var item: LibraryData.selectedItem
    // 没选中时 C++ 给的是空 map，没有 title 字段
    readonly property bool hasItem: item.title !== undefined

    // 未选中：中间一行提示
    Label {
        anchors.centerIn: parent
        visible: !root.hasItem
        text: qsTr("Select an item to see its details")
        color: Theme.textSecondary
        font.pixelSize: Theme.sp(15)
    }

    // 已选中：内容可能比屏幕长（简介很长），所以放进可滚动的 Flickable
    Flickable {
        anchors.fill: parent
        visible: root.hasItem
        clip: true
        contentHeight: content.implicitHeight + Theme.dp(48)
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar { }

        ColumnLayout {
            id: content
            x: Theme.dp(24)
            y: Theme.dp(24)
            width: parent.width - Theme.dp(48)
            spacing: Theme.dp(8)

            // 类型（橙色小字）
            Label {
                text: root.item.typeLabel ?? ""
                color: Theme.accent
                font.family: Theme.monoFont
                font.pixelSize: Theme.sp(13)
            }

            // 标题
            Label {
                Layout.fillWidth: true
                text: root.item.title ?? ""
                textFormat: Text.PlainText     // 标题里的 "<Code>" 按原样显示
                wrapMode: Text.Wrap
                color: Theme.textPrimary
                font.pixelSize: Theme.sp(30)
                font.bold: true
            }

            // 创作者
            Label {
                Layout.fillWidth: true
                text: root.item.creator ?? ""
                textFormat: Text.PlainText
                wrapMode: Text.Wrap
                color: Theme.textSecondary
                font.pixelSize: Theme.sp(17)
            }

            // 年份 · 时长 / 页数
            Label {
                Layout.fillWidth: true
                visible: text.length > 0
                text: root.item.metaText ?? ""
                color: Theme.textSecondary
                font.family: Theme.monoFont
                font.pixelSize: Theme.sp(13)
            }

            // 分隔线
            Rectangle {
                Layout.fillWidth: true
                Layout.topMargin: Theme.dp(12)
                Layout.bottomMargin: Theme.dp(12)
                implicitHeight: 1
                color: Theme.border
            }

            // 简介
            Label {
                Layout.fillWidth: true
                text: root.item.description ?? ""
                textFormat: Text.PlainText
                wrapMode: Text.Wrap
                lineHeight: 1.4
                color: Theme.textPrimary
                font.pixelSize: Theme.sp(15)
            }
        }
    }
}

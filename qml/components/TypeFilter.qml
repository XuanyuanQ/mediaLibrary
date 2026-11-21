// 类型筛选下拉框（带 ▾）：选项来自 LibraryData.categories，选中后通过 UiEvents.post(TypeSelected) 上发。
// 选了某个类型（不是 All types）时，边框和文字变成橙色，提示当前正在筛选
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

ComboBox {
    id: root

    // 当前是否在筛选（"" 表示全部类型）
    readonly property bool active: LibraryData.category !== ""

    // 选项：C++ 生成的 [{key, label}, ...]
    // textRole：显示哪个字段；valueRole：选中后 currentValue 取哪个字段
    model: LibraryData.categories
    textRole: "label"
    valueRole: "key"

    // 上发（界面 → C++）：用户选了某一项（onActivated 只响应用户操作，和搜索框的 onTextEdited 同理）。
    // 发一条 TypeSelected 消息，C++ 的 LibraryViewModel::handleUiEvent() 交给 setCategory()，立即重新过滤
    onActivated: UiEvents.post(UiEvents.TypeSelected, currentValue)

    // 启动时：按 C++ 里当前的 category（默认 ""）选中对应的一项（All types）
    Component.onCompleted: currentIndex = indexOfValue(LibraryData.category)

    implicitWidth: Theme.dp(140)
    implicitHeight: Theme.dp(40)
    font.pixelSize: Theme.sp(13)

    // 框里显示的文字（当前选中项的 label）
    contentItem: Text {
        leftPadding: Theme.dp(12)
        rightPadding: root.indicator.width + Theme.dp(16)
        text: root.displayText
        font: root.font
        color: root.active ? Theme.accent : Theme.textPrimary
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    // 右侧箭头：展开时 ▴，收起时 ▾
    indicator: Text {
        x: root.width - width - Theme.dp(12)
        y: (root.height - height) / 2
        text: root.popup.visible ? "▴" : "▾"
        font.pixelSize: Theme.sp(12)
        color: root.active ? Theme.accent : Theme.textSecondary
    }

    background: Rectangle {
        radius: Theme.dp(8)
        color: Theme.surface
        border.width: 1
        border.color: root.active || root.popup.visible ? Theme.accent : Theme.border
    }

    // 弹出列表里的每一项
    delegate: ItemDelegate {
        id: option

        // 由 ComboBox 自动赋值：这一项的数据和位置
        required property var model
        required property int index

        readonly property bool selected: model[root.valueRole] === LibraryData.category

        width: root.popup.availableWidth
        height: Theme.dp(36)
        highlighted: root.highlightedIndex === index   // 鼠标悬停 / 键盘移动到的那一项

        contentItem: Item {
            Text {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: option.model[root.textRole]
                font: root.font
                color: option.selected ? Theme.accent : Theme.textPrimary
            }
            // 当前选中的那一项打 ✓
            Text {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                visible: option.selected
                text: "✓"
                font: root.font
                color: Theme.accent
            }
        }

        background: Rectangle {
            radius: Theme.dp(6)
            color: option.highlighted ? Theme.surfaceRaised : "transparent"
        }
    }

    // 弹出层：出现在下拉框正下方，深色背景
    popup: Popup {
        y: root.height + Theme.dp(4)
        width: root.width
        padding: Theme.dp(4)

        contentItem: ListView {
            implicitHeight: contentHeight
            clip: true
            model: root.popup.visible ? root.delegateModel : null
            currentIndex: root.highlightedIndex
        }

        background: Rectangle {
            radius: Theme.dp(8)
            color: Theme.surfaceRaised
            border.width: 1
            border.color: Theme.border
        }
    }
}

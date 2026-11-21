// 搜索框：输入内容写入 LibraryViewModel.searchText（防抖和过滤都在 C++ 里），
// 右侧有清除按钮，按 Esc 也能清空
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

TextField {
    id: root

    // 显示（C++ → 界面）：绑定 ViewModel 里的搜索词。
    // C++ 的 setSearchText() 里 emit searchTextChanged（Q_PROPERTY 的 NOTIFY 信号）后，
    // QML 自动重新取值，搜索框文字随之更新（✕ 按钮、Esc 清空就是靠这一行）
    text: LibraryViewModel.searchText
    // 输入（界面 → C++）：用户每打一个字，就写回 ViewModel。
    // 这个赋值会被 QML 引擎转成对 C++ setSearchText(text) 的调用
    // （依据 Q_PROPERTY 里的 WRITE setSearchText，不经过任何信号）。
    // setSearchText 之后的流程都在 C++：200ms 防抖 → 代理重新过滤 → 发出 rowsRemoved 等信号 → 列表更新。
    // 用 onTextEdited 而不是 onTextChanged：只响应用户输入，避免和上面的绑定互相触发
    onTextEdited: LibraryViewModel.searchText = text

    placeholderText: qsTr("Search title or creator")
    selectByMouse: true

    color: Theme.textPrimary
    placeholderTextColor: Theme.textSecondary
    selectionColor: Theme.accent
    selectedTextColor: Theme.background
    font.pixelSize: Theme.sp(14)

    leftPadding: Theme.dp(12)
    rightPadding: clearButton.visible ? clearButton.width + Theme.dp(8) : Theme.dp(12)
    implicitHeight: Theme.dp(40)

    background: Rectangle {
        radius: Theme.dp(8)
        color: Theme.surface
        border.width: 1
        // 获得焦点时边框变成橙色
        border.color: root.activeFocus ? Theme.accent : Theme.border
    }

    // Esc 清空搜索
    Keys.onEscapePressed: LibraryViewModel.searchText = ""

    // 清除按钮：有内容时才显示
    ToolButton {
        id: clearButton
        anchors.right: parent.right
        anchors.rightMargin: Theme.dp(4)
        anchors.verticalCenter: parent.verticalCenter
        visible: root.text.length > 0
        focusPolicy: Qt.NoFocus
        text: "✕"
        font.pixelSize: Theme.sp(12)
        palette.buttonText: Theme.textSecondary
        background: null
        onClicked: {
            LibraryViewModel.searchText = ""
            root.forceActiveFocus()
        }
        Accessible.name: qsTr("Clear search")
    }
}

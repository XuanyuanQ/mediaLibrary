// 搜索框：输入内容通过 UiEvents.post(SearchEdited) 上发给 C++（防抖和过滤都在 C++ 里），
// 右侧有清除按钮
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

TextField {
    id: root

    // 下发（C++ → 界面）：绑定 LibraryData 里的搜索词。
    // C++ 的 setSearchText() 里调用 m_data.publish(UiData::SearchText, …) 下发，发出 searchTextChanged 后，
    // QML 自动重新取值，搜索框文字随之更新（✕ 按钮清空就是靠这一行）
    text: LibraryData.searchText
    // 上发（界面 → C++）：用户每打一个字，就发一条 SearchEdited 消息。
    // C++ 的 LibraryViewModel::handleUiEvent() 收到后交给 setSearchText()，
    // 之后的流程都在 C++：200ms 防抖 → 代理重新过滤 → 发出 rowsRemoved 等信号 → 列表更新。
    // 用 onTextEdited 而不是 onTextChanged：只响应用户输入，避免和上面的绑定互相触发
    onTextEdited: UiEvents.post(UiEvents.SearchEdited, text)

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
            UiEvents.post(UiEvents.SearchEdited, "")
            root.forceActiveFocus()
        }
        Accessible.name: qsTr("Clear search")
    }
}

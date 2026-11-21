// Search box. Text goes to C++ (debounce and filtering happen there).
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

TextField {
    id: root

    // To UI: shows the text from C++, so clearing in C++ also clears the box
    text: LibraryData.toUiSearchText
    // From UI: onTextEdited fires on user input only, so it does not loop with the binding above
    onTextEdited: UiEvents.post(FromUi.SearchEdited, text)

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
        border.color: root.activeFocus ? Theme.accent : Theme.border
    }

    // Clear button, shown only when there is text
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
            UiEvents.post(FromUi.SearchEdited, "")
            root.forceActiveFocus()
        }
        Accessible.name: qsTr("Clear search")
    }
}

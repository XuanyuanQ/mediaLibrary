// EN | SV language buttons. The current language is highlighted.
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

Rectangle {
    id: root

    implicitWidth: buttons.implicitWidth + Theme.dp(8)
    implicitHeight: Theme.dp(36)
    radius: Theme.dp(8)
    color: Theme.surface
    border.width: 1
    border.color: Theme.border

    Row {
        id: buttons
        anchors.centerIn: parent
        spacing: Theme.dp(2)

        // modelData is {code, label}
        Repeater {
            model: LanguageData.toUiAvailableLanguages

            delegate: AbstractButton {
                id: button

                required property var modelData
                readonly property bool current: LanguageData.toUiLanguage === modelData.code

                implicitWidth: Theme.dp(40)
                implicitHeight: Theme.dp(28)
                focusPolicy: Qt.NoFocus

                onClicked: UiEvents.post(FromUi.LanguageSelected, modelData.code)

                contentItem: Text {
                    text: button.modelData.label
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Theme.sp(13)
                    font.bold: button.current
                    color: button.current ? Theme.background : Theme.textSecondary
                }

                background: Rectangle {
                    radius: Theme.dp(6)
                    color: button.current ? Theme.accent
                         : button.hovered ? Theme.surfaceRaised : "transparent"
                }

                Accessible.name: button.modelData.label
            }
        }
    }
}

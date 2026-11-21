// 语言切换：EN | 中 | SV 分段按钮。
// 下发：选项来自 LanguageData.availableLanguages，当前语言 LanguageData.language 高亮（橙色）。
// 上发：点击后 UiEvents.post(LanguageSelected, 语言代码)，C++ 的 LanguageManager 加载翻译并刷新界面。
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

        // 每种语言一个按钮。modelData 是 C++ 下发的 {code, label}
        Repeater {
            model: LanguageData.availableLanguages

            delegate: AbstractButton {
                id: button

                required property var modelData
                readonly property bool current: LanguageData.language === modelData.code

                implicitWidth: Theme.dp(40)
                implicitHeight: Theme.dp(28)
                focusPolicy: Qt.NoFocus

                // 上发：只报告“用户选了哪种语言”，翻译加载和界面刷新都在 C++ 里
                onClicked: UiEvents.post(UiEvents.LanguageSelected, modelData.code)

                contentItem: Text {
                    text: button.modelData.label
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Theme.sp(13)
                    font.bold: button.current
                    color: button.current ? Theme.background : Theme.textSecondary
                }

                // 当前语言：橙色底；鼠标悬停：稍亮的底
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

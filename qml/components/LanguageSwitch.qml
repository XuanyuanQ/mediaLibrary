// 语言切换：EN | 中 分段按钮
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

Placeholder {
    id: root

    name: "LanguageSwitch"
    hint: "UiEvents.post(LanguageSelected, code) · 当前 " + LanguageManager.language

    // TODO: 换成分段按钮：Repeater over LanguageManager.availableLanguages
}

// 语言切换：EN | 中 分段按钮
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

Placeholder {
    id: root

    required property LanguageManager language

    name: "LanguageSwitch"
    hint: "language.setLanguage() · 当前 " + root.language.language

    // TODO: 换成分段按钮：Repeater over language.availableLanguages
}

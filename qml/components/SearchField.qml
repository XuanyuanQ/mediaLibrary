// 搜索框：清除按钮、Esc 清空。防抖在 ViewModel 里，这里每次输入都直接写入。
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

Placeholder {
    id: root

    required property LibraryViewModel libraryVm

    name: "SearchField"
    hint: "↔ libraryVm.searchText"

    // TODO: 换成真正的输入框：text 写入 libraryVm.searchText，Esc 清空
}

// 列表中的一行：类型图标、标题、创作者、年份；点击后选中
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

Placeholder {
    id: root

    // 这一行的数据：由 ListView 根据模型 roleNames() 自动赋值
    required property int mediaId
    required property string title
    required property string creator
    required property string type
    required property int year

    name: "MediaRow"
    hint: root.title + " — " + root.creator + "  →  LibraryViewModel.select(" + root.mediaId + ")"
    border.color: LibraryViewModel.selectedId === root.mediaId ? Theme.accent : Theme.border

    // TODO: 换成 ItemDelegate：CoverArt + 标题（高亮）+ 创作者 + 年份
    TapHandler {
        onTapped: LibraryViewModel.select(root.mediaId)
    }
}

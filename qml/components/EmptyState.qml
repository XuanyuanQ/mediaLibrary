// 空状态提示：NoData（库为空 / 加载失败）或 NoMatch（搜索、筛选无结果）
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

Placeholder {
    id: root

    name: "EmptyState"
    hint: "LibraryViewModel.emptyState"

    // TODO: 按 emptyState 显示一行提示文字（不加按钮）
}

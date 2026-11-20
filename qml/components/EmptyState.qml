// 空状态提示：NoData（库为空 / 加载失败）或 NoMatch（搜索、筛选无结果）
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

Placeholder {
    id: root

    required property LibraryViewModel libraryVm

    name: "EmptyState"
    hint: "libraryVm.emptyState → clearFilters()"

    // TODO: 按 emptyState 显示不同文案；NoMatch 时提供“清除搜索和筛选”按钮
}

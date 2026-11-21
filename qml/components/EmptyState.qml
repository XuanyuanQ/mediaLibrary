// 空状态提示：列表里一行都没有时，在列表中间显示两行文字（原因 + 计数）。
// 显示 / 隐藏由 LibraryPane 控制（emptyState 不是 None 时显示）
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

Column {
    spacing: Theme.dp(6)

    // emptyState 由 C++ 的 LibraryViewModel::publishCounts() 算好后下发：
    //   NoMatch：有数据，但搜索 / 筛选后没有匹配项
    //   NoData： 媒体库本身是空的（或 media.json 加载失败）
    // 每次结果行数变化，C++ 都会发出 emptyStateChanged，这里的文字自动更新
    Label {
        anchors.horizontalCenter: parent.horizontalCenter
        text: LibraryData.emptyState === LibraryData.NoMatch
              ? qsTr("No matching results")
              : qsTr("No media in the library")
        color: Theme.textSecondary
        font.pixelSize: Theme.sp(15)
    }

    // 计数：例如 “0 of 24 items”，说明库里其实有 24 个，只是当前搜索 / 筛选一个都没匹配到
    Label {
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("%1 of %2 items").arg(LibraryData.resultCount).arg(LibraryData.totalCount)
        color: Theme.textSecondary
        font.pixelSize: Theme.sp(13)
    }
}

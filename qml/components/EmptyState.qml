// 空状态提示：列表里一行都没有时，在列表中间显示一行文字。
// 显示 / 隐藏由 LibraryPane 控制（emptyState 不是 None 时显示）
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

Label {
    // emptyState 由 C++ 的 getEmptyState() 算出：
    //   NoMatch：有数据，但搜索 / 筛选后没有匹配项
    //   NoData： 媒体库本身是空的（或 media.json 加载失败）
    // 每次结果行数变化，C++ 都会发出 emptyStateChanged，这里的文字自动更新
    text: LibraryViewModel.emptyState === LibraryViewModel.NoMatch
          ? qsTr("No matching results")
          : qsTr("No media in the library")
    color: Theme.textSecondary
    font.pixelSize: Theme.sp(15)
}

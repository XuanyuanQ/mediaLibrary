// 媒体列表：绑定 LibraryViewModel.items（过滤后的代理模型），每一行用 MediaRow 显示
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

ListView {
    id: list

    clip: true                                  // 滚动时内容不超出列表区域
    spacing: Theme.dp(6)
    boundsBehavior: Flickable.StopAtBounds      // 滚到头不回弹，更像桌面/车机列表
    model: LibraryViewModel.items

    // ListView 为模型里的每一行创建一个 MediaRow，
    // 并按 roleNames() 自动给它的 required property 赋值（title、creator …）
    delegate: MediaRow {
        width: ListView.view.width
    }

    ScrollBar.vertical: ScrollBar { }
}

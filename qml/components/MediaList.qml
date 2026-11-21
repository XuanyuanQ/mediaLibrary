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
    // 数据来源：过滤后的代理模型（MediaFilterModel）。
    // 设置 model 的这一刻，Qt 内部自动完成（看不到显式代码）：
    //   - connect 模型的 modelReset / rowsInserted / rowsRemoved / dataChanged 等信号，
    //     搜索时代理发出 rowsRemoved，ListView 就自动删掉对应的行
    //   - 调用 roleNames() 记下字段名，调用 rowCount() 得知一共几行
    //   - 根据滚动位置、列表高度和行高，算出屏幕上能看到第几行到第几行，
    //     只为这些行创建 MediaRow（看不见的行不创建，滚动时再按需创建/销毁）
    model: LibraryViewModel.items

    // ListView 为每个可见行创建一个 MediaRow，并对每个字段调用一次模型的 data()，
    // 把结果自动填进 MediaRow 里和 roleNames() 同名的 required property（title、creator …），
    // 以及 ListView 自带的 index（第几行）
    delegate: MediaRow {
        width: ListView.view.width
    }

    // 列表末尾：显示“结果数 / 总数”，滚到最下面才能看到。
    // footer 是 ListView 自带的属性：放在最后一行之后，跟着列表一起滚动。
    // 没有结果时不显示（那时由 EmptyState 在列表中间显示计数）
    footer: Label {
        width: ListView.view.width
        height: LibraryViewModel.resultCount > 0 ? implicitHeight : 0
        visible: LibraryViewModel.resultCount > 0
        topPadding: Theme.dp(12)
        bottomPadding: Theme.dp(12)
        horizontalAlignment: Text.AlignHCenter
        // resultCount / totalCount 都是 C++ 的 Q_PROPERTY，搜索、筛选后自动更新
        text: qsTr("%1 of %2 items").arg(LibraryViewModel.resultCount).arg(LibraryViewModel.totalCount)
        color: Theme.textSecondary
        font.pixelSize: Theme.sp(13)
    }

    ScrollBar.vertical: ScrollBar { }
}

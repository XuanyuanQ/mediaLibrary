// 窄屏布局（手机竖屏）：StackView 两级页面，列表 -> 详情。
// 页面栈深度是纯视觉状态，留在 View；选中项本身在 LibraryViewModel 单例里。
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

StackView {
    id: stack

    initialItem: libraryPage

    // TODO: 监听 LibraryViewModel.selectedIdChanged：选中后 push(detailPage)
    // TODO: 返回时调用 LibraryViewModel.clearSelection() 并 pop()
    // TODO: 处理 Android 返回键

    Component {
        id: libraryPage
        LibraryPane { }
    }

    Component {
        id: detailPage
        DetailView { }
    }
}

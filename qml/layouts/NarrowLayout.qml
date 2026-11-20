// 窄屏布局（手机竖屏）：StackView 两级页面，列表 -> 详情。
// 页面栈深度是纯视觉状态，留在 View；选中项本身在 libraryVm 里。
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

StackView {
    id: stack

    required property LibraryViewModel libraryVm
    required property LanguageManager language

    initialItem: libraryPage

    // TODO: 监听 libraryVm.selectedIdChanged：选中后 push(detailPage)
    // TODO: 返回时调用 libraryVm.clearSelection() 并 pop()
    // TODO: 处理 Android 返回键

    Component {
        id: libraryPage
        LibraryPane {
            libraryVm: stack.libraryVm
            language: stack.language
        }
    }

    Component {
        id: detailPage
        DetailView {
            libraryVm: stack.libraryVm
        }
    }
}

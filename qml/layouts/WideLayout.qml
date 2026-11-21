// 宽屏布局（车机横屏、桌面）：左列表（约 42%）+ 右详情
import QtQuick
import QtQuick.Layouts
import MediaLibrary

RowLayout {
    id: root

    spacing: 0

    LibraryPane {
        Layout.fillHeight: true
        Layout.preferredWidth: root.width * 0.42
    }

    // 分隔线
    Rectangle {
        Layout.fillHeight: true
        implicitWidth: 1
        color: Theme.border
    }

    DetailView {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}

// 详情：封面、类型、标题、创作者、年份 / 时长、简介。
// 只绑定 libraryVm.selectedItem，显示文字都已在 ViewModel 里格式化好。
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MediaLibrary
import MediaLibrary.Core

Item {
    id: root

    required property LibraryViewModel libraryVm

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.dp(16)
        spacing: Theme.dp(10)

        CoverArt {
            Layout.preferredWidth: Theme.dp(160)
            Layout.preferredHeight: Theme.dp(160)
        }

        Placeholder {
            Layout.fillWidth: true
            Layout.fillHeight: true
            name: "DetailView"
            hint: root.libraryVm.selectedId < 0
                  ? "libraryVm.selectedItem（未选中）"
                  : "libraryVm.selectedItem · id = " + root.libraryVm.selectedId
        }
    }

    // TODO: 实现详情内容；窄屏下显示“返回”按钮
}

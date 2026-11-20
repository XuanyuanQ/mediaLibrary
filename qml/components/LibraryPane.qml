// 列表面板：标题栏 + 语言切换、搜索框 + 类型下拉、计数、列表、空状态。
// 宽屏和窄屏布局复用同一份。
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MediaLibrary
import MediaLibrary.Core

Item {
    id: root

    required property LibraryViewModel libraryVm
    required property LanguageManager language

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.dp(16)
        spacing: Theme.dp(10)

        // 标题栏
        RowLayout {
            Layout.fillWidth: true
            Label {
                Layout.fillWidth: true
                text: qsTr("Media library")
                color: Theme.textPrimary
                font.pixelSize: Theme.sp(28)
                font.bold: true
            }
            LanguageSwitch {
                language: root.language
            }
        }

        // 搜索 + 筛选
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.dp(8)
            SearchField {
                Layout.fillWidth: true
                libraryVm: root.libraryVm
            }
            TypeFilter {
                libraryVm: root.libraryVm
            }
        }

        // 计数
        Placeholder {
            Layout.fillWidth: true
            name: "Count"
            hint: "libraryVm.resultCount / totalCount = "
                  + root.libraryVm.resultCount + " / " + root.libraryVm.totalCount
        }

        // 列表 + 空状态（叠放）
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            MediaList {
                anchors.fill: parent
                libraryVm: root.libraryVm
            }
            EmptyState {
                anchors.centerIn: parent
                libraryVm: root.libraryVm
                visible: root.libraryVm.emptyState !== LibraryViewModel.None
            }
        }
    }
}

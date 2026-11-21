// 列表面板：标题栏 + 语言切换、搜索框 + 类型下拉、列表（末尾显示计数）、空状态。
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MediaLibrary
import MediaLibrary.Core

Item {
    id: root

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
                elide: Text.ElideRight
            }
            LanguageSwitch { }
        }

        // 搜索 + 筛选
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.dp(8)
            SearchField {
                Layout.fillWidth: true
            }
            TypeFilter { }
        }

        // 列表 + 空状态（叠放）
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            MediaList {
                anchors.fill: parent
            }
            EmptyState {
                anchors.centerIn: parent
                visible: LibraryData.emptyState !== LibraryData.None
            }
        }
    }
}

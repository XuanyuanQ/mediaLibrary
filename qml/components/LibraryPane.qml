// Left pane: title, language switch, search, type filter, list and empty state.
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

        // Header
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

        // Search and filter
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.dp(8)
            SearchField {
                Layout.fillWidth: true
            }
            TypeFilter { }
        }

        // List with the empty-state message on top
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            MediaList {
                anchors.fill: parent
            }
            EmptyState {
                anchors.centerIn: parent
                visible: LibraryData.toUiEmptyState !== LibraryData.None
            }
        }
    }
}

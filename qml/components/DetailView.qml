// Details of the selected item. All texts are prepared in C++ (LibraryManager).
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MediaLibrary
import MediaLibrary.Core

Item {
    id: root

    // QVariantMap from C++, empty when nothing is selected
    readonly property var item: LibraryData.toUiSelectedItem
    readonly property bool hasItem: item.title !== undefined

    Label {
        anchors.centerIn: parent
        visible: !root.hasItem
        text: qsTr("Select an item to see its details")
        color: Theme.textSecondary
        font.pixelSize: Theme.sp(15)
    }

    // Scrollable, the description can be long
    Flickable {
        anchors.fill: parent
        visible: root.hasItem
        clip: true
        contentHeight: content.implicitHeight + Theme.dp(48)
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar { }

        ColumnLayout {
            id: content
            x: Theme.dp(24)
            y: Theme.dp(24)
            width: parent.width - Theme.dp(48)
            spacing: Theme.dp(8)

            Label {
                text: root.item.typeLabel ?? ""
                color: Theme.accent
                font.family: Theme.monoFont
                font.pixelSize: Theme.sp(13)
            }

            Label {
                Layout.fillWidth: true
                text: root.item.title ?? ""
                textFormat: Text.PlainText
                wrapMode: Text.Wrap
                color: Theme.textPrimary
                font.pixelSize: Theme.sp(30)
                font.bold: true
            }

            Label {
                Layout.fillWidth: true
                text: root.item.creator ?? ""
                textFormat: Text.PlainText
                wrapMode: Text.Wrap
                color: Theme.textSecondary
                font.pixelSize: Theme.sp(17)
            }

            Label {
                Layout.fillWidth: true
                visible: text.length > 0
                text: root.item.metaText ?? ""
                color: Theme.textSecondary
                font.family: Theme.monoFont
                font.pixelSize: Theme.sp(13)
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.topMargin: Theme.dp(12)
                Layout.bottomMargin: Theme.dp(12)
                implicitHeight: 1
                color: Theme.border
            }

            Label {
                Layout.fillWidth: true
                text: root.item.description ?? ""
                textFormat: Text.PlainText
                wrapMode: Text.Wrap
                lineHeight: 1.4
                color: Theme.textPrimary
                font.pixelSize: Theme.sp(15)
            }
        }
    }
}

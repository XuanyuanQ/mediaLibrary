// One list row: number, title, creator and year
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MediaLibrary
import MediaLibrary.Core

ItemDelegate {
    id: root

    // Set by ListView: row index and role values.
    required property int index

    // Names must match roleNames() in MediaLibraryModel.
    required property int mediaId
    required property string title
    required property string creator
    required property string type
    required property int year

    readonly property bool selected: LibraryData.toUiSelectedId === root.mediaId

    padding: Theme.dp(12)

    background: Rectangle {
        radius: Theme.dp(8)
        color: root.selected || root.down ? Theme.surfaceRaised : Theme.surface
        border.width: 1
        border.color: root.selected ? Theme.accent : Theme.border
    }

    contentItem: RowLayout {
        spacing: Theme.dp(12)

        // Row number, right-aligned so one- and two-digit numbers line up
        Label {
            Layout.preferredWidth: Theme.dp(24)
            text: root.index + 1
            horizontalAlignment: Text.AlignRight
            color: Theme.textSecondary
            font.family: Theme.monoFont
            font.pixelSize: Theme.sp(13)
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: Theme.dp(2)

            Label {
                Layout.fillWidth: true
                text: root.title
                textFormat: Text.PlainText    // show "<Code>" as text, not markup
                elide: Text.ElideRight
                color: Theme.textPrimary
                font.pixelSize: Theme.sp(15)
                font.bold: true
            }
            Label {
                Layout.fillWidth: true
                text: root.creator
                textFormat: Text.PlainText
                elide: Text.ElideRight
                color: Theme.textSecondary
                font.pixelSize: Theme.sp(13)
            }
        }

        Label {
            text: root.year > 0 ? root.year : ""
            color: Theme.textSecondary
            font.pixelSize: Theme.sp(13)
        }
    }

    onClicked: UiEvents.post(FromUi.ItemClicked, root.mediaId)

    Accessible.name: root.title + ", " + root.creator
}

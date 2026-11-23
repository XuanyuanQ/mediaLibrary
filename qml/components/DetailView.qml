// Details of the selected item. C++ sends the data; the texts are made and translated here.
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

    // Type name for each type in the data; unknown types are shown as-is
    readonly property var typeNames: ({
        "Movie":     qsTr("Movie"),
        "Music":     qsTr("Music"),
        "Ebook":     qsTr("Book"),
        "Podcast":   qsTr("Podcast"),
        "Audiobook": qsTr("Audiobook"),
    })

    // e.g. "2021 · 45 min" or "2019 · 320 pages"
    function metaText(item) {
        const parts = []
        if (item.year > 0)
            parts.push(item.year)
        if (item.durationSec > 0) {
            const minutes = Math.round(item.durationSec / 60)
            parts.push(minutes >= 60 ? qsTr("%1 h %2 min").arg(Math.floor(minutes / 60)).arg(minutes % 60)
                                     : qsTr("%1 min").arg(minutes))
        }
        if (item.pages > 0)
            parts.push(qsTr("%1 pages").arg(item.pages))
        return parts.join(" · ")
    }

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
                text: root.typeNames[root.item.type] ?? root.item.type ?? ""
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
                text: root.metaText(root.item)
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

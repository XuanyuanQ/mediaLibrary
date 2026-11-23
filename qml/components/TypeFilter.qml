// Type filter dropdown. Turns orange while a type (not "All types") is selected.
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

ComboBox {
    id: root

    readonly property bool active: LibraryData.toUiCategory !== ""

    // Options: show label, send key. The key matches the data's type ("" = all,
    // "Others" = types not listed here).
    model: [
        { key: "",        label: qsTr("All types") },
        { key: "Movie",   label: qsTr("Movies") },
        { key: "Music",   label: qsTr("Music") },
        { key: "Ebook",   label: qsTr("Books") },
        { key: "Podcast", label: qsTr("Podcasts") },
        { key: "Others",  label: qsTr("Others") },
    ]
    textRole: "label"
    valueRole: "key"

    // onActivated fires only on user selection
    onActivated: UiEvents.post(FromUi.TypeSelected, currentValue)

    function syncCurrentIndex() { currentIndex = indexOfValue(LibraryData.toUiCategory) }

    Component.onCompleted: syncCurrentIndex()

    // After a language change the labels are re-made, so the model changes and
    // ComboBox resets to index 0. Select the current category again.
    onModelChanged: Qt.callLater(syncCurrentIndex)

    implicitWidth: Theme.dp(140)
    implicitHeight: Theme.dp(40)
    font.pixelSize: Theme.sp(13)

    contentItem: Text {
        leftPadding: Theme.dp(12)
        rightPadding: root.indicator.width + Theme.dp(16)
        text: root.displayText
        font: root.font
        color: root.active ? Theme.accent : Theme.textPrimary
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    indicator: Text {
        x: root.width - width - Theme.dp(12)
        y: (root.height - height) / 2
        text: root.popup.visible ? "▴" : "▾"
        font.pixelSize: Theme.sp(12)
        color: root.active ? Theme.accent : Theme.textSecondary
    }

    background: Rectangle {
        radius: Theme.dp(8)
        color: Theme.surface
        border.width: 1
        border.color: root.active || root.popup.visible ? Theme.accent : Theme.border
    }

    // One option in the popup
    delegate: ItemDelegate {
        id: option

        required property var model
        required property int index

        readonly property bool selected: model[root.valueRole] === LibraryData.toUiCategory

        width: root.popup.availableWidth
        height: Theme.dp(36)
        highlighted: root.highlightedIndex === index   // hovered or keyboard-focused

        contentItem: Item {
            Text {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: option.model[root.textRole]
                font: root.font
                color: option.selected ? Theme.accent : Theme.textPrimary
            }
            Text {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                visible: option.selected
                text: "✓"
                font: root.font
                color: Theme.accent
            }
        }

        background: Rectangle {
            radius: Theme.dp(6)
            color: option.highlighted ? Theme.surfaceRaised : "transparent"
        }
    }

    popup: Popup {
        y: root.height + Theme.dp(4)
        width: root.width
        padding: Theme.dp(4)

        contentItem: ListView {
            implicitHeight: contentHeight
            clip: true
            model: root.popup.visible ? root.delegateModel : null
            currentIndex: root.highlightedIndex
        }

        background: Rectangle {
            radius: Theme.dp(8)
            color: Theme.surfaceRaised
            border.width: 1
            border.color: Theme.border
        }
    }
}

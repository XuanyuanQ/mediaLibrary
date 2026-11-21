// Filtered media list, one MediaRow per item
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

ListView {
    id: list

    clip: true
    spacing: Theme.dp(6)
    boundsBehavior: Flickable.StopAtBounds      // no bounce at the ends
    // The filter model. ListView connects to its row signals itself
    // and only creates delegates for visible rows.
    model: LibraryData.toUiItems

    // Role values fill the required properties of MediaRow
    delegate: MediaRow {
        width: ListView.view.width
    }

    // Count below the last row. Hidden when empty (EmptyState shows it then).
    footer: Label {
        width: ListView.view.width
        height: LibraryData.toUiResultCount > 0 ? implicitHeight : 0
        visible: LibraryData.toUiResultCount > 0
        topPadding: Theme.dp(12)
        bottomPadding: Theme.dp(12)
        horizontalAlignment: Text.AlignHCenter
        text: qsTr("%1 of %2 items").arg(LibraryData.toUiResultCount).arg(LibraryData.toUiTotalCount)
        color: Theme.textSecondary
        font.pixelSize: Theme.sp(13)
    }

    ScrollBar.vertical: ScrollBar { }
}

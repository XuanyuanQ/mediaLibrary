// Shown in the middle of the list when it is empty: reason and count.
// Visibility is controlled by LibraryPane.
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

Column {
    spacing: Theme.dp(6)

    // NoMatch: library has data but nothing matches. NoData: library is empty.
    Label {
        anchors.horizontalCenter: parent.horizontalCenter
        text: LibraryData.toUiEmptyState === LibraryData.NoMatch
              ? qsTr("No matching results")
              : qsTr("No media in the library")
        color: Theme.textSecondary
        font.pixelSize: Theme.sp(15)
    }

    // e.g. "0 of 24 items": the library has data, the search just found nothing
    Label {
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("%1 of %2 items").arg(LibraryData.toUiResultCount).arg(LibraryData.toUiTotalCount)
        color: Theme.textSecondary
        font.pixelSize: Theme.sp(13)
    }
}

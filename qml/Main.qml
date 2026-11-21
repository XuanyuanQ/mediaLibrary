// Main window: landscape only, list on the left, details on the right.
// No app state here: it lives in C++ and is read through LibraryData.
import QtQuick
import QtQuick.Controls
import MediaLibrary

ApplicationWindow {
    id: window

    width: 1280
    height: 720
    minimumWidth: 900      // narrower does not fit two columns
    minimumHeight: 480
    visible: true
    title: qsTr("Media Library")
    color: Theme.background

    WideLayout {
        anchors.fill: parent
    }
}

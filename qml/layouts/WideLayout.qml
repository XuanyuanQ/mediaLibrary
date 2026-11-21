// List (42%) on the left, details on the right
import QtQuick
import QtQuick.Layouts
import MediaLibrary

RowLayout {
    id: root

    spacing: 0

    LibraryPane {
        Layout.fillHeight: true
        Layout.preferredWidth: root.width * 0.42
    }

    // Divider
    Rectangle {
        Layout.fillHeight: true
        implicitWidth: 1
        color: Theme.border
    }

    DetailView {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}

// 媒体列表：绑定 LibraryViewModel.items（过滤后的代理模型）
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

Placeholder {
    id: root

    name: "MediaList (ListView)"
    hint: "model: LibraryViewModel.items · 行数 " + list.count

    ListView {
        id: list
        anchors.fill: parent
        anchors.topMargin: Theme.dp(48)
        anchors.margins: Theme.dp(8)
        clip: true
        spacing: Theme.dp(6)
        model: LibraryViewModel.items

        delegate: MediaRow {
            width: ListView.view.width
        }
    }
}

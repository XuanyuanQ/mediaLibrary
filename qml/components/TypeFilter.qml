// 类型筛选下拉框（带 ▾），选中某个类型后高亮显示
import QtQuick
import QtQuick.Controls
import MediaLibrary
import MediaLibrary.Core

Placeholder {
    id: root

    name: "TypeFilter ▾"
    hint: "LibraryViewModel.categories → category"

    // TODO: 换成 ComboBox：model = LibraryViewModel.categories，选中后写入 LibraryViewModel.category
}

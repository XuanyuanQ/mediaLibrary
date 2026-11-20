// 封面占位：按类型生成色块 + 图标（mock 数据没有图片）
import QtQuick
import MediaLibrary

Placeholder {
    id: root

    property string type

    name: "CoverArt"
    hint: "Theme.typeColor / typeGlyph"

    // TODO: 类型色底 + 居中图标
}

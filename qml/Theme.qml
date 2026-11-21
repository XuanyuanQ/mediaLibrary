// 全局主题（单例）：颜色、字体、尺寸缩放都集中在这里。
// 风格：深色编辑风（接近纯黑底、白色粗体标题、等宽大写小标签、橙色强调）。
pragma Singleton
import QtQuick

QtObject {
    // ---------- 颜色 ----------
    readonly property color background: "#0E0E0F"
    readonly property color surface: "#18181B"        // 面板
    readonly property color surfaceRaised: "#222226"  // 选中行、弹出层
    readonly property color border: "#2C2C30"
    readonly property color textPrimary: "#F4F4F2"
    readonly property color textSecondary: "#9A9AA0"
    readonly property color accent: "#FF5A1F"         // 橙色强调

    // ---------- 字体 ----------
    readonly property string monoFont: Qt.platform.os === "windows" ? "Consolas" : "monospace"

    // ---------- 分辨率适配 ----------
    // 缩放系数，由 Main.qml 根据窗口尺寸设置（见 ARCHITECTURE.md 第 8 节）
    property real scale: 1.0

    // 字号和尺寸统一通过这两个函数换算，不直接写死像素值
    function sp(px) { return Math.round(px * scale) }   // 字号
    function dp(px) { return Math.round(px * scale) }   // 间距、尺寸

    // TODO: 按类型返回颜色和图标
    function typeColor(type) { return surfaceRaised }
    function typeGlyph(type) { return "•" }
}

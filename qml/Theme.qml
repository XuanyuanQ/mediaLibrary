// 全局主题（单例）：颜色、字体、尺寸都集中在这里。
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

    // ---------- 尺寸 ----------
    // 字号和尺寸统一通过这两个函数写（目前按固定像素，不随窗口缩放）。
    // 以后要做分辨率适配，只需在这里乘一个缩放系数，所有组件不用改
    function sp(px) { return px }   // 字号
    function dp(px) { return px }   // 间距、尺寸
}

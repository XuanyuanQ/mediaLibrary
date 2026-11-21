// Global theme (singleton): colors, fonts and sizes.
pragma Singleton
import QtQuick

QtObject {
    // Colors
    readonly property color background: "#0E0E0F"
    readonly property color surface: "#18181B"        // panels
    readonly property color surfaceRaised: "#222226"  // selected row, popups
    readonly property color border: "#2C2C30"
    readonly property color textPrimary: "#F4F4F2"
    readonly property color textSecondary: "#9A9AA0"
    readonly property color accent: "#FF5A1F"         // orange

    // Fonts
    readonly property string monoFont: Qt.platform.os === "windows" ? "Consolas" : "monospace"

    // Sizes
    // All sizes go through these. Fixed pixels for now;
    // scaling for other resolutions can be added here only.
    function sp(px) { return px }   // font size
    function dp(px) { return px }   // spacing, size
}

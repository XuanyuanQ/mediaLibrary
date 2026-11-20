.pragma library
// 只放纯显示用的工具函数（不含业务逻辑；业务逻辑都在 C++ ViewModel 里）。
// 第一行的 .pragma library 表示整个程序只加载一份，所有 QML 文件共享。

// 转义 HTML 特殊字符，防止标题里的 "<" "&" 被当成标签
function escapeHtml(s) {
    // TODO
    return String(s)
}

// 把命中搜索关键字的部分加上高亮标签，返回 StyledText
function highlight(text, query) {
    // TODO
    return escapeHtml(text)
}

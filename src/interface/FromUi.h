#pragma once

#include <QObject>

/*
 * 【上发消息表】UI（QML）→ C++ 的全部消息，名字统一在这里定义。
 *
 * 和下发消息表 ToUi.h 对称，从名字就能看出方向：
 *   上发：UiEvents.post(FromUi.ItemClicked, id)              FromUi = 从 UI 发出
 *   下发：m_data.publish(ToUi::SelectedItem, detail)         ToUi   = 发往 UI
 *
 * 规则：
 *   - QML 只通过 UiEvents.post(FromUi.xxx, 数据) 报告“发生了什么”，不直接调用任何业务逻辑
 *   - 按模块分段：媒体库 100~199，语言 200~299，以后的模块继续往后排
 *   - 每个模块的 handleUiEvent() 只处理本模块那一段，其他的直接忽略
 *   - 注释里写明 data 的类型，处理函数会检查，类型不对打警告并丢弃
 *
 * main.cpp 用 qmlRegisterUncreatableMetaObject 把这个命名空间交给 QML，QML 里写 FromUi.ItemClicked。
 */
namespace FromUi {
Q_NAMESPACE   // 让 QML 能用 FromUi.xxx，也让 qDebug 能打印出名字（例如 FromUi::ItemClicked）

enum Message {
    // ---- 媒体库（100~199）：LibraryManager 处理 ----
    SearchEdited = 100,     // data: QString 搜索框文字（输入、✕ 清空）          ← SearchField.qml
    TypeSelected,           // data: QString 类型 key（""、"Movie"、"Others" …）  ← TypeFilter.qml
    ItemClicked,            // data: int     被点击那一项的 ID                    ← MediaRow.qml

    // ---- 语言（200~299）：LanguageManager 处理 ----
    LanguageSelected = 200, // data: QString 语言代码（"en"、"sv"）              ← LanguageSwitch.qml
};
Q_ENUM_NS(Message)

// 模块编号 = 消息编号 / 100
enum Module {
    Library = 1,
    Language = 2,
};
Q_ENUM_NS(Module)

// 这条消息属于哪个模块。各模块的 handleUiEvent() 用它过滤掉不属于自己的消息
inline Module moduleOf(Message message) { return Module(int(message) / 100); }

} // namespace FromUi

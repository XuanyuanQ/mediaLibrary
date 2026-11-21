#pragma once

#include <QObject>

/*
 * 【下发消息表】C++ → UI（QML）的全部下发数据，名字统一在这里定义。
 *
 * 和上发消息表 FromUi.h 对称，从名字就能看出方向：
 *   上发：UiEvents.post(FromUi.ItemClicked, id)              FromUi = 从 UI 发出
 *   下发：m_data.publish(ToUi::SelectedItem, detail)         ToUi   = 发往 UI
 *
 * 规则：
 *   - 用到的下发数据必须先在这里登记，没登记的名字 publish 时编译就报错
 *   - 每个名字要在对应的数据类里用 UI_PROPERTY(类型, 名字) 定义成属性，名字必须一模一样
 *     （宏里写 ToUi::名字 去登记，这里没有就编译失败）
 *   - QML 里的属性名 = "toUi" + 名字：ResultCount → LibraryData.toUiResultCount
 *   - 按模块分段，和 FromUi 的编号规则一致：媒体库 100~199，语言 200~299
 */
namespace ToUi {
Q_NAMESPACE   // 让 qDebug 能打印出名字（例如 ToUi::ResultCount），方便看下发日志

enum Message {
    // ---- 媒体库（100~199）：LibraryManager 下发到 LibraryData ----
    SearchText = 100,   // QString       搜索框文字
    Category,           // QString       当前类型 key
    Categories,         // QVariantList  下拉框选项 [{key, label}]
    ResultCount,        // int           过滤后的条数
    TotalCount,         // int           全部条数
    EmptyState,         // LibraryData::EmptyState  列表为空的原因
    SelectedId,         // int           当前选中项 ID，-1 = 未选中
    SelectedItem,       // QVariantMap   详情 {title, creator, typeLabel, metaText, description}

    // ---- 语言（200~299）：LanguageManager 下发到 LanguageData ----
    Language = 200,     // QString       当前语言代码："en"、"sv"
    AvailableLanguages, // QVariantList  可选语言 [{code, label}]
};
Q_ENUM_NS(Message)

} // namespace ToUi

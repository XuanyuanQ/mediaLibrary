#pragma once

#include <QObject>
#include <QVariant>

/*
 * 【上发接口 / 消息总线】界面（QML）→ C++ 的唯一入口。
 *
 * 规则：
 *   - QML 只通过 post(类型, 数据) 报告“发生了什么”，不直接调用任何业务逻辑
 *   - 消息类型按模块分段：媒体库 100~199，语言 200~299，以后的模块继续往后排
 *   - 每个模块有自己的 handleUiEvent(type, data)，只处理本模块的消息，其他的直接忽略
 *   - main.cpp 里每个模块 connect 一次（connect 的数量 = 模块数，不是消息数）；
 *     加新消息只改枚举和对应模块的 handleUiEvent，main.cpp 不用动
 *   - ViewModel / Service 不对 QML 开放写入（没有 WRITE / Q_INVOKABLE），QML 想改数据只能走这里
 *
 * 用法：main.cpp 用 qmlRegisterSingletonInstance 注册成单例，QML 里：
 *       UiEvents.post(UiEvents.ItemClicked, mediaId)
 *       （QML 可以像调用函数一样调用信号，调用一次就发出一次信号）
 *
 * 只有信号和枚举，没有实现代码，所以没有 .cpp 文件。
 */
class UiEvents : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    // 模块编号 = 消息类型 / 100
    enum Module {
        Library = 1,
        Language = 2,
    };
    Q_ENUM(Module)

    // 消息类型。注释里写明 data 的类型，处理函数会检查，类型不对打警告
    enum Type {
        // ---- 媒体库（100~199），LibraryViewModel 处理 ----
        SearchEdited = 100,   // data: QString 搜索框文字（输入、✕ 清空）     ← SearchField.qml
        TypeSelected,         // data: QString 类型 key（""、"Movie"、"Others" …）  ← TypeFilter.qml
        ItemClicked,          // data: int     被点击那一项的 ID                    ← MediaRow.qml

        // ---- 语言（200~299），LanguageManager 处理 ----
        LanguageSelected = 200,   // data: QString 语言代码（"en"、"zh_CN"、"sv"）       ← LanguageSwitch.qml
    };
    Q_ENUM(Type)

    // 这条消息属于哪个模块。各模块的 handleUiEvent 用它过滤掉不属于自己的消息
    static Module moduleOf(Type type) { return Module(int(type) / 100); }

signals:
    // 唯一的上发信号
    void post(UiEvents::Type type, const QVariant &data);
};

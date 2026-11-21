#pragma once

#include <QObject>
#include <QVariant>

#include "FromUi.h"

/*
 * 【上发接口 / 消息总线】UI（QML）→ C++ 的唯一入口。
 *
 *   QML：UiEvents.post(FromUi.ItemClicked, mediaId)
 *
 * 消息名字定义在 FromUi.h；这里只有一个信号。
 *   - main.cpp 里每个模块 connect 一次（connect 的数量 = 模块数，不是消息数），
 *     各模块的 handleUiEvent() 只处理本模块那一段消息
 *   - 加新消息只改 FromUi.h 和对应模块的 handleUiEvent，main.cpp 不用动
 *   - 逻辑类不对 QML 开放（不注册、没有 Q_INVOKABLE），QML 想改数据只能走这里
 *
 * main.cpp 用 qmlRegisterSingletonInstance 注册成单例。QML 可以像调用函数一样调用信号，调用一次就发出一次信号。
 * 只有信号，没有实现代码，所以没有 .cpp 文件。
 */
class UiEvents : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

signals:
    // 唯一的上发信号
    void post(FromUi::Message message, const QVariant &data);
};

#pragma once

#include <QDebug>
#include <QObject>

#include "UiDataBase.h"

/*
 * 【下发属性宏】一行定义一个“下发给 QML 的只读属性”。参考车机 cluster 项目的 DATA_PROPERTY_DEFINE。
 * 只能用在继承 UiDataBase 的数据类（LibraryData、LanguageData）里。
 *
 *   UI_PROPERTY(int, resultCount, ResultCount)
 *
 * 参数：类型、QML 里用的属性名、首字母大写的名字（必须和 UiData.h 里登记的名字一样）。
 *
 * 展开后相当于手写了：
 *   Q_PROPERTY(int resultCount READ getResultCount NOTIFY resultCountChanged)   // QML 只读（没有 WRITE）
 *   int  getResultCount() const;               // QML 读取时调用；C++ 逻辑也可以用它读当前值
 *   void resultCountChanged();                 // 信号：QML 收到后，绑定了这个属性的地方自动刷新
 *   private: void setResultCount(int value);   // 私有：外部只能通过 publish(UiData::ResultCount, 值) 下发
 *   对象创建时把 UiData::ResultCount → setResultCount 登记到 UiDataBase 的表里
 *
 * UiData.h 里没有 ResultCount 这个名字 → 编译报错，保证所有下发数据都在一个文件里集中登记。
 *
 * 注意：宏要放在类里 Q_OBJECT 之后。moc 会展开这个宏，所以里面的 Q_PROPERTY 和 Q_SIGNAL 都能生效。
 */
#define UI_PROPERTY(Type, name, Name)                                               \
    Q_PROPERTY(Type name READ get##Name NOTIFY name##Changed)                       \
public:                                                                             \
    Type get##Name() const { return m_##name; }                                     \
    Q_SIGNAL void name##Changed();                                                  \
                                                                                    \
private:                                                                            \
    void set##Name(const Type &value)                                               \
    {                                                                               \
        if (m_##name == value)                                                      \
            return; /* 值没变不发信号，避免界面无意义地刷新 */                        \
        m_##name = value;                                                           \
        emit name##Changed();                                                       \
    }                                                                               \
    Type m_##name{};                                                                \
    /* 对象创建时登记 “UiData::Name → setter”，publish() 靠这张表找到 setter */      \
    bool m_registered##Name = registerSetter(UiData::Name, [this](const QVariant &v) { \
        if (!valueMatches<Type>(v)) {                                               \
            qCWarning(lcUiData) << "UiData::" #Name " expects " #Type ", got" << v; \
            return;                                                                 \
        }                                                                           \
        set##Name(valueAs<Type>(v));                                                \
    });

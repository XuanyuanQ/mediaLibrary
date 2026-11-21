#pragma once

#include <QHash>
#include <QLoggingCategory>
#include <QObject>
#include <QVariant>

#include <functional>
#include <type_traits>

#include "ToUi.h"

Q_DECLARE_LOGGING_CATEGORY(lcUiData)

/*
 * 【下发统一入口】所有数据类（LibraryData、LanguageData）的基类。
 *
 *   m_data.publish(ToUi::ResultCount, 5);
 *
 * 工作方式：
 *   - 数据类里每个 UI_PROPERTY 在对象创建时，自动把“名字 → setter”登记到 m_setters 表里
 *   - publish() 按名字查表，找到就调用 setter：检查类型 → 值变了才发出 xxxChanged → QML 绑定刷新
 *   - 名字不属于这个数据类（例如往 LibraryData 发 ToUi::Language）→ 打警告并丢弃
 *   - 值的类型不对（例如往 int 属性发字符串）→ 打警告并丢弃
 *
 * 为什么不用 QObject::setProperty("名字", 值)：
 *   - setProperty 要求属性带 WRITE，带了 WRITE 之后 QML 也能写这个属性，“QML 只读”就保证不了
 *   - 名字写错时 setProperty 会悄悄新建一个动态属性，不报错，界面也不刷新
 *
 * 查看全部下发日志：运行前设置环境变量 QT_LOGGING_RULES="ui.data.debug=true"
 */
class UiDataBase : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    // 唯一的下发入口
    void publish(ToUi::Message key, const QVariant &value);

protected:
    // 由 UI_PROPERTY 宏在对象创建时调用，返回值只是为了能写在成员初始化里
    bool registerSetter(ToUi::Message key, std::function<void(const QVariant &)> setter);

    // 检查下发的值和属性类型是否一致。数字类属性（int、枚举）接受任意数字，其他类型必须完全一致
    template<typename T>
    static bool valueMatches(const QVariant &value)
    {
        if (value.metaType() == QMetaType::fromType<T>())
            return true;
        if constexpr (std::is_arithmetic_v<T> || std::is_enum_v<T>) {
            switch (value.metaType().id()) {
            case QMetaType::Int: case QMetaType::UInt: case QMetaType::LongLong:
            case QMetaType::ULongLong: case QMetaType::Double: case QMetaType::Bool:
                return true;
            default:
                return false;
            }
        }
        return false;
    }

    // 从 QVariant 里取出属性类型的值（枚举先转成 int 再转换）
    template<typename T>
    static T valueAs(const QVariant &value)
    {
        if constexpr (std::is_enum_v<T>)
            return static_cast<T>(value.toInt());
        else
            return value.value<T>();
    }

private:
    QHash<int, std::function<void(const QVariant &)>> m_setters;   // 名字 → setter
};

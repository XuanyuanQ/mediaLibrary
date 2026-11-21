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
 * Base class of the data classes (LibraryData, LanguageData).
 * Single entry point for sending data to the UI:
 *
 *   m_data.publish(ToUi::ResultCount, 5);
 *
 * Each UI_PROPERTY registers "name -> setter" in a table; publish() looks it up,
 * checks the value type and calls the setter (which emits only if the value changed).
 * Wrong name or wrong type: warning, value dropped.
 *
 * Not using QObject::setProperty(): it needs a WRITE accessor (QML could then write too),
 * and a misspelled name silently creates a dynamic property.
 *
 * Log every publish: set QT_LOGGING_RULES="ui.data.debug=true".
 */
class UiDataBase : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    void publish(ToUi::Message key, const QVariant &value);

protected:
    // Called by UI_PROPERTY. Returns bool only so it can be used as a member initializer.
    bool registerSetter(ToUi::Message key, std::function<void(const QVariant &)> setter);

    // Numeric properties (int, enum) accept any number; other types must match exactly.
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

    template<typename T>
    static T valueAs(const QVariant &value)
    {
        if constexpr (std::is_enum_v<T>)
            return static_cast<T>(value.toInt());
        else
            return value.value<T>();
    }

private:
    QHash<int, std::function<void(const QVariant &)>> m_setters;   // message -> setter
};

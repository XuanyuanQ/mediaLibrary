#pragma once

#include <QDebug>
#include <QObject>

#include "UiDataBase.h"

/*
 * Declares one read-only property sent to the UI. Use inside a UiDataBase subclass, after Q_OBJECT.
 *
 *   UI_PROPERTY(int, ResultCount)
 *
 * Expands to:
 *   Q_PROPERTY(int toUiResultCount READ getResultCount NOTIFY toUiResultCountChanged)  // no WRITE
 *   int getResultCount() const;
 *   signal toUiResultCountChanged();
 *   private setResultCount()      // emits only when the value changes
 *   registration of ToUi::ResultCount -> setResultCount, used by publish()
 *
 * The name must exist in ToUi.h, otherwise it does not compile.
 */
#define UI_PROPERTY(Type, Name)                                                     \
    Q_PROPERTY(Type toUi##Name READ get##Name NOTIFY toUi##Name##Changed)           \
public:                                                                             \
    Type get##Name() const { return m_##Name; }                                     \
    Q_SIGNAL void toUi##Name##Changed();                                            \
                                                                                    \
private:                                                                            \
    void set##Name(const Type &value)                                               \
    {                                                                               \
        if (m_##Name == value)                                                      \
            return;                                                                 \
        m_##Name = value;                                                           \
        emit toUi##Name##Changed();                                                 \
    }                                                                               \
    Type m_##Name{};                                                                \
    bool m_registered##Name = registerSetter(ToUi::Name, [this](const QVariant &v) { \
        if (!valueMatches<Type>(v)) {                                               \
            qCWarning(lcUiData) << "ToUi::" #Name " expects " #Type ", got" << v;   \
            return;                                                                 \
        }                                                                           \
        set##Name(valueAs<Type>(v));                                                \
    });

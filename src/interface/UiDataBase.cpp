#include "UiDataBase.h"

#include <QDebug>

// Off by default. Enable with QT_LOGGING_RULES="ui.data.debug=true"
Q_LOGGING_CATEGORY(lcUiData, "ui.data", QtWarningMsg)

void UiDataBase::publish(ToUi::Message key, const QVariant &value)
{
    const auto it = m_setters.constFind(key);
    if (it == m_setters.constEnd()) {
        // The message exists, but belongs to another data class
        qCWarning(lcUiData) << metaObject()->className() << "has no property for" << key;
        return;
    }
    qCDebug(lcUiData) << key << value;
    (*it)(value);
}

bool UiDataBase::registerSetter(ToUi::Message key, std::function<void(const QVariant &)> setter)
{
    Q_ASSERT_X(!m_setters.contains(key), "UiDataBase", "duplicate ToUi message");
    m_setters.insert(key, std::move(setter));
    return true;
}

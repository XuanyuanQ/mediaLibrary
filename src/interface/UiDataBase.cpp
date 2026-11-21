#include "UiDataBase.h"

#include <QDebug>

// 下发日志分类，默认关闭。打开：QT_LOGGING_RULES="ui.data.debug=true"
Q_LOGGING_CATEGORY(lcUiData, "ui.data", QtWarningMsg)

void UiDataBase::publish(UiData::Key key, const QVariant &value)
{
    const auto it = m_setters.constFind(key);
    if (it == m_setters.constEnd()) {
        // 这个名字在 UiData.h 里有，但不属于当前数据类（发错了对象）
        qCWarning(lcUiData) << metaObject()->className() << "has no property for" << key;
        return;
    }
    qCDebug(lcUiData) << key << value;   // 统一的下发日志：C++ 给界面发了什么
    (*it)(value);
}

bool UiDataBase::registerSetter(UiData::Key key, std::function<void(const QVariant &)> setter)
{
    Q_ASSERT_X(!m_setters.contains(key), "UiDataBase", "duplicate UiData key");
    m_setters.insert(key, std::move(setter));
    return true;
}

#include "LanguageManager.h"

#include <QDebug>

#include <QQmlEngine>

// 骨架阶段：接口已定义，翻译加载待实现

LanguageManager::LanguageManager(QObject *parent)
    : QObject(parent)
{
    // 启动时下发初始数据。语言名称本身不翻译：用户在任何语言下都应该能认出自己的语言
    m_data.publish(UiData::Language, QStringLiteral("en"));
    m_data.publish(UiData::AvailableLanguages, QVariantList{
        QVariantMap{{QStringLiteral("code"), QStringLiteral("en")}, {QStringLiteral("label"), QStringLiteral("EN")}},
        QVariantMap{{QStringLiteral("code"), QStringLiteral("zh_CN")}, {QStringLiteral("label"), QStringLiteral("中")}},
    });
}

void LanguageManager::handleUiEvent(UiEvents::Type type, const QVariant &data)
{
    // 谁调用：QML 里 UiEvents.post(...) → main.cpp 的 connect → 这里。
    // 不是语言模块的消息直接忽略
    if (UiEvents::moduleOf(type) != UiEvents::Language)
        return;

    switch (type) {
    case UiEvents::LanguageSelected:
        if (data.typeId() != QMetaType::QString) {
            qWarning() << "UiEvents:" << type << "expects a language code, got" << data;
            return;
        }
        setLanguage(data.toString());
        break;

    default:
        qWarning() << "UiEvents: unhandled language message" << type;
        break;
    }
}

void LanguageManager::setLanguage(const QString &code)
{
    // TODO: 卸载旧翻译；非 "en" 时加载 :/i18n/medialibrary_<code>.qm 并安装；
    //       m_engine->retranslate()
    if (m_data.getLanguage() == code)
        return;
    m_data.publish(UiData::Language, code);   // 下发：当前语言
    emit languageChanged();     // 通知其他 C++ 模块（main.cpp 连到 LibraryViewModel::retranslate）
}

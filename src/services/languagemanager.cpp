#include "languagemanager.h"

#include <QQmlEngine>

// 骨架阶段：接口已定义，翻译加载待实现

LanguageManager::LanguageManager(QQmlEngine *engine, QObject *parent)
    : QObject(parent)
    , m_engine(engine)
{
}

QVariantList LanguageManager::availableLanguages() const
{
    // 语言名称本身不翻译：用户在任何语言下都应该能认出自己的语言
    return {
        QVariantMap{{QStringLiteral("code"), QStringLiteral("en")}, {QStringLiteral("label"), QStringLiteral("EN")}},
        QVariantMap{{QStringLiteral("code"), QStringLiteral("zh_CN")}, {QStringLiteral("label"), QStringLiteral("中")}},
    };
}

void LanguageManager::setLanguage(const QString &code)
{
    // TODO: 卸载旧翻译；非 "en" 时加载 :/i18n/medialibrary_<code>.qm 并安装；
    //       m_engine->retranslate()
    if (m_language == code)
        return;
    m_language = code;
    emit languageChanged();
}

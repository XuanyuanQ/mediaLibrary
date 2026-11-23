#include "LanguageManager.h"

#include <QCoreApplication>
#include <QDebug>
#include <QQmlEngine>

LanguageManager::LanguageManager(QObject *parent)
    : QObject(parent)
{
    // Language names are not translated, so users can always find their own
    m_data.publish(ToUi::Language, QStringLiteral("en"));
    m_data.publish(ToUi::AvailableLanguages, QVariantList{
        QVariantMap{{QStringLiteral("code"), QStringLiteral("en")}, {QStringLiteral("label"), QStringLiteral("EN")}},
        QVariantMap{{QStringLiteral("code"), QStringLiteral("sv")}, {QStringLiteral("label"), QStringLiteral("SV")}},
    });
}

void LanguageManager::handleUiEvent(FromUi::Message type, const QVariant &data)
{
    // Ignore messages for other modules
    if (FromUi::moduleOf(type) != FromUi::Language)
        return;

    switch (type) {
    case FromUi::LanguageSelected:
        if (data.typeId() != QMetaType::QString) {
            qWarning() << type << "expects a language code, got" << data;
            return;
        }
        setLanguage(data.toString());
        break;

    default:
        qWarning() << "unhandled language message" << type;
        break;
    }
}

void LanguageManager::setLanguage(const QString &code)
{
    if (m_data.getLanguage() == code)
        return;

    // English is the source language and needs no table
    QTranslator *next = nullptr;
    if (code != QLatin1String("en")) {
        next = translatorFor(code);
        if (!next)
            return;
    }

    // Swap tables (removing only uninstalls it; it stays loaded for next time)
    if (m_current)
        QCoreApplication::removeTranslator(m_current);
    if (next)
        QCoreApplication::installTranslator(next);
    m_current = next;

    // Re-evaluate every qsTr() in QML
    if (m_engine)
        m_engine->retranslate();

    m_data.publish(ToUi::Language, code);
}

QTranslator *LanguageManager::translatorFor(const QString &code)
{
    if (QTranslator *loaded = m_translators.value(code))
        return loaded;

    // First use: load the .qm built by qt_add_translations into :/i18n/
    auto *translator = new QTranslator(this);
    if (!translator->load(QStringLiteral(":/i18n/medialibrary_%1.qm").arg(code))) {
        qWarning() << "LanguageManager: no translation file for" << code;
        delete translator;
        return nullptr;
    }
    m_translators.insert(code, translator);
    return translator;
}

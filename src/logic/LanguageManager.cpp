#include "LanguageManager.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
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

    // English is the source language and needs no file.
    // Other .qm files are built by qt_add_translations into :/i18n/.
    const bool isSource = (code == QLatin1String("en"));
    const QString qmFile = QStringLiteral(":/i18n/medialibrary_%1.qm").arg(code);
    if (!isSource && !QFile::exists(qmFile)) {
        qWarning() << "LanguageManager: no translation file for" << code;
        return;
    }

    QCoreApplication::removeTranslator(&m_translator);
    if (!isSource && m_translator.load(qmFile))
        QCoreApplication::installTranslator(&m_translator);

    // Re-evaluate every qsTr() in QML
    if (m_engine)
        m_engine->retranslate();

    m_data.publish(ToUi::Language, code);

    // Texts built in C++ with tr() are refreshed by LibraryManager::retranslate
    emit languageChanged();
}

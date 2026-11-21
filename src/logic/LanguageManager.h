#pragma once

#include <QObject>
#include <QTranslator>
#include <QVariantList>

#include "LanguageData.h"
#include "UiEvents.h"

class QQmlEngine;

/*
 * Switches the UI language at runtime. Not visible to QML.
 *
 *   From UI: UiEvents.post(FromUi.LanguageSelected, code) -> handleUiEvent() -> setLanguage()
 *   To UI:   m_data.publish(ToUi::Language, code)
 *
 * Steps: remove old translator -> load :/i18n/medialibrary_<code>.qm -> install
 *        -> QQmlEngine::retranslate() -> emit languageChanged
 */
class LanguageManager : public QObject
{
    Q_OBJECT

public:
    explicit LanguageManager(QObject *parent = nullptr);

    // Set in main.cpp once the engine exists
    void setEngine(QQmlEngine *engine) { m_engine = engine; }

    // Registered as the QML singleton LanguageData in main.cpp
    LanguageData *data() { return &m_data; }

    // Connected to UiEvents::post in main.cpp. Handles language messages only (200-299).
    void handleUiEvent(FromUi::Message type, const QVariant &data);

    void setLanguage(const QString &code);

signals:
    // Emitted after a switch. main.cpp connects it to LibraryManager::retranslate.
    void languageChanged();

private:
    QQmlEngine *m_engine = nullptr;   // not owned
    QTranslator m_translator;
    LanguageData m_data;
};

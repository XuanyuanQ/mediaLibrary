#pragma once

#include <QHash>
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
 * Each language table (:/i18n/medialibrary_<code>.qm) is loaded the first time it is
 * used and then kept, so later switches only swap tables and do not load again.
 * Steps: remove current table -> install the new one
 *        -> QQmlEngine::retranslate() (all translated texts are qsTr() in QML)
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

private:
    // Loaded table for a language code, loading it on first use. nullptr if missing.
    QTranslator *translatorFor(const QString &code);

    QQmlEngine *m_engine = nullptr;               // not owned
    QHash<QString, QTranslator *> m_translators;  // loaded tables, owned (QObject parent)
    QTranslator *m_current = nullptr;             // installed table, nullptr for English
    LanguageData m_data;
};

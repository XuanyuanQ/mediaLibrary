#pragma once

#include <QVariantList>

#include "UiProperty.h"

/*
 * Data sent to the UI for language switching (read-only for QML).
 * LanguageManager publishes; QML reads LanguageData.toUiLanguage.
 */
class LanguageData : public UiDataBase
{
    Q_OBJECT

public:
    using UiDataBase::UiDataBase;

    UI_PROPERTY(QString, Language)                 // current code: "en", "sv"
    UI_PROPERTY(QVariantList, AvailableLanguages)  // [{code, label}]
};

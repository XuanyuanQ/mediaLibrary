#pragma once

#include <QObject>

/*
 * Data sent from C++ to the UI (with UiDataBase::publish).
 * The counterpart for messages from the UI is FromUi.h.
 *
 * Every name must also be declared with UI_PROPERTY in a data class.
 * In QML the property gets a "toUi" prefix: ResultCount -> LibraryData.toUiResultCount.
 * Numbered by module like FromUi: library 100-199, language 200-299.
 */
namespace ToUi {
Q_NAMESPACE

enum Message {
    // Library (LibraryManager -> LibraryData)
    SearchText = 100,   // QString
    Category,           // QString
    Categories,         // QVariantList [{key, label}]
    ResultCount,        // int
    TotalCount,         // int
    EmptyState,         // LibraryData::EmptyState
    SelectedId,         // int, -1 = none
    SelectedItem,       // QVariantMap

    // Language (LanguageManager -> LanguageData)
    Language = 200,     // QString
    AvailableLanguages, // QVariantList [{code, label}]
};
Q_ENUM_NS(Message)

} // namespace ToUi

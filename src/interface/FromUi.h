#pragma once

#include <QObject>

/*
 * Messages from the UI to C++ (sent with UiEvents.post).
 * The counterpart for data sent to the UI is ToUi.h.
 *
 * Numbered by module: library 100-199, language 200-299.
 * The comment shows the expected data type; handlers check it and warn if wrong.
 * Exposed to QML as FromUi (see main.cpp).
 */
namespace FromUi {
Q_NAMESPACE

enum Message {
    // Library (LibraryManager)
    SearchEdited = 100,     // QString: search text            <- SearchField
    TypeSelected,           // QString: type key ("", "Movie") <- TypeFilter
    ItemClicked,            // int: media ID                   <- MediaRow

    // Language (LanguageManager)
    LanguageSelected = 200, // QString: language code ("en", "sv") <- LanguageSwitch
};
Q_ENUM_NS(Message)

enum Module {
    Library = 1,
    Language = 2,
};
Q_ENUM_NS(Module)

// Module of a message = message / 100
inline Module moduleOf(Message message) { return Module(int(message) / 100); }

} // namespace FromUi

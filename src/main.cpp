// Creates and wires all C++ objects, then registers the interface objects as QML singletons.
// QML sees only UiEvents (from UI), LibraryData and LanguageData (to UI); logic classes stay hidden.
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QtQml/qqml.h>

#include "LanguageManager.h"
#include "LibraryManager.h"
#include "MediaJsonLoader.h"
#include "MediaLibraryModel.h"
#include "FromUi.h"
#include "UiEvents.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("Media Library"));
    QQuickStyle::setStyle(QStringLiteral("Basic"));   // fully customizable controls

    // Model: load the mock data
    MediaLibraryModel library;
    const LoadResult loaded =
        MediaJsonLoader::load(QStringLiteral(":/qt/qml/MediaLibrary/data/media.json"));
    if (loaded.ok())
        library.setItems(loaded.items);
    else
        qWarning("Failed to load media data: %s", qPrintable(loaded.error));

    // Logic
    LibraryManager libraryManager(&library);
    LanguageManager language;

    // Refresh C++ translated texts after a language change
    QObject::connect(&language, &LanguageManager::languageChanged,
                     &libraryManager, &LibraryManager::retranslate);

    // From UI: one connection per module; each module ignores other modules' messages
    UiEvents uiEvents;
    QObject::connect(&uiEvents, &UiEvents::post, &libraryManager, &LibraryManager::handleUiEvent); // 100-199
    QObject::connect(&uiEvents, &UiEvents::post, &language, &LanguageManager::handleUiEvent);       // 200-299

    // QML singletons (must be registered before loading QML). C++ keeps ownership.
    qmlRegisterSingletonInstance("MediaLibrary.Core", 1, 0, "UiEvents", &uiEvents);
    qmlRegisterUncreatableMetaObject(FromUi::staticMetaObject, "MediaLibrary.Core", 1, 0, "FromUi",
                                     QStringLiteral("FromUi only provides message names"));
    qmlRegisterSingletonInstance("MediaLibrary.Core", 1, 0, "LibraryData", libraryManager.data());
    qmlRegisterSingletonInstance("MediaLibrary.Core", 1, 0, "LanguageData", language.data());

    // Created last so it is destroyed first, while the C++ objects above still exist
    QQmlApplicationEngine engine;
    language.setEngine(&engine);

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
        [] { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
    engine.loadFromModule("MediaLibrary", "Main");

    return app.exec();
}

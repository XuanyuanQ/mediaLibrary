// 程序入口：在这里创建并组装所有 C++ 对象（依赖注入），再交给 QML。
// QML 不自己创建任何业务对象，只接收 main.cpp 注入的 libraryVm 和 language。
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QtQml/qqmlextensionplugin.h>

#include "languagemanager.h"
#include "libraryviewmodel.h"
#include "mediajsonloader.h"
#include "medialibrarymodel.h"

// mediacore 是静态库形式的 QML 模块（MediaLibrary.Core），需要手动导入它的插件，
// QML 里才能使用 LibraryViewModel、LanguageManager 这些类型名
Q_IMPORT_QML_PLUGIN(MediaLibrary_CorePlugin)

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("Media Library"));
    QQuickStyle::setStyle(QStringLiteral("Basic"));   // Basic 风格允许完全自定义控件外观

    QQmlApplicationEngine engine;

    // ---- Model：加载数据 ----
    MediaLibraryModel library;
    const LoadResult loaded =
        MediaJsonLoader::load(QStringLiteral(":/qt/qml/MediaLibrary/data/media.json"));
    if (loaded.ok())
        library.setItems(loaded.items);
    else
        qWarning("Failed to load media data: %s", qPrintable(loaded.error));

    // ---- ViewModel / Service ----
    LibraryViewModel libraryVm(&library);
    LanguageManager language(&engine);

    // 语言切换后，ViewModel 里用 tr() 生成的文字也要刷新。
    // 两个类互不引用，由这里连接，保持各自独立。
    QObject::connect(&language, &LanguageManager::languageChanged,
                     &libraryVm, &LibraryViewModel::retranslate);

    // ---- 注入 View ----
    // 对应 Main.qml 中的 required property libraryVm / language
    engine.setInitialProperties({
        {QStringLiteral("libraryVm"), QVariant::fromValue(&libraryVm)},
        {QStringLiteral("language"), QVariant::fromValue(&language)},
    });

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
        [] { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
    engine.loadFromModule("MediaLibrary", "Main");

    return app.exec();
}

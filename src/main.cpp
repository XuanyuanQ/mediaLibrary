// 程序入口：在这里创建并组装所有 C++ 对象，再注册成 QML 单例。
// QML 不自己创建任何业务对象，直接通过单例名访问：LibraryViewModel.xxx、LanguageManager.xxx
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QtQml/qqml.h>

#include "LanguageManager.h"
#include "LibraryViewModel.h"
#include "MediaJsonLoader.h"
#include "MediaLibraryModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("Media Library"));
    QQuickStyle::setStyle(QStringLiteral("Basic"));   // Basic 风格允许完全自定义控件外观

    // ---- Model：加载数据 ----
    MediaLibraryModel library;
    const LoadResult loaded =
        MediaJsonLoader::load(QStringLiteral(":/qt/qml/MediaLibrary/data/media.json"));
    if (loaded.ok())
        library.setItems(loaded.items);
    else
        qWarning("Failed to load media data: %s", qPrintable(loaded.error));

    // ---- ViewModel / Service：整个程序各只有这一个对象 ----
    LibraryViewModel libraryVm(&library);
    LanguageManager language;

    // 语言切换后，ViewModel 里用 tr() 生成的文字也要刷新。
    // 两个类互不引用，由这里连接，保持各自独立。
    QObject::connect(&language, &LanguageManager::languageChanged,
                     &libraryVm, &LibraryViewModel::retranslate);

    // ---- 注册成 QML 单例（必须在加载 QML 之前）----
    // 参数：模块名、主版本、次版本、QML 中使用的名字、对象指针。
    // QML 里 import MediaLibrary.Core 之后，就能直接写 LibraryViewModel.searchText。
    // 对象仍归 C++ 所有（QML 引擎不会删除它），所以要保证它活得比引擎久，见下方说明。
    qmlRegisterSingletonInstance("MediaLibrary.Core", 1, 0, "LibraryViewModel", &libraryVm);
    qmlRegisterSingletonInstance("MediaLibrary.Core", 1, 0, "LanguageManager", &language);

    // 引擎最后创建：局部变量按“创建的反序”销毁，引擎会最先销毁，
    // 保证 QML 销毁时它引用的 C++ 对象都还活着
    QQmlApplicationEngine engine;
    language.setEngine(&engine);

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
        [] { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
    engine.loadFromModule("MediaLibrary", "Main");

    return app.exec();
}

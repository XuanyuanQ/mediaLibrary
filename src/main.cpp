// 程序入口：在这里创建并组装所有 C++ 对象，再注册成 QML 单例。
// QML 不自己创建任何业务对象，直接通过单例名访问：
//   上发（QML → C++）：UiEvents.post(类型, 数据)，按模块分发给各自的 handleUiEvent()
//   下发（C++ → QML）：LibraryViewModel.xxx、LanguageManager.xxx 的属性，QML 绑定后自动刷新
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QtQml/qqml.h>

#include "LanguageManager.h"
#include "LibraryViewModel.h"
#include "MediaJsonLoader.h"
#include "MediaLibraryModel.h"
#include "UiEvents.h"

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

    // ---- 上发：每个模块 connect 一次 ----
    // 每个模块都会收到所有消息，各自的 handleUiEvent() 只处理本模块那一段（见 UiEvents.h 的枚举）。
    // 加新消息不用改这里；加新模块才在这里加一行
    UiEvents uiEvents;
    QObject::connect(&uiEvents, &UiEvents::post, &libraryVm, &LibraryViewModel::handleUiEvent); // 100~199
    QObject::connect(&uiEvents, &UiEvents::post, &language, &LanguageManager::handleUiEvent);   // 200~299

    // ---- 注册成 QML 单例（必须在加载 QML 之前）----
    // 参数：模块名、主版本、次版本、QML 中使用的名字、对象指针。
    // QML 里 import MediaLibrary.Core 之后，就能直接写 LibraryViewModel.searchText。
    // 对象仍归 C++ 所有（QML 引擎不会删除它），所以要保证它活得比引擎久，见下方说明。
    qmlRegisterSingletonInstance("MediaLibrary.Core", 1, 0, "LibraryViewModel", &libraryVm);
    qmlRegisterSingletonInstance("MediaLibrary.Core", 1, 0, "LanguageManager", &language);
    qmlRegisterSingletonInstance("MediaLibrary.Core", 1, 0, "UiEvents", &uiEvents);

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

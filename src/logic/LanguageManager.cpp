#include "LanguageManager.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QQmlEngine>

// 运行时切换语言：英文是源语言（代码里 qsTr/tr 写的就是英文），其他语言从 .qm 翻译文件加载

LanguageManager::LanguageManager(QObject *parent)
    : QObject(parent)
{
    // 启动时下发初始数据。语言名称本身不翻译：用户在任何语言下都应该能认出自己的语言
    m_data.publish(UiData::Language, QStringLiteral("en"));
    m_data.publish(UiData::AvailableLanguages, QVariantList{
        QVariantMap{{QStringLiteral("code"), QStringLiteral("en")}, {QStringLiteral("label"), QStringLiteral("EN")}},
        QVariantMap{{QStringLiteral("code"), QStringLiteral("sv")}, {QStringLiteral("label"), QStringLiteral("SV")}},
    });
}

void LanguageManager::handleUiEvent(UiEvents::Type type, const QVariant &data)
{
    // 谁调用：QML 里 UiEvents.post(...) → main.cpp 的 connect → 这里。
    // 不是语言模块的消息直接忽略
    if (UiEvents::moduleOf(type) != UiEvents::Language)
        return;

    switch (type) {
    case UiEvents::LanguageSelected:
        if (data.typeId() != QMetaType::QString) {
            qWarning() << "UiEvents:" << type << "expects a language code, got" << data;
            return;
        }
        setLanguage(data.toString());
        break;

    default:
        qWarning() << "UiEvents: unhandled language message" << type;
        break;
    }
}

void LanguageManager::setLanguage(const QString &code)
{
    // 谁调用：handleUiEvent() 收到 LanguageSelected（来自 LanguageSwitch.qml 的点击）
    if (m_data.getLanguage() == code)
        return;

    // 1. 找翻译文件，找不到就保持当前语言不变。
    //    英文是源语言（代码里 qsTr/tr 写的就是英文），不需要翻译文件；
    //    其他语言的 .qm 由 CMake 的 qt_add_translations 在编译时生成并打包进资源 :/i18n/ 下
    const bool isSource = (code == QLatin1String("en"));
    const QString qmFile = QStringLiteral(":/i18n/medialibrary_%1.qm").arg(code);
    if (!isSource && !QFile::exists(qmFile)) {
        qWarning() << "LanguageManager: no translation file for" << code;
        return;
    }

    // 2. 卸载旧翻译，安装新翻译（英文时只卸载，不安装）。
    //    installTranslator 之后，所有 qsTr() / tr() 查字符串时都会先查这个翻译器
    QCoreApplication::removeTranslator(&m_translator);
    if (!isSource && m_translator.load(qmFile))
        QCoreApplication::installTranslator(&m_translator);

    // 3. 让 QML 里所有 qsTr() 重新求值：界面上的固定文字立即换成新语言
    if (m_engine)
        m_engine->retranslate();

    m_data.publish(UiData::Language, code);   // 下发：当前语言（LanguageSwitch 据此高亮）

    // 4. 通知其他 C++ 模块：main.cpp 把它连到 LibraryManager::retranslate()，
    //    重新生成 C++ 里用 tr() 拼好的文字（下拉框选项、详情里的类型和时长）并下发
    emit languageChanged();
}

#pragma once

#include <QObject>
#include <QTranslator>
#include <QVariantList>

class QQmlEngine;

/*
 * 【Service 层】运行时切换界面语言。
 *
 * 切换流程：卸载旧翻译 -> 加载 :/i18n/medialibrary_<code>.qm -> 安装 ->
 *           QQmlEngine::retranslate()（所有 qsTr() 自动刷新）-> 发出 languageChanged
 *
 * 普通类。main.cpp 创建对象后用 qmlRegisterSingletonInstance 注册成 QML 单例，
 * QML 里 import MediaLibrary.Core 之后直接写 LanguageManager.xxx。
 *
 * 它不知道 LibraryViewModel 的存在；ViewModel 里用 tr() 生成的文字，
 * 由 main.cpp 把 languageChanged 连接到 LibraryViewModel::retranslate() 来刷新。
 */
class LanguageManager : public QObject
{
    Q_OBJECT

    // 当前语言代码，例如 "en"、"zh_CN"
    Q_PROPERTY(QString language READ getLanguage NOTIFY languageChanged)
    // 可选语言 [{code, label}]，例如 [{code:"en", label:"EN"}, {code:"zh_CN", label:"中"}]
    Q_PROPERTY(QVariantList availableLanguages READ getAvailableLanguages CONSTANT)

public:
    explicit LanguageManager(QObject *parent = nullptr);

    // 切换语言后要调用 engine->retranslate()；引擎在 main.cpp 里创建好后再设置进来
    void setEngine(QQmlEngine *engine) { m_engine = engine; }

    QString getLanguage() const { return m_language; }
    QVariantList getAvailableLanguages() const;

    Q_INVOKABLE void setLanguage(const QString &code);

signals:
    void languageChanged();

private:
    QQmlEngine *m_engine = nullptr;   // 不拥有；用于 retranslate()，测试中可以传 nullptr
    QTranslator m_translator;
    QString m_language = QStringLiteral("en");
};

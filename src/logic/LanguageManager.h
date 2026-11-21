#pragma once

#include <QObject>
#include <QTranslator>
#include <QVariantList>

#include "LanguageData.h"
#include "UiEvents.h"

class QQmlEngine;

/*
 * 【逻辑层 · Service】运行时切换界面语言。不对 QML 开放（不注册到 QML）。
 *
 * 切换流程：卸载旧翻译 -> 加载 :/i18n/medialibrary_<code>.qm -> 安装 ->
 *           QQmlEngine::retranslate()（所有 qsTr() 自动刷新）-> 发出 languageChanged
 *
 * 逻辑类，不对 QML 开放：
 *   上发：QML → UiEvents.post(LanguageSelected, code) → handleUiEvent() → setLanguage()
 *   下发：m_data.publish(UiData::Language, code) → LanguageData 发出 languageChanged → QML 绑定刷新
 *
 * 它不知道 LibraryManager 的存在；ViewModel 里用 tr() 生成的文字，
 * 由 main.cpp 把 languageChanged 连接到 LibraryManager::retranslate() 来刷新。
 */
class LanguageManager : public QObject
{
    Q_OBJECT

public:
    explicit LanguageManager(QObject *parent = nullptr);

    // 切换语言后要调用 engine->retranslate()；引擎在 main.cpp 里创建好后再设置进来
    void setEngine(QQmlEngine *engine) { m_engine = engine; }

    // 下发给 QML 的数据。main.cpp 把它注册成单例 LanguageData
    LanguageData *data() { return &m_data; }

    // ---- 处理上发消息 ----
    // 入口：main.cpp 把 UiEvents::post connect 到这里。只处理语言模块（200~299）的消息
    void handleUiEvent(UiEvents::Type type, const QVariant &data);

    // 由 handleUiEvent 分发（LanguageSelected）。不加 Q_INVOKABLE：QML 不能直接调用
    void setLanguage(const QString &code);

signals:
    // 语言真正切换完成后发出（给其他 C++ 模块用，例如 main.cpp 连到 LibraryManager::retranslate）
    void languageChanged();

private:
    QQmlEngine *m_engine = nullptr;   // 不拥有；用于 retranslate()
    QTranslator m_translator;
    LanguageData m_data;              // 下发数据（QML 只读它）
};

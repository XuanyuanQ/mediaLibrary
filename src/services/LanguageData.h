#pragma once

#include <QVariantList>

#include "UiProperty.h"

/*
 * 【下发接口 · 数据类】语言相关的显示数据。QML 只读这里。
 *
 *   LanguageManager 切换语言后 → m_data.publish(UiData::Language, code)
 *   → 发出 languageChanged → QML 绑定自动刷新
 *
 * 属性名必须先在 UiData.h 里登记。
 * main.cpp 用 qmlRegisterSingletonInstance 注册成单例，QML 里写 LanguageData.language。
 */
class LanguageData : public UiDataBase
{
    Q_OBJECT

public:
    using UiDataBase::UiDataBase;

    UI_PROPERTY(QString, language, Language)                          // 当前语言代码："en"、"zh_CN"
    UI_PROPERTY(QVariantList, availableLanguages, AvailableLanguages) // 可选语言 [{code, label}]
};

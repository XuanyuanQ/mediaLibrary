#pragma once

#include <QAbstractItemModel>
#include <QVariantList>
#include <QVariantMap>

#include "UiProperty.h"

/*
 * 【下发接口 · 数据类】媒体库页面要显示的全部数据。QML 只读这里，不读任何逻辑类。
 *
 *   C++ 逻辑（LibraryManager）算完结果 → m_data.publish(UiData::Xxx, 值)
 *   → 对应属性的 setter → 值变了发出 xxxChanged → QML 绑定自动刷新
 *
 * 只有数据，没有任何逻辑。属性名必须先在 UiData.h 里登记。
 * main.cpp 用 qmlRegisterSingletonInstance 注册成单例，QML 里写 LibraryData.resultCount。
 */
class LibraryData : public UiDataBase
{
    Q_OBJECT

    // ---- 列表 ----
    // 唯一的例外：列表是 QAbstractItemModel，靠 rowsRemoved / rowsInserted 等信号增量更新，
    // 不能整体替换，所以只在启动时设置一次（CONSTANT），不走 publish
    Q_PROPERTY(QAbstractItemModel *items READ getItems CONSTANT)

public:
    // 空状态：View 根据它决定显示哪种提示
    enum EmptyState {
        None,      // 有结果，不显示提示
        NoData,    // 媒体库本身就是空的（或加载失败）
        NoMatch,   // 有数据，但当前搜索 / 筛选没有匹配项
    };
    Q_ENUM(EmptyState)

    explicit LibraryData(QAbstractItemModel *items, QObject *parent = nullptr)
        : UiDataBase(parent), m_items(items)
    {
        publish(UiData::SelectedId, -1);   // 宏生成的成员默认是 0，这里改成“未选中”
    }

    QAbstractItemModel *getItems() const { return m_items; }

    // ---- 搜索 / 筛选 ----
    UI_PROPERTY(QString, searchText, SearchText)       // 搜索框文字（输入后立即下发，过滤有 200ms 防抖）
    UI_PROPERTY(QString, category, Category)           // 当前类型 key：""=全部；"Others"=主要类型以外的
    UI_PROPERTY(QVariantList, categories, Categories)  // 下拉框选项 [{key, label}]，label 已翻译

    // ---- 计数 / 空状态 ----
    UI_PROPERTY(int, resultCount, ResultCount)         // 过滤后的条数
    UI_PROPERTY(int, totalCount, TotalCount)           // 全部条数
    UI_PROPERTY(EmptyState, emptyState, EmptyState)    // 列表为空的原因

    // ---- 选中 / 详情 ----
    // 记 ID 不记行号：搜索、排序后行号会变，ID 不变。-1 表示没选中
    UI_PROPERTY(int, selectedId, SelectedId)
    // 详情页直接绑定：{title, creator, typeLabel, metaText, description}，文字已格式化好；没选中时为空
    UI_PROPERTY(QVariantMap, selectedItem, SelectedItem)

private:
    QAbstractItemModel *m_items = nullptr;   // 不拥有，属于 LibraryManager
};

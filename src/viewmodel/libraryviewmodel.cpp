#include "libraryviewmodel.h"

#include "mediafiltermodel.h"
#include "medialibrarymodel.h"

// 骨架阶段：对象之间的连接关系已搭好，界面逻辑待实现

LibraryViewModel::LibraryViewModel(MediaLibraryModel *library, QObject *parent)
    : QObject(parent)
    , m_library(library)
    , m_filter(new MediaFilterModel(this))
{
    m_filter->setSourceModel(m_library);

    // 防抖：每次输入都重新计时，停止输入 200ms 后才执行一次 applySearch()
    m_searchDebounce.setSingleShot(true);
    m_searchDebounce.setInterval(200);
    connect(&m_searchDebounce, &QTimer::timeout, this, &LibraryViewModel::applySearch);

    // TODO: 监听 m_filter 的行数变化 -> resultCountChanged / emptyStateChanged
    // TODO: 监听 m_library 的 countChanged -> totalCountChanged / emptyStateChanged

    rebuildCategories();
}

QAbstractItemModel *LibraryViewModel::items() const
{
    return m_filter;
}

void LibraryViewModel::setSearchText(const QString &text)
{
    if (m_searchText == text)
        return;
    m_searchText = text;
    emit searchTextChanged();
    m_searchDebounce.start();   // 重新开始计时
}

void LibraryViewModel::setCategory(const QString &category)
{
    // TODO: 保存后立即交给 m_filter（下拉框选择不需要防抖）
    if (m_category == category)
        return;
    m_category = category;
    emit categoryChanged();
}

int LibraryViewModel::resultCount() const
{
    return m_filter->rowCount();
}

int LibraryViewModel::totalCount() const
{
    return m_library ? m_library->count() : 0;
}

LibraryViewModel::EmptyState LibraryViewModel::emptyState() const
{
    // TODO: totalCount()==0 -> NoData；resultCount()==0 -> NoMatch；否则 None
    return None;
}

void LibraryViewModel::setSearchDebounceInterval(int ms)
{
    m_searchDebounce.setInterval(ms);
}

void LibraryViewModel::select(int mediaId)
{
    // TODO: 保存 ID，rebuildSelectedItem()，发出信号
    Q_UNUSED(mediaId)
}

void LibraryViewModel::clearSelection()
{
    // TODO: selectedId = -1，清空 selectedItem
}

void LibraryViewModel::clearFilters()
{
    // TODO: 清空搜索词（不经过防抖，立即生效）和分类
}

void LibraryViewModel::retranslate()
{
    rebuildCategories();
    rebuildSelectedItem();
}

void LibraryViewModel::applySearch()
{
    m_filter->setSearchText(m_searchText);
    // TODO: 发出 resultCountChanged / emptyStateChanged
}

void LibraryViewModel::rebuildCategories()
{
    // TODO: 用 tr() 生成 [{key:"", label:"All types"}, {key:"Movie", label:"Movies"}, …]
    m_categories = {};
    emit categoriesChanged();
}

void LibraryViewModel::rebuildSelectedItem()
{
    // TODO: 从 m_library->itemById(m_selectedId) 取数据，格式化 durationText、categoryLabel 等
    emit selectedItemChanged();
}

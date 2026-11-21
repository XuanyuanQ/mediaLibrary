#include "LibraryViewModel.h"

#include "MediaFilterModel.h"
#include "MediaLibraryModel.h"

// 已实现：搜索、计数。待实现：类型筛选、选中、空状态、语言相关文字

LibraryViewModel::LibraryViewModel(MediaLibraryModel *library, QObject *parent)
    : QObject(parent)
    , m_library(library)
    , m_filter(new MediaFilterModel(this))
{
    // 代理的数据源 = 媒体库。setSourceModel() 内部 Qt 会自动：
    //   - connect 源模型的 modelReset / rowsInserted / rowsRemoved / dataChanged 等信号，
    //     源数据一变，代理就重新过滤，再把变化通知给 ListView（不需要我们手写 connect）
    //   - 立即对源模型逐行调用 filterAcceptsRow()，建立第一份“显示哪几行”的对照表
    m_filter->setSourceModel(m_library);

    // 防抖：每次输入都重新计时，停止输入 200ms 后才执行一次 applySearch()
    m_searchDebounce.setSingleShot(true);
    m_searchDebounce.setInterval(200);
    connect(&m_searchDebounce, &QTimer::timeout, this, &LibraryViewModel::applySearch);

    // 过滤结果的行数变化时（搜索后行被删除/加入、整体重置），通知 QML 刷新“结果数量”。
    // 注意：这里的 connect 只用于计数文字。列表本身的增删不靠它——
    // ListView 在 QML 设置 model 时，Qt 已经自动 connect 了代理的这些信号，自己会更新
    const auto onResultsChanged = [this] {
        emit resultCountChanged();
        emit emptyStateChanged();
    };
    connect(m_filter, &QAbstractItemModel::rowsInserted, this, onResultsChanged);
    connect(m_filter, &QAbstractItemModel::rowsRemoved, this, onResultsChanged);
    connect(m_filter, &QAbstractItemModel::modelReset, this, onResultsChanged);
    connect(m_filter, &QAbstractItemModel::layoutChanged, this, onResultsChanged);

    // 媒体库总数变化时，通知 QML 刷新“总数”
    connect(m_library, &MediaLibraryModel::countChanged, this, [this] {
        emit totalCountChanged();
        emit emptyStateChanged();
    });

    rebuildCategories();
}

QAbstractItemModel *LibraryViewModel::getItems() const
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

int LibraryViewModel::getResultCount() const
{
    return m_filter->rowCount();
}

int LibraryViewModel::getTotalCount() const
{
    return m_library ? m_library->getCount() : 0;
}

LibraryViewModel::EmptyState LibraryViewModel::getEmptyState() const
{
    // TODO: getTotalCount()==0 -> NoData；getResultCount()==0 -> NoMatch；否则 None
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
    // 防抖结束：把搜索词交给过滤模型。
    // 结果行数的变化会通过上面构造函数里连接的信号，自动通知 QML
    m_filter->setSearchText(m_searchText);
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

#include "MediaFilterModel.h"

#include "MediaLibraryModel.h"

// 已实现：搜索（按标题或创作者）、类型筛选。待实现：排序

MediaFilterModel::MediaFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    // TODO: setDynamicSortFilter(true); 设置源模型后 sort(0)
}

void MediaFilterModel::setSearchText(const QString &text)
{
    if (m_searchText == text)
        return;

    // 修改过滤条件前后都要通知代理，它才会重新对每一行调用 filterAcceptsRow()。
    // Qt 6.10 起推荐 beginFilterChange()/endFilterChange()（在修改条件之前先通知），
    // 旧版本用 invalidateFilter()（在修改之后调用）。
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
    beginFilterChange();
#endif

    m_searchText = text;
    // simplified()：去掉首尾空白，并把连续空白压成一个空格；再按空格拆成多个关键字。
    // 例如 "  dana   mars " -> ["dana", "mars"]
    m_terms = text.simplified().split(QLatin1Char(' '), Qt::SkipEmptyParts);

    // 执行下面这一行时，Qt 在函数内部同步完成以下所有事情，返回时列表已经更新好：
    //   1. 对源模型的每一行调用 filterAcceptsRow(第几行, 根节点)，得到新的“显示哪几行”
    //   2. 和旧结果比较，对要删除的每一段行调用 beginRemoveRows()/endRemoveRows()
    //      → 发出 rowsRemoved；对要加回的行发出 rowsInserted
    //   3. 这些信号的接收者立即处理：
    //      - ListView：在 QML 里设置 model 时 Qt 自动 connect 的，收到后删除/创建对应的行
    //      - LibraryManager：构造函数里手动 connect 的，收到后刷新结果计数
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
    endFilterChange(QSortFilterProxyModel::Direction::Rows);   // 只有“行”的过滤条件变了
#else
    invalidateFilter();
#endif
}

void MediaFilterModel::setCategory(const QString &category)
{
    if (m_category == category)
        return;

    // 和 setSearchText() 一样：修改条件前后通知代理，
    // endFilterChange() 内部会逐行重新调用 filterAcceptsRow()，并发出 rowsRemoved / rowsInserted
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
    beginFilterChange();
#endif

    m_category = category;

#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
#else
    invalidateFilter();
#endif
}

const QStringList &MediaFilterModel::primaryTypes()
{
    static const QStringList types = {
        QStringLiteral("Movie"), QStringLiteral("Music"),
        QStringLiteral("Ebook"), QStringLiteral("Podcast"),
    };
    return types;
}

bool MediaFilterModel::matchesCategory(const QString &type, const QString &category)
{
    if (category.isEmpty())
        return true;                                   // ""：全部类型
    if (category == QLatin1String("Others"))
        return !primaryTypes().contains(type);         // "Others"：不属于四个主要类型（例如 Audiobook）
    return type == category;                           // 其他：类型必须完全相同
}

// 谁调用：QSortFilterProxyModel 内部。在 setSourceModel()、源数据变化（modelReset 等）、
//         以及我们调用 endFilterChange() 时，Qt 会对源模型的每一行调用一次本函数。
// sourceRow：   源模型（MediaLibraryModel）的行号，0 ~ 行数-1，由 Qt 的循环逐个传入
// sourceParent：这一行的父节点。列表没有层级，永远是根节点（无效的 QModelIndex），原样使用即可
// 返回值：      true = 这一行显示，false = 过滤掉
bool MediaFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    // 先用行号向源模型要这一行的“坐标卡片”，再用 idx.data() 取字段
    // （idx.data() 实际调用的是 MediaLibraryModel::data()）
    const QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);

    // 1. 类型筛选：类型不符合，直接过滤掉
    if (!matchesCategory(idx.data(MediaLibraryModel::TypeRole).toString(), m_category))
        return false;

    // 2. 搜索：没有关键字时，通过了类型筛选就显示
    if (m_terms.isEmpty())
        return true;

    const QString title = idx.data(MediaLibraryModel::TitleRole).toString();
    const QString creator = idx.data(MediaLibraryModel::CreatorRole).toString();

    // 每个关键字都必须出现在标题或创作者里（多个关键字是“并且”关系），不区分大小写。
    // 例如搜 "dana mars"：创作者含 Dana、标题含 Mars 的 Echoes of Mars 会被找到
    for (const QString &term : m_terms) {
        if (!title.contains(term, Qt::CaseInsensitive)
            && !creator.contains(term, Qt::CaseInsensitive))
            return false;
    }
    return true;
}

bool MediaFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    // TODO: 按标题 localeAwareCompare 排序
    return QSortFilterProxyModel::lessThan(left, right);
}

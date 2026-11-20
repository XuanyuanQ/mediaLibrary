#include "mediafiltermodel.h"

// 骨架阶段：接口已定义，过滤和排序规则待实现

MediaFilterModel::MediaFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    // TODO: setDynamicSortFilter(true); 设置源模型后 sort(0)
}

void MediaFilterModel::setSearchText(const QString &text)
{
    // TODO: 保存并拆分关键字，然后 beginFilterChange()/endFilterChange() 重新过滤
    m_searchText = text;
}

void MediaFilterModel::setCategory(const QString &category)
{
    // TODO: 保存后重新过滤
    m_category = category;
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
    // TODO: ""=全部；"Others"=不在 primaryTypes() 里；其他=相等
    Q_UNUSED(type)
    Q_UNUSED(category)
    return true;
}

bool MediaFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    // TODO: 先按分类筛选，再要求每个关键字都命中标题或创作者
    Q_UNUSED(sourceRow)
    Q_UNUSED(sourceParent)
    return true;
}

bool MediaFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    // TODO: 按标题 localeAwareCompare 排序
    return QSortFilterProxyModel::lessThan(left, right);
}

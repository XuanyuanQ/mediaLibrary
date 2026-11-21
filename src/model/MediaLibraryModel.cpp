#include "MediaLibraryModel.h"

// itemById() 供详情页使用，待实现

MediaLibraryModel::MediaLibraryModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int MediaLibraryModel::rowCount(const QModelIndex &parent) const
{
    // 列表模型没有子节点：只有根节点（无效的 parent）才有行
    return parent.isValid() ? 0 : int(m_items.size());
}

QVariant MediaLibraryModel::data(const QModelIndex &index, int role) const
{
    // 视图可能传来越界或无效的索引，先检查
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size())
        return {};

    // 按角色（role）返回这一行的某个字段；QML 委托里的 title、creator 等就是从这里取的
    const MediaItem &item = m_items.at(index.row());
    switch (role) {
    case Qt::DisplayRole:   // 默认显示角色也返回标题，方便调试和通用视图
    case TitleRole:   return item.title;
    case IdRole:      return item.id;
    case CreatorRole: return item.creator;
    case TypeRole:    return item.type;
    case YearRole:    return item.year;
    default:          return {};
    }
}

QHash<int, QByteArray> MediaLibraryModel::roleNames() const
{
    // 这些名字就是 QML 委托里可以直接使用的属性名
    return {
        {IdRole, "mediaId"},   // 不用 "id"：id 在 QML 中是保留字
        {TitleRole, "title"},
        {CreatorRole, "creator"},
        {TypeRole, "type"},
        {YearRole, "year"},
    };
}

void MediaLibraryModel::setItems(const QList<MediaItem> &items)
{
    // 整体替换数据：begin/endResetModel 通知所有视图“数据全换了，请重新读取”
    beginResetModel();
    m_items = items;
    endResetModel();
    emit countChanged();
}

std::optional<MediaItem> MediaLibraryModel::itemById(int id) const
{
    // TODO: 按 id 查找
    Q_UNUSED(id)
    return std::nullopt;
}

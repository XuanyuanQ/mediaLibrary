#include "MediaLibraryModel.h"

// 骨架阶段：模型的基础结构已就绪，data()/setItems()/itemById() 待实现

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
    // TODO: 按 role 返回 m_items[index.row()] 的对应字段
    Q_UNUSED(index)
    Q_UNUSED(role)
    return {};
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
    // TODO: beginResetModel(); m_items = items; endResetModel(); emit countChanged();
    Q_UNUSED(items)
}

std::optional<MediaItem> MediaLibraryModel::itemById(int id) const
{
    // TODO: 按 id 查找
    Q_UNUSED(id)
    return std::nullopt;
}

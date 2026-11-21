#include "MediaLibraryModel.h"

MediaLibraryModel::MediaLibraryModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

// rowCount(), data() and roleNames() are called by Qt (the proxy model and ListView), not by our code.

int MediaLibraryModel::rowCount(const QModelIndex &parent) const
{
    // A list has no children
    return parent.isValid() ? 0 : int(m_items.size());
}

QVariant MediaLibraryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size())
        return {};

    const MediaItem &item = m_items.at(index.row());
    switch (role) {
    case Qt::DisplayRole:
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
    // Names used in QML delegates; ListView fills the matching required properties of MediaRow
    return {
        {IdRole, "mediaId"},   // "id" is reserved in QML
        {TitleRole, "title"},
        {CreatorRole, "creator"},
        {TypeRole, "type"},
        {YearRole, "year"},
    };
}

void MediaLibraryModel::setItems(const QList<MediaItem> &items)
{
    // Tells views (and the proxy) that all data changed
    beginResetModel();
    m_items = items;
    endResetModel();
    emit countChanged();
}

std::optional<MediaItem> MediaLibraryModel::itemById(int id) const
{
    // Linear search is fine for a small list; use a QHash for large data
    for (const MediaItem &item : m_items) {
        if (item.id == id)
            return item;
    }
    return std::nullopt;
}

#pragma once

#include <QAbstractListModel>
#include <QList>
#include <optional>

#include "MediaItem.h"

/*
 * All media items as a list model. Filled by main.cpp.
 * The UI does not use it directly: ListView shows MediaFilterModel, which wraps this model.
 */
class MediaLibraryModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ getCount NOTIFY countChanged)

public:
    // Custom roles start after Qt::UserRole to avoid Qt's built-in roles
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        CreatorRole,
        TypeRole,
        YearRole,
    };
    Q_ENUM(Roles)

    explicit MediaLibraryModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int getCount() const { return int(m_items.size()); }

    // Replaces all items
    void setItems(const QList<MediaItem> &items);

    // Full record for the detail view; std::nullopt if not found
    std::optional<MediaItem> itemById(int id) const;

signals:
    void countChanged();

private:
    QList<MediaItem> m_items;
};

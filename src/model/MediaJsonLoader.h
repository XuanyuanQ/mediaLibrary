#pragma once

#include <QByteArray>
#include <QJsonObject>
#include <QList>
#include <QString>
#include <QStringList>
#include <optional>

#include "MediaItem.h"

// A bad record is skipped and noted in warnings; only a broken file sets error.
struct LoadResult
{
    QList<MediaItem> items;
    QStringList warnings;
    QString error;

    bool ok() const { return error.isEmpty(); }
};

/*
 * Reads and validates media data from JSON.
 * Kept separate from MediaLibraryModel so the data source can change
 * (database, network) without touching the model.
 */
class MediaJsonLoader
{
public:
    // path can be a file or a resource, e.g. ":/qt/qml/MediaLibrary/data/media.json"
    static LoadResult load(const QString &path);

    static LoadResult parse(const QByteArray &json);

private:
    // Returns std::nullopt if id or title is missing
    static std::optional<MediaItem> parseItem(const QJsonObject &obj);
};

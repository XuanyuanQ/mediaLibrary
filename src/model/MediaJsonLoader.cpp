#include "MediaJsonLoader.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>

LoadResult MediaJsonLoader::load(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        LoadResult result;
        result.error = QStringLiteral("Cannot open %1: %2").arg(path, file.errorString());
        return result;
    }
    return parse(file.readAll());
}

LoadResult MediaJsonLoader::parse(const QByteArray &json)
{
    LoadResult result;

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(json, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        result.error = QStringLiteral("Invalid JSON at offset %1: %2")
                           .arg(parseError.offset)
                           .arg(parseError.errorString());
        return result;
    }

    // Expected format: [ {...}, {...} ]
    if (!doc.isArray()) {
        result.error = QStringLiteral("Top-level JSON value must be an array");
        return result;
    }

    // A bad record is skipped; the others are still loaded
    const QJsonArray array = doc.array();
    for (qsizetype i = 0; i < array.size(); ++i) {
        const std::optional<MediaItem> item = parseItem(array.at(i).toObject());
        if (item)
            result.items.append(*item);
        else
            result.warnings.append(QStringLiteral("Record %1 skipped: missing id or title").arg(i));
    }
    return result;
}

std::optional<MediaItem> MediaJsonLoader::parseItem(const QJsonObject &obj)
{
    MediaItem item;
    item.id = obj.value(QStringLiteral("id")).toInt(-1);
    item.title = obj.value(QStringLiteral("title")).toString().trimmed();

    // id and title are required
    if (item.id < 0 || item.title.isEmpty())
        return std::nullopt;

    item.creator = obj.value(QStringLiteral("creator")).toString().trimmed();
    item.type = obj.value(QStringLiteral("type")).toString();
    item.year = obj.value(QStringLiteral("year")).toInt();
    item.durationSec = obj.value(QStringLiteral("duration")).toInt();
    item.pages = obj.value(QStringLiteral("pages")).toInt();
    item.description = obj.value(QStringLiteral("description")).toString();
    return item;
}

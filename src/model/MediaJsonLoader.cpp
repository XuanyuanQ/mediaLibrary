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
    // 读取和解析分开：parse() 只关心 JSON 文本，和数据从哪里来无关
    return parse(file.readAll());
}

LoadResult MediaJsonLoader::parse(const QByteArray &json)
{
    LoadResult result;

    // 1. 解析 JSON 文本；格式错误时整体失败
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(json, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        result.error = QStringLiteral("Invalid JSON at offset %1: %2")
                           .arg(parseError.offset)
                           .arg(parseError.errorString());
        return result;
    }

    // 2. 顶层必须是数组：[ {...}, {...} ]
    if (!doc.isArray()) {
        result.error = QStringLiteral("Top-level JSON value must be an array");
        return result;
    }

    // 3. 逐条解析；坏记录只跳过并记下原因，不影响其他记录
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
    item.id = obj.value(QStringLiteral("id")).toInt(-1);   // 没有 id 时得到 -1
    item.title = obj.value(QStringLiteral("title")).toString().trimmed();

    // id 和 title 是必填字段：缺少任何一个，这条记录就无法显示和识别
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

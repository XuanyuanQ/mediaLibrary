#include "MediaJsonLoader.h"

// 骨架阶段：只定义接口，具体实现待完成

LoadResult MediaJsonLoader::load(const QString &path)
{
    // TODO: 用 QFile 读取 path，失败时填写 error；成功后交给 parse()
    Q_UNUSED(path)
    LoadResult result;
    result.error = QStringLiteral("MediaJsonLoader::load is not implemented yet");
    return result;
}

LoadResult MediaJsonLoader::parse(const QByteArray &json)
{
    // TODO: QJsonDocument::fromJson；顶层必须是数组；逐条 parseItem()，坏记录写进 warnings
    Q_UNUSED(json)
    LoadResult result;
    result.error = QStringLiteral("MediaJsonLoader::parse is not implemented yet");
    return result;
}

std::optional<MediaItem> MediaJsonLoader::parseItem(const QJsonObject &obj)
{
    // TODO: 读取各字段；id < 0 或 title 为空时返回 std::nullopt
    Q_UNUSED(obj)
    return std::nullopt;
}

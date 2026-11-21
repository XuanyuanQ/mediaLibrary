#pragma once

#include <QByteArray>
#include <QJsonObject>
#include <QList>
#include <QString>
#include <QStringList>
#include <optional>

#include "MediaItem.h"

// 加载结果：成功时 error 为空；单条坏记录不会导致整体失败，只记录到 warnings
struct LoadResult
{
    QList<MediaItem> items;
    QStringList warnings;   // 被跳过的坏记录说明
    QString error;          // 整体失败的原因（文件打不开、不是合法 JSON 等）

    bool ok() const { return error.isEmpty(); }
};

/*
 * 【Model 层】从 JSON 读取媒体数据并校验。
 *
 * 和 MediaLibraryModel 分开的原因：
 *   - 单一职责：模型只管“持有数据并暴露给视图”，不关心数据从哪来
 *   - 可以单独测试各种坏数据
 *   - 以后换数据源（数据库、网络）只需替换这里
 */
class MediaJsonLoader
{
public:
    // 从文件（可以是 qrc 资源路径，如 ":/qt/qml/MediaLibrary/data/media.json"）加载
    static LoadResult load(const QString &path);

    // 直接解析 JSON 文本（方便测试，不用真的写文件）
    static LoadResult parse(const QByteArray &json);

private:
    // 解析一条记录；缺少必填字段（id、title）时返回 std::nullopt
    static std::optional<MediaItem> parseItem(const QJsonObject &obj);
};

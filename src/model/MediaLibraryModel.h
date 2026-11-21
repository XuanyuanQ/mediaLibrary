#pragma once

#include <QAbstractListModel>
#include <QList>
#include <optional>

#include "MediaItem.h"

/*
 * 【Model 层】全部媒体数据（只读）。
 *
 * 继承 QAbstractListModel：ListView 通过 rowCount()/data()/roleNames() 读取数据，
 * 委托里可以直接写 title、creator 等字段名。
 *
 * 由 main.cpp 创建并填充数据。
 * 界面不直接使用它：ListView 绑定的是 LibraryViewModel::items（过滤后的代理模型）。
 */
class MediaLibraryModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ getCount NOTIFY countChanged)

public:
    // 自定义角色从 Qt::UserRole + 1 开始，避免和 Qt 内置角色冲突
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        CreatorRole,
        TypeRole,
        YearRole,
    };
    Q_ENUM(Roles)

    explicit MediaLibraryModel(QObject *parent = nullptr);

    // ---- QAbstractListModel 接口 ----
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int getCount() const { return int(m_items.size()); }

    // 整体替换数据（main.cpp 加载完 JSON 后调用）
    void setItems(const QList<MediaItem> &items);

    // 按 ID 查完整记录，供 ViewModel 生成详情；找不到返回 std::nullopt
    std::optional<MediaItem> itemById(int id) const;

signals:
    void countChanged();

private:
    QList<MediaItem> m_items;
};

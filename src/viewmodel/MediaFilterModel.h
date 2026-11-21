#pragma once

#include <QSortFilterProxyModel>
#include <QStringList>

/*
 * 【ViewModel 内部】过滤 + 排序代理模型。
 *
 * 包在 MediaLibraryModel 外面，本身不存数据，只决定：
 *   - 哪些行可见 -> filterAcceptsRow()
 *   - 行的顺序   -> lessThan()
 *
 * 由 LibraryViewModel 持有和控制，界面通过 LibraryViewModel::items 拿到它，
 * 不直接设置它的属性。所以这里不需要注册成 QML 类型，也不需要 Q_PROPERTY。
 */
class MediaFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit MediaFilterModel(QObject *parent = nullptr);

    // 搜索文本：按空格拆成多个关键字，每个都要命中标题或创作者（AND 关系）
    void setSearchText(const QString &text);
    QString searchText() const { return m_searchText; }

    // 分类：""=全部；"Others"=四个主要类型以外的；其他=精确匹配类型
    void setCategory(const QString &category);
    QString category() const { return m_category; }

    // 四个主要类型（Movie / Music / Ebook / Podcast），其余归入 Others
    static const QStringList &primaryTypes();
    static bool matchesCategory(const QString &type, const QString &category);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    QString m_searchText;
    QStringList m_terms;     // 预先拆好的关键字，避免每行过滤时都重复拆分
    QString m_category;
};

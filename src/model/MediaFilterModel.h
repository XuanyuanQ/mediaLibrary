#pragma once

#include <QSortFilterProxyModel>
#include <QStringList>

/*
 * Filters MediaLibraryModel by search text and type. Does not store data or sort
 * (keeps the order of media.json). Owned by LibraryManager; QML gets it as LibraryData.toUiItems.
 *
 * Only filterAcceptsRow() is overridden. rowCount(), data() and roleNames() come from
 * QSortFilterProxyModel, which maps proxy rows to source rows.
 */
class MediaFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit MediaFilterModel(QObject *parent = nullptr);

    // Split by spaces; every word must match the title or the creator
    void setSearchText(const QString &text);
    QString searchText() const { return m_searchText; }

    // "" = all, "Others" = not one of the main types, otherwise exact type
    void setCategory(const QString &category);
    QString category() const { return m_category; }

    // Movie / Music / Ebook / Podcast
    static const QStringList &primaryTypes();
    static bool matchesCategory(const QString &type, const QString &category);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QString m_searchText;
    QStringList m_terms;     // split once, not on every row
    QString m_category;
};

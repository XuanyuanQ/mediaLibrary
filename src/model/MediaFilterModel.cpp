#include "MediaFilterModel.h"

#include "MediaLibraryModel.h"

MediaFilterModel::MediaFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void MediaFilterModel::setSearchText(const QString &text)
{
    if (m_searchText == text)
        return;

    // begin/endFilterChange (Qt 6.10+) re-runs filterAcceptsRow() on every row
    // and emits rowsRemoved / rowsInserted, so ListView updates.
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
    beginFilterChange();
#endif

    m_searchText = text;
    // "  dana   mars " -> ["dana", "mars"]
    m_terms = text.simplified().split(QLatin1Char(' '), Qt::SkipEmptyParts);

#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
#else
    invalidateFilter();
#endif
}

void MediaFilterModel::setCategory(const QString &category)
{
    if (m_category == category)
        return;

#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
    beginFilterChange();
#endif

    m_category = category;

#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
#else
    invalidateFilter();
#endif
}

const QStringList &MediaFilterModel::primaryTypes()
{
    static const QStringList types = {
        QStringLiteral("Movie"), QStringLiteral("Music"),
        QStringLiteral("Ebook"), QStringLiteral("Podcast"),
    };
    return types;
}

bool MediaFilterModel::matchesCategory(const QString &type, const QString &category)
{
    if (category.isEmpty())
        return true;
    if (category == QLatin1String("Others"))
        return !primaryTypes().contains(type);   // e.g. Audiobook
    return type == category;
}

// Called by Qt for each source row when the filter changes or the source data changes.
// Returns true to show the row.
bool MediaFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    const QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);

    if (!matchesCategory(idx.data(MediaLibraryModel::TypeRole).toString(), m_category))
        return false;

    if (m_terms.isEmpty())
        return true;

    const QString title = idx.data(MediaLibraryModel::TitleRole).toString();
    const QString creator = idx.data(MediaLibraryModel::CreatorRole).toString();

    // Every word must appear in the title or the creator (case-insensitive)
    for (const QString &term : m_terms) {
        if (!title.contains(term, Qt::CaseInsensitive)
            && !creator.contains(term, Qt::CaseInsensitive))
            return false;
    }
    return true;
}

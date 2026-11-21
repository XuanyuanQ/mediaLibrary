#include "LibraryManager.h"

#include <QDebug>

#include "MediaFilterModel.h"
#include "MediaLibraryModel.h"

LibraryManager::LibraryManager(MediaLibraryModel *library, QObject *parent)
    : QObject(parent)
    , m_library(library)
    , m_filter(new MediaFilterModel(this))
    , m_data(m_filter)   // the list given to QML is the filter model
{
    // The proxy listens to the source model and re-filters when it changes
    m_filter->setSourceModel(m_library);

    // Search runs 200 ms after the last key press
    m_searchDebounce.setSingleShot(true);
    m_searchDebounce.setInterval(200);
    connect(&m_searchDebounce, &QTimer::timeout, this, &LibraryManager::applySearch);

    // Update counts when the filtered rows change.
    // (ListView connects to these signals itself to update the list.)
    connect(m_filter, &QAbstractItemModel::rowsInserted, this, &LibraryManager::publishCounts);
    connect(m_filter, &QAbstractItemModel::rowsRemoved, this, &LibraryManager::publishCounts);
    connect(m_filter, &QAbstractItemModel::modelReset, this, &LibraryManager::publishCounts);
    connect(m_filter, &QAbstractItemModel::layoutChanged, this, &LibraryManager::publishCounts);
    connect(m_library, &MediaLibraryModel::countChanged, this, &LibraryManager::publishCounts);

    publishCounts();
    publishCategories();
}

void LibraryManager::handleUiEvent(FromUi::Message type, const QVariant &data)
{
    // Every module receives every message; ignore the ones for other modules
    if (FromUi::moduleOf(type) != FromUi::Library)
        return;

    // data is a QVariant, so check its type first
    switch (type) {
    case FromUi::SearchEdited:
    case FromUi::TypeSelected:
        if (data.typeId() != QMetaType::QString) {
            qWarning() << type << "expects a string, got" << data;
            return;
        }
        if (type == FromUi::SearchEdited)
            setSearchText(data.toString());
        else
            setCategory(data.toString());
        break;

    case FromUi::ItemClicked: {
        bool ok = false;
        const int id = data.toInt(&ok);   // QML numbers may arrive as int or double
        if (!ok) {
            qWarning() << type << "expects a media id, got" << data;
            return;
        }
        select(id);
        break;
    }

    default:
        qWarning() << "unhandled library message" << type;
        break;
    }
}

void LibraryManager::setSearchText(const QString &text)
{
    if (m_data.getSearchText() == text)
        return;
    m_data.publish(ToUi::SearchText, text);   // update the text box now
    m_searchDebounce.start();                 // filter later (restart the timer)
}

void LibraryManager::setCategory(const QString &category)
{
    if (m_data.getCategory() == category)
        return;
    // One selection is final, so no debounce
    m_filter->setCategory(category);
    m_data.publish(ToUi::Category, category);
}

void LibraryManager::publishCounts()
{
    m_data.publish(ToUi::ResultCount, m_filter->rowCount());
    m_data.publish(ToUi::TotalCount, m_library ? m_library->getCount() : 0);
    m_data.publish(ToUi::EmptyState, QVariant::fromValue(computeEmptyState()));
}

LibraryData::EmptyState LibraryManager::computeEmptyState() const
{
    // Check the library first: if it is empty, the result count is 0 too
    if (!m_library || m_library->getCount() == 0)
        return LibraryData::NoData;
    if (m_filter->rowCount() == 0)
        return LibraryData::NoMatch;
    return LibraryData::None;
}

void LibraryManager::select(int mediaId)
{
    if (m_data.getSelectedId() == mediaId)
        return;
    m_data.publish(ToUi::SelectedId, mediaId);   // highlights the row
    publishSelectedItem();                       // fills the detail view
}

void LibraryManager::retranslate()
{
    publishCategories();
    publishSelectedItem();
}

void LibraryManager::applySearch()
{
    // Row changes trigger publishCounts() through the connections above
    m_filter->setSearchText(m_data.getSearchText());
}

void LibraryManager::publishCategories()
{
    // key goes to the filter (matches the data's type), label is shown in the UI
    const auto option = [](const QString &key, const QString &label) {
        return QVariantMap{{QStringLiteral("key"), key}, {QStringLiteral("label"), label}};
    };
    m_data.publish(ToUi::Categories, QVariantList{
        option(QString(), tr("All types")),
        option(QStringLiteral("Movie"), tr("Movies")),
        option(QStringLiteral("Music"), tr("Music")),
        option(QStringLiteral("Ebook"), tr("Books")),
        option(QStringLiteral("Podcast"), tr("Podcasts")),
        option(QStringLiteral("Others"), tr("Others")),
    });
}

void LibraryManager::publishSelectedItem()
{
    // All display texts are prepared here; QML only lays them out.
    QVariantMap detail;   // stays empty when nothing is selected

    const std::optional<MediaItem> item = m_library ? m_library->itemById(m_data.getSelectedId())
                                                    : std::nullopt;
    if (item) {
        QString typeLabel = item->type;   // unknown types are shown as-is
        if (item->type == QLatin1String("Movie"))          typeLabel = tr("Movie");
        else if (item->type == QLatin1String("Music"))     typeLabel = tr("Music");
        else if (item->type == QLatin1String("Ebook"))     typeLabel = tr("Book");
        else if (item->type == QLatin1String("Podcast"))   typeLabel = tr("Podcast");
        else if (item->type == QLatin1String("Audiobook")) typeLabel = tr("Audiobook");

        // e.g. "2021 · 45 min" or "2019 · 320 pages"
        QStringList meta;
        if (item->year > 0)
            meta << QString::number(item->year);
        if (item->durationSec > 0) {
            const int minutes = (item->durationSec + 30) / 60;   // rounded
            meta << (minutes >= 60 ? tr("%1 h %2 min").arg(minutes / 60).arg(minutes % 60)
                                   : tr("%1 min").arg(minutes));
        }
        if (item->pages > 0)
            meta << tr("%1 pages").arg(item->pages);

        detail = {
            {QStringLiteral("title"), item->title},
            {QStringLiteral("creator"), item->creator},
            {QStringLiteral("typeLabel"), typeLabel},
            {QStringLiteral("metaText"), meta.join(QStringLiteral(" · "))},
            {QStringLiteral("description"), item->description},
        };
    }
    m_data.publish(ToUi::SelectedItem, detail);
}

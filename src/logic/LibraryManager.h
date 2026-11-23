#pragma once

#include <QObject>
#include <QTimer>
#include <QVariant>

#include "LibraryData.h"
#include "UiEvents.h"

class MediaLibraryModel;
class MediaFilterModel;

/*
 * Logic of the library page (the ViewModel in MVVM). Not visible to QML.
 *
 *   From UI: UiEvents.post(FromUi.xxx) -> handleUiEvent() -> setSearchText / setCategory / select
 *   To UI:   m_data.publish(ToUi::xxx, value) -> LibraryData -> QML bindings update
 */
class LibraryManager : public QObject
{
    Q_OBJECT

public:
    explicit LibraryManager(MediaLibraryModel *library, QObject *parent = nullptr);

    // Registered as the QML singleton LibraryData in main.cpp
    LibraryData *data() { return &m_data; }

    // Connected to UiEvents::post in main.cpp. Handles library messages only (100-199).
    void handleUiEvent(FromUi::Message type, const QVariant &data);

    void setSearchText(const QString &text);   // FromUi::SearchEdited
    void setCategory(const QString &category); // FromUi::TypeSelected
    void select(int mediaId);                  // FromUi::ItemClicked

private:
    void applySearch();             // after the debounce delay
    void publishCounts();           // resultCount, totalCount, emptyState
    void publishSelectedItem();     // fields of the selected item
    LibraryData::EmptyState computeEmptyState() const;

    MediaLibraryModel *m_library = nullptr;   // not owned
    MediaFilterModel *m_filter = nullptr;     // owned (QObject parent)
    LibraryData m_data;
    QTimer m_searchDebounce;
};

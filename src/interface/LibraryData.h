#pragma once

#include <QAbstractItemModel>
#include <QVariantList>
#include <QVariantMap>

#include "UiProperty.h"

/*
 * Data sent to the UI for the library page (read-only for QML).
 *
 * LibraryManager computes values and calls m_data.publish(ToUi::Xxx, value);
 * QML reads them as LibraryData.toUiXxx. No logic lives here.
 */
class LibraryData : public UiDataBase
{
    Q_OBJECT

    // The list is a model updated row by row (rowsInserted / rowsRemoved),
    // so it is set once and does not go through publish().
    Q_PROPERTY(QAbstractItemModel *toUiItems READ getItems CONSTANT)

public:
    // Why the list is empty
    enum EmptyState {
        None,      // has results
        NoData,    // library is empty or failed to load
        NoMatch,   // nothing matches the search / filter
    };
    Q_ENUM(EmptyState)

    explicit LibraryData(QAbstractItemModel *items, QObject *parent = nullptr)
        : UiDataBase(parent), m_items(items)
    {
        publish(ToUi::SelectedId, -1);   // -1 = nothing selected
    }

    QAbstractItemModel *getItems() const { return m_items; }

    // Search / filter
    UI_PROPERTY(QString, SearchText)          // search box text
    UI_PROPERTY(QString, Category)            // type key: "" = all, "Others" = non-main types

    // Counts / empty state
    UI_PROPERTY(int, ResultCount)             // rows after filtering
    UI_PROPERTY(int, TotalCount)              // all rows
    UI_PROPERTY(EmptyState, EmptyState)

    // Selection / details
    UI_PROPERTY(int, SelectedId)              // item ID (not row index, which changes when filtering)
    UI_PROPERTY(QVariantMap, SelectedItem)    // {title, creator, type, year, durationSec, pages, description}, empty if none

private:
    QAbstractItemModel *m_items = nullptr;   // not owned
};

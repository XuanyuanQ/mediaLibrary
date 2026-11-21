#pragma once

#include <QString>

// One media record. Plain data, cheap to copy.
struct MediaItem
{
    int id = -1;          // unique, stays the same after filtering
    QString title;
    QString creator;      // artist / director / host / author
    QString type;         // Movie / Music / Ebook / Podcast; anything else counts as Others
    int year = 0;
    int durationSec = 0;  // movies, music, podcasts, audiobooks
    int pages = 0;        // ebooks
    QString description;
};

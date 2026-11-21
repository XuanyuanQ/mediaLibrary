#pragma once

#include <QString>

// 【Model 层】一条媒体记录。纯数据结构，不依赖 Qt 对象系统，可以随意拷贝。
struct MediaItem
{
    int id = -1;          // 唯一 ID：过滤、排序后依然能找到“同一条”记录
    QString title;        // 标题
    QString creator;      // 创作者（歌手 / 导演 / 主播 / 作者）
    QString type;         // 类型：Movie / Music / Ebook / Podcast，其他归入 Others
    int year = 0;         // 发行年份
    int durationSec = 0;  // 时长（秒）——电影、音乐、播客、有声书
    int pages = 0;        // 页数——电子书
    QString description;  // 简介
};

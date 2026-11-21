#pragma once

#include <QObject>
#include <QTimer>
#include <QVariant>

#include "LibraryData.h"
#include "UiEvents.h"

class MediaLibraryModel;
class MediaFilterModel;

/*
 * 【逻辑层 · ViewModel】媒体库页面的界面逻辑。不对 QML 开放（不注册到 QML）。
 *
 * 数据流：
 *   上发：QML → UiEvents.post(类型, 数据) → main.cpp connect → handleUiEvent() → setSearchText / setCategory / select
 *   下发：这里算完结果 → m_data.publish(UiData::名字, 值) → LibraryData 发出 xxxChanged → QML 绑定刷新
 *
 * 所有“下发”都走统一入口 publish()，名字在 UiData.h 里集中登记；搜索 "publish(" 就能看到全部下发点。
 * 这个类不引用任何 QML 类型，不启动界面也能单独创建和使用。
 */
class LibraryViewModel : public QObject
{
    Q_OBJECT

public:
    explicit LibraryViewModel(MediaLibraryModel *library, QObject *parent = nullptr);

    // 下发给 QML 的数据。main.cpp 把它注册成单例 LibraryData
    LibraryData *data() { return &m_data; }

    // ---- 处理上发消息 ----
    // 入口：main.cpp 把 UiEvents::post connect 到这里。只处理媒体库模块（100~199）的消息
    void handleUiEvent(UiEvents::Type type, const QVariant &data);

    // 具体处理函数（由 handleUiEvent 分发）
    void setSearchText(const QString &text);   // ← UiEvents::SearchEdited
    void setCategory(const QString &category); // ← UiEvents::TypeSelected
    void select(int mediaId);                  // ← UiEvents::ItemClicked

public slots:
    // 语言切换后由 main.cpp 连接的信号触发：重新下发带翻译的文字（categories、selectedItem）
    void retranslate();

private:
    void applySearch();             // 防抖计时结束后，把搜索词交给过滤模型
    void publishCounts();           // 下发 resultCount、totalCount、emptyState
    void publishCategories();       // 下发下拉框选项（带翻译）
    void publishSelectedItem();     // 下发详情数据（带格式化）
    LibraryData::EmptyState computeEmptyState() const;

    MediaLibraryModel *m_library = nullptr;   // 不拥有，由 main.cpp 管理生命周期
    MediaFilterModel *m_filter = nullptr;     // 拥有（Qt 父子关系自动释放）
    LibraryData m_data;                       // 下发数据（QML 只读它）
    QTimer m_searchDebounce;
};

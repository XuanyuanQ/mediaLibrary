#include "LibraryViewModel.h"

#include "MediaFilterModel.h"
#include "MediaLibraryModel.h"

// 已实现：搜索、类型筛选、计数、空状态、选中和详情。待实现：语言相关文字

LibraryViewModel::LibraryViewModel(MediaLibraryModel *library, QObject *parent)
    : QObject(parent)
    , m_library(library)
    , m_filter(new MediaFilterModel(this))
{
    // 代理的数据源 = 媒体库。setSourceModel() 内部 Qt 会自动：
    //   - connect 源模型的 modelReset / rowsInserted / rowsRemoved / dataChanged 等信号，
    //     源数据一变，代理就重新过滤，再把变化通知给 ListView（不需要我们手写 connect）
    //   - 立即对源模型逐行调用 filterAcceptsRow()，建立第一份“显示哪几行”的对照表
    m_filter->setSourceModel(m_library);

    // 防抖：每次输入都重新计时，停止输入 200ms 后才执行一次 applySearch()
    m_searchDebounce.setSingleShot(true);
    m_searchDebounce.setInterval(200);
    connect(&m_searchDebounce, &QTimer::timeout, this, &LibraryViewModel::applySearch);

    // 过滤结果的行数变化时（搜索后行被删除/加入、整体重置），通知 QML 刷新“结果数量”。
    // 注意：这里的 connect 只用于计数文字。列表本身的增删不靠它——
    // ListView 在 QML 设置 model 时，Qt 已经自动 connect 了代理的这些信号，自己会更新
    const auto onResultsChanged = [this] {
        emit resultCountChanged();
        emit emptyStateChanged();
    };
    connect(m_filter, &QAbstractItemModel::rowsInserted, this, onResultsChanged);
    connect(m_filter, &QAbstractItemModel::rowsRemoved, this, onResultsChanged);
    connect(m_filter, &QAbstractItemModel::modelReset, this, onResultsChanged);
    connect(m_filter, &QAbstractItemModel::layoutChanged, this, onResultsChanged);

    // 媒体库总数变化时，通知 QML 刷新“总数”
    connect(m_library, &MediaLibraryModel::countChanged, this, [this] {
        emit totalCountChanged();
        emit emptyStateChanged();
    });

    rebuildCategories();
}

QAbstractItemModel *LibraryViewModel::getItems() const
{
    return m_filter;
}

void LibraryViewModel::setSearchText(const QString &text)
{
    if (m_searchText == text)
        return;
    m_searchText = text;
    emit searchTextChanged();
    m_searchDebounce.start();   // 重新开始计时
}

void LibraryViewModel::setCategory(const QString &category)
{
    if (m_category == category)
        return;
    m_category = category;
    // 下拉框选一次就是最终结果，不需要像搜索那样防抖，立即交给过滤模型
    m_filter->setCategory(category);
    emit categoryChanged();
}

int LibraryViewModel::getResultCount() const
{
    return m_filter->rowCount();
}

int LibraryViewModel::getTotalCount() const
{
    return m_library ? m_library->getCount() : 0;
}

LibraryViewModel::EmptyState LibraryViewModel::getEmptyState() const
{
    // 先判断库是不是空的：库空时结果数也是 0，但原因不是“没匹配到”
    if (getTotalCount() == 0)
        return NoData;
    // 库里有数据，但搜索 / 类型筛选之后一行都不剩
    if (getResultCount() == 0)
        return NoMatch;
    return None;
}

void LibraryViewModel::setSearchDebounceInterval(int ms)
{
    m_searchDebounce.setInterval(ms);
}

void LibraryViewModel::select(int mediaId)
{
    // 谁调用：MediaRow.qml 的 onClicked: LibraryViewModel.select(root.mediaId)
    // （Q_INVOKABLE 让 QML 可以直接调用这个函数）
    if (m_selectedId == mediaId)
        return;
    m_selectedId = mediaId;
    emit selectedIdChanged();   // MediaRow 的 selected 绑定刷新 → 被点的那一行加橙色边框
    rebuildSelectedItem();      // 生成详情数据，并发出 selectedItemChanged → DetailView 刷新
}

void LibraryViewModel::retranslate()
{
    rebuildCategories();
    rebuildSelectedItem();
}

void LibraryViewModel::applySearch()
{
    // 防抖结束：把搜索词交给过滤模型。
    // 结果行数的变化会通过上面构造函数里连接的信号，自动通知 QML
    m_filter->setSearchText(m_searchText);
}

void LibraryViewModel::rebuildCategories()
{
    // 下拉框的选项：key 交给过滤模型（与数据里的 type 对应），label 显示在界面上。
    // label 用 tr() 包起来，以后做语言切换时可以翻译
    const auto option = [](const QString &key, const QString &label) {
        return QVariantMap{{QStringLiteral("key"), key}, {QStringLiteral("label"), label}};
    };
    m_categories = {
        option(QString(), tr("All types")),
        option(QStringLiteral("Movie"), tr("Movies")),
        option(QStringLiteral("Music"), tr("Music")),
        option(QStringLiteral("Ebook"), tr("Books")),
        option(QStringLiteral("Podcast"), tr("Podcasts")),
        option(QStringLiteral("Others"), tr("Others")),
    };
    emit categoriesChanged();
}

void LibraryViewModel::rebuildSelectedItem()
{
    // 详情页要显示的内容全部在这里准备好（包括格式化后的文字），QML 只负责摆放
    m_selectedItem.clear();

    const std::optional<MediaItem> item = m_library ? m_library->itemById(m_selectedId)
                                                    : std::nullopt;
    if (item) {
        // 类型显示名：四种主要类型和有声书用翻译后的名字，其他类型直接显示原文
        QString typeLabel = item->type;
        if (item->type == QLatin1String("Movie"))          typeLabel = tr("Movie");
        else if (item->type == QLatin1String("Music"))     typeLabel = tr("Music");
        else if (item->type == QLatin1String("Ebook"))     typeLabel = tr("Book");
        else if (item->type == QLatin1String("Podcast"))   typeLabel = tr("Podcast");
        else if (item->type == QLatin1String("Audiobook")) typeLabel = tr("Audiobook");

        // 附加信息一行：年份 · 时长（或页数），没有的项不显示。例如 "2021 · 45 min"
        QStringList meta;
        if (item->year > 0)
            meta << QString::number(item->year);
        if (item->durationSec > 0) {
            const int minutes = (item->durationSec + 30) / 60;   // 四舍五入到分钟
            meta << (minutes >= 60 ? tr("%1 h %2 min").arg(minutes / 60).arg(minutes % 60)
                                   : tr("%1 min").arg(minutes));
        }
        if (item->pages > 0)
            meta << tr("%1 pages").arg(item->pages);

        // QML 里用 LibraryViewModel.selectedItem.title 这样的写法读取
        m_selectedItem = {
            {QStringLiteral("title"), item->title},
            {QStringLiteral("creator"), item->creator},
            {QStringLiteral("typeLabel"), typeLabel},
            {QStringLiteral("metaText"), meta.join(QStringLiteral(" · "))},
            {QStringLiteral("description"), item->description},
        };
    }
    // 没找到（未选中）时 m_selectedItem 是空的，DetailView 据此显示“请选择”提示
    emit selectedItemChanged();
}

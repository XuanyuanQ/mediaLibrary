#include "LibraryManager.h"

#include <QDebug>

#include "MediaFilterModel.h"
#include "MediaLibraryModel.h"

// 已实现：搜索、类型筛选、计数、空状态、选中和详情。待实现：语言相关文字
// 所有下发都走统一入口 m_data.publish(UiData::名字, 值)，搜索 "publish(" 可以看到全部下发点

LibraryManager::LibraryManager(MediaLibraryModel *library, QObject *parent)
    : QObject(parent)
    , m_library(library)
    , m_filter(new MediaFilterModel(this))
    , m_data(m_filter)   // 列表（过滤模型）只在这里交给 LibraryData 一次
{
    // 代理的数据源 = 媒体库。setSourceModel() 内部 Qt 会自动：
    //   - connect 源模型的 modelReset / rowsInserted / rowsRemoved / dataChanged 等信号，
    //     源数据一变，代理就重新过滤，再把变化通知给 ListView（不需要我们手写 connect）
    //   - 立即对源模型逐行调用 filterAcceptsRow()，建立第一份“显示哪几行”的对照表
    m_filter->setSourceModel(m_library);

    // 防抖：每次输入都重新计时，停止输入 200ms 后才执行一次 applySearch()
    m_searchDebounce.setSingleShot(true);
    m_searchDebounce.setInterval(200);
    connect(&m_searchDebounce, &QTimer::timeout, this, &LibraryManager::applySearch);

    // 过滤结果的行数变化时（搜索后行被删除/加入、整体重置），重新下发计数和空状态。
    // 注意：这里的 connect 只用于计数文字。列表本身的增删不靠它——
    // ListView 在 QML 设置 model 时，Qt 已经自动 connect 了代理的这些信号，自己会更新
    connect(m_filter, &QAbstractItemModel::rowsInserted, this, &LibraryManager::publishCounts);
    connect(m_filter, &QAbstractItemModel::rowsRemoved, this, &LibraryManager::publishCounts);
    connect(m_filter, &QAbstractItemModel::modelReset, this, &LibraryManager::publishCounts);
    connect(m_filter, &QAbstractItemModel::layoutChanged, this, &LibraryManager::publishCounts);
    // 媒体库总数变化时（重新加载数据），同样重新下发
    connect(m_library, &MediaLibraryModel::countChanged, this, &LibraryManager::publishCounts);

    // 启动时下发一次初始数据
    publishCounts();
    publishCategories();
}

void LibraryManager::handleUiEvent(UiEvents::Type type, const QVariant &data)
{
    // 谁调用：QML 里 UiEvents.post(...) → main.cpp 的 connect → 这里。
    // 所有模块都会收到每一条消息，不是媒体库模块的直接忽略
    if (UiEvents::moduleOf(type) != UiEvents::Library)
        return;

    // data 是 QVariant，没有编译期类型检查，所以先检查类型，不对就打警告并丢弃
    switch (type) {
    case UiEvents::SearchEdited:
    case UiEvents::TypeSelected:
        if (data.typeId() != QMetaType::QString) {
            qWarning() << "UiEvents:" << type << "expects a string, got" << data;
            return;
        }
        if (type == UiEvents::SearchEdited)
            setSearchText(data.toString());
        else
            setCategory(data.toString());
        break;

    case UiEvents::ItemClicked: {
        bool ok = false;
        const int id = data.toInt(&ok);   // QML 传来的数字可能是 int 也可能是 double，都能转
        if (!ok) {
            qWarning() << "UiEvents:" << type << "expects a media id, got" << data;
            return;
        }
        select(id);
        break;
    }

    default:
        qWarning() << "UiEvents: unhandled library message" << type;
        break;
    }
}

void LibraryManager::setSearchText(const QString &text)
{
    // 谁调用：handleUiEvent() 收到 SearchEdited（来自 SearchField.qml）
    if (m_data.getSearchText() == text)
        return;
    m_data.publish(UiData::SearchText, text);   // 下发：搜索框文字立即更新（✕ 清空就是靠它把框清空）
    m_searchDebounce.start();     // 过滤要等停止输入 200ms 后再做（重新开始计时）
}

void LibraryManager::setCategory(const QString &category)
{
    // 谁调用：handleUiEvent() 收到 TypeSelected（来自 TypeFilter.qml）
    if (m_data.getCategory() == category)
        return;
    // 下拉框选一次就是最终结果，不需要像搜索那样防抖，立即交给过滤模型
    // （过滤后行数变化 → publishCounts() 下发计数）
    m_filter->setCategory(category);
    m_data.publish(UiData::Category, category);   // 下发：当前类型（下拉框据此变橙色）
}

void LibraryManager::publishCounts()
{
    // 谁调用：过滤结果行数变化、媒体库重新加载、构造函数
    m_data.publish(UiData::ResultCount, m_filter->rowCount());                    // 下发：结果数
    m_data.publish(UiData::TotalCount, m_library ? m_library->getCount() : 0);    // 下发：总数
    m_data.publish(UiData::EmptyState, QVariant::fromValue(computeEmptyState())); // 下发：空状态
}

LibraryData::EmptyState LibraryManager::computeEmptyState() const
{
    // 先判断库是不是空的：库空时结果数也是 0，但原因不是“没匹配到”
    if (!m_library || m_library->getCount() == 0)
        return LibraryData::NoData;
    // 库里有数据，但搜索 / 类型筛选之后一行都不剩
    if (m_filter->rowCount() == 0)
        return LibraryData::NoMatch;
    return LibraryData::None;
}

void LibraryManager::select(int mediaId)
{
    // 谁调用：handleUiEvent() 收到 ItemClicked（来自 MediaRow.qml 的点击）
    if (m_data.getSelectedId() == mediaId)
        return;
    m_data.publish(UiData::SelectedId, mediaId);   // 下发：MediaRow 的 selected 绑定刷新 → 被点的那一行加橙色边框
    publishSelectedItem();           // 下发：详情数据 → DetailView 刷新
}

void LibraryManager::retranslate()
{
    publishCategories();
    publishSelectedItem();
}

void LibraryManager::applySearch()
{
    // 防抖结束：把搜索词交给过滤模型。
    // 结果行数的变化会通过构造函数里连接的信号，触发 publishCounts() 下发计数
    m_filter->setSearchText(m_data.getSearchText());
}

void LibraryManager::publishCategories()
{
    // 下拉框的选项：key 交给过滤模型（与数据里的 type 对应），label 显示在界面上。
    // label 用 tr() 包起来，以后做语言切换时可以翻译
    const auto option = [](const QString &key, const QString &label) {
        return QVariantMap{{QStringLiteral("key"), key}, {QStringLiteral("label"), label}};
    };
    m_data.publish(UiData::Categories, QVariantList{   // 下发：下拉框选项
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
    // 详情页要显示的内容全部在这里准备好（包括格式化后的文字），QML 只负责摆放
    QVariantMap detail;   // 没找到（未选中）时保持为空，DetailView 据此显示“请选择”提示

    const std::optional<MediaItem> item = m_library ? m_library->itemById(m_data.getSelectedId())
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

        // QML 里用 LibraryData.selectedItem.title 这样的写法读取
        detail = {
            {QStringLiteral("title"), item->title},
            {QStringLiteral("creator"), item->creator},
            {QStringLiteral("typeLabel"), typeLabel},
            {QStringLiteral("metaText"), meta.join(QStringLiteral(" · "))},
            {QStringLiteral("description"), item->description},
        };
    }
    m_data.publish(UiData::SelectedItem, detail);   // 下发：详情
}

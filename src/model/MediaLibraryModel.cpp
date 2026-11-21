#include "MediaLibraryModel.h"

// itemById() 供详情页使用，待实现

MediaLibraryModel::MediaLibraryModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

// ---------------------------------------------------------------------------
// rowCount() / data() / roleNames() 都是 QAbstractItemModel 的虚函数，
// 我们只负责“回答问题”，从不自己调用它们。调用者都在 Qt 内部：
//   - MediaFilterModel（代理）：建立“显示哪几行”的对照表时调用 rowCount()，
//     过滤时在 filterAcceptsRow() 里通过 idx.data() 调用 data()，
//     ListView 向代理要数据时，代理换算行号后再转调这里的 data()
//   - ListView（经过代理）：接上模型时调用 roleNames()，
//     为屏幕上可见的每一行创建 MediaRow 时，对每个字段调用一次 data()
// ---------------------------------------------------------------------------

int MediaLibraryModel::rowCount(const QModelIndex &parent) const
{
    // 列表模型没有子节点：只有根节点（无效的 parent）才有行
    return parent.isValid() ? 0 : int(m_items.size());
}

QVariant MediaLibraryModel::data(const QModelIndex &index, int role) const
{
    // index：Qt 传进来的“坐标卡片”，index.row() 就是要取的第几行（已经是本模型的行号，
    //        代理已经替我们把“屏幕上的第几行”换算好了）
    // role： 要取哪个字段（TitleRole、CreatorRole …），对应 roleNames() 里的名字
    // 视图可能传来越界或无效的索引，先检查
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size())
        return {};

    // 按角色（role）返回这一行的某个字段；QML 委托里的 title、creator 等就是从这里取的
    const MediaItem &item = m_items.at(index.row());
    switch (role) {
    case Qt::DisplayRole:   // 默认显示角色也返回标题，方便调试和通用视图
    case TitleRole:   return item.title;
    case IdRole:      return item.id;
    case CreatorRole: return item.creator;
    case TypeRole:    return item.type;
    case YearRole:    return item.year;
    default:          return {};
    }
}

QHash<int, QByteArray> MediaLibraryModel::roleNames() const
{
    // 这些名字就是 QML 委托里可以直接使用的属性名：
    // ListView 创建 MediaRow 时，会把这一行的数据自动填进 MediaRow 里
    // 同名的 required property（title、creator …），不需要手动传递
    return {
        {IdRole, "mediaId"},   // 不用 "id"：id 在 QML 中是保留字
        {TitleRole, "title"},
        {CreatorRole, "creator"},
        {TypeRole, "type"},
        {YearRole, "year"},
    };
}

void MediaLibraryModel::setItems(const QList<MediaItem> &items)
{
    // 整体替换数据：begin/endResetModel 通知所有视图“数据全换了，请重新读取”
    beginResetModel();   // Qt 在这里发出 modelAboutToBeReset 信号
    m_items = items;
    endResetModel();     // Qt 在这里发出 modelReset 信号（同步，函数返回前接收者已处理完）：
                         //   代理 MediaFilterModel 收到（setSourceModel 时 Qt 自动 connect 的）
                         //   → 重新逐行过滤 → 再向 ListView 发出自己的 modelReset → 列表重建
    emit countChanged(); // count 是我们自己加的属性，Qt 不会帮我们通知，要手动发
}

std::optional<MediaItem> MediaLibraryModel::itemById(int id) const
{
    // TODO: 按 id 查找
    Q_UNUSED(id)
    return std::nullopt;
}

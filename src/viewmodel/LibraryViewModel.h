#pragma once

#include <QAbstractItemModel>
#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>

#include "UiEvents.h"

class MediaLibraryModel;
class MediaFilterModel;

/*
 * 【ViewModel 层】媒体库页面的全部状态和界面逻辑。
 *
 * View（QML）只做两件事：
 *   1. 绑定这里的属性来显示（items、resultCount、selectedItem …）
 *   2. 用户操作不直接调用这里，而是 UiEvents.post(类型, 数据) 上发，
 *      main.cpp 把 UiEvents::post connect 到这里的 handleUiEvent()，再分发给 setSearchText / setCategory / select
 *
 * 这个类不引用任何 QML 类型，可以直接在单元测试里 new 出来测试。
 *
 * 普通类。main.cpp 创建对象后用 qmlRegisterSingletonInstance 注册成 QML 单例，
 * QML 里 import MediaLibrary.Core 之后直接写 LibraryViewModel.xxx。
 */
class LibraryViewModel : public QObject
{
    Q_OBJECT

    // 这里的 Q_PROPERTY 就是【下发接口】：QML 只读，不能写。
    // Q_PROPERTY(类型 名字 READ 读函数 NOTIFY 变化信号)：
    //   - QML 读取 LibraryViewModel.名字 时，Qt 调用 READ 函数
    //   - 值变了，C++ 要自己 emit NOTIFY 信号；QML 收到后，所有绑定了这个属性的地方自动刷新
    //   - 不写 WRITE：QML 不能给属性赋值，改数据只能通过 UiEvents 上发
    //   - 这套对应关系由 moc 在编译时根据这里的声明生成代码，所以看不到显式的调用
    //   - CONSTANT：属性永远不变，不需要 WRITE 和 NOTIFY

    // ---- 列表 ----
    // 过滤后的列表（内部是 MediaFilterModel），ListView 直接绑定
    Q_PROPERTY(QAbstractItemModel *items READ getItems CONSTANT)

    // ---- 搜索 / 筛选 ----
    // 输入框内容立即更新；停止输入 200ms 后才真正过滤（防抖）
    Q_PROPERTY(QString searchText READ getSearchText NOTIFY searchTextChanged)
    // ""=全部；"Others"=四个主要类型以外的
    Q_PROPERTY(QString category READ getCategory NOTIFY categoryChanged)
    // 下拉框选项 [{key, label}]，label 已翻译，切换语言后自动更新
    Q_PROPERTY(QVariantList categories READ getCategories NOTIFY categoriesChanged)

    // ---- 计数 / 空状态 ----
    Q_PROPERTY(int resultCount READ getResultCount NOTIFY resultCountChanged)
    Q_PROPERTY(int totalCount READ getTotalCount NOTIFY totalCountChanged)
    Q_PROPERTY(EmptyState emptyState READ getEmptyState NOTIFY emptyStateChanged)

    // ---- 选中 / 详情 ----
    // 记 ID 不记行号：搜索、排序后行号会变，ID 不变
    Q_PROPERTY(int selectedId READ getSelectedId NOTIFY selectedIdChanged)
    // 详情页直接绑定；包含已格式化好的显示文字（durationText、categoryLabel 等）
    Q_PROPERTY(QVariantMap selectedItem READ getSelectedItem NOTIFY selectedItemChanged)

public:
    // 空状态：View 根据它决定显示哪种提示
    enum EmptyState {
        None,      // 有结果，不显示提示
        NoData,    // 媒体库本身就是空的（或加载失败）
        NoMatch,   // 有数据，但当前搜索 / 筛选没有匹配项
    };
    Q_ENUM(EmptyState)

    explicit LibraryViewModel(MediaLibraryModel *library, QObject *parent = nullptr);

    QAbstractItemModel *getItems() const;

    QString getSearchText() const { return m_searchText; }
    QString getCategory() const { return m_category; }

    QVariantList getCategories() const { return m_categories; }

    int getResultCount() const;
    int getTotalCount() const;
    EmptyState getEmptyState() const;

    int getSelectedId() const { return m_selectedId; }
    QVariantMap getSelectedItem() const { return m_selectedItem; }

    // 防抖间隔（毫秒）。测试里可以设成 0，不用真的等待
    void setSearchDebounceInterval(int ms);

    // ---- 处理上发消息 ----
    // 入口：main.cpp 把 UiEvents::post connect 到这里。只处理媒体库模块（100~199）的消息
    void handleUiEvent(UiEvents::Type type, const QVariant &data);

    // 具体处理函数（由 handleUiEvent 分发；测试里也可以直接调用）。
    // 普通 public 函数，不加 Q_INVOKABLE：QML 不能直接调用
    void setSearchText(const QString &text);   // ← UiEvents::SearchEdited
    void setCategory(const QString &category); // ← UiEvents::TypeSelected
    void select(int mediaId);                  // ← UiEvents::ItemClicked

public slots:
    // 语言切换后由 main.cpp 连接的信号触发：重建 categories 和 selectedItem 的显示文字
    void retranslate();

signals:
    void searchTextChanged();
    void categoryChanged();
    void categoriesChanged();
    void resultCountChanged();
    void totalCountChanged();
    void emptyStateChanged();
    void selectedIdChanged();
    void selectedItemChanged();

private:
    void applySearch();          // 防抖计时结束后，把 searchText 交给过滤模型
    void rebuildCategories();    // 生成下拉框选项（带翻译）
    void rebuildSelectedItem();  // 根据 selectedId 生成详情数据（带格式化）

    MediaLibraryModel *m_library = nullptr;   // 不拥有，由 main.cpp 管理生命周期
    MediaFilterModel *m_filter = nullptr;     // 拥有（Qt 父子关系自动释放）
    QTimer m_searchDebounce;

    QString m_searchText;
    QString m_category;
    QVariantList m_categories;
    int m_selectedId = -1;
    QVariantMap m_selectedItem;
};

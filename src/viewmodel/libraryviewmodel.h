#pragma once

#include <QAbstractItemModel>
#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

class MediaLibraryModel;
class MediaFilterModel;

/*
 * 【ViewModel 层】媒体库页面的全部状态和界面逻辑。
 *
 * View（QML）只做两件事：
 *   1. 绑定这里的属性来显示（items、resultCount、selectedItem …）
 *   2. 把用户操作转成对这里的调用（searchText = …、select(id) …）
 *
 * 这个类不引用任何 QML 类型，可以直接在单元测试里 new 出来测试。
 * 宽屏、窄屏两种布局共用同一个实例，所以切换布局时状态不会丢。
 *
 * 由 main.cpp 创建并通过 setInitialProperties 注入 QML（QML 不能自己创建）。
 */
class LibraryViewModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("LibraryViewModel is created in C++ (main.cpp) and injected")

    // ---- 列表 ----
    // 过滤后的列表（内部是 MediaFilterModel），ListView 直接绑定
    Q_PROPERTY(QAbstractItemModel *items READ items CONSTANT)

    // ---- 搜索 / 筛选 ----
    // 输入框内容立即更新；停止输入 200ms 后才真正过滤（防抖）
    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)
    // ""=全部；"Others"=四个主要类型以外的
    Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)
    // 下拉框选项 [{key, label}]，label 已翻译，切换语言后自动更新
    Q_PROPERTY(QVariantList categories READ categories NOTIFY categoriesChanged)

    // ---- 计数 / 空状态 ----
    Q_PROPERTY(int resultCount READ resultCount NOTIFY resultCountChanged)
    Q_PROPERTY(int totalCount READ totalCount NOTIFY totalCountChanged)
    Q_PROPERTY(EmptyState emptyState READ emptyState NOTIFY emptyStateChanged)

    // ---- 选中 / 详情 ----
    // 记 ID 不记行号：搜索、排序后行号会变，ID 不变
    Q_PROPERTY(int selectedId READ selectedId NOTIFY selectedIdChanged)
    // 详情页直接绑定；包含已格式化好的显示文字（durationText、categoryLabel 等）
    Q_PROPERTY(QVariantMap selectedItem READ selectedItem NOTIFY selectedItemChanged)

public:
    // 空状态：View 根据它决定显示哪种提示
    enum EmptyState {
        None,      // 有结果，不显示提示
        NoData,    // 媒体库本身就是空的（或加载失败）
        NoMatch,   // 有数据，但当前搜索 / 筛选没有匹配项
    };
    Q_ENUM(EmptyState)

    explicit LibraryViewModel(MediaLibraryModel *library, QObject *parent = nullptr);

    QAbstractItemModel *items() const;

    QString searchText() const { return m_searchText; }
    void setSearchText(const QString &text);

    QString category() const { return m_category; }
    void setCategory(const QString &category);

    QVariantList categories() const { return m_categories; }

    int resultCount() const;
    int totalCount() const;
    EmptyState emptyState() const;

    int selectedId() const { return m_selectedId; }
    QVariantMap selectedItem() const { return m_selectedItem; }

    // 防抖间隔（毫秒）。测试里可以设成 0，不用真的等待
    void setSearchDebounceInterval(int ms);

    // ---- 供 View 调用的操作 ----
    Q_INVOKABLE void select(int mediaId);
    Q_INVOKABLE void clearSelection();
    Q_INVOKABLE void clearFilters();

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

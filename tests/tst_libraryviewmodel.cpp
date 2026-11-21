// 【测试】LibraryViewModel：界面状态和界面逻辑（不启动 QML）
// 骨架阶段：测试用例已列出，实现完成后逐个去掉 QSKIP 并补充断言
#include <QtTest>

#include "LibraryViewModel.h"
#include "MediaLibraryModel.h"

class TestLibraryViewModel : public QObject
{
    Q_OBJECT

private slots:
    // 架构检查：ViewModel 可以脱离 QML 直接创建，items 指向过滤模型
    void constructsWithoutQml()
    {
        MediaLibraryModel library;
        LibraryViewModel vm(&library);
        QVERIFY(vm.getItems() != nullptr);
        QCOMPARE(vm.getSelectedId(), -1);
    }

    void debounceAppliesSearchOnce()          { QSKIP("Not implemented yet"); }
    void selectAndClearSelection()            { QSKIP("Not implemented yet"); }
    void selectionSurvivesFiltering()         { QSKIP("Not implemented yet"); }
    void emptyStateNoData()                   { QSKIP("Not implemented yet"); }
    void emptyStateNoMatch()                  { QSKIP("Not implemented yet"); }
    void clearFiltersResetsSearchAndCategory(){ QSKIP("Not implemented yet"); }
    void countsEmitSignals()                  { QSKIP("Not implemented yet"); }
    void retranslateRebuildsDisplayText()     { QSKIP("Not implemented yet"); }
};

QTEST_GUILESS_MAIN(TestLibraryViewModel)
#include "tst_libraryviewmodel.moc"

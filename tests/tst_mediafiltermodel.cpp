// 【测试】MediaFilterModel：搜索、类型筛选、排序
// 骨架阶段：测试用例已列出，实现完成后逐个去掉 QSKIP 并补充断言
#include <QtTest>

#include "MediaFilterModel.h"
#include "MediaLibraryModel.h"

class TestMediaFilterModel : public QObject
{
    Q_OBJECT

private slots:
    void emptySearchShowsAll()               { QSKIP("Not implemented yet"); }
    void matchesTitle()                      { QSKIP("Not implemented yet"); }
    void matchesCreator()                    { QSKIP("Not implemented yet"); }
    void isCaseInsensitive()                 { QSKIP("Not implemented yet"); }
    void multipleTermsAreAnded()             { QSKIP("Not implemented yet"); }
    void filtersByCategory()                 { QSKIP("Not implemented yet"); }
    void othersCategoryMatchesNonPrimary()   { QSKIP("Not implemented yet"); }
    void combinesSearchAndCategory()         { QSKIP("Not implemented yet"); }
    void sortsByTitle()                      { QSKIP("Not implemented yet"); }
};

QTEST_GUILESS_MAIN(TestMediaFilterModel)
#include "tst_mediafiltermodel.moc"

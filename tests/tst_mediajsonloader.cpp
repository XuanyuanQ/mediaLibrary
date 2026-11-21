// 【测试】MediaJsonLoader：各种正常 / 异常数据的加载
// 骨架阶段：测试用例已列出，实现完成后逐个去掉 QSKIP 并补充断言
#include <QtTest>

#include "MediaJsonLoader.h"

class TestMediaJsonLoader : public QObject
{
    Q_OBJECT

private slots:
    void loadsValidFile()
    {
        // 期望：4 条记录全部加载，字段正确（含 pages、duration）
        QSKIP("Not implemented yet: " TEST_DATA_DIR "/sample_valid.json");
    }

    void reportsMissingFile()
    {
        // 期望：ok() == false，error 说明文件打不开
        QSKIP("Not implemented yet");
    }

    void reportsBrokenJson()
    {
        // 期望：sample_broken.json -> ok() == false
        QSKIP("Not implemented yet");
    }

    void skipsInvalidRecords()
    {
        // 期望：sample_invalid_records.json -> 只加载 1 条，warnings 有 2 条
        QSKIP("Not implemented yet");
    }
};

QTEST_GUILESS_MAIN(TestMediaJsonLoader)
#include "tst_mediajsonloader.moc"

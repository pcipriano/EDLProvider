#include "PathAppenderTest.h"

#include "PathAppender.h"

using namespace common::util::tests;

void PathAppenderTest::initTestCase_data()
{
    QTest::addColumn<QString>("path1");
    QTest::addColumn<QString>("path2");
    QTest::addColumn<QString>("result");

#ifdef Q_OS_WIN
    QTest::newRow("Working path") << "c:\\program files\\app" << "config" << "c:/program files/app/config";
#else
    QTest::newRow("Working path") << "/tmp/test" << "config" << "/tmp/test/config";
#endif
}

void PathAppenderTest::combineTest()
{
    QFETCH_GLOBAL(QString, path1);
    QFETCH_GLOBAL(QString, path2);
    QFETCH_GLOBAL(QString, result);

    QCOMPARE(PathAppender::combine(path1, path2), result);
    QCOMPARE(PathAppender(path1, path2).toString(), result);
}

void PathAppenderTest::operatorTest()
{
    QFETCH_GLOBAL(QString, path1);
    QFETCH_GLOBAL(QString, path2);
    QFETCH_GLOBAL(QString, result);

    PathAppender appender(path1);
    appender << path2;
    QCOMPARE(appender.toString(), result);
}

DECLARE_TEST(PathAppenderTest)

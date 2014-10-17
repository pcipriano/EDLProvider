#include "TimeCodeTest.h"

#include "TimeCode.h"

using namespace timeutils::tests;

void TimeCodeTest::fromTimecodeStringTest_data()
{
    QTest::addColumn<QString>("timecode");
    QTest::addColumn<int32_t>("rateNum");
    QTest::addColumn<int32_t>("rateDen");
    QTest::addColumn<bool>("valid");
    QTest::addColumn<int32_t>("hours");
    QTest::addColumn<int32_t>("minutes");
    QTest::addColumn<int32_t>("seconds");
    QTest::addColumn<int32_t>("frames");

    QTest::newRow("Valid PAL") << "10:00:00:00" << 25 << 1 << true << 10 << 0 << 0 << 0;
    QTest::newRow("Valid NTSC") << "00:04:04;27" << 30000 << 1001 << true << 0 << 4 << 4 << 27;
    QTest::newRow("Invalid NTSC") << "00:01:00;00" << 30000 << 1001 << true << 0 << 1 << 0 << 2;
}

void TimeCodeTest::fromTimecodeStringTest()
{
    QFETCH(QString, timecode);
    QFETCH(int32_t, rateNum);
    QFETCH(int32_t, rateDen);
    QFETCH(bool, valid);
    QFETCH(int32_t, hours);
    QFETCH(int32_t, minutes);
    QFETCH(int32_t, seconds);
    QFETCH(int32_t, frames);

    Timecode tc = Timecode::fromTimecodeString(timecode.toStdWString(), rateNum, rateDen);

    QCOMPARE(!tc.isInvalid(), valid);
    QCOMPARE((int32_t) tc.getHour(), hours);
    QCOMPARE((int32_t) tc.getMin(), minutes);
    QCOMPARE((int32_t) tc.getSec(), seconds);
    QCOMPARE((int32_t) tc.getFrame(), frames);
}

void TimeCodeTest::getTotalFramesTest_data()
{
    QTest::addColumn<int32_t>("rateNum");
    QTest::addColumn<int32_t>("rateDen");
    QTest::addColumn<int32_t>("hours");
    QTest::addColumn<int32_t>("minutes");
    QTest::addColumn<int32_t>("seconds");
    QTest::addColumn<int32_t>("frames");
    QTest::addColumn<bool>("dropFrame");
    QTest::addColumn<uint64_t>("framecount");


    QTest::newRow("Non drop frame count 1") << 25 << 1 << 10 << 0 << 0 << 0 << false << (uint64_t) 900000;
    QTest::newRow("Non drop frame count 2") << 25 << 1 << 0 << 5 << 22 << 12 << false << (uint64_t) 8062;
    QTest::newRow("Non drop frame count 3, frames overflow") << 25 << 1 << 4 << 20 << 3 << 36 << false << (uint64_t) 390086;

    QTest::newRow("Drop frame count 1") << 30000 << 1001 << 10 << 0 << 0 << 0 << true << (uint64_t) 1078920;
    QTest::newRow("Drop frame count 2") << 30000 << 1001 << 0 << 5 << 22 << 12 << true << (uint64_t) 9662;
    QTest::newRow("Drop frame count 3, frames overflow") << 30000 << 1001 << 4 << 20 << 3 << 45 << true << (uint64_t) 467637;
}

void TimeCodeTest::getTotalFramesTest()
{
    QFETCH(int32_t, rateNum);
    QFETCH(int32_t, rateDen);
    QFETCH(int32_t, hours);
    QFETCH(int32_t, minutes);
    QFETCH(int32_t, seconds);
    QFETCH(int32_t, frames);
    QFETCH(bool, dropFrame);
    QFETCH(uint64_t, framecount);

    Timecode tc(rateNum, rateDen, dropFrame, hours, minutes, seconds, frames);

    QCOMPARE(tc.getTotalFrames(), framecount);
}

void TimeCodeTest::fromMillisecondsTest_data()
{
    QTest::addColumn<double>("milliseconds");
    QTest::addColumn<int32_t>("rateNum");
    QTest::addColumn<int32_t>("rateDen");
    QTest::addColumn<bool>("dropFrame");
    QTest::addColumn<uint64_t>("framecount");
    QTest::addColumn<int16_t>("hours");
    QTest::addColumn<int16_t>("minutes");
    QTest::addColumn<int16_t>("seconds");
    QTest::addColumn<int16_t>("frames");

    QTest::newRow("Non drop milliseconds 1") << 36000000.0 << 25 << 1 << false << (uint64_t) 900000 << (int16_t) 10 << (int16_t) 0 << (int16_t) 0 << (int16_t) 0;
    QTest::newRow("Non drop milliseconds 2") << 322480.0 << 25 << 1 << false << (uint64_t) 8062 << (int16_t) 0 << (int16_t) 5 << (int16_t) 22 << (int16_t) 12;
    QTest::newRow("Non drop milliseconds 3") << 15603440.0 << 25 << 1 << false << (uint64_t) 390086 << (int16_t) 4 << (int16_t) 20 << (int16_t) 3 << (int16_t) 11;

    QTest::newRow("Drop milliseconds 1") << 36000000.0 << 30000 << 1001 << true << (uint64_t) 1078920 << (int16_t) 10 << (int16_t) 0 << (int16_t) 0 << (int16_t) 0;
    QTest::newRow("Drop milliseconds 2") << 322400.4 << 30000 << 1001 << true << (uint64_t) 9662 << (int16_t) 0 << (int16_t) 5 << (int16_t) 22 << (int16_t) 12;
    QTest::newRow("Drop milliseconds 3") << 15603500.5 << 30000 << 1001 << true << (uint64_t) 467637 << (int16_t) 4 << (int16_t) 20 << (int16_t) 3 << (int16_t) 15;
    QTest::newRow("Drop milliseconds 4") << 600000.0 << 30000 << 1001 << true << (uint64_t) 17982 << (int16_t) 0 << (int16_t) 10 << (int16_t) 0 << (int16_t) 0;
}

void TimeCodeTest::fromMillisecondsTest()
{
    QFETCH(double, milliseconds);
    QFETCH(int32_t, rateNum);
    QFETCH(int32_t, rateDen);
    QFETCH(bool, dropFrame);
    QFETCH(uint64_t, framecount);
    QFETCH(int16_t, hours);
    QFETCH(int16_t, minutes);
    QFETCH(int16_t, seconds);
    QFETCH(int16_t, frames);

    Timecode tc = Timecode::fromMilliseconds(milliseconds, rateNum, rateDen, dropFrame);

    QCOMPARE(tc.getTotalFrames(), framecount);
    QCOMPARE(tc.getHour(), hours);
    QCOMPARE(tc.getMin(), minutes);
    QCOMPARE(tc.getSec(), seconds);
    QCOMPARE(tc.getFrame(), frames);
}

DECLARE_TEST(TimeCodeTest)

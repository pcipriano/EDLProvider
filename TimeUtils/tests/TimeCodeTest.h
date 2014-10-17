#ifndef TIMEUTILS_TESTS_TIMECODETEST_H
#define TIMEUTILS_TESTS_TIMECODETEST_H

#include "AutoTest.h"

namespace timeutils
{
namespace tests
{

class TimeCodeTest final : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void fromTimecodeStringTest_data();

    void fromTimecodeStringTest();

    void getTotalFramesTest_data();

    void getTotalFramesTest();

    void fromMillisecondsTest_data();

    void fromMillisecondsTest();
};

}
}

#endif // TIMEUTILS_TESTS_TIMECODETEST_H

#ifndef COMMON_TESTS_PATHAPPENDERTEST_H
#define COMMON_TESTS_PATHAPPENDERTEST_H

#include "AutoTest.h"

namespace common
{
namespace util
{
namespace tests
{

class PathAppenderTest final : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase_data();

    void combineTest();

    void operatorTest();
};

}
}
}

#endif // COMMON_TESTS_PATHAPPENDERTEST_H

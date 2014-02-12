#ifndef COMMON_TESTS_PIMPLTEST_H
#define COMMON_TESTS_PIMPLTEST_H

#include "AutoTest.h"

namespace common
{
namespace util
{
namespace tests
{

class PimplTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void pimplConstructionTest();
};

}
}
}

#endif // COMMON_TESTS_PIMPLTEST_H

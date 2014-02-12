#include "PimplTest.h"

#include "Pimpl.h"

using namespace common::util::tests;

class TestClass
{
public:
    TestClass(int value1, const QString& value2 = QString())
        : impl_(value1, value2)
    {
    }

    int getValue() const
    {
        return impl_->value1_;
    }

    QString getValueString() const
    {
        return impl_->value2_;
    }

private:
    struct Impl
    {
        int value1_;
        QString value2_;

        Impl(int value)
            : value1_(value)
        {
        }

        Impl(int value1, const QString& value2)
            : value1_(value1),
              value2_(value2)
        {
        }
    };
    common::util::Pimpl<Impl> impl_;
};

void PimplTest::pimplConstructionTest()
{
    TestClass instance(5);

    QCOMPARE(5, instance.getValue());

    TestClass instance2(3, "TestString");

    QCOMPARE(instance2.getValue(), 3);
    QCOMPARE(instance2.getValueString(), QString("TestString"));
}

DECLARE_TEST(PimplTest)

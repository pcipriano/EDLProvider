#include "PimplTest.h"

#include "Pimpl.h"

using namespace common::util::tests;

class TestClass
{
public:
    TestClass()
    {
    }

    explicit TestClass(int value1, const QString& value2 = QString())
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

    QString concatenate() const
    {
        return impl_->concatenate();
    }

private:
    struct Impl
    {
        int value1_;
        QString value2_;

        Impl()
            : value1_(50)
        {
        }

        Impl(int value)
            : value1_(value)
        {
        }

        Impl(int value1, const QString& value2)
            : value1_(value1),
              value2_(value2)
        {
        }

        QString concatenate() const
        {
            return QString("%1-%2").arg(value1_).arg(value2_);
        }
    };
    common::util::Pimpl<Impl> impl_;
};

void PimplTest::pimplConstructionTest()
{
    TestClass instance;

    QCOMPARE(50, instance.getValue());

    TestClass instance1(5);

    QCOMPARE(5, instance1.getValue());

    TestClass instance2(3, "TestString");

    QCOMPARE(instance2.getValue(), 3);
    QCOMPARE(instance2.getValueString(), QString("TestString"));
    QCOMPARE(instance2.concatenate(), QString("3-TestString"));
}

void PimplTest::pimplCopyTest()
{
    TestClass test(20, "TestCopy");

    TestClass testConstruct = test;
    QCOMPARE(testConstruct.getValue(), 20);
    QCOMPARE(testConstruct.getValueString(), QString("TestCopy"));
    QCOMPARE(testConstruct.concatenate(), QString("20-TestCopy"));

    TestClass testAssign(50, "Initial");
    testAssign = test;
    QCOMPARE(testAssign.getValue(), 20);
    QCOMPARE(testAssign.getValueString(), QString("TestCopy"));
    QCOMPARE(testAssign.concatenate(), QString("20-TestCopy"));
}

DECLARE_TEST(PimplTest)

#ifndef CONFIGURATIONREADERTEST_H
#define CONFIGURATIONREADERTEST_H

#include "AutoTest.h"

namespace common
{
namespace util
{
namespace tests
{

class ConfigurationReaderTest : public QObject
{
    Q_OBJECT
public:
    ConfigurationReaderTest();

private Q_SLOTS:
    void initTestCase_data();

    void loadFileTest();

private:
    QString validConfig_;
    QString invalidXmlConfig_;
    QString malConstructedXmlConfig_;
};

}
}
}

#endif // CONFIGURATIONREADERTEST_H

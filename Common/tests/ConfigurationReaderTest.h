#ifndef CONFIGURATIONREADERTEST_H
#define CONFIGURATIONREADERTEST_H

#include <QTemporaryFile>

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
    void initTestCase();

    void loadFileTest_data();

    void loadFileTest();

    void loadXmlTest_data();

    void loadXmlTest();

    void saveTest();

    void getValueTest();

    void getSectionTest();

    void setValueTest();

private:
    void setData();

    QTemporaryFile schemaFile_;

    QString validConfig_;
    QString invalidXmlConfig_;
    QString malConstructedXmlConfig_;
    QString validationSchema_;
};

}
}
}

#endif // CONFIGURATIONREADERTEST_H

#include "ConfigurationReaderTest.h"

#include <QTemporaryFile>

#include "ConfigurationReader.h"

using namespace common::util::tests;

ConfigurationReaderTest::ConfigurationReaderTest()
{
    validConfig_ = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                           "<root>"
                               "<section1 sectionId=\"1\">"
                                   "<subsection subsectionId=\"2\">"
                                       "<value1>Value number 1</value1>"
                                       "<value2>Value number 1</value2>"
                                       "<value3>Value number 1</value3>"
                                       "<value4>Value number 1</value4>"
                                       "<value5>Value number 1</value5>"
                                       "<value6>Value number 1</value6>"
                                   "</subsection>"
                               "</section1>"
                               "<section2>"
                                   "<subsection>"
                                       "<value1>Value number 1</value1>"
                                   "</subsection>"
                                   "<othersection subsectionId=\"3\">"
                                       "<avalue>Another value1</avalue>"
                                       "<bvalue>Another value1</bvalue>"
                                   "</othersection>"
                               "</section2>"
                           "</root>");

    invalidXmlConfig_ = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                "<root>"
                                    "<section1 sectionId=\"1\">"
                                    "</section1>"
                                    "<section2>"
                                        "<subsection>"
                                            "<value1>Value number 1</value1>"
                                        "</subsection>"
                                        "<othersection subsectionId=\"3\">"
                                            "<avalue>Another value1</avalue>"
                                            "<bvalue>Another value1</bvalue>"
                                        "</othersection>"
                                    "</section2>"
                                "</root>");

    malConstructedXmlConfig_ = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                       "<root>"
                                           "<section1 sectionId=\"1\">"
                                               "<subsection subsectionId=\"2\">"
                                                   "<value1>Value number 1</value1>"
                                                   "<value2>Value number 1</value2>"
                                                   "<value3>Value number 1</value3>"
                                                   "<value4>Value number 1</value4>"
                                                   "<value5>Value number 1</value5>"
                                                   "<value6>Value number 1</value6>"
                                               "</subsection>"
                                           "</wrongendtag>"
                                           "<section2>"
                                               "<subsection>"
                                                   "<value1>Value number 1</value1>"
                                               "</subsection>"
                                               "<othersection subsectionId=\"3\">"
                                                   "<avalue>Another value1</avalue>"
                                                   "<bvalue>Another value1</avalue>"
                                               "</othersection>"
                                           "</section2>"
                                       "</root>");
}

void ConfigurationReaderTest::initTestCase_data()
{
    QTest::addColumn<QString>("xml");
    QTest::addColumn<bool>("result");

    QTest::newRow("Valid XML") << validConfig_ << true;
    QTest::newRow("Invalid XML") << validConfig_ << false;
    QTest::newRow("Badly constructed XML") << validConfig_ << false;
}

void ConfigurationReaderTest::loadFileTest()
{
    QFETCH_GLOBAL(QString, xml);
    QFETCH_GLOBAL(bool, result);

    QTemporaryFile file;
    if (file.open())
    {
        file.write(xml.toUtf8());
        QVERIFY(file.flush());

        ConfigurationReader confReader;
        QCOMPARE(confReader.loadFile(file.fileName()), result);
    }
}

DECLARE_TEST(ConfigurationReaderTest)

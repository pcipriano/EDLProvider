#include "ConfigurationReaderTest.h"

#include <QTextStream>
#include <QTemporaryDir>

#include "PathAppender.h"
#include "ConfigurationReader.h"

using namespace common::util::tests;

ConfigurationReaderTest::ConfigurationReaderTest()
{
    validConfig_ = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                           "<root rootId=\"1\">"
                               "<section1 sectionId=\"1\">"
                                   "<subsection subsectionId=\"2\">"
                                       "<value1>Value number 1</value1>"
                                       "<value2>Value number 2</value2>"
                                       "<value3>Value number 3</value3>"
                                       "<value4>Value number 4</value4>"
                                       "<value5>Value number 5</value5>"
                                       "<value6>Value number 6</value6>"
                                   "</subsection>"
                               "</section1>"
                               "<section2>"
                                   "<subsection>"
                                       "<value1>Value number 1</value1>"
                                   "</subsection>"
                                   "<othersection subsectionId=\"3\">"
                                       "<avalue>Another value1</avalue>"
                                       "<bvalue>Another value2</bvalue>"
                                       "<intvalue>30</intvalue>"
                                       "<boolvalue>true</boolvalue>"
                                       "<doublevalue>1.5</doublevalue>"
                                       "<datevalue>2012-05-07T08:17:25Z</datevalue>"
                                       "<urlvalue>http://test/resource.xml</urlvalue>"
                                   "</othersection>"
                               "</section2>"
                               "<value>Value Test</value>"
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
                                                   "<value2>Value number 2</value2>"
                                                   "<value3>Value number 3</value3>"
                                                   "<value4>Value number 4</value4>"
                                                   "<value5>Value number 5</value5>"
                                                   "<value6>Value number 6</value6>"
                                               "</subsection>"
                                           "</wrongendtag>"
                                           "<section2>"
                                               "<subsection>"
                                                   "<value1>Value number 1</value1>"
                                               "</subsection>"
                                               "<othersection subsectionId=\"3\">"
                                                   "<avalue>Another value1</avalue>"
                                                   "<bvalue>Another value2</avalue>"
                                               "</othersection>"
                                           "</section2>"
                                       "</root>");

    validationSchema_ = QString("<?xml version=\"1.0\" encoding=\"utf-8\"?>"
                                "<xs:schema attributeFormDefault=\"unqualified\" elementFormDefault=\"qualified\" xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">"
                                    "<xs:element name=\"root\">"
                                        "<xs:complexType>"
                                            "<xs:sequence>"
                                                "<xs:element name=\"section1\">"
                                                    "<xs:complexType>"
                                                        "<xs:sequence>"
                                                            "<xs:element name=\"subsection\">"
                                                                "<xs:complexType>"
                                                                    "<xs:sequence>"
                                                                        "<xs:element name=\"value1\" type=\"xs:string\" />"
                                                                        "<xs:element name=\"value2\" type=\"xs:string\" />"
                                                                        "<xs:element name=\"value3\" type=\"xs:string\" />"
                                                                        "<xs:element name=\"value4\" type=\"xs:string\" />"
                                                                        "<xs:element name=\"value5\" type=\"xs:string\" />"
                                                                        "<xs:element name=\"value6\" type=\"xs:string\" />"
                                                                    "</xs:sequence>"
                                                                    "<xs:attribute name=\"subsectionId\" type=\"xs:unsignedByte\" use=\"required\" />"
                                                                "</xs:complexType>"
                                                            "</xs:element>"
                                                        "</xs:sequence>"
                                                        "<xs:attribute name=\"sectionId\" type=\"xs:unsignedByte\" use=\"required\" />"
                                                    "</xs:complexType>"
                                                "</xs:element>"
                                                "<xs:element name=\"section2\">"
                                                    "<xs:complexType>"
                                                        "<xs:sequence>"
                                                            "<xs:element name=\"subsection\">"
                                                                "<xs:complexType>"
                                                                    "<xs:sequence>"
                                                                        "<xs:element name=\"value1\" type=\"xs:string\" />"
                                                                    "</xs:sequence>"
                                                                "</xs:complexType>"
                                                            "</xs:element>"
                                                            "<xs:element name=\"othersection\">"
                                                                "<xs:complexType>"
                                                                    "<xs:sequence>"
                                                                        "<xs:element name=\"avalue\" type=\"xs:string\" />"
                                                                        "<xs:element name=\"bvalue\" type=\"xs:string\" />"
                                                                        "<xs:element name=\"intvalue\" type=\"xs:unsignedByte\" />"
                                                                        "<xs:element name=\"boolvalue\" type=\"xs:boolean\" />"
                                                                        "<xs:element name=\"doublevalue\" type=\"xs:decimal\" />"
                                                                        "<xs:element name=\"datevalue\" type=\"xs:dateTime\" />"
                                                                        "<xs:element name=\"urlvalue\" type=\"xs:anyURI\" />"
                                                                    "</xs:sequence>"
                                                                    "<xs:attribute name=\"subsectionId\" type=\"xs:unsignedByte\" use=\"required\" />"
                                                                "</xs:complexType>"
                                                            "</xs:element>"
                                                        "</xs:sequence>"
                                                    "</xs:complexType>"
                                                "</xs:element>"
                                                "<xs:element name=\"value\" type=\"xs:string\" />"
                                            "</xs:sequence>"
                                            "<xs:attribute name=\"rootId\" type=\"xs:unsignedByte\" use=\"required\" />"
                                        "</xs:complexType>"
                                    "</xs:element>"
                                "</xs:schema>");
}

void ConfigurationReaderTest::initTestCase()
{
    schemaFile_.open();
    schemaFile_.write(validationSchema_.toUtf8());
    schemaFile_.close();
}

void ConfigurationReaderTest::loadFileTest_data()
{
    setData();
}

void ConfigurationReaderTest::loadFileTest()
{
    QFETCH(QString, xml);
    QFETCH(bool, result);

    QTemporaryFile file;
    if (file.open())
    {
        file.write(xml.toUtf8());
        QVERIFY(file.flush());

        ConfigurationReader confReader(schemaFile_.fileName());
        QCOMPARE(confReader.loadFile(file.fileName()), result);
        QCOMPARE(confReader.getIsValid(), result);
    }
}

void ConfigurationReaderTest::loadXmlTest_data()
{
    setData();
}

void ConfigurationReaderTest::loadXmlTest()
{
    QFETCH(QString, xml);
    QFETCH(bool, result);

    ConfigurationReader confReader(schemaFile_.fileName());
    QCOMPARE(confReader.loadXml(xml), result);
    QCOMPARE(confReader.getIsValid(), result);
}

void ConfigurationReaderTest::saveTest()
{
    ConfigurationReader confReader;
    QVERIFY(confReader.loadXml(validConfig_));

    QTemporaryDir tempDir;
    bool valid = tempDir.isValid();
    QVERIFY(valid);
    if (valid)
    {
        QString tempConfFile = PathAppender::combine(tempDir.path(), "tempFile.xml");
        QVERIFY(confReader.save(tempConfFile));

        //Must fail file already exists
        QVERIFY(!confReader.save(tempConfFile, false));

        QVERIFY(confReader.save(tempConfFile));

        QVERIFY(QFile::exists(tempConfFile));

        ConfigurationReader readerFromNewFile;
        QVERIFY(readerFromNewFile.loadFile(tempConfFile));

        //Verify some values
        QCOMPARE(confReader.getValueOrDefault<QString>("value"), readerFromNewFile.getValueOrDefault<QString>("value"));

        QCOMPARE(confReader.getValueOrDefault<QString>(QStringList() << "section1" << "subsection" << "value1"),
                 readerFromNewFile.getValueOrDefault<QString>(QStringList() << "section1" << "subsection" << "value1"));
        QCOMPARE(confReader.getValueOrDefault<QString>(QStringList() << "section1" << "subsection" << "value2"),
                 readerFromNewFile.getValueOrDefault<QString>(QStringList() << "section1" << "subsection" << "value2"));
        QCOMPARE(confReader.getValueOrDefault<QString>(QStringList() << "section1" << "subsection" << "value3"),
                 readerFromNewFile.getValueOrDefault<QString>(QStringList() << "section1" << "subsection" << "value3"));
        QCOMPARE(confReader.getValueOrDefault<QString>(QStringList() << "section1" << "subsection" << "value4"),
                 readerFromNewFile.getValueOrDefault<QString>(QStringList() << "section1" << "subsection" << "value4"));
        QCOMPARE(confReader.getValueOrDefault<QString>(QStringList() << "section1" << "subsection" << "value5"),
                 readerFromNewFile.getValueOrDefault<QString>(QStringList() << "section1" << "subsection" << "value5"));
        QCOMPARE(confReader.getValueOrDefault<QString>(QStringList() << "section1" << "subsection" << "value6"),
                 readerFromNewFile.getValueOrDefault<QString>(QStringList() << "section1" << "subsection" << "value6"));
    }
}

void ConfigurationReaderTest::getValueTest()
{
    ConfigurationReader confReader;
    QVERIFY(confReader.loadXml(validConfig_));

    bool ok;
    QCOMPARE(confReader.getValue<QString>("value", &ok), QString("Value Test"));
    QVERIFY(ok);
    confReader.getValue<QString>("value_not_found", &ok);
    QVERIFY(!ok);

    QCOMPARE(confReader.getValueOrDefault<QString>("value"), QString("Value Test"));
    QCOMPARE(confReader.getValueOrDefault<QString>("value_not_found", "Not Found"), QString("Not Found"));

    QStringList elementPath;
    elementPath << "section1" << "subsection" << "value1";
    QCOMPARE(confReader.getValue<QString>(elementPath, &ok), QString("Value number 1"));
    QVERIFY(ok);
    confReader.getValue<QString>(elementPath << "non_existent", &ok);
    QVERIFY(!ok);

    QCOMPARE(confReader.getValueOrDefault<int>(QStringList() << "section2" << "othersection" << "intvalue"), 30);
    QCOMPARE(confReader.getValueOrDefault<bool>(QStringList() << "section2" << "othersection" << "boolvalue"), true);
    QCOMPARE(confReader.getValueOrDefault<double>(QStringList() << "section2" << "othersection" << "doublevalue"), 1.5);
    QCOMPARE(confReader.getValueOrDefault<QDateTime>(QStringList() << "section2" << "othersection" << "datevalue"),
             QDateTime::fromString("2012-05-07 08:17:25Z", Qt::ISODate));
    QCOMPARE(confReader.getValueOrDefault<QUrl>(QStringList() << "section2" << "othersection" << "urlvalue"),
             QUrl("http://test/resource.xml"));
}

void ConfigurationReaderTest::getAttributeTest()
{
    ConfigurationReader confReader;
    QVERIFY(confReader.loadXml(validConfig_));

    bool ok;
    QCOMPARE(confReader.getAttribute<int>("rootId", "", &ok), 1);
    QVERIFY(ok);
    confReader.getAttribute<int>("rootnotfoundId", "", &ok);
    QVERIFY(!ok);

    QCOMPARE(confReader.getAttributeOrDefault<int>("rootId", "", 5), 1);
    QCOMPARE(confReader.getAttributeOrDefault<int>("rootnotfoundId", "", 5), 5);

    QStringList elementPath;
    elementPath << "section1" << "subsection";
    QCOMPARE(confReader.getAttribute<int>("subsectionId", elementPath, &ok), 2);
    QVERIFY(ok);
    confReader.getAttribute<int>("subsectionIdnotfound", elementPath, &ok);
    QVERIFY(!ok);

    QCOMPARE(confReader.getAttributeOrDefault<int>("subsectionId", elementPath, 5), 2);
    QCOMPARE(confReader.getAttributeOrDefault<int>("subsectionIdnotfound", elementPath, 5), 5);
}

void ConfigurationReaderTest::getSectionTest()
{
    ConfigurationReader confReader;
    QVERIFY(confReader.loadXml(validConfig_));

    bool ok;
    auto section = confReader.getSection<QString>(QStringList() << "section1" << "subsection", &ok);
    QVERIFY(ok);
    QVERIFY(section.count() == 6);
    QCOMPARE(section.at(0).first, QString("value1"));
    QCOMPARE(section.at(0).second, QString("Value number 1"));
    QCOMPARE(section.at(1).first, QString("value2"));
    QCOMPARE(section.at(1).second, QString("Value number 2"));
    QCOMPARE(section.at(2).first, QString("value3"));
    QCOMPARE(section.at(2).second, QString("Value number 3"));
    QCOMPARE(section.at(3).first, QString("value4"));
    QCOMPARE(section.at(3).second, QString("Value number 4"));
    QCOMPARE(section.at(4).first, QString("value5"));
    QCOMPARE(section.at(4).second, QString("Value number 5"));
    QCOMPARE(section.at(5).first, QString("value6"));
    QCOMPARE(section.at(5).second, QString("Value number 6"));

    section = confReader.getSection<QString>("section2", &ok);
    QVERIFY(ok);
    QVERIFY(section.count() == 2);
    QCOMPARE(section.at(0).first, QString("subsection"));
    QCOMPARE(section.at(1).first, QString("othersection"));

    section = confReader.getSection<QString>("value", &ok);
    QVERIFY(!ok);
}

void ConfigurationReaderTest::setValueTest()
{
    ConfigurationReader confReader;
    QVERIFY(confReader.loadXml(validConfig_));

    QVERIFY(!confReader.setValue("test", 10));
    QVERIFY(confReader.setValue("test", 10, true));
    QCOMPARE(confReader.getValueOrDefault<int>("test", 20), 10);

    QVERIFY(!confReader.setValue(QStringList() << "section1" << "subsection" << "value7", "Value number 7"));
    QVERIFY(confReader.setValue(QStringList() << "section1" << "subsection" << "value7", "Value number 7", true));
    QCOMPARE(confReader.getValueOrDefault<QString>(QStringList() << "section1" << "subsection" << "value7", "Value number 10"),
             QString("Value number 7"));

    QVERIFY(confReader.setValue(QStringList() << "section3" << "subsection" << "value", "Inserted value", true));
    QCOMPARE(confReader.getValueOrDefault<QString>(QStringList() << "section3" << "subsection" << "value", "Inserted wrong value"),
             QString("Inserted value"));

    //Now create a temporary file and read it back to confirm the values
    QTemporaryFile file;
    if (file.open())
    {
        file.close();
        QVERIFY(confReader.save(file.fileName()));
    }

    ConfigurationReader confFromFile;
    confFromFile.loadFile(file.fileName());
    QCOMPARE(confFromFile.getValueOrDefault<int>("test", 20), 10);
    QCOMPARE(confFromFile.getValueOrDefault<QString>(QStringList() << "section1" << "subsection" << "value7", "Value number 10"),
             QString("Value number 7"));
    QCOMPARE(confFromFile.getValueOrDefault<QString>(QStringList() << "section3" << "subsection" << "value", "Inserted wrong value"),
             QString("Inserted value"));

}

void ConfigurationReaderTest::setData()
{
    QTest::addColumn<QString>("xml");
    QTest::addColumn<bool>("result");

    QTest::newRow("Valid XML") << validConfig_ << true;
    QTest::newRow("Invalid XML") << invalidXmlConfig_ << false;
    QTest::newRow("Badly constructed XML") << malConstructedXmlConfig_ << false;
}

DECLARE_TEST(ConfigurationReaderTest)

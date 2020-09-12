#include <QtTest>

#include "commonsettings.h"

class CommonSettingsTest : public QObject
{
    Q_OBJECT

public:
    CommonSettingsTest();
    ~CommonSettingsTest();

private:
    QString settingsFilePath;

private slots:
    void initTestCase();
    void readSettingsFromFile();
    void useDefaultValuesIfSettingsFileIsNotFound();

};

void CommonSettingsTest::initTestCase()
{
    CommonSettings settings("Protokollitaja", "Protokollitaja test conf");
    settingsFilePath = settings.fileName();
}

CommonSettingsTest::CommonSettingsTest()
{

}

CommonSettingsTest::~CommonSettingsTest()
{

}

void CommonSettingsTest::readSettingsFromFile()
{
    QFile settingsFile(settingsFilePath);

    if(settingsFile.exists())
        settingsFile.remove();

    QCOMPARE(settingsFile.open(QIODevice::ReadWrite | QIODevice::Text), true);
    QTextStream out(&settingsFile);
    out << "[siusConnection]\n" << "competitionShotTypes=\"0,3,4,5\"\n";
    out << "sighterShotTypes=\"32,35,36\"\n";
    settingsFile.close();

    CommonSettings settings("Protokollitaja", "Protokollitaja test conf");
    QCOMPARE(settings.competitionShotTypesString(), "0,3,4,5");
    QCOMPARE(settings.sighterShotTypesString(), "32,35,36");

    if(settingsFile.exists())
        settingsFile.remove();
}

void CommonSettingsTest::useDefaultValuesIfSettingsFileIsNotFound()
{
    QFile settingsFile(settingsFilePath);
    if(settingsFile.exists())
        settingsFile.remove();
    CommonSettings settings("Protokollitaja", "Protokollitaja test conf");

    QCOMPARE(settings.competitionShotTypesString(), "0,3,4,5,7,512,515,519,1028,1029,1036,2304");
    QCOMPARE(settings.sighterShotTypesString(), "32,35,36,37,39,544,547,551,1060");

    if(settingsFile.exists())
        settingsFile.remove();
}

QTEST_APPLESS_MAIN(CommonSettingsTest)

#include "tst_commonsettings.moc"

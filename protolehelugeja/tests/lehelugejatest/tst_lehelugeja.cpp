#include <QtTest>
#include <QCoreApplication>

#include <QTextStream>
#include <QComboBox>
#include "lehelugeja.h"
#include "version.h"

QString programmiNimi = VER_INTERNALNAME_STR; //"Protolehelugeja 0.3 Beta"; // TODO uuendada nime
QString versioon = VER_PRODUCTVERSION_STR; //"0.3.0 Beta";
QString aasta = VER_AASTA;
QString argument;
bool verbose = false;
bool veryVerbose = false;

class LehelugejaTest : public QObject
{
    Q_OBJECT

public:
    LehelugejaTest();
    ~LehelugejaTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_changeComPort();
    void test_changeScoringMachineType();
};

LehelugejaTest::LehelugejaTest()
{

}

LehelugejaTest::~LehelugejaTest()
{

}

void LehelugejaTest::initTestCase()
{

}

void LehelugejaTest::cleanupTestCase()
{

}

void LehelugejaTest::test_changeComPort()
{
    Lehelugeja lehelugeja;
    QSignalSpy spy(&lehelugeja, SIGNAL(changeVariable(QString)));

    lehelugeja.changeVariable("comPort;COM21");

    QComboBox* comPort = lehelugeja.findChild<QComboBox*>("comPort");

    QVERIFY(comPort->currentText().compare("COM21") == 0);
}

void LehelugejaTest::test_changeScoringMachineType()
{
    Lehelugeja lehelugeja;
    QSignalSpy spy(&lehelugeja, SIGNAL(changeVariable(QString)));

    lehelugeja.changeVariable("scoringMachineType;RMIV");

    QVERIFY(lehelugeja.scoringMachineType() == ScoringMachineConnection::RMIV);
}

QTEST_MAIN(LehelugejaTest)
#include "tst_lehelugeja.moc"

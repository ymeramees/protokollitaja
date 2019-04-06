#include <QtTest>
#include <QCoreApplication>

// add necessary includes here
#include <QTextStream>
#include "../../protokollitaja/src/lask.h"
#include "../src/scoringmachineconnection.h"

class ScoringMachineConnectionTest : public QObject
{
    Q_OBJECT

public:
    ScoringMachineConnectionTest();
    ~ScoringMachineConnectionTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_connectToMachineFailedAttempt();
    void test_connectToMachineWithoutPort();
    void test_CRCreturnValue();
    void test_extractRMIIIShot();
    void test_extractRMIVShot();
};

ScoringMachineConnectionTest::ScoringMachineConnectionTest()
{

}

ScoringMachineConnectionTest::~ScoringMachineConnectionTest()
{

}

void ScoringMachineConnectionTest::initTestCase()
{

}

void ScoringMachineConnectionTest::cleanupTestCase()
{

}

void ScoringMachineConnectionTest::test_connectToMachineFailedAttempt(){
    ScoringMachineConnection machine;
    QSignalSpy spy(&machine, SIGNAL(connectionStatusChanged(QString)));

    machine.setPortName("COM1");
    machine.connectToMachine();

    QCOMPARE(spy.count(), 1);
    QList<QVariant> statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("Ühendamine: RMIII, COM1") == 0);

    QVERIFY(spy.wait(500));

    QCOMPARE(spy.count(), 1);
    statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("Ühendamine: RMIV, COM1") == 0);

    QVERIFY(spy.wait(500));

    QCOMPARE(spy.count(), 1);
    statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("Viga, ei õnnestu ühenduda ei RMIII ega RMIV'ga!") == 0);
}

void ScoringMachineConnectionTest::test_connectToMachineWithoutPort(){
    ScoringMachineConnection machine;
    QSignalSpy spy(&machine, SIGNAL(connectionStatusChanged(QString)));

    machine.connectToMachine();

    QCOMPARE(spy.count(), 1);
    QList<QVariant> statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("Viga: Pordi nime pole määratud, ei saa ühenduda!") == 0);
}

void ScoringMachineConnectionTest::test_CRCreturnValue()
{
    //QTextStream(stdout) << "This is a test!" << endl;
    ScoringMachineConnection machine; // = new ScoringMachineConnection();
    QByteArray testString1 = QString("EXIT").toLatin1();
    QByteArray testString2 = QString("This is some test string").toLatin1();

    QCOMPARE(machine.CRC(&testString1), 0);
    QCOMPARE(machine.CRC(&testString2), 43);
}

void ScoringMachineConnectionTest::test_extractRMIIIShot()
{
    ScoringMachineConnection machine;
    machine.setTargetType(ScoringMachineConnection::AirRifle);

    Lask toCompare1(90, "3,33" ,"-0,35");
    Lask toCompare2(90, "-1,1" ,"2,4");
    Lask toCompare3(60, "67,2" ,"21,12");
    Lask toCompare4(60, "67,3" ,"21,12");
    Lask empty(-999, "-999", "-999");

    QCOMPARE(machine.extractRMIIIShot("7;9.0;-;1.33;-0.14;N"), toCompare1);
    QCOMPARE(machine.extractRMIIIShot("8;9.0;-;-0.44;0.96;N"), toCompare2);

    machine.setTargetType(ScoringMachineConnection::AirPistol);
    QCOMPARE(machine.extractRMIIIShot("4;6.0;-;4.20;1.32;N"), toCompare3);

    machine.setTargetType(ScoringMachineConnection::SmallboreRifle);
    QCOMPARE(machine.extractRMIIIShot("4;6.0;-;4.20;1.32;N"), toCompare3);

    // Make sure comparing not equal shots are not considered equal
    QVERIFY(!(machine.extractRMIIIShot("4;6.0;-;4.20;1.32;N") == toCompare4));

    // If input row is too short, an empty shot must be returned
    QCOMPARE(machine.extractRMIIIShot("4;6.0;-;4.20"), empty);
}

void ScoringMachineConnectionTest::test_extractRMIVShot()
{
    ScoringMachineConnection machine;
    machine.setTargetType(ScoringMachineConnection::AirRifle);

    Lask toCompare1(72, "7,84", "-28,68");
    Lask empty(-999, "-999", "-999");

    Lask shot = machine.extractRMIVShot("SCH=1;7.2;2973.0;164.7;G");

    QCOMPARE(machine.extractRMIVShot("SCH=1;7.2;2973.0;164.7;G"), toCompare1);

    // If input row is too short, an empty shot must be returned
    QCOMPARE(machine.extractRMIIIShot("4;6.0;-;4.20"), empty);
}

QTEST_MAIN(ScoringMachineConnectionTest)

#include "tst_scoringmachineconnection.moc"

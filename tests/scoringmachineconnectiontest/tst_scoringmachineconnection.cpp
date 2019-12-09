#include <QtTest>
#include <QCoreApplication>

#include <QTextStream>
#include "lask.h"
#include "scoringmachineconnection.h"

class ScoringMachineConnectionTest : public QObject
{
    Q_OBJECT

public:
    ScoringMachineConnectionTest();
    ~ScoringMachineConnectionTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_calculateIsInnerTen();
    void test_connectToMachineFailedAttempt();
    void test_connectToMachineWithoutPort();
    void test_connectToRMIV();
    void test_CRCreturnValue();
    void test_extractRMIIIShot();
    void test_extractRMIVShot();
    void test_sendSettingsRMIIIAirPistol();
    void test_sendSettingsRMIIIAirRifle();
    void test_sendSettingsRMIIIAirRifleTargetBands();
    void test_sendSettingsRMIIISmallboreRifle();
    void test_sendSettingsRMIVAirPistol();
    void test_sendSettingsRMIVAirRifle();
    void test_sendSettingsRMIVAirRifleTargetBands();
    void test_sendSettingsRMIVSmallboreRifle();
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

void ScoringMachineConnectionTest::test_calculateIsInnerTen()
{
    ScoringMachineConnection machine;

    machine.setTargetType(ScoringMachineConnection::AirRifle);
    QVERIFY(machine.calculateIsInnerTen(float(0.49), float(0.59)));
    QVERIFY(machine.calculateIsInnerTen(float(0.07), float(0.90)));
    QVERIFY(!(machine.calculateIsInnerTen(float(0.35), float(1.99))));
    QVERIFY(!(machine.calculateIsInnerTen(float(15.35), float(-0.99))));

    machine.setTargetType(ScoringMachineConnection::AirPistol);
    QVERIFY(machine.calculateIsInnerTen(float(-4.5), float(0.95)));
    QVERIFY(!(machine.calculateIsInnerTen(float(15.35), float(9.59))));
    QVERIFY(!(machine.calculateIsInnerTen(float(1.33), float(5.17))));

    machine.setTargetType(ScoringMachineConnection::SmallboreRifle);
    QVERIFY(machine.calculateIsInnerTen(float(2.22), float(3.21)));
    QVERIFY(machine.calculateIsInnerTen(float(-0.74), float(-0.94)));
    QVERIFY(!(machine.calculateIsInnerTen(float(-2.28), float(-6.54))));
    QVERIFY(!(machine.calculateIsInnerTen(float(-4.75), float(7.38))));
}

void ScoringMachineConnectionTest::test_connectToMachineFailedAttempt()
{
    ScoringMachineConnection machine;
    QSignalSpy spy(&machine, SIGNAL(connectionStatusChanged(QString)));

    machine.setPortName("COM1");
    machine.connectToMachine();

    QCOMPARE(spy.count(), 2);
    QList<QVariant> statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("Ühendamine: RMIII, COM1") == 0);
    statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("Ei õnnestu serial port'i avada! Kontrollige, et mõni teine programm seda juba ei kasuta.") == 0);

//    QVERIFY(spy.wait(500));

//    QCOMPARE(spy.count(), 1);
//    statusMessages = spy.takeFirst();
//    QVERIFY(statusMessages.at(0).toString().compare("Ühendamine: RMIV, COM1") == 0);

//    QVERIFY(spy.wait(500));

//    QCOMPARE(spy.count(), 1);
//    statusMessages = spy.takeFirst();
//    QVERIFY(statusMessages.at(0).toString().compare("Viga, ei õnnestu ühenduda ei RMIII ega RMIV'ga!") == 0);
}

void ScoringMachineConnectionTest::test_connectToMachineWithoutPort()
{
    ScoringMachineConnection machine;
    QSignalSpy spy(&machine, SIGNAL(connectionStatusChanged(QString)));

    machine.connectToMachine();

    QCOMPARE(spy.count(), 1);
    QList<QVariant> statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("Viga: Pordi nime pole määratud, ei saa ühenduda!") == 0);
}

void ScoringMachineConnectionTest::test_connectToRMIV()
{
    ScoringMachineConnection machine;
    QSignalSpy spy(&machine, SIGNAL(connectionStatusChanged(QString)));

    machine.setPortName("COM1");
    machine.m_scoringMachineType = ScoringMachineConnection::RMIV;
//    QMetaObject::invokeMethod(&machine, "connectToRMIV", Qt::DirectConnection);
    machine.connectToMachine();

    QCOMPARE(spy.count(), 1);
    QList<QVariant> statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("Ühendamine: RMIV, COM1") == 0);

    machine.m_serialBuffer = "SNR=";
    machine.m_serialBuffer.append(0x0d);

    spy.wait(500);
    QCOMPARE(spy.count(), 2);
    statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("RMIV: SNR=\r") == 0);

    statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("Ühendatud: RMIV") == 0);
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

    Lask toCompare1(90, 3330, 350);
    Lask toCompare2(106, -200, -770, true);
    Lask toCompare3(87, 560, 17760);
    Lask toCompare4(104, -2640 ,3760, true);
    Lask toCompare5(74, 16480, -23360);
    Lask toCompare6(107, 640, -1680, true);
    Lask empty(-999, -999, -999);

    QCOMPARE(machine.extractRMIIIShot("7;9.0;-;1.33;-0.14;N"), toCompare1);
    QCOMPARE(machine.extractRMIIIShot("8;10.6;-;-0.08;0.31;N"), toCompare2);

    machine.setTargetType(ScoringMachineConnection::AirPistol);
    QCOMPARE(machine.extractRMIIIShot("4;8.7;-;0.07;-2.22;N"), toCompare3);
    QCOMPARE(machine.extractRMIIIShot("8;10.4;-;-0.33;-0.47;N"), toCompare4);

    machine.setTargetType(ScoringMachineConnection::SmallboreRifle);
    QCOMPARE(machine.extractRMIIIShot("4;7.4;-;2.06;2.92;N"), toCompare5);
    QCOMPARE(machine.extractRMIIIShot("4;10.7;-;0.08;0.21;N"), toCompare6);

    // Make sure comparing not equal shots are not considered equal
    QVERIFY(!(machine.extractRMIIIShot("4;6.0;-;4.20;1.32;N") == toCompare5));

    // If input row is too short, an empty shot must be returned
    QCOMPARE(machine.extractRMIIIShot("4;6.0;-;4.20"), empty);
}

void ScoringMachineConnectionTest::test_extractRMIVShot()
{
    ScoringMachineConnection machine;
    machine.setTargetType(ScoringMachineConnection::AirRifle);

    Lask toCompare1(72, 7840, -28680);
    Lask empty(-999, -999, -999);

    Lask shot = machine.extractRMIVShot("SCH=1;7.2;2973.0;164.7;G");

    QCOMPARE(machine.extractRMIVShot("SCH=1;7.2;2973.0;164.7;G"), toCompare1);

    // If input row is too short, an empty shot must be returned
    QCOMPARE(machine.extractRMIIIShot("4;6.0;-;4.20"), empty);
}

void ScoringMachineConnectionTest::test_sendSettingsRMIIIAirPistol()
{
    ScoringMachineConnection machine;
    machine.setTargetType(ScoringMachineConnection::AirPistol);
    machine.setScoringMachineType(ScoringMachineConnection::RMIII);

    QSignalSpy spy(&machine, SIGNAL(connectionStatusChanged(QString)));
    QList<QVariant> statusMessages;

    for(int i = 1; i <= 6; i++){
        machine.setNoOfShotsPerTarget(i);
        machine.sendSettings();

        QCOMPARE(spy.count(), 1);
        statusMessages = spy.takeFirst();
        QVERIFY(statusMessages.at(0).toString().compare(QString("Seadisamine: 21121111%1").arg(i)) == 0);
    }

    machine.setNoOfShotsPerTarget(10);
    machine.sendSettings();

    QCOMPARE(spy.count(), 1);
    statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("Seadisamine: 21121111A") == 0);

    machine.setNoOfShotsPerTarget(11);
    machine.sendSettings();

    QCOMPARE(spy.count(), 1);
    statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("Seadisamine: 21121111B") == 0);
}

void ScoringMachineConnectionTest::test_sendSettingsRMIIIAirRifle()
{
    ScoringMachineConnection machine;
    machine.setTargetType(ScoringMachineConnection::AirRifle);
    machine.setScoringMachineType(ScoringMachineConnection::RMIII);

    QSignalSpy spy(&machine, SIGNAL(connectionStatusChanged(QString)));
    QList<QVariant> statusMessages;

    for(int i = 1; i <= 6; i++){
        machine.setNoOfShotsPerTarget(i);
        machine.sendSettings();

        QCOMPARE(spy.count(), 1);
        statusMessages = spy.takeFirst();
        QVERIFY(statusMessages.at(0).toString().compare(QString("Seadisamine: 11121111%1").arg(i)) == 0);
    }

    machine.setNoOfShotsPerTarget(10);
    machine.sendSettings();

    QCOMPARE(spy.count(), 1);
    statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("Seadisamine: 11121111A") == 0);

    machine.setNoOfShotsPerTarget(11);
    machine.sendSettings();

    QCOMPARE(spy.count(), 1);
    statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("Seadisamine: 11121111B") == 0);
}

void ScoringMachineConnectionTest::test_sendSettingsRMIIIAirRifleTargetBands()
{
    ScoringMachineConnection machine;
    machine.setTargetType(ScoringMachineConnection::AirRifle5Band);
    machine.setScoringMachineType(ScoringMachineConnection::RMIII);

    QSignalSpy spy(&machine, SIGNAL(connectionStatusChanged(QString)));
    QList<QVariant> statusMessages;

    for(int i = 1; i <= 6; i++){
        machine.setNoOfShotsPerTarget(i);
        machine.sendSettings();

        QCOMPARE(spy.count(), 1);
        statusMessages = spy.takeFirst();
        QVERIFY(statusMessages.at(0).toString().compare(QString("Seadisamine: 12121111%1").arg(i)) == 0);
    }

    machine.setNoOfShotsPerTarget(10);
    machine.sendSettings();

    QCOMPARE(spy.count(), 1);
    statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("Seadisamine: 12121111A") == 0);

    machine.setNoOfShotsPerTarget(11);
    machine.sendSettings();

    QCOMPARE(spy.count(), 1);
    statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("Seadisamine: 12121111B") == 0);

    machine.setTargetType(ScoringMachineConnection::AirRifle10Band);
    for(int i = 1; i <= 6; i++){
        machine.setNoOfShotsPerTarget(i);
        machine.sendSettings();

        QCOMPARE(spy.count(), 1);
        statusMessages = spy.takeFirst();
        QVERIFY(statusMessages.at(0).toString().compare(QString("Seadisamine: 13121111%1").arg(i)) == 0);
    }
}

void ScoringMachineConnectionTest::test_sendSettingsRMIIISmallboreRifle()
{
    ScoringMachineConnection machine;
    machine.setTargetType(ScoringMachineConnection::SmallboreRifle);
    machine.setScoringMachineType(ScoringMachineConnection::RMIII);

    QSignalSpy spy(&machine, SIGNAL(connectionStatusChanged(QString)));
    QList<QVariant> statusMessages;

    for(int i = 1; i <= 6; i++){
        machine.setNoOfShotsPerTarget(i);
        machine.sendSettings();

        QCOMPARE(spy.count(), 1);
        statusMessages = spy.takeFirst();
        QVERIFY(statusMessages.at(0).toString().compare(QString("Seadisamine: 61121111%1").arg(i)) == 0);
    }

    machine.setNoOfShotsPerTarget(10);
    machine.sendSettings();

    QCOMPARE(spy.count(), 1);
    statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("Seadisamine: 61121111A") == 0);

    machine.setNoOfShotsPerTarget(11);
    machine.sendSettings();

    QCOMPARE(spy.count(), 1);
    statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("Seadisamine: 61121111B") == 0);
}

void ScoringMachineConnectionTest::test_sendSettingsRMIVAirPistol()
{
    ScoringMachineConnection machine;
    machine.setTargetType(ScoringMachineConnection::AirPistol);
    machine.setScoringMachineType(ScoringMachineConnection::RMIV);

    QSignalSpy spy(&machine, SIGNAL(connectionStatusChanged(QString)));
    QList<QVariant> statusMessages;

    for(int i = 1; i <= 11; i++){
        machine.setNoOfShotsPerTarget(i);
        machine.sendSettings();

        QCOMPARE(spy.count(), 1);
        statusMessages = spy.takeFirst();
        QVERIFY(statusMessages.at(0).toString().compare(QString("Seadisamine: SCH=LP;TEA=KT;RIA=ZR;SSC=%1;SGE=10;SZI=10;").arg(i)) == 0);
    }
}

void ScoringMachineConnectionTest::test_sendSettingsRMIVAirRifle()
{
    ScoringMachineConnection machine;
    machine.setTargetType(ScoringMachineConnection::AirRifle);
    machine.setScoringMachineType(ScoringMachineConnection::RMIV);

    QSignalSpy spy(&machine, SIGNAL(connectionStatusChanged(QString)));
    QList<QVariant> statusMessages;

    for(int i = 1; i <= 11; i++){
        machine.setNoOfShotsPerTarget(i);
        machine.sendSettings();

        QCOMPARE(spy.count(), 1);
        statusMessages = spy.takeFirst();
        QVERIFY(statusMessages.at(0).toString().compare(QString("Seadisamine: SCH=LGES;TEA=KT;RIA=ZR;SSC=%1;SGE=10;SZI=10;").arg(i)) == 0);
    }
}

void ScoringMachineConnectionTest::test_sendSettingsRMIVAirRifleTargetBands()
{
    ScoringMachineConnection machine;
    machine.setTargetType(ScoringMachineConnection::AirRifle5Band);
    machine.setScoringMachineType(ScoringMachineConnection::RMIV);

    QSignalSpy spy(&machine, SIGNAL(connectionStatusChanged(QString)));
    QList<QVariant> statusMessages;

    for(int i = 1; i <= 11; i++){
        machine.setNoOfShotsPerTarget(i);
        machine.sendSettings();

        QCOMPARE(spy.count(), 1);
        statusMessages = spy.takeFirst();
        QVERIFY(statusMessages.at(0).toString().compare(QString("Seadisamine: SCH=LG5;TEA=KT;RIA=ZR;SSC=%1;SGE=10;SZI=10;").arg(i)) == 0);
    }

    machine.setTargetType(ScoringMachineConnection::AirRifle10Band);
    for(int i = 1; i <= 11; i++){
        machine.setNoOfShotsPerTarget(i);
        machine.sendSettings();

        QCOMPARE(spy.count(), 1);
        statusMessages = spy.takeFirst();
        QVERIFY(statusMessages.at(0).toString().compare(QString("Seadisamine: SCH=LG10;TEA=KT;RIA=ZR;SSC=%1;SGE=10;SZI=10;").arg(i)) == 0);
    }
}

void ScoringMachineConnectionTest::test_sendSettingsRMIVSmallboreRifle()
{
    ScoringMachineConnection machine;
    machine.setTargetType(ScoringMachineConnection::SmallboreRifle);
    machine.setScoringMachineType(ScoringMachineConnection::RMIV);

    QSignalSpy spy(&machine, SIGNAL(connectionStatusChanged(QString)));
    QList<QVariant> statusMessages;

    for(int i = 1; i <= 11; i++){
        machine.setNoOfShotsPerTarget(i);
        machine.sendSettings();

        QCOMPARE(spy.count(), 1);
        statusMessages = spy.takeFirst();
        QVERIFY(statusMessages.at(0).toString().compare(QString("Seadisamine: SCH=KK50;TEA=KT;RIA=ZR;SSC=%1;SGE=10;SZI=10;").arg(i)) == 0);
    }
}

QTEST_MAIN(ScoringMachineConnectionTest)
#include "tst_scoringmachineconnection.moc"

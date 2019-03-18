#include <QtTest>
#include <QCoreApplication>

// add necessary includes here
#include <QTextStream>
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
    void test_CRCreturnValue();

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

void ScoringMachineConnectionTest::test_CRCreturnValue()
{
    //QTextStream(stdout) << "This is a test!" << endl;
    ScoringMachineConnection *machine = new ScoringMachineConnection();
    QByteArray testString1 = QString("EXIT").toLatin1();
    QByteArray testString2 = QString("This is some test string").toLatin1();

    QCOMPARE(machine->CRC(&testString1), 0);
    QCOMPARE(machine->CRC(&testString2), 43);
}

QTEST_MAIN(ScoringMachineConnectionTest)

#include "tst_scoringmachineconnection.moc"

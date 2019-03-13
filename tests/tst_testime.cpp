#include <QtTest>
#include <QCoreApplication>

// add necessary includes here
#include <QTextStream>
#include "../src/scoringmachineconnection.h"

class Testime : public QObject
{
    Q_OBJECT

public:
    Testime();
    ~Testime();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();

};

Testime::Testime()
{

}

Testime::~Testime()
{

}

void Testime::initTestCase()
{

}

void Testime::cleanupTestCase()
{

}

void Testime::test_case1()
{
    QTextStream(stdout) << "This is a test!" << endl;
    ScoringMachineConnection *machine = new ScoringMachineConnection();
    QCOMPARE(machine->testFunction("20"), 200);
}

QTEST_MAIN(Testime)

#include "tst_testime.moc"

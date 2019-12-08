#include <QtTest>
#include <QCoreApplication>

#include <QTextStream>
#include "globalvariables.h"
#include "siusdataconnection.h"

bool verbose = true;

class SiusDataConnectionTest : public QObject
{
    Q_OBJECT

public:
    SiusDataConnectionTest();
    ~SiusDataConnectionTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_calculateIsInnerTen();
};

SiusDataConnectionTest::SiusDataConnectionTest()
{

}

SiusDataConnectionTest::~SiusDataConnectionTest()
{

}

void SiusDataConnectionTest::initTestCase()
{

}

void SiusDataConnectionTest::cleanupTestCase()
{

}

void SiusDataConnectionTest::test_calculateIsInnerTen()
{

}

QTEST_MAIN(SiusDataConnectionTest)
#include "tst_siusdataconnection.moc"

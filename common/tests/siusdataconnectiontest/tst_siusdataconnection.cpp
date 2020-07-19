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
    void test_handlePartialSiusTransfer();
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
    // FIXME To be implemented
}

void SiusDataConnectionTest::test_handlePartialSiusTransfer()
{
    QFile file;
    QTextStream log;
    SiusDataConnection conn("127.0.0.1", 4000, 0, &file, &log);
    QSignalSpy spy(&conn, SIGNAL(linesRead(QStringList, int)));

    conn.siusBuffer.append("_SHOT;6;7;90;60;41;10:08:25.05;3;1;0;10;102;0;14;0.00232;-0.00593;900;0;0;655.35;1731649946;64;559;0\n"
                           "_TOTL;6;7;90;103;T;131;0;Q;0;0;S;131;0;P;131;0;G;95;0;G;36;0;\n"
                           "_SUBT;4;5;88;60;27;10:08:27.83;7;0;96;989;1731650225\n"
                           "_GRPH;1;2;85;60;28;10:08:35.86;8;0;2;1;0;0;64;1731651054\n"
                           "_SHOT;1;2;85;60;29;10:08:36.47;3;1;0;10;102;0;");

    conn.readFromSius();
    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QStringList lines = arguments.at(0).toStringList();

    QCOMPARE(lines.size(), 4);
    QCOMPARE(lines.last().left(5), "_GRPH");

    conn.siusBuffer.append("14;0.00232;-0.00593;900;0;0;655.35;1731649946;64;559;0\n");
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments2 = spy.takeFirst();
    QStringList lines2 = arguments2.at(0).toStringList();

    QCOMPARE(lines2.size(), 1);
    QCOMPARE(lines2.last().left(5), "_SHOT");
}

QTEST_MAIN(SiusDataConnectionTest)
#include "tst_siusdataconnection.moc"

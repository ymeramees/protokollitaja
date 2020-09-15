#include <QtTest>
#include <QCoreApplication>

#include <QTextStream>
#include "commonsettings.h"
#include "globalvariables.h"
#include "siusdataconnection.h"

bool verbose = true;

class SiusDataConnectionTest : public QObject
{
    Q_OBJECT

public:
    SiusDataConnectionTest();
    ~SiusDataConnectionTest();

private:
    CommonSettings settings; // Tests are run with default values

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_extractShotDataCompetitionShot();
    void test_extractShotDataNotMatchingIds();
    void test_extractShotDataUnknownShotTypeCompetitionShot();
    void test_extractShotDataUnknownShotTypeSightingShot();
    void test_extractShotDataSightingShot();
    void test_handlePartialSiusTransfer();
};

SiusDataConnectionTest::SiusDataConnectionTest() : settings("", "")
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

void SiusDataConnectionTest::test_extractShotDataCompetitionShot()
{
    QTextStream log = QTextStream(stdout);
    std::optional<SiusShotData> shotData = SiusDataConnection::extractShotData(
                "_TOTL;6;7;90;103;T;131;0;Q;0;0;S;131;0;P;131;0;G;95;0;G;36;0;\n",
                "_SHOT;6;7;90;60;41;10:08:25.05;3;1;0;10;102;0;14;0.00232;-0.00593;900;0;0;655.35;1731649946;64;559;0\n",
                0,
                &log,
                &settings
                );

    QCOMPARE(shotData.value().id, 90);
    QCOMPARE(shotData.value().socketIndex, 0);
    QCOMPARE(shotData.value().siusShotNo, 14);
    QCOMPARE(shotData.value().shot.getSLask(), "10,2");
    QCOMPARE(shotData.value().shot.X(), 2320);
    QCOMPARE(shotData.value().shot.Y(), -5930);
    QCOMPARE(shotData.value().shot.isCompetitionShot(), true);
    QCOMPARE(shotData.value().shot.shotOrigin(), Lask::Sius);
}

void SiusDataConnectionTest::test_extractShotDataNotMatchingIds()
{
    QTextStream log = QTextStream(stdout);
    std::optional<SiusShotData> shotData;
    try {
    shotData = SiusDataConnection::extractShotData(
                "_TOTL;6;7;91;103;T;131;0;Q;0;0;S;131;0;P;131;0;G;95;0;G;36;0;\n",
                "_SHOT;6;7;90;60;41;10:08:25.05;3;1;57;10;102;0;14;0.00232;-0.00593;900;0;0;655.35;1731649946;64;559;0\n",
                0,
                &log,
                &settings
                );
    } catch (...) {
    }

    QCOMPARE(shotData.has_value(), false);
}

void SiusDataConnectionTest::test_extractShotDataUnknownShotTypeCompetitionShot()
{
    QTextStream log = QTextStream(stdout);
    std::optional<SiusShotData> shotData = SiusDataConnection::extractShotData(
                "_TOTL;6;7;90;103;T;131;0;Q;0;0;S;131;0;P;131;0;G;95;0;G;36;0;\n",
                "_SHOT;6;7;90;60;41;10:08:25.05;3;1;57;10;102;0;14;0.00232;-0.00593;900;0;0;655.35;1731649946;64;559;0\n",
                0,
                &log,
                &settings
                );

    QCOMPARE(shotData.value().id, 90);
    QCOMPARE(shotData.value().socketIndex, 0);
    QCOMPARE(shotData.value().siusShotNo, 14);
    QCOMPARE(shotData.value().shot.getSLask(), "10,2");
    QCOMPARE(shotData.value().shot.X(), 2320);
    QCOMPARE(shotData.value().shot.Y(), -5930);
    QCOMPARE(shotData.value().shot.isCompetitionShot(), true);
    QCOMPARE(shotData.value().shot.shotOrigin(), Lask::Sius);
}

void SiusDataConnectionTest::test_extractShotDataUnknownShotTypeSightingShot()
{
    QTextStream log = QTextStream(stdout);
    std::optional<SiusShotData> shotData = SiusDataConnection::extractShotData(
                "_TOTL;23;24;4;103;T;0;0;Q;0;0;S;0;0;\n",
                "_SHOT;23;24;4;60;4;09:49:49.53;3;1;307;0;0;0;1;-0.07557213;0.19096041;900;0;0;467.12;1792017711;64;560;0\n",
                0,
                &log,
                &settings
                );

    QCOMPARE(shotData.value().id, 4);
    QCOMPARE(shotData.value().socketIndex, 0);
    QCOMPARE(shotData.value().siusShotNo, 1);
    QCOMPARE(shotData.value().shot.getSLask(), "0,0");
    QCOMPARE(shotData.value().shot.X(), -75572);
    QCOMPARE(shotData.value().shot.Y(), 190960);
    QCOMPARE(shotData.value().shot.isCompetitionShot(), false);
    QCOMPARE(shotData.value().shot.shotOrigin(), Lask::Sius);
}

void SiusDataConnectionTest::test_extractShotDataSightingShot()
{
    QTextStream log = QTextStream(stdout);
    std::optional<SiusShotData> shotData = SiusDataConnection::extractShotData(
                "_TOTL;11;12;12;103;T;0;0;Q;0;0;S;0;0;\n",
                "_SHOT;11;12;12;60;5;09:49:21.82;3;1;32;7;71;0;2;-0.00762645;0.02968479;900;0;0;655.35;1792015072;64;559;0\n",
                0,
                &log,
                &settings
                );

    QCOMPARE(shotData.value().id, 12);
    QCOMPARE(shotData.value().socketIndex, 0);
    QCOMPARE(shotData.value().siusShotNo, 2);
    QCOMPARE(shotData.value().shot.getSLask(), "7,1");
    QCOMPARE(shotData.value().shot.X(), -7626);
    QCOMPARE(shotData.value().shot.Y(), 29685);
    QCOMPARE(shotData.value().shot.isCompetitionShot(), false);
    QCOMPARE(shotData.value().shot.shotOrigin(), Lask::Sius);
}

void SiusDataConnectionTest::test_handlePartialSiusTransfer()
{
    QFile file;
    QTextStream log = QTextStream(stdout);
    SiusDataConnection conn("127.0.0.1", 4000, 0, &file, &log, &settings);
    qRegisterMetaType<SiusShotData>();
    QSignalSpy spy(&conn, SIGNAL(shotRead(SiusShotData)));

    conn.siusBuffer.append("_SHOT;6;7;90;60;41;10:08:25.05;3;1;0;10;102;0;14;0.00232;-0.00593;900;0;0;655.35;1731649946;64;559;0\n"
                           "_TOTL;6;7;90;103;T;131;0;Q;0;0;S;131;0;P;131;0;G;95;0;G;36;0;\n"
                           "_SUBT;4;5;88;60;27;10:08:27.83;7;0;96;989;1731650225\n"
                           "_GRPH;1;2;85;60;28;10:08:35.86;8;0;2;1;0;0;64;1731651054\n"
                           "_SHOT;1;2;85;60;29;10:08:36.47;3;1;0;10;102;0;");

    conn.readFromSius();
    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    SiusShotData shotData = qvariant_cast<SiusShotData>(arguments.at(0));

    QCOMPARE(shotData.id, 90);
    QCOMPARE(shotData.socketIndex, 0);
    QCOMPARE(shotData.siusShotNo, 14);

    conn.siusBuffer.append("15;0.00232;-0.00593;900;0;0;655.35;1731649946;64;559;0\n"
                           "_TOTL;1;2;85;103;T;131;0;Q;0;0;S;131;0;P;131;0;G;95;0;G;36;0;\n");
    if (spy.count() < 1)
        QVERIFY(spy.wait(1000));
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments2 = spy.takeFirst();
    SiusShotData shotData2 = qvariant_cast<SiusShotData>(arguments2.at(0));
    QCOMPARE(shotData2.id, 85);
    QCOMPARE(shotData2.socketIndex, 0);
    QCOMPARE(shotData2.siusShotNo, 15);
    QCOMPARE(shotData2.shot.getSLask(), "10,2");
    QCOMPARE(shotData2.shot.X(), 2320);
    QCOMPARE(shotData2.shot.Y(), -5930);
    QCOMPARE(shotData2.shot.isCompetitionShot(), true);
    QCOMPARE(shotData2.shot.shotOrigin(), Lask::Sius);
}

QTEST_MAIN(SiusDataConnectionTest)
#include "tst_siusdataconnection.moc"

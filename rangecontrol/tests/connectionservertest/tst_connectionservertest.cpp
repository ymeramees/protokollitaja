#include <QtTest>

#include "connectionserver.h"

#define CR QChar(0x0d)
#define LF QChar(0x0a)

class ConnectionServerTest : public QObject
{
    Q_OBJECT

public:
    ConnectionServerTest();
    ~ConnectionServerTest();

private slots:
    void test_newProtocol();
    void test_oldProtocol();

};

ConnectionServerTest::ConnectionServerTest()
{

}

ConnectionServerTest::~ConnectionServerTest()
{

}

void ConnectionServerTest::test_newProtocol()
{
    QTextStream log;
    ConnectionServer server(&log, this);
    server.start(4000, 5451);
    QSignalSpy spy(&server, SIGNAL(statusUpdate(int, QString, QString)));
    qRegisterMetaType<Lask>();
    qRegisterMetaType<SiusShotData>();
    QSignalSpy shotsSpy(&server, SIGNAL(newShot(int, SiusShotData)));

    QTcpSocket inband;
    inband.connectToHost("localhost", 5451);
    inband.waitForConnected(1000);

    inband.write("1;InBand_Scoring;v4.100;192.168.1.45\r");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
    inband.write("2;InBand_Scoring;v4.100;192.168.1.46\r");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
    inband.write("3;InBand_Scoring;v4.100;192.168.1.47\r");
    inband.waitForBytesWritten(1000);
    QTest::qWait(500);

    QCOMPARE(spy.count(), 3);
    QVERIFY(inband.bytesAvailable() > 0);

    QStringList receivedRows;

    while(inband.bytesAvailable() > 0) {
        static QString inbandBuffer;
        inbandBuffer.append(inband.readAll());
        // QTextStream(stdout) << "test_sendAllShotsOnNewConnection(): inbandBuffer: " << inbandBuffer << Qt::endl;

        while(inbandBuffer.contains(CR)) {
            QString row = "";
            if (inbandBuffer.indexOf(CR, 0) > 0) {
                row = inbandBuffer.left(inbandBuffer.indexOf(CR, 0) + 1);
                inbandBuffer.remove(0, row.size());
            }
            // QTextStream(stdout) << "test_sendAllShotsOnNewConnection(): received: " << row << Qt::endl;
            receivedRows.append(row);

            QCoreApplication::processEvents();
        }
    }
    QCOMPARE(receivedRows.length(), 3);

    inband.write("2;shot;10.2;1;0.34;-1.35;false;12:03:24;false;0.6;Ok\r");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);

    inband.write("2;shot;10.5;3;12.34;-5.67;true;14:30:45;true;0.3;Test message\r");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);

    QCOMPARE(shotsSpy.count(), 2);
    SiusShotData shot1 = qvariant_cast<SiusShotData>(shotsSpy.at(0).at(1));
    SiusShotData shot2 = qvariant_cast<SiusShotData>(shotsSpy.at(1).at(1));
    QCOMPARE(shot1.siusShotNo, 1);
    // QCOMPARE(shot1.id, 0);  // Would need to be set in UI
    QCOMPARE(shot1.shot.get10Lask(), 102);
    QCOMPARE(shot1.shot.stringX(), "0.34");
    QCOMPARE(shot1.shot.stringY(), "-1.35");
    QCOMPARE(shot1.shot.shotTime(), QTime::fromString("12:03:24"));
    QCOMPARE(shot1.shot.isCompetitionShot(), false);
    QCOMPARE(shot1.shot.isInnerTen(), false);
    QCOMPARE(shot1.shot.shotOrigin(), Lask::OriginType::NewInband);

    QCOMPARE(shot2.siusShotNo, 3);
    QCOMPARE(shot2.shot.get10Lask(), 105);
    QCOMPARE(shot2.shot.stringX(), "12.34");
    QCOMPARE(shot2.shot.stringY(), "-5.67");
    QCOMPARE(shot2.shot.shotTime(), QTime::fromString("14:30:45"));
    QCOMPARE(shot2.shot.isCompetitionShot(), true);
    QCOMPARE(shot2.shot.isInnerTen(), true);
    QCOMPARE(shot2.shot.shotOrigin(), Lask::OriginType::NewInband);
}

void ConnectionServerTest::test_oldProtocol()
{
    // QString msgEnd = "message end";
    QTextStream log;
    ConnectionServer server(&log, this);
    server.start(4000, 5451);
    QSignalSpy spy(&server, SIGNAL(statusUpdate(int, QString, QString)));
    QSignalSpy infoSpy(&server, SIGNAL(info(QString)));

    QTcpSocket inband;
    inband.connectToHost("localhost", 5451);
    inband.waitForConnected(1000);

    inband.write("1\ncall in\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
    inband.write("2\ncall in\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
    inband.write("3\ncall in\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(500);

    QCOMPARE(spy.count(), 3);
    QCOMPARE(infoSpy.count(), 3);
    // QVERIFY(inband.bytesAvailable() > 0);

    // QTextStream(stdout) << "test_sendAllShotsOnNewConnection(): inband.bytesAvailable: " << inband.bytesAvailable() << Qt::endl;

    // QStringList receivedRows;

    // while(inband.bytesAvailable() > 0) {
    //     static QString inbandBuffer;
    //     inbandBuffer.append(inband.readAll());
    //     QTextStream(stdout) << "test_sendAllShotsOnNewConnection(): inbandBuffer: " << inbandBuffer << Qt::endl;

    //     while(inbandBuffer.contains(msgEnd)) {
    //         QString row = "";
    //         if (inbandBuffer.indexOf(msgEnd, 0) > 0) {
    //             row = inbandBuffer.left(inbandBuffer.indexOf(msgEnd, 0) + msgEnd.size());
    //             inbandBuffer.remove(0, row.size());
    //         }
    //         // QTextStream(stdout) << "test_sendAllShotsOnNewConnection(): received: " << row << Qt::endl;
    //         receivedRows.append(row);

    //         QCoreApplication::processEvents();
    //     }
    // }
    // QCOMPARE(receivedRows.length(), 3);
}

QTEST_MAIN(ConnectionServerTest)

#include "tst_connectionservertest.moc"

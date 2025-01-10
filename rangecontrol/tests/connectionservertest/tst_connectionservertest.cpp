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
    QSignalSpy spy(&server, SIGNAL(newTarget(int, QString)));

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
}

void ConnectionServerTest::test_oldProtocol()
{
    // QString msgEnd = "message end";
    QTextStream log;
    ConnectionServer server(&log, this);
    server.start(4000, 5451);
    QSignalSpy spy(&server, SIGNAL(newTarget(int, QString)));
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

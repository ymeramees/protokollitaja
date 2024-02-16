#include <QtTest>
#include <QCoreApplication>

#include "rangecontrol.h"
#include "lane.h"

#define CR QChar(0x0d)
#define LF QChar(0x0a)

class IntegrationTest : public QObject
{
    Q_OBJECT

public:
    IntegrationTest();
    ~IntegrationTest();

private slots:
    void test_acceptAllShotsTogether();
    void test_acceptIncomingStartList();
    void test_sendAllShotsOnNewConnection();
    void test_sendNewShotsToExistingConnection();
    void test_togetherWithProtokollitaja();

};

IntegrationTest::IntegrationTest()
{

}

IntegrationTest::~IntegrationTest()
{

}

void IntegrationTest::test_acceptAllShotsTogether()
{
    QFile file("range_control_conf.json");  // Start with clean settings
    file.remove();

    RangeControl rangeControl("Eesti");
    QTcpSocket inband;

    inband.connectToHost("localhost", 5451);
    inband.waitForConnected(1000);
    QTest::qWait(100);
    QVERIFY(inband.state() == QTcpSocket::ConnectedState);
    inband.write("4\ncall in\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
    inband.write("4\nstatus\nip_match_sighters\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
    inband.write("4\nstatus\nip_match\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
    inband.write("4\nall shots\n8.9\n5.9\n-2.9\n8.4\n-6.5\n11.9\n9.3\n4.1\n-0.3\n5.5\n4.0\n-5.1\n8.3\n-4.0\n-3.1\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(500);

    QTcpSocket protokollitaja;
    protokollitaja.connectToHost("localhost", 4000);
    protokollitaja.waitForConnected(1000);
    QVERIFY(protokollitaja.state() == QTcpSocket::ConnectedState);
    protokollitaja.waitForReadyRead(1000);
    QTest::qWait(200);
    QVERIFY(protokollitaja.bytesAvailable() > 0);

    QStringList receivedRows;

    while(protokollitaja.bytesAvailable() > 0) {
        static QString siusBuffer;
        siusBuffer.append(protokollitaja.readAll());
        QTextStream(stdout) << "test_sendAllShotsOnNewConnection(): siusBuffer: " << siusBuffer << Qt::endl;

        while(siusBuffer.contains('_')) {

            QString row = "";
            if (siusBuffer.indexOf('_', 1) == -1) {
                if (siusBuffer.contains(CR) || siusBuffer.contains(LF) || siusBuffer.contains("\n")) { // Make sure complete last row has arrived
                    row = QString("%1").arg(siusBuffer);
                    siusBuffer.clear();
                } else
                    break;
            } else {
                row = siusBuffer.left(siusBuffer.indexOf('_', 1));
                siusBuffer.remove(0, siusBuffer.indexOf('_', 1));
            }
            QTextStream(stdout) << "test_sendAllShotsOnNewConnection(): received: " << row << Qt::endl;
            receivedRows.append(row);

            QCoreApplication::processEvents();
        }
    }

    QTextStream(stdout) << "test_sendAllShotsOnNewConnection(): receivedRows: " << receivedRows.join(",") << Qt::endl;

    QList<QLabel*> labels = rangeControl.findChildren<QLabel*>();
    QCOMPARE(labels.at(1)->text(), "(5.): 8,3");
    QCOMPARE(receivedRows.size(), 11);   // Excluding sighting shots
    QCOMPARE(receivedRows.first().left(5), "_GRPH");
    QStringList rowParts = receivedRows.at(1).split(";");
    QCOMPARE(rowParts.at(9), "0");
    QCOMPARE(rowParts.at(10), "89");
    QCOMPARE(receivedRows.last().left(5), "_TOTL");
    QVERIFY(protokollitaja.state() == QTcpSocket::ConnectedState);
}

void IntegrationTest::test_acceptIncomingStartList()
{
    QFile file("range_control_conf.json");  // Start with clean settings
    file.remove();

    RangeControl rangeControl("Eesti");
    QTcpSocket inband;

    inband.connectToHost("localhost", 5451);
    inband.waitForConnected(1000);
    QTest::qWait(100);
    QVERIFY(inband.state() == QTcpSocket::ConnectedState);
    inband.write("1\ncall in\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
    inband.write("2\ncall in\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
    inband.write("3\ncall in\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(500);

    QTcpSocket protokollitaja;
    protokollitaja.connectToHost("localhost", 4000);
    protokollitaja.waitForConnected(1000);
    QVERIFY(protokollitaja.state() == QTcpSocket::ConnectedState);
    protokollitaja.waitForReadyRead(1000);
    QTest::qWait(200);

    QString data = "1;13;Isås;STRÖM;Ülenurme GSK;40l Õhupüss;1;40\n2;73;Mõttetu;MEES;Narva LSK;40l Õhupüss;1;40\n3;33;Jürto;NII-DOMMZONN;KL MäLK;40l Õhupüss;1;40";
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_5);
    out << (quint16)0;
    out << (quint16)1;  // Connection protocol version
    out << data;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    protokollitaja.write(block);
    block.clear();

    QTest::qWait(200);

    QList<QLineEdit*> textBoxes = rangeControl.findChildren<QLineEdit*>();
    QCOMPARE(textBoxes.size(), 21);
    QCOMPARE(textBoxes.at(0)->text(), "1");
    QCOMPARE(textBoxes.at(1)->text(), "13");
    QCOMPARE(textBoxes.at(2)->text(), "Isås");
    QCOMPARE(textBoxes.at(3)->text(), "STRÖM");
    QCOMPARE(textBoxes.at(4)->text(), "Ülenurme GSK");
    QCOMPARE(textBoxes.at(5)->text(), "40");
    QCOMPARE(textBoxes.at(7)->text(), "2");
    QCOMPARE(textBoxes.at(8)->text(), "73");
    QCOMPARE(textBoxes.at(9)->text(), "Mõttetu");
    QCOMPARE(textBoxes.at(10)->text(), "MEES");
    QCOMPARE(textBoxes.at(11)->text(), "Narva LSK");
    QCOMPARE(textBoxes.at(12)->text(), "40");
    QList<QSpinBox*> shotCountBoxes = rangeControl.findChildren<QSpinBox*>();
    QCOMPARE(shotCountBoxes.size(), 3);
    QCOMPARE(shotCountBoxes.at(0)->value(), 40);
    QVERIFY(protokollitaja.state() == QTcpSocket::ConnectedState);
}

void IntegrationTest::test_sendAllShotsOnNewConnection()
{
    RangeControl rangeControl("Eesti");
    QTcpSocket inband;

    inband.connectToHost("localhost", 5451);
    inband.waitForConnected(1000);
    QTest::qWait(100);
    QVERIFY(inband.state() == QTcpSocket::ConnectedState);
    inband.write("4\ncall in\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
    inband.write("4\nstatus\nip_match_sighters\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
    inband.write("4\nshot\n9.7\n1\n-14.9\n12.0\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
    inband.write("4\nshot\n9.6\n2\n-7.5\n-9.3\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
    inband.write("4\nstatus\nip_match\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
    inband.write("4\nshot\n10.7\n1\n-0.9\n0.2\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
    inband.write("4\nshot\n8.5\n2\n-17.5\n-9.2\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
    inband.write("4\nshot\n10.0\n3\n-1.5\n-0.3\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);

    QTcpSocket protokollitaja;
    protokollitaja.connectToHost("localhost", 4000);
    protokollitaja.waitForConnected(1000);
    QVERIFY(protokollitaja.state() == QTcpSocket::ConnectedState);
    protokollitaja.waitForReadyRead(1000);
    QTest::qWait(200);
    QVERIFY(protokollitaja.bytesAvailable() > 0);

    QStringList receivedRows;

    while(protokollitaja.bytesAvailable() > 0) {
        static QString siusBuffer;
        siusBuffer.append(protokollitaja.readAll());
        QTextStream(stdout) << "test_sendAllShotsOnNewConnection(): siusBuffer: " << siusBuffer << Qt::endl;

        while(siusBuffer.contains('_')) {

            QString row = "";
            if (siusBuffer.indexOf('_', 1) == -1) {
                if (siusBuffer.contains(CR) || siusBuffer.contains(LF) || siusBuffer.contains("\n")) { // Make sure complete last row has arrived
                    row = QString("%1").arg(siusBuffer);
                    siusBuffer.clear();
                } else
                    break;
            } else {
                row = siusBuffer.left(siusBuffer.indexOf('_', 1));
                siusBuffer.remove(0, siusBuffer.indexOf('_', 1));
            }
            QTextStream(stdout) << "test_sendAllShotsOnNewConnection(): received: " << row << Qt::endl;
            receivedRows.append(row);

            QCoreApplication::processEvents();
        }
    }

    QTextStream(stdout) << "test_sendAllShotsOnNewConnection(): receivedRows: " << receivedRows.join(",") << Qt::endl;

    QCOMPARE(receivedRows.size(), 7);   // Excluding sighting shots
    QCOMPARE(receivedRows.first().left(5), "_GRPH");
    QStringList rowParts = receivedRows.at(1).split(";");
    QCOMPARE(rowParts.at(9), "512");
    QCOMPARE(rowParts.at(10), "107");
    QCOMPARE(receivedRows.last().left(5), "_TOTL");
    QVERIFY(protokollitaja.state() == QTcpSocket::ConnectedState);
}

void IntegrationTest::test_sendNewShotsToExistingConnection()
{
    RangeControl rangeControl("Eesti");
    QTcpSocket inband;

    inband.connectToHost("localhost", 5451);
    inband.waitForConnected(1000);
    QTest::qWait(100);
    QVERIFY(inband.state() == QTcpSocket::ConnectedState);
    inband.write("4\ncall in\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);

    QTcpSocket protokollitaja;
    protokollitaja.connectToHost("localhost", 4000);
    protokollitaja.waitForConnected(1000);
    QVERIFY(protokollitaja.state() == QTcpSocket::ConnectedState);

    inband.write("4\nshot\n9.7\n1\n-14.9\n12.0\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
    inband.write("4\nshot\n9.6\n2\n-7.5\n-9.3\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);

    protokollitaja.waitForReadyRead(1000);
    QTest::qWait(200);
    QVERIFY(protokollitaja.bytesAvailable() > 0);

    QStringList receivedRows;

    while(protokollitaja.bytesAvailable() > 0) {
        static QString siusBuffer;
        siusBuffer.append(protokollitaja.readAll());
        QTextStream(stdout) << "test_sendAllShotsOnNewConnection(): siusBuffer: " << siusBuffer << Qt::endl;

        while(siusBuffer.contains('_')) {

            QString row = "";
            if (siusBuffer.indexOf('_', 1) == -1) {
                if (siusBuffer.contains(CR) || siusBuffer.contains(LF) || siusBuffer.contains("\n")) { // Make sure complete last row has arrived
                    row = QString("%1").arg(siusBuffer);
                    siusBuffer.clear();
                } else
                    break;
            } else {
                row = siusBuffer.left(siusBuffer.indexOf('_', 1));
                siusBuffer.remove(0, siusBuffer.indexOf('_', 1));
            }
            QTextStream(stdout) << "test_sendAllShotsOnNewConnection(): received: " << row << Qt::endl;
            receivedRows.append(row);

            QCoreApplication::processEvents();
        }
    }

    QTextStream(stdout) << "test_sendAllShotsOnNewConnection(): receivedRows: " << receivedRows.join(",") << Qt::endl;

    QCOMPARE(receivedRows.size(), 5);
    QCOMPARE(receivedRows.first().left(5), "_GRPH");
    QStringList rowParts = receivedRows.at(1).split(";");
    QCOMPARE(rowParts.at(9), "32");
    QCOMPARE(rowParts.at(10), "97");
    QCOMPARE(receivedRows.last().left(5), "_TOTL");
    QVERIFY(protokollitaja.state() == QTcpSocket::ConnectedState);
}

void IntegrationTest::test_togetherWithProtokollitaja()
{
    RangeControl rangeControl("Eesti");
    QTcpSocket inband;

    inband.connectToHost("localhost", 5451);
    inband.waitForConnected(1000);
    QTest::qWait(100);
    QVERIFY(inband.state() == QTcpSocket::ConnectedState);
    inband.write("4\ncall in\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
    QList<QLineEdit*> boxes = rangeControl.findChildren<QLineEdit*>();
    QTextStream(stdout) << "test_togetherWithProtokollitaja(): boxes.size() = " << boxes.size() << Qt::endl;
    foreach(QLineEdit *box, boxes) {
        if (box->placeholderText() == "ID")
            box->setText("4");
    }
    QTextStream(stdout) << "test_togetherWithProtokollitaja(): waiting for Protokollitaja connection..." << Qt::endl;
    QTest::qWait(3000);

    inband.write("4\nshot\n9.7\n1\n-14.9\n12.0\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
    inband.write("4\nshot\n9.6\n2\n-7.5\n-9.3\nmessage end");
    inband.waitForBytesWritten(1000);
    QTest::qWait(100);
}

QTEST_MAIN(IntegrationTest)

#include "tst_integrationtest.moc"

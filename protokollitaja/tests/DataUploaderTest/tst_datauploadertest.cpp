#include <QtTest>
#include <QCoreApplication>

#include "datauploader.h"

class DataUploaderTest : public QObject
{
    Q_OBJECT

public:
    DataUploaderTest();
    ~DataUploaderTest();

private slots:
    void test_getUnauthorizedResponseWithTestUser();

};

DataUploaderTest::DataUploaderTest()
{

}

DataUploaderTest::~DataUploaderTest()
{

}

void DataUploaderTest::test_getUnauthorizedResponseWithTestUser()
{
    DataUploader dataUploader(true, this);
    QSignalSpy spy(&dataUploader, SIGNAL(uploadFinished(bool, QString, QString)));

    QUrl url;
    url.setScheme("https");
#ifdef QT_DEBUG
    url.setHost("localhost");
    url.setPath("/api/v1/competitions");
    url.setPort(3005);
#else
    url.setHost("ymeramees.no-ip.org");
    url.setPath("/api/v1/competitions");
    url.setPort(3005);
#endif

    QString concatenated = "testuser:TestPass";
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString restHeaderData = "Basic " + data;

    QJsonDocument jsonData;

    dataUploader.uploadResults(url, restHeaderData, "", jsonData);

    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments[0].toBool(), false);
    QCOMPARE(arguments[1].toString(), "Insufficient access rights for username");
    QCOMPARE(arguments[2].toString(), "Error transferring https://ymeramees.no-ip.org:3005/api/v1/competitions - server replied: Forbidden");
}

QTEST_MAIN(DataUploaderTest)

#include "tst_datauploadertest.moc"

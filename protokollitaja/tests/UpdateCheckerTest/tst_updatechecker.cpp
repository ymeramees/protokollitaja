#include <QtTest>

#include "updatechecker.h"

class UpdateCheckerTest : public QObject
{
    Q_OBJECT

public:
    UpdateCheckerTest();
    ~UpdateCheckerTest();

private slots:
    void test_getLatestVersionInfoNewer();
    void test_getLatestVersionInfoOlder();
    void test_getLatestVersionInfoWeb();
    void test_isCurrentVersionOld();

};

UpdateCheckerTest::UpdateCheckerTest()
{

}

UpdateCheckerTest::~UpdateCheckerTest()
{

}

void UpdateCheckerTest::test_getLatestVersionInfoNewer()
{
    UpdateChecker checker("10.7.15");
    QSignalSpy spy(&checker, SIGNAL(versionInfoResponse(bool, QString)));
    checker.getLatestVersionInfo("ymeramees", "protokollitaja");

    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    bool updateExists = arguments.at(0).toBool();
    QCOMPARE(updateExists, false);
}

void UpdateCheckerTest::test_getLatestVersionInfoOlder()
{
    UpdateChecker checker("0.7.15");
    QSignalSpy spy(&checker, SIGNAL(versionInfoResponse(bool, QString)));
    checker.getLatestVersionInfo("ymeramees", "protokollitaja");

    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    bool updateExists = arguments.at(0).toBool();
    int points = arguments.at(1).toString().count('.');
    QCOMPARE(updateExists, true);
    QCOMPARE(points, 2);
}

void UpdateCheckerTest::test_getLatestVersionInfoWeb()
{
    UpdateChecker checker("0.7.15");
    QSignalSpy spy(&checker, SIGNAL(versionInfoResponse(bool, QString)));
    checker.getLatestVersionInfo("ymm", "protokollitaja");

    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    bool updateExists = arguments.at(0).toBool();
    int points = arguments.at(1).toString().count('.');
    QCOMPARE(updateExists, true);
    QCOMPARE(points, 2);
}

void UpdateCheckerTest::test_isCurrentVersionOld()
{
    UpdateChecker checker("0.7.15");
    QCOMPARE(checker.isCurrentVersionOld("0.7.15", "0.7.16"), true);
    QCOMPARE(checker.isCurrentVersionOld("0.7.15", "0.7.15"), false);
    QCOMPARE(checker.isCurrentVersionOld("0.7.15", "0.7.14"), false);

    QCOMPARE(checker.isCurrentVersionOld("0.7.15", "0.8.1"), true);
    QCOMPARE(checker.isCurrentVersionOld("0.7.15", "1.0.0"), true);
    QCOMPARE(checker.isCurrentVersionOld("0.8.0", "0.7.16"), false);
    QCOMPARE(checker.isCurrentVersionOld("10.7.15", "0.7.16"), false);
}

QTEST_MAIN(UpdateCheckerTest)

#include "tst_updatechecker.moc"

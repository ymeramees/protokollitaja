#include <QtTest>
#include <QCoreApplication>

#include "shotedit.h"

class ShotEditTest : public QObject
{
    Q_OBJECT

public:
    ShotEditTest();
    ~ShotEditTest();

private slots:
    void test_createFromJson();
    void test_ignored();
    void test_setShotFromGui();
    void test_setSiusShot();
    void test_toJson();
};

ShotEditTest::ShotEditTest()
{

}

ShotEditTest::~ShotEditTest()
{

}

void ShotEditTest::test_createFromJson()
{
    QJsonObject shotJson;
    shotJson["shotValue"] = "9,4";
    shotJson["shotX"] = -1876;
    shotJson["shotY"] = 3472;
    shotJson["shotTime"] = QTime::fromString("10:43:56.17").toString();
    shotJson["innerTen"] = false;

    QJsonObject shotEditJson;
    shotEditJson["shot"] = shotJson;
    shotEditJson["ignored"] = false;
    shotEditJson["originalShotValue"] = "10,7";

    ShotEdit shotEdit(shotEditJson);

    Lask expectedShot(shotJson);

    QVERIFY(shotEdit.shot() == expectedShot);
    QVERIFY(shotEdit.ignored() == false);
    QVERIFY(shotEdit.palette().base().color() == Qt::yellow);   // originalShotValue differs from shot value, so should be shown as edited
    QVERIFY(shotEdit.text() == "9,4");
}

void ShotEditTest::test_ignored()
{
    ShotEdit shotEdit;
    QSignalSpy spyIgnored(&shotEdit, SIGNAL(shotIgnored()));
    QSignalSpy spyUnignored(&shotEdit, SIGNAL(shotUnignored()));
    QVERIFY(shotEdit.palette().base().color() == Qt::white);
    QCOMPARE(spyIgnored.count(), 0);
    QCOMPARE(spyUnignored.count(), 0);
    shotEdit.setIgnored(true);
    QCOMPARE(spyIgnored.count(), 1);
    QCOMPARE(spyUnignored.count(), 0);
    QVERIFY(shotEdit.palette().base().color() == Qt::red);

    shotEdit.setIgnored(false);
    QCOMPARE(spyUnignored.count(), 1);
    QCOMPARE(spyIgnored.count(), 1);
    QVERIFY(shotEdit.palette().base().color() == Qt::white);
}

void ShotEditTest::test_setShotFromGui()
{
    ShotEdit shotEdit;

    QVERIFY(shotEdit.palette().base().color() == Qt::white);
    shotEdit.setText("10,4");
    QTest::keyClick(&shotEdit, Qt::Key_Return);
    QVERIFY(shotEdit.palette().base().color() == Qt::white);
    QVERIFY(shotEdit.shot().getSLask() == "10,4");
    QVERIFY(shotEdit.text() == "10,4");

    shotEdit.setSiusShot("_SHOT;9;10;36;60;74;10:43:56.17;3;31;7;94;0;0;49;-0.00187626;0.00347202;900;0;0;655.35;98903519;61;450;0");
    QVERIFY(shotEdit.palette().base().color() == Qt::white);
    QVERIFY(shotEdit.shot().getSLask() == "9,4");
    QVERIFY(shotEdit.text() == "9,4");
    shotEdit.setText("10,4");
    QTest::keyClick(&shotEdit, Qt::Key_Return);
    QVERIFY(shotEdit.palette().base().color() == Qt::yellow);
    QVERIFY(shotEdit.shot().getSLask() == "10,4");
    QVERIFY(shotEdit.text() == "10,4");

    shotEdit.setIgnored(true);
    QVERIFY(shotEdit.palette().base().color() == Qt::red);
    shotEdit.setIgnored(false);
    QVERIFY(shotEdit.palette().base().color() == Qt::yellow);

    shotEdit.setText("9,4");
    QTest::keyClick(&shotEdit, Qt::Key_Return);
    QVERIFY(shotEdit.palette().base().color() == Qt::white);
    QVERIFY(shotEdit.shot().getSLask() == "9,4");
    QVERIFY(shotEdit.text() == "9,4");
}

void ShotEditTest::test_setSiusShot()
{
    ShotEdit shotEdit;

    QVERIFY(shotEdit.text().isEmpty());
    QVERIFY(shotEdit.shot().getSLask() == "");
    shotEdit.setSiusShot("_SHOT;9;10;36;60;74;10:43:56.17;3;31;7;94;0;0;49;-0.00187626;0.00347202;900;0;0;655.35;98903519;61;450;0");

    Lask testShot("_SHOT;9;10;36;60;74;10:43:56.17;3;31;7;94;0;0;49;-0.00187626;0.00347202;900;0;0;655.35;98903519;61;450;0");
    QVERIFY(shotEdit.shot().equals(testShot));
    QVERIFY(shotEdit.text() == "9,4");
}

void ShotEditTest::test_toJson()
{
    ShotEdit shotEdit;
    shotEdit.setSiusShot("_SHOT;9;10;36;60;74;10:43:56.17;3;31;7;94;0;0;49;-0.00187626;0.00347202;900;0;0;655.35;98903519;61;450;0");

    QJsonObject json = shotEdit.toJson();
    QJsonObject expectedShotJson;
    expectedShotJson["shotValue"] = "9,4";
    expectedShotJson["shotX"] = -1876;
    expectedShotJson["shotY"] = 3472;
    expectedShotJson["shotTime"] = QTime::fromString("10:43:56.17").toString();
    expectedShotJson["innerTen"] = false;
    expectedShotJson["competitionShot"] = true;
    expectedShotJson["shotOrigin"] = 2;

    QVERIFY(json["shot"].toObject() == expectedShotJson);
    QVERIFY(json["ignored"].toBool() == false);
    QVERIFY(json["originalShotValue"].toString() == "9,4");
}

QTEST_MAIN(ShotEditTest)

#include "tst_shotedittest.moc"

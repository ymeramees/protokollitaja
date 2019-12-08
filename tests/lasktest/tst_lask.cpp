#include <QtTest>

#include "lask.h"

class LaskTest : public QObject
{
    Q_OBJECT

public:
    LaskTest();
    ~LaskTest();

private slots:
    void test_coordinates();
    void test_createShotFromSiusRow();
    void test_equalsAndSet();
    void test_json();
    void test_shotTime();
    void test_getShotValues();
    void test_setShotValues();
};

LaskTest::LaskTest()
{

}

LaskTest::~LaskTest()
{

}

void LaskTest::test_coordinates()
{
    Lask shot;

    shot.setMX("0.0424624");
    shot.setMY("-0.0002244");

    QCOMPARE(shot.X(), 42462);
    QCOMPARE(shot.Y(), -224);

    shot.setX("0.3267");
    shot.setY("98,796487");

    QCOMPARE(shot.X(), 327);
    QCOMPARE(shot.Y(), 98796);

    shot.setX(96.4228f);
    shot.setY(23.982f);

    QCOMPARE(shot.X(), 96423);
    QCOMPARE(shot.Y(), 23982);

    QPoint p(-98,94873);
    shot.setNanoXY(p);

    QCOMPARE(shot.X(), -98);
    QCOMPARE(shot.Y(), 94873);
    QVERIFY(shot.stringX().compare("-0,098"));
    QVERIFY(shot.stringY().compare("94,873"));

    shot.clear();

    QCOMPARE(shot.X(), -999);
    QCOMPARE(shot.Y(), -999);
    QVERIFY(shot.stringX().compare("-999"));
    QVERIFY(shot.stringY().compare("-999"));
}

void LaskTest::test_createShotFromSiusRow()
{
    QString siusRow = "_SHOT;19;20;10;60;6;10:09:17.56;3;1;32;9;90;0;3;-0.01513;-0.00194;900;0;0;655.35;801895602;64;559;0";
    Lask shot(siusRow);

    QCOMPARE(shot.get10Lask(), 90);
    QCOMPARE(shot.getSLask(), "9,0");
    QCOMPARE(shot.X(), -15130);
    QCOMPARE(shot.Y(), -1940);
    QCOMPARE(shot.shotTime(), QTime(10, 9, 17, 560));
    QVERIFY(shot.isInnerTen() == false);

    QString siusRow2 = "_SHOT;2;3;2504;60;24;16:46:55.84;3;31;0;88;0;0;8;-0.01735604;0.00080582;900;0;0;655.35;170209718;60;449;0";
    Lask shot2(siusRow2);

    QCOMPARE(shot2.get10Lask(), 88);
    QCOMPARE(shot2.getSLask(), "8,8");
    QCOMPARE(shot2.X(), -17356);
    QCOMPARE(shot2.Y(), 806);
    QCOMPARE(shot2.shotTime(), QTime(16, 46, 55, 840));
    QVERIFY(shot2.isInnerTen() == false);

    QString siusRow3 = "_SHOT;4;5;4304;60;24;16:46:23.03;3;31;512;106;0;0;7;-0.00242729;-0.00144642;900;0;0;655.35;170206441;60;449;0";
    Lask shot3(siusRow3);

    QCOMPARE(shot3.get10Lask(), 106);
    QCOMPARE(shot3.getSLask(), "10,6");
    QCOMPARE(shot3.X(), -2427);
    QCOMPARE(shot3.Y(), -1446);
    QCOMPARE(shot3.shotTime(), QTime(16, 46, 23, 30));
    QVERIFY(shot3.isInnerTen() == true);
}

void LaskTest::test_equalsAndSet()
{
    QTime currentTime = QTime::currentTime();
    Lask shot1(103, -1236, 63579, false, currentTime);

    QCOMPARE(shot1.get10Lask(), 103);
    QCOMPARE(shot1.X(), -1236);
    QCOMPARE(shot1.Y(), 63579);
    QVERIFY(!shot1.isInnerTen());
    QCOMPARE(shot1.shotTime(), currentTime);
    QVERIFY(!shot1.isEmpty());

    Lask shot2;
    shot2.set10Lask(103);
    shot2.setX("-1,236");
    shot2.setY("63.579");
    shot2.setInnerTen(false);
    shot2.setShotTime(currentTime);

    QVERIFY(shot1.equals(shot2));

    Lask shot3;
    shot3.set(&shot2);

    QVERIFY(shot3.equals(shot1));
}

void LaskTest::test_json()
{
    QTime time = QTime::currentTime();
    Lask shot(106, 477, -1042, true, time);

    QJsonObject json = shot.toJson();

    QVERIFY(json["shotValue"].toString().compare("10,6") == 0);
    QCOMPARE(json["shotX"].toInt(), 477);
    QCOMPARE(json["shotY"].toInt(), -1042);
    QVERIFY(json["shotTime"].toString().compare(time.toString()) == 0);
    QCOMPARE(json["innerTen"].toBool(), true);
}

void LaskTest::test_shotTime()
{
    QTime time = QTime::currentTime();
    Lask shot(56, 63477, -10462, false, time);

    QCOMPARE(shot.shotTime(), time);

    QTest::qSleep(125);    // Make sure timestamps are different
    time = QTime::currentTime();
    shot.setShotTime(time);

    QCOMPARE(shot.shotTime(), time);
}

void LaskTest::test_getShotValues()
{
    Lask shot(56, 63477, -10462, false, QTime::currentTime());

    QCOMPARE(shot.getILask(), 5);
    QCOMPARE(shot.getSLask(), "5,6");
    QCOMPARE(shot.get10Lask(), 56);
}

void LaskTest::test_setShotValues()
{
    Lask shot;
    shot.setLask(9);

    QCOMPARE(shot.getSLask(), "9,0");
    QCOMPARE(shot.get10Lask(), 90);

    shot.setLask(4.2f);

    QCOMPARE(shot.getSLask(), "4,2");
    QCOMPARE(shot.get10Lask(), 42);

    shot.setLask("7.0");

    QCOMPARE(shot.getSLask(), "7,0");
    QCOMPARE(shot.get10Lask(), 70);

    shot.set10Lask(86);

    QCOMPARE(shot.getSLask(), "8,6");
    QCOMPARE(shot.get10Lask(), 86);

    shot.set10Lask("100");

    QCOMPARE(shot.getSLask(), "10,0");
    QCOMPARE(shot.get10Lask(), 100);
}

QTEST_APPLESS_MAIN(LaskTest)

#include "tst_lask.moc"

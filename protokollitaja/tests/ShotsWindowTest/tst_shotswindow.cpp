#include <QtTest>

#include "lask.h"

bool verbose = false;
bool veryVerbose = false;

class ShotsWindowTest : public QObject
{
    Q_OBJECT

public:
    ShotsWindowTest();
    ~ShotsWindowTest();

private slots:
    void test_isFinished();
//    void test_readSiusShotAdditionalShot();
//    void test_readSiusShotAdditionalShotInMultiStage();
//    void test_readSiusShotIgnoreWrongId();
//    void test_readSiusShotIgnoreWrongSocketIndex();
//    void test_readSiusShotReadCompetitionShotsWithoutSighters();
//    void test_readSiusShotRepeatedShotDataInFirstStage();
//    void test_readSiusShotRepeatedShotDataInSecondStage();
//    void test_readSiusShotSecondStageCompetitionShots();
//    void test_seriesMissingShots();

};

ShotsWindowTest::ShotsWindowTest()
{

}

ShotsWindowTest::~ShotsWindowTest()
{

}

void ShotsWindowTest::test_isFinished()
{
//    QString eventType = "40l õhupüss";
//    bool writeAssistant = false;
//    bool withDecimals = true;
//    int sorting = 1;

//    Laskur competitor(nullptr, 4, 0, 0, &writeAssistant, &withDecimals, 35, &sorting, &eventType, 10, nullptr);
//    Lask shot("_SHOT;9;10;36;60;74;10:43:56.17;3;31;7;94;0;0;49;-0.00187626;0.00347202;900;0;0;655.35;98903519;61;450;0");

//    QVERIFY(competitor.isFinished() == false);

//    competitor.lasud[3][9]->set(&shot);
//    QVERIFY(competitor.isFinished() == true);
}

//void ShotsWindowTest::test_readSiusShotAdditionalShot()
//{
//    QString eventType = "3x40l Standard";
//    bool writeAssistant = false;
//    bool withDecimals = false;
//    int sorting = 0;

//    Laskur competitor(nullptr, 12, 4, 0, &writeAssistant, &withDecimals, 13, &sorting, &eventType, 10, nullptr);

//    SiusShotData shot1(13, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot1), true);
//    QCOMPARE(competitor.lasud[0][0]->getSLask(), "10,1");

//    SiusShotData shot2(13, 0, 40, Lask("_SHOT;17;18;13;60;31;10:05:39.28;3;1;512;10;106;0;40;0.00128;-0.00261;900;0;0;655.35;387153707;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot2), true);
//    QCOMPARE(competitor.lasud[3][9]->getSLask(), "10,6");

//    SiusShotData shot3(13, 0, 41, Lask("_SHOT;17;18;13;60;24;10:07:20.01;3;1;0;10;103;0;41;0.00256;0.00482;900;0;0;655.35;387121839;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot3), false);
//    QCOMPARE(competitor.lasud[4][0]->getSLask(), "");

//    SiusShotData shot4(13, 0, 121, Lask("_SHOT;17;18;13;60;25;10:01:02.73;3;1;0;9;96;0;121;0.01110;-0.00101;900;0;0;655.35;387126084;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot3), false);
//    QCOMPARE(competitor.lasud[11][9]->getSLask(), "");
//}

//void ShotsWindowTest::test_readSiusShotAdditionalShotInMultiStage()
//{
//    QString eventType = "3x40l Standard";
//    bool writeAssistant = false;
//    bool withDecimals = false;
//    int sorting = 0;

//    Laskur competitor(nullptr, 12, 4, 0, &writeAssistant, &withDecimals, 13, &sorting, &eventType, 10, nullptr);

//    SiusShotData shot1(13, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot1), true);
//    QCOMPARE(competitor.lasud[0][0]->getSLask(), "10,1");

//    SiusShotData shot2(13, 0, 40, Lask("_SHOT;17;18;13;60;31;10:05:39.28;3;1;512;10;106;0;40;0.00128;-0.00261;900;0;0;655.35;387153707;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot2), true);
//    QCOMPARE(competitor.lasud[3][9]->getSLask(), "10,6");

//    SiusShotData shot3(13, 0, 41, Lask("_SHOT;17;18;13;60;24;10:07:20.01;3;1;0;10;103;0;41;0.00256;0.00482;900;0;0;655.35;387121839;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot3), false);
//    QCOMPARE(competitor.lasud[4][0]->getSLask(), "");

//    SiusShotData shot4(13, 0, 1, Lask("_SHOT;17;18;13;60;78;10:47:12.22;3;1;0;10;102;0;1;-0.00469;-0.00281;900;0;0;655.35;387402959;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot4), true);

//    QCOMPARE(competitor.lasud[4][0]->getSLask(), "10,2");
//    QCOMPARE(competitor.lasud[4][1]->getSLask(), "");
//    QCOMPARE(competitor.lasud[8][0]->getSLask(), "");

//    SiusShotData shot5(13, 0, 121, Lask("_SHOT;17;18;13;60;25;10:01:02.73;3;1;0;9;96;0;121;0.01110;-0.00101;900;0;0;655.35;387126084;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot5), false);
//    QCOMPARE(competitor.lasud[11][9]->getSLask(), "");
//}

//void ShotsWindowTest::test_readSiusShotIgnoreWrongId()
//{
//    QString eventType = "3x40l Standard";
//    bool writeAssistant = false;
//    bool withDecimals = false;
//    int sorting = 0;

//    Laskur competitor(nullptr, 12, 4, 0, &writeAssistant, &withDecimals, 13, &sorting, &eventType, 10, nullptr);

//    SiusShotData shot1(17, 0, 2, Lask("_SHOT;16;17;15;60;17;09:57:56.05;3;1;0;8;88;0;2;0.01295;0.01074;900;0;0;655.35;387107423;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot1), false);
//}

//void ShotsWindowTest::test_readSiusShotIgnoreWrongSocketIndex()
//{
//    QString eventType = "3x40l Standard";
//    bool writeAssistant = false;
//    bool withDecimals = false;
//    int sorting = 0;

//    Laskur competitor(nullptr, 12, 4, 0, &writeAssistant, &withDecimals, 13, &sorting, &eventType, 10, nullptr);

//    SiusShotData shot1(13, 0, 10, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;10;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot1), true);

//    SiusShotData shot2(13, 1, 11, Lask("_SHOT;17;18;13;60;31;10:05:39.28;3;1;512;10;106;0;11;0.00128;-0.00261;900;0;0;655.35;387153707;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot2), false);
//}

//void ShotsWindowTest::test_readSiusShotReadCompetitionShotsWithoutSighters()
//{
//    QString eventType = "3x40l Standard";
//    bool writeAssistant = false;
//    bool withDecimals = false;
//    int sorting = 0;

//    Laskur competitor(nullptr, 12, 4, 0, &writeAssistant, &withDecimals, 13, &sorting, &eventType, 10, nullptr);

//    SiusShotData shot1(13, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot1), true);
//    QCOMPARE(competitor.lasud[0][0]->getSLask(), "10,1");

//    SiusShotData shot2(13, 0, 2, Lask("_SHOT;17;18;13;60;31;10:05:39.28;3;1;512;10;106;0;2;0.00128;-0.00261;900;0;0;655.35;387153707;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot2), true);
//    QCOMPARE(competitor.lasud[0][1]->getSLask(), "10,6");
//}

//void ShotsWindowTest::test_readSiusShotRepeatedShotDataInFirstStage()
//{
//    QString eventType = "3x40l Standard";
//    bool writeAssistant = false;
//    bool withDecimals = false;
//    int sorting = 0;

//    Laskur competitor(nullptr, 12, 4, 0, &writeAssistant, &withDecimals, 13, &sorting, &eventType, 10, nullptr);

//    QCOMPARE(competitor.lasud.length(), 12);
//    QCOMPARE(competitor.lasud[0].length(), 11);
//    QCOMPARE(competitor.competitionStage(), 0);

//    SiusShotData shot1(13, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot1), true);
//    SiusShotData shot2(13, 0, 40, Lask("_SHOT;17;18;13;60;64;10:28:22.14;3;1;512;10;103;0;40;-0.00464;-0.00133;900;0;0;655.35;387290026;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot2), true);

//    QCOMPARE(competitor.lasud[0][0]->getSLask(), "10,1");
//    QCOMPARE(competitor.lasud[3][9]->getSLask(), "10,3");


//    QCOMPARE(competitor.readSiusShot(shot1), true);
//    QCOMPARE(competitor.readSiusShot(shot2), true);

//    QCOMPARE(competitor.lasud[0][0]->getSLask(), "10,1");
//    QCOMPARE(competitor.lasud[3][9]->getSLask(), "10,3");
//    QCOMPARE(competitor.lasud[4][0]->getSLask(), "");
//    QCOMPARE(competitor.lasud[7][9]->getSLask(), "");

//    SiusShotData shot3(13, 0, 2, Lask("_SHOT;17;18;13;60;79;10:47:34.90;3;1;512;10;106;0;2;-0.00148;-0.00207;900;0;0;655.35;387405285;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot3), true);
//    QCOMPARE(competitor.lasud[0][1]->getSLask(), "10,6");
//}

//void ShotsWindowTest::test_readSiusShotRepeatedShotDataInSecondStage()
//{
//    QString eventType = "3x40l Standard";
//    bool writeAssistant = false;
//    bool withDecimals = false;
//    int sorting = 0;

//    Laskur competitor(nullptr, 12, 4, 0, &writeAssistant, &withDecimals, 13, &sorting, &eventType, 10, nullptr);

//    QCOMPARE(competitor.lasud.length(), 12);
//    QCOMPARE(competitor.lasud[0].length(), 11);
//    QCOMPARE(competitor.competitionStage(), 0);

//    SiusShotData shot1(13, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot1), true);
//    SiusShotData shot2(13, 0, 40, Lask("_SHOT;17;18;13;60;64;10:28:22.14;3;1;512;10;103;0;40;-0.00464;-0.00133;900;0;0;655.35;387290026;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot2), true);

//    QCOMPARE(competitor.lasud[0][0]->getSLask(), "10,1");
//    QCOMPARE(competitor.lasud[3][9]->getSLask(), "10,3");

//    SiusShotData shot3(13, 0, 1, Lask("_SHOT;17;18;13;60;78;10:47:12.22;3;1;0;10;102;0;1;-0.00469;-0.00281;900;0;0;655.35;387402959;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot3), true);

//    QCOMPARE(competitor.readSiusShot(shot1), true);
//    QCOMPARE(competitor.readSiusShot(shot2), true);
//    QCOMPARE(competitor.readSiusShot(shot3), true);

//    QCOMPARE(competitor.lasud[0][0]->getSLask(), "10,1");
//    QCOMPARE(competitor.lasud[3][9]->getSLask(), "10,3");
//    QCOMPARE(competitor.lasud[4][0]->getSLask(), "10,2");
//    QCOMPARE(competitor.lasud[7][9]->getSLask(), "");
//    QCOMPARE(competitor.lasud[8][0]->getSLask(), "");
//    QCOMPARE(competitor.lasud[11][9]->getSLask(), "");

//    SiusShotData shot4(13, 0, 2, Lask("_SHOT;17;18;13;60;79;10:47:34.90;3;1;512;10;106;0;2;-0.00148;-0.00207;900;0;0;655.35;387405285;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot4), true);

//    QCOMPARE(competitor.lasud[0][1]->getSLask(), "");
//    QCOMPARE(competitor.lasud[4][1]->getSLask(), "10,6");
//    QCOMPARE(competitor.lasud[7][9]->getSLask(), "");
//    QCOMPARE(competitor.lasud[8][0]->getSLask(), "");
//    QCOMPARE(competitor.lasud[8][1]->getSLask(), "");
//    QCOMPARE(competitor.lasud[11][9]->getSLask(), "");
//}

//void ShotsWindowTest::test_readSiusShotSecondStageCompetitionShots()
//{
//    QString eventType = "3x40l Standard";
//    bool writeAssistant = false;
//    bool withDecimals = false;
//    int sorting = 0;

//    Laskur competitor(nullptr, 12, 4, 0, &writeAssistant, &withDecimals, 13, &sorting, &eventType, 10, nullptr);
//    Lask shot("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0");
//    Lask lastShot("_SHOT;17;18;13;60;64;10:28:22.14;3;1;512;10;103;0;40;-0.00464;-0.00133;900;0;0;655.35;387290026;64;559;0");

//    QCOMPARE(competitor.lasud.length(), 12);
//    QCOMPARE(competitor.lasud[0].length(), 11);
//    QCOMPARE(competitor.competitionStage(), 0);

//    competitor.lasud[0][0]->set(&shot);
//    competitor.lasud[3][9]->set(&lastShot);
//    QCOMPARE(competitor.lasud[0][0]->getSLask(), "10,1");
//    QCOMPARE(competitor.lasud[3][9]->getSLask(), "10,3");

//    SiusShotData shot1(13, 0, 1, Lask("_SHOT;17;18;13;60;78;10:47:12.22;3;1;0;10;103;0;1;-0.00469;-0.00281;900;0;0;655.35;387402959;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot1), true);

//    QCOMPARE(competitor.lasud[0][0]->getSLask(), "10,1");
//    QCOMPARE(competitor.lasud[4][0]->getSLask(), "10,3");

//    SiusShotData shot2(13, 0, 2, Lask("_SHOT;17;18;13;60;79;10:47:34.90;3;1;512;10;106;0;2;-0.00148;-0.00207;900;0;0;655.35;387405285;64;559;0"));
//    QCOMPARE(competitor.readSiusShot(shot2), true);

//    QCOMPARE(competitor.lasud[0][1]->getSLask(), "");
//    QCOMPARE(competitor.lasud[4][1]->getSLask(), "10,6");
//}

//void ShotsWindowTest::test_seriesMissingShots()
//{
//    QString eventType = "40l õhupüss";
//    bool writeAssistant = false;
//    bool withDecimals = true;
//    int sorting = 1;

//    Laskur competitor(nullptr, 4, 0, 0, &writeAssistant, &withDecimals, 36, &sorting, &eventType, 10, nullptr);
//    Lask shot("_SHOT;9;10;36;60;74;10:43:56.17;3;31;7;94;0;0;49;-0.00187626;0.00347202;900;0;0;655.35;98903519;61;450;0");

//    competitor.lasud[0][0]->set(&shot);
//    QVERIFY(competitor.seeriad[0]->palette().base().color() == Qt::white);
//    competitor.liida();
//    QVERIFY(competitor.seeriad[0]->palette().base().color() == Qt::yellow);

//    for(int i = 0; i < 10; i++) {
//        competitor.lasud[0][i]->set(&shot);
//    }
//    competitor.liida();
//    QVERIFY(competitor.seeriad[0]->palette().base().color() == Qt::white);

//    competitor.lasud[1][4]->set(&shot);
//    QVERIFY(competitor.seeriad[1]->palette().base().color() == Qt::white);
//    competitor.liida();
//    QVERIFY(competitor.seeriad[1]->palette().base().color() == Qt::yellow);

//    competitor.lasud[3][9]->set(&shot);
//    QVERIFY(competitor.seeriad[3]->palette().base().color() == Qt::white);
//    competitor.liida();
//    QVERIFY(competitor.seeriad[3]->palette().base().color() == Qt::red);
//    QVERIFY(competitor.seeriad[1]->palette().base().color() == Qt::red);

//    for(int i = 0; i < 10; i++) {
//        competitor.lasud[1][i]->set(&shot);
//    }
//    competitor.liida();
//    QVERIFY(competitor.seeriad[1]->palette().base().color() == Qt::white);
//}

QTEST_MAIN(ShotsWindowTest)

#include "tst_shotswindow.moc"

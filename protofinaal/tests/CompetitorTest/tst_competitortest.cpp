#include <QtTest>
#include <QCoreApplication>

#include "competitor.h"

bool verbose = false;

class CompetitorTest : public QObject
{
    Q_OBJECT

public:
    CompetitorTest();
    ~CompetitorTest();

private slots:
    void test_createCompetitorFromJsonArray();
    void test_createCompetitorFromJsonArray_data();
    void test_createCompetitorFromJsonObject();
    void test_createCompetitorFromJsonObject_data();
    void test_createCompetitorFromSavedJsonObject();
    void test_handleIgnoredShot();
    void test_handleUnignoredShot();
    void test_handleUnignoredShot2();
    void test_lastResultAndSum();
    void test_readSiusShotAdditionalShot();
    void test_readSiusShotIgnoreWrongId();
    void test_readSiusShotReadCompetitionShotsWithoutSighters();
    void test_readSiusShotRepeatedShotDataInFirstStage();
    void test_shotAtOutOfBounds();
    void test_sumWithIgnoredShot();
    void test_toJson();

};

CompetitorTest::CompetitorTest()
{

}

CompetitorTest::~CompetitorTest()
{

}

void CompetitorTest::test_createCompetitorFromJsonArray()
{
    QFETCH(QJsonArray, shotsConf);
    QFETCH(int, fieldCount);
    QFETCH(int, idAndSumsCount);

    Competitor *competitor = new Competitor(1, shotsConf);
    QHBoxLayout* hBox = competitor->findChild<QHBoxLayout*>();
    QVERIFY(hBox != 0);
    QList<QLabel*> idAndSums = competitor->findChildren<QLabel*>();
    QCOMPARE(hBox->count(), fieldCount);
    QCOMPARE(idAndSums.count(), idAndSumsCount);
    QCOMPARE(idAndSums.at(0)->text(), "0"); // ID should be 0
}

void CompetitorTest::test_createCompetitorFromJsonArray_data()
{
    QTest::addColumn<QJsonArray>("shotsConf");
    QTest::addColumn<int>("fieldCount");
    QTest::addColumn<int>("idAndSumsCount");

    QTest::newRow("AR") << QJsonArray{5, 5, 14} << 29 << 4;
    QTest::newRow("2") << QJsonArray{3, 3, 10} << 21 << 4;
    QTest::newRow("3") << QJsonArray{5, 12} << 21 << 3;
    QTest::newRow("4") << QJsonArray{10} << 13 << 2;
    QTest::newRow("5") << QJsonArray{24} << 27 << 2;
    QTest::newRow("3P") << QJsonArray{5, 5, 5, 5, 5, 5, 5, 5, 5} << 56 << 10;
}

void CompetitorTest::test_createCompetitorFromJsonObject()
{
    QFETCH(QJsonObject, savedConf);
    QFETCH(QString, name);
    QFETCH(int, fieldCount);
    QFETCH(int, idAndSumsCount);

    Competitor *competitor = new Competitor(savedConf);
    QHBoxLayout* hBox = competitor->findChild<QHBoxLayout*>();
    QVERIFY(hBox != 0);
    QList<QLabel*> idAndSums = competitor->findChildren<QLabel*>();
    QCOMPARE(hBox->count(), fieldCount);
    QCOMPARE(idAndSums.count(), idAndSumsCount);
    QCOMPARE(idAndSums.at(0)->text(), "0"); // ID should be 0
    QLineEdit* nameBox = competitor->findChild<QLineEdit*>();
    QVERIFY(nameBox != 0);
    QCOMPARE(nameBox->text(), name);
    QCOMPARE(competitor->name(), name);
    QCOMPARE(competitor->lastSum(), "0,0");
}

void CompetitorTest::test_createCompetitorFromJsonObject_data()
{
    QTest::addColumn<QJsonObject>("savedConf");
    QTest::addColumn<QString>("name");
    QTest::addColumn<int>("fieldCount");
    QTest::addColumn<int>("idAndSumsCount");

    QTest::newRow("Peeter") << QJsonDocument::fromJson(QString("{\"Series\": [{\"Shots\": [\"\",\"\",\"\",\"\",\"\"],\"Sum\": \"0,0\"},{\"Shots\": [\"\",\"\",\"\",\"\",\"\"],\"Sum\": \"0,0\"},{\"Shots\": [\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"],\"Sum\": \"0,0\"}],\"id\": 0,\"nameEdit\": \"Pedaal P.\"}").toUtf8()).object() << "Pedaal P." << 29 << 4;
    QTest::newRow("Paavel") << QJsonDocument::fromJson(QString("{\"Series\": [{\"Shots\": [\"\",\"\",\"\"],\"Sum\": \"0,0\"},{\"Shots\": [\"\",\"\",\"\"],\"Sum\": \"0,0\"},{\"Shots\": [\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"],\"Sum\": \"0,0\"}],\"id\": 0,\"nameEdit\": \"Pakiraam P.\"}").toUtf8()).object() << "Pakiraam P." << 21 << 4;
}

void CompetitorTest::test_createCompetitorFromSavedJsonObject()
{
    QJsonObject savedConf = QJsonDocument::fromJson(
                QString("{\"Series\": [{\"Shots\": ["
                        "{\"ignored\": false,\"originalShotValue\":\"\",\"shot\": {\"innerTen\": true,\"shotTime\":\"\",\"shotValue\": \"10,5\",\"shotX\":-999,\"shotY\":-999}},"
                        "{\"ignored\": false,\"shotTime\":\"\",\"shot\": {\"innerTen\": true,\"shotValue\": \"10,8\"}},"
                        "{\"ignored\": false,\"shotTime\":\"\",\"shot\": {\"innerTen\": false,\"shotValue\": \"9,2\"}},"
                        "{\"ignored\": false,\"shotTime\":\"\",\"shot\": {\"innerTen\": false,\"shotValue\": \"10,0\"}},"
                        "{\"ignored\": false,\"shotTime\":\"\",\"shot\": {\"innerTen\": false,\"shotValue\": \"\"}}]"
                        ",\"Sum\": \"40,5\"},{\"Shots\": [\"\",\"\",\"\",\"\",\"\"],\"Sum\": \"0,0\"},"
                        "{\"Shots\": [\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"],"
                        "\"Sum\": \"40,5\"}],\"id\": 536,\"nameEdit\": \"Pedaal P.\"}"
                        ).toUtf8()).object();

    Competitor *competitor = new Competitor(savedConf);
    QHBoxLayout* hBox = competitor->findChild<QHBoxLayout*>();
    QVERIFY(hBox != 0);
    QList<QLabel*> idAndSums = competitor->findChildren<QLabel*>();
    QList<QLineEdit*> nameAndShots = competitor->findChildren<QLineEdit*>();

    QCOMPARE(idAndSums.at(0)->text(), "536");
    QCOMPARE(nameAndShots.at(0)->text(), "Pedaal P.");
    QCOMPARE(nameAndShots.at(1)->text(), "10,5");
    QCOMPARE(nameAndShots.at(2)->text(), "10,8");
    QCOMPARE(nameAndShots.at(3)->text(), "9,2");
    QCOMPARE(nameAndShots.at(4)->text(), "10,0");
    QCOMPARE(idAndSums.at(1)->text(), "40,5");
    QCOMPARE(idAndSums.at(2)->text(), "0,0");
    QCOMPARE(idAndSums.at(idAndSums.count() - 1)->text(), "40,5");
    QCOMPARE(competitor->id(), 536);
    QCOMPARE(competitor->name(), "Pedaal P.");
    QCOMPARE(competitor->lastResult(), "40,5");
    QCOMPARE(competitor->lastSum(), "40,5");
}

void CompetitorTest::test_handleIgnoredShot()
{
    QJsonArray conf = {5, 5, 14};
    Competitor competitor(1, conf);
    competitor.setShot(0, Lask(104, 354, -983, true, QTime::currentTime()));
    competitor.setShot(1, Lask(83, 354, -983, true, QTime::currentTime()));
    competitor.setShot(2, Lask(103, 354, -983, true, QTime::currentTime()));
    competitor.setShot(5, Lask(100, 354, -983, true, QTime::currentTime()));
    competitor.setShot(6, Lask(58, 354, -983, true, QTime::currentTime()));
    competitor.setShot(10, Lask(105, 354, -983, true, QTime::currentTime()));
    competitor.setShot(23, Lask(109, 354, -983, true, QTime::currentTime()));
    QCOMPARE(competitor.lastResult(), "10,9");
    QCOMPARE(competitor.lastSum(), "66,2");

    // Make sure sum is last in QHBoxLayout
    QHBoxLayout *hBox = competitor.findChild<QHBoxLayout*>();
    QVERIFY(dynamic_cast<QLabel*>(hBox->itemAt(hBox->count() - 1)->widget())->text() == "66,2");
    QVERIFY(dynamic_cast<ShotEdit*>(hBox->itemAt(hBox->count() - 2)->widget())->text() == "10,9");
    // Make sure 7th position is a series sum
    QVERIFY(dynamic_cast<QLabel*>(hBox->itemAt(7)->widget())->text() == "29,0");
    QVERIFY(dynamic_cast<QLabel*>(hBox->itemAt(13)->widget())->text() == "15,8");

    QList<ShotEdit*> shotEdits = competitor.findChildren<ShotEdit*>();
    QVERIFY(shotEdits.last()->text() == "10,9");
    QVERIFY(shotEdits.at(shotEdits.length() - 2)->text() == "");
    shotEdits.at(2)->setIgnored(true);
    QList<ShotEdit*> shotEdits2 = competitor.findChildren<ShotEdit*>();
    QVERIFY(shotEdits2.last()->text() == "");
    QVERIFY(shotEdits2.at(shotEdits2.length() - 2)->text() == "10,9");

    // Make sure sum is still last in QHBoxLayout
    QHBoxLayout *hBox2 = competitor.findChild<QHBoxLayout*>();
    QVERIFY(dynamic_cast<QLabel*>(hBox2->itemAt(hBox->count() - 1)->widget())->text() == "55,9");
    QVERIFY(dynamic_cast<ShotEdit*>(hBox2->itemAt(hBox->count() - 2)->widget())->text().isEmpty());
    // Make sure 7th position is now a shot and 8th position is a series sum
    QVERIFY(dynamic_cast<ShotEdit*>(hBox->itemAt(7)->widget())->text() == "10,0");
    QVERIFY(dynamic_cast<QLabel*>(hBox->itemAt(8)->widget())->text() == "28,7");
    QVERIFY(dynamic_cast<QLabel*>(hBox->itemAt(14)->widget())->text() == "16,3");
}

void CompetitorTest::test_handleUnignoredShot()
{
    QJsonArray conf = {5, 5, 14};
    Competitor competitor(1, conf);
    competitor.setShot(0, Lask(104, 354, -983, true, QTime::currentTime()));
    competitor.setShot(1, Lask(83, 354, -983, true, QTime::currentTime()));
    competitor.setShot(2, Lask(103, 354, -983, true, QTime::currentTime()));
    competitor.setShot(5, Lask(100, 354, -983, true, QTime::currentTime()));
    competitor.setShot(6, Lask(58, 354, -983, true, QTime::currentTime()));
    competitor.setShot(10, Lask(105, 354, -983, true, QTime::currentTime()));
    competitor.setShot(23, Lask(109, 354, -983, true, QTime::currentTime()));
    QCOMPARE(competitor.lastResult(), "10,9");
    QCOMPARE(competitor.lastSum(), "66,2");

    // Make sure sum is last in QHBoxLayout
    QHBoxLayout *hBox = competitor.findChild<QHBoxLayout*>();
    QCOMPARE(dynamic_cast<QLabel*>(hBox->itemAt(hBox->count() - 1)->widget())->text(), "66,2");
    QCOMPARE(dynamic_cast<ShotEdit*>(hBox->itemAt(hBox->count() - 2)->widget())->text(), "10,9");
    // Make sure 7th position is a series sum
    QCOMPARE(dynamic_cast<QLabel*>(hBox->itemAt(7)->widget())->text(), "29,0");
    QCOMPARE(dynamic_cast<QLabel*>(hBox->itemAt(13)->widget())->text(), "15,8");

    QList<ShotEdit*> shotEdits = competitor.findChildren<ShotEdit*>();
    QCOMPARE(shotEdits.last()->text(), "10,9");
    QCOMPARE(shotEdits.at(shotEdits.length() - 2)->text(), "");
    QCOMPARE(dynamic_cast<ShotEdit*>(hBox->itemAt(8)->widget())->text(), "10,0");
    shotEdits.at(2)->setIgnored(true);
    shotEdits.at(6)->setIgnored(true);
    QList<ShotEdit*> shotEdits2 = competitor.findChildren<ShotEdit*>();
    QCOMPARE(shotEdits2.last()->text(), "");
    QCOMPARE(shotEdits2.at(shotEdits2.length() - 2)->text(), "");
    QCOMPARE(shotEdits2.at(shotEdits2.length() - 3)->text(), "10,9");
    QCOMPARE(dynamic_cast<ShotEdit*>(hBox->itemAt(7)->widget())->text(), "10,0");
    QCOMPARE(dynamic_cast<QLabel*>(hBox->itemAt(8)->widget())->text(), "28,7");
    QCOMPARE(dynamic_cast<QLabel*>(hBox->itemAt(15)->widget())->text(), "10,5");
    QHBoxLayout *hBox2 = competitor.findChild<QHBoxLayout*>();
    QCOMPARE(dynamic_cast<QLabel*>(hBox2->itemAt(hBox->count() - 1)->widget())->text(), "50,1");
    QVERIFY(dynamic_cast<ShotEdit*>(hBox2->itemAt(hBox->count() - 2)->widget())->text().isEmpty());
    QVERIFY(dynamic_cast<ShotEdit*>(hBox2->itemAt(hBox->count() - 3)->widget())->text().isEmpty());


    shotEdits.at(2)->setIgnored(false);
    // Make sure 8th position is now a shot and 7th position is a series sum
    QCOMPARE(dynamic_cast<ShotEdit*>(hBox->itemAt(8)->widget())->text(), "10,0");
    QCOMPARE(dynamic_cast<QLabel*>(hBox->itemAt(7)->widget())->text(), "29,0");
    QCOMPARE(dynamic_cast<QLabel*>(hBox->itemAt(14)->widget())->text(), "20,5");
}

void CompetitorTest::test_handleUnignoredShot2()
{
    QJsonArray conf = {5, 5, 14};
    Competitor competitor(1, conf);
    competitor.setShot(0, Lask(104, 354, -983, true, QTime::currentTime()));
    competitor.setShot(4, Lask(105, 354, -983, true, QTime::currentTime()));
    competitor.setShot(5, Lask(106, 354, -983, true, QTime::currentTime()));
    competitor.setShot(9, Lask(107, 354, -983, true, QTime::currentTime()));
    competitor.setShot(10, Lask(108, 354, -983, true, QTime::currentTime()));
    competitor.setShot(23, Lask(109, 354, -983, true, QTime::currentTime()));
    QCOMPARE(competitor.lastResult(), "10,9");
    QCOMPARE(competitor.lastSum(), "63,9");

    // Make sure sum is last in QHBoxLayout
    QHBoxLayout *hBox = competitor.findChild<QHBoxLayout*>();
    QCOMPARE(dynamic_cast<QLabel*>(hBox->itemAt(hBox->count() - 1)->widget())->text(), "63,9");
    QCOMPARE(dynamic_cast<ShotEdit*>(hBox->itemAt(hBox->count() - 2)->widget())->text(), "10,9");
    // Make sure 7th position is a series sum
    QCOMPARE(dynamic_cast<QLabel*>(hBox->itemAt(7)->widget())->text(), "20,9");
    QCOMPARE(dynamic_cast<QLabel*>(hBox->itemAt(13)->widget())->text(), "21,3");

    QList<ShotEdit*> shotEdits = competitor.findChildren<ShotEdit*>();
    QCOMPARE(shotEdits.last()->text(), "10,9");
    QCOMPARE(shotEdits.at(shotEdits.length() - 2)->text(), "");
    shotEdits.at(7)->setIgnored(true);
    QList<ShotEdit*> shotEdits2 = competitor.findChildren<ShotEdit*>();
    QCOMPARE(shotEdits2.last()->text(), "");
    QCOMPARE(shotEdits2.at(shotEdits2.length() - 2)->text(), "10,9");
    QCOMPARE(dynamic_cast<QLabel*>(hBox->itemAt(7)->widget())->text(), "20,9");
    QCOMPARE(dynamic_cast<QLabel*>(hBox->itemAt(14)->widget())->text(), "32,1");
    QHBoxLayout *hBox2 = competitor.findChild<QHBoxLayout*>();
    QCOMPARE(dynamic_cast<QLabel*>(hBox2->itemAt(hBox->count() - 1)->widget())->text(), "63,9");
    QVERIFY(dynamic_cast<ShotEdit*>(hBox2->itemAt(hBox->count() - 2)->widget())->text().isEmpty());


    shotEdits.at(7)->setIgnored(false);
    QCOMPARE(dynamic_cast<QLabel*>(hBox->itemAt(7)->widget())->text(), "20,9");
    QCOMPARE(dynamic_cast<QLabel*>(hBox->itemAt(13)->widget())->text(), "21,3");
}

void CompetitorTest::test_lastResultAndSum()
{
    QJsonArray conf = {5, 5, 14};
    Competitor competitor(1, conf);
    competitor.setShot(0, Lask(104, 354, -983, true, QTime::currentTime()));
    QCOMPARE(competitor.lastResult(), "10,4");
    QCOMPARE(competitor.lastSum(), "10,4");

    competitor.setShot(1, Lask(83, 354, -983, true, QTime::currentTime()));
    QCOMPARE(competitor.lastResult(), "18,7");
    QCOMPARE(competitor.lastSum(), "18,7");

    competitor.setShot(5, Lask(100, 354, -983, true, QTime::currentTime()));
    QCOMPARE(competitor.lastResult(), "10,0");
    QCOMPARE(competitor.lastSum(), "28,7");

    competitor.setShot(6, Lask(58, 354, -983, true, QTime::currentTime()));
    QCOMPARE(competitor.lastResult(), "15,8");
    QCOMPARE(competitor.lastSum(), "34,5");

    competitor.setShot(10, Lask(109, 354, -983, true, QTime::currentTime()));
    QCOMPARE(competitor.lastResult(), "10,9");
    QCOMPARE(competitor.lastSum(), "45,4");

    competitor.setShot(23, Lask(100, 354, -983, true, QTime::currentTime()));
    QCOMPARE(competitor.lastResult(), "10,0");
    QCOMPARE(competitor.lastSum(), "55,4");
}

void CompetitorTest::test_readSiusShotAdditionalShot()
{
    QJsonArray conf = {3, 3, 10};
    Competitor competitor(13, conf);

    SiusShotData shot1(13, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0"));
    QCOMPARE(competitor.readSiusShot(shot1), true);
    QCOMPARE(competitor.shotAt(0)->getSLask(), "10,1");

    SiusShotData shot2(13, 0, 16, Lask("_SHOT;17;18;13;60;31;10:05:39.28;3;1;512;10;106;0;16;0.00128;-0.00261;900;0;0;655.35;387153707;64;559;0"));
    QCOMPARE(competitor.readSiusShot(shot2), true);
    QCOMPARE(competitor.shotAt(15)->getSLask(), "10,6");

    SiusShotData shot3(13, 0, 16, Lask("_SHOT;17;18;13;60;24;10:07:20.01;3;1;0;10;103;0;16;0.00256;0.00482;900;0;0;655.35;387121839;64;559;0"));
    QCOMPARE(competitor.readSiusShot(shot3), false);
    QCOMPARE(competitor.shotAt(15)->getSLask(), "10,6");
}

void CompetitorTest::test_readSiusShotIgnoreWrongId()
{
    QJsonArray conf = {3, 3, 10};
    Competitor competitor(13, conf);

    SiusShotData shot1(17, 0, 2, Lask("_SHOT;16;17;15;60;17;09:57:56.05;3;1;0;8;88;0;2;0.01295;0.01074;900;0;0;655.35;387107423;64;559;0"));
    QCOMPARE(competitor.readSiusShot(shot1), false);
}

void CompetitorTest::test_readSiusShotReadCompetitionShotsWithoutSighters()
{
    QJsonArray conf = {3, 3, 10};
    Competitor competitor(13, conf);

    SiusShotData shot1(13, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0"));
    QCOMPARE(competitor.readSiusShot(shot1), true);
    QCOMPARE(competitor.shotAt(0)->getSLask(), "10,1");

    SiusShotData shot2(13, 0, 2, Lask("_SHOT;17;18;13;60;31;10:05:39.28;3;1;512;10;106;0;2;0.00128;-0.00261;900;0;0;655.35;387153707;64;559;0"));
    QCOMPARE(competitor.readSiusShot(shot2), true);
    QCOMPARE(competitor.shotAt(1)->getSLask(), "10,6");
}

void CompetitorTest::test_readSiusShotRepeatedShotDataInFirstStage()
{
    QJsonArray conf = {5, 5, 14};
    Competitor competitor(13, conf);

    SiusShotData shot1(13, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0"));
    QCOMPARE(competitor.readSiusShot(shot1), true);
    SiusShotData shot2(13, 0, 24, Lask("_SHOT;17;18;13;60;64;10:28:22.14;3;1;512;10;103;0;40;-0.00464;-0.00133;900;0;0;655.35;387290026;64;559;0"));
    QCOMPARE(competitor.readSiusShot(shot2), true);

    QCOMPARE(competitor.shotAt(0)->getSLask(), "10,1");
    QCOMPARE(competitor.shotAt(23)->getSLask(), "10,3");


    QCOMPARE(competitor.readSiusShot(shot1), true);
    QCOMPARE(competitor.readSiusShot(shot2), true);

    QCOMPARE(competitor.shotAt(0)->getSLask(), "10,1");
    QCOMPARE(competitor.shotAt(23)->getSLask(), "10,3");
    QCOMPARE(competitor.shotAt(1)->getSLask(), "");
    QCOMPARE(competitor.shotAt(22)->getSLask(), "");

    SiusShotData shot3(13, 0, 2, Lask("_SHOT;17;18;13;60;79;10:47:34.90;3;1;512;10;106;0;2;-0.00148;-0.00207;900;0;0;655.35;387405285;64;559;0"));
    QCOMPARE(competitor.readSiusShot(shot3), true);
    QCOMPARE(competitor.shotAt(1)->getSLask(), "10,6");
}

void CompetitorTest::test_shotAtOutOfBounds()
{

    QJsonArray conf = {5, 5, 14};
    Competitor competitor(13, conf);
    competitor.setShot(0, Lask(104, 354, -983, true, QTime::currentTime()));
    QCOMPARE(competitor.lastResult(), "10,4");
    QCOMPARE(competitor.lastSum(), "10,4");
    QCOMPARE(competitor.shotAt(0)->getSLask(), "10,4");
    QCOMPARE(competitor.shotAt(24), std::nullopt);
}

void CompetitorTest::test_sumWithIgnoredShot()
{
    QJsonArray conf = {5, 5, 14};
    Competitor competitor(13, conf);
    competitor.setShot(0, Lask(104, 354, -983, true, QTime::currentTime()));
    QCOMPARE(competitor.lastResult(), "10,4");
    QCOMPARE(competitor.lastSum(), "10,4");

    competitor.setShot(1, Lask(83, 354, -983, true, QTime::currentTime()));
    QCOMPARE(competitor.lastResult(), "18,7");
    QCOMPARE(competitor.lastSum(), "18,7");

    competitor.setShot(3, Lask(100, 354, -983, true, QTime::currentTime()));
    QCOMPARE(competitor.lastResult(), "28,7");
    QCOMPARE(competitor.lastSum(), "28,7");

    QList<ShotEdit*> shotEdits = competitor.findChildren<ShotEdit*>();
    shotEdits.at(3)->setIgnored(true);
    QCOMPARE(competitor.lastResult(), "18,7");
    QCOMPARE(competitor.lastSum(), "18,7");

    competitor.setShot(10, Lask(58, 354, -983, true, QTime::currentTime()));
    QCOMPARE(competitor.lastResult(), "5,8");
    QCOMPARE(competitor.lastSum(), "24,5");

    competitor.setShot(11, Lask(109, 354, -983, true, QTime::currentTime()));
    QCOMPARE(competitor.lastResult(), "10,9");
    QCOMPARE(competitor.lastSum(), "35,4");

    shotEdits.at(11)->setIgnored(true);
    QCOMPARE(competitor.lastResult(), "5,8");
    QCOMPARE(competitor.lastSum(), "24,5");
}

void CompetitorTest::test_toJson()
{
    // FIXME To be fixed
//    QJsonObject savedConf = QJsonDocument::fromJson(
//                QString("{\"Series\": [{\"Shots\": ["
//                         "{\"ignored\": false,\"originalShotValue\":\"\",\"shot\": {\"innerTen\": true,\"shotTime\":\"\",\"shotValue\": \"10,5\",\"shotX\":-999,\"shotY\":-999}},"
//                         "{\"ignored\": false,\"shotTime\":\"\",\"shot\": {\"innerTen\": true,\"shotValue\": \"10,8\"}},"
//                        "{\"ignored\": false,\"shotTime\":\"\",\"shot\": {\"innerTen\": false,\"shotValue\": \"9,2\"}},"
//                        "{\"ignored\": false,\"shotTime\":\"\",\"shot\": {\"innerTen\": false,\"shotValue\": \"10,0\"}},"
//                        "{\"ignored\": false,\"shotTime\":\"\",\"shot\": {\"innerTen\": false,\"shotValue\": \"\"}}]"
//                        ",\"Sum\": \"40,5\"},{\"Shots\": [\"\",\"\",\"\",\"\",\"\"],\"Sum\": \"0,0\"},"
//                        "{\"Shots\": [\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"],"
//                        "\"Sum\": \"40,5\"}],\"id\": 536,\"nameEdit\": \"Pedaal P.\"}"
//                        ).toUtf8()).object();

//    Competitor *competitor = new Competitor(savedConf);
////    QString json = QJsonDocument(competitor->toJson()).toJson(QJsonDocument::Compact);
////    QTextStream(stdout) << json << Qt::endl;

//    QJsonObject expectedJson = QJsonDocument::fromJson(
//                QString("{\"Series\":[{\"Shots\":[{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":true,\"shotTime\":\"\",\"shotValue\":\"10,5\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":true,\"shotTime\":\"\",\"shotValue\":\"10,8\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"9,2\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"10,0\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}}],\"Sum\":\"40,5\"},{\"Shots\":[{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}}],\"Sum\":\"0,0\"},{\"Shots\":[{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}},{\"ignored\":false,\"originalShotValue\":\"\",\"shot\":{\"innerTen\":false,\"shotTime\":\"\",\"shotValue\":\"\",\"shotX\":-999,\"shotY\":-999}}],\"Sum\":\"40,5\"}],\"id\":536,\"nameEdit\":\"Pedaal P.\"}"
//                        ).toUtf8()).object();

//    QCOMPARE(competitor->toJson(), expectedJson);
}

QTEST_MAIN(CompetitorTest)

#include "tst_competitortest.moc"

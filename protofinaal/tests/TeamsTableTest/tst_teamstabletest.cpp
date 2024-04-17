#include <QtTest>
#include <QtDebug>
#include <QCoreApplication>

#include "teamstable.h"

bool verbose = false;

class TeamsTableTest : public QObject
{
    Q_OBJECT

public:
    TeamsTableTest();
    ~TeamsTableTest();

private slots:
    void test_pointsGivenWithNoEqualResults();
    void test_pointsGivenWithEqualResults();
    void test_readSiusShotWithOffset();
    void test_returnSortedResults();

};

TeamsTableTest::TeamsTableTest()
{

}

TeamsTableTest::~TeamsTableTest()
{

}

void TeamsTableTest::test_pointsGivenWithNoEqualResults()
{
    QJsonObject json = QJsonDocument::fromJson(QString("{\"event\": \"60l Õhupüss\",\"teams\": 4,\"membersInTeam\": 1,\"relaysTogether\": 1,\"shots\": [10, 5],\"scoringWithPoints\": true}").toUtf8()).object();

                                                                                                                                                                                                           TeamsTable teamsTable;
    teamsTable.createLayout(json, true);

    QCOMPARE(teamsTable.lastValidShotIndex(), -1);

    QVector<SiusShotData> shot1;
    shot1.append(SiusShotData(11, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot1.append(SiusShotData(21, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;107;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot1.append(SiusShotData(31, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;105;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot1.append(SiusShotData(41, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;98;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));

    teamsTable.readSiusInfo(shot1.first());
    QCOMPARE(teamsTable.lastValidShotIndex(), 0);
    TeamsTable::Result expected1 = { "", "10,1", "", "0", 0 };
    TeamsTable::Result actual1 = teamsTable.getCurrentResults().first();
    QCOMPARE(actual1.shotValue, expected1.shotValue);
    QCOMPARE(actual1.seriesOrPoints, expected1.seriesOrPoints);
    QCOMPARE(actual1.totalScore, expected1.totalScore);

    foreach(SiusShotData shotData, shot1) {
        teamsTable.readSiusInfo(shotData);
    }
    QVector<TeamsTable::Result> expected2;
    expected2.append(TeamsTable::Result { "", "10,1", "2", "2", 20 });
    expected2.append(TeamsTable::Result { "", "10,7", "4", "4", 40 });
    expected2.append(TeamsTable::Result { "", "10,5", "3", "3", 30 });
    expected2.append(TeamsTable::Result { "", "9,8", "1", "1", 10 });

    auto actual2 = teamsTable.getCurrentResults();
    for(int i = 0; i < 4; i++) {
        QCOMPARE(actual2.value(i + 1).shotValue, expected2.at(i).shotValue);
        QCOMPARE(actual2.value(i + 1).seriesOrPoints, expected2.at(i).seriesOrPoints);
        QCOMPARE(actual2.value(i + 1).totalScore, expected2.at(i).totalScore);
    }

    QList<Competitor*> competitors = teamsTable.findChildren<Competitor*>();
    competitors.last()->setActive(false);

    QVector<SiusShotData> shot2;
    shot2.append(SiusShotData(11, 0, 12, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;103;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot2.append(SiusShotData(21, 0, 12, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;106;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot2.append(SiusShotData(31, 0, 12, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;100;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));

    foreach(SiusShotData shotData, shot2) {
        teamsTable.readSiusInfo(shotData);
    }
    QCOMPARE(teamsTable.lastValidShotIndex(), 11);
    QVector<TeamsTable::Result> expected3;
    expected3.append(TeamsTable::Result { "", "10,3", "3", "5", 50 });
    expected3.append(TeamsTable::Result { "", "10,6", "4", "8", 80 });
    expected3.append(TeamsTable::Result { "", "10,0", "2", "5", 50 });
    expected3.append(TeamsTable::Result { "", "", "", "1", 10 });

    auto actual3 = teamsTable.getCurrentResults();
    for(int i = 0; i < 4; i++) {
        QCOMPARE(actual3.value(i + 1).shotValue, expected3.at(i).shotValue);
        QCOMPARE(actual3.value(i + 1).seriesOrPoints, expected3.at(i).seriesOrPoints);
        QCOMPARE(actual3.value(i + 1).totalScore, expected3.at(i).totalScore);
    }

    SiusShotData shot3(11, 0, 13, Lask("_SHOT;17;18;13;60;31;10:05:39.28;3;1;512;10;106;0;16;0.00128;-0.00261;900;0;0;655.35;387153707;64;559;0"));
    teamsTable.readSiusInfo(shot3);
    QCOMPARE(teamsTable.lastValidShotIndex(), 12);
    TeamsTable::Result expected4 = { "", "10,6", "", "5", 50 };
    TeamsTable::Result actual4 = teamsTable.getCurrentResults().first();
    QCOMPARE(actual4.shotValue, expected4.shotValue);
    QCOMPARE(actual4.seriesOrPoints, expected4.seriesOrPoints);
    QCOMPARE(actual4.totalScore, expected4.totalScore);
}

void TeamsTableTest::test_pointsGivenWithEqualResults()
{
    QJsonObject json = QJsonDocument::fromJson(QString("{\"event\": \"60l Õhupüss\",\"teams\": 4,\"membersInTeam\": 1,\"relaysTogether\": 1,\"shots\": [10, 5],\"scoringWithPoints\": true}").toUtf8()).object();

                                                                                                                                                                                                           TeamsTable teamsTable;
    teamsTable.createLayout(json, true);

    QCOMPARE(teamsTable.lastValidShotIndex(), -1);

    QVector<SiusShotData> shot1;
    shot1.append(SiusShotData(11, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot1.append(SiusShotData(21, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;107;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot1.append(SiusShotData(31, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;107;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot1.append(SiusShotData(41, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;98;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));

    teamsTable.readSiusInfo(shot1.first());
    QCOMPARE(teamsTable.lastValidShotIndex(), 0);
    TeamsTable::Result expected1 = { "", "10,1", "", "0", 0 };
    TeamsTable::Result actual1 = teamsTable.getCurrentResults().first();
    QCOMPARE(actual1.shotValue, expected1.shotValue);
    QCOMPARE(actual1.seriesOrPoints, expected1.seriesOrPoints);
    QCOMPARE(actual1.totalScore, expected1.totalScore);

    foreach(SiusShotData shotData, shot1) {
        teamsTable.readSiusInfo(shotData);
    }
    QVector<TeamsTable::Result> expected2;
    expected2.append(TeamsTable::Result { "", "10,1", "2", "2", 20 });
    expected2.append(TeamsTable::Result { "", "10,7", "3,5", "3,5", 35 });
    expected2.append(TeamsTable::Result { "", "10,7", "3,5", "3,5", 35 });
    expected2.append(TeamsTable::Result { "", "9,8", "1", "1", 10 });

    auto actual2 = teamsTable.getCurrentResults();
    for(int i = 0; i < 4; i++) {
        QCOMPARE(actual2.value(i + 1).shotValue, expected2.at(i).shotValue);
        QCOMPARE(actual2.value(i + 1).seriesOrPoints, expected2.at(i).seriesOrPoints);
        QCOMPARE(actual2.value(i + 1).totalScore, expected2.at(i).totalScore);
    }

    QVector<SiusShotData> shot2;
    shot2.append(SiusShotData(11, 0, 2, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;107;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot2.append(SiusShotData(21, 0, 2, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;107;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot2.append(SiusShotData(31, 0, 2, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;107;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot2.append(SiusShotData(41, 0, 2, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;98;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));

    foreach(SiusShotData shotData, shot2) {
        teamsTable.readSiusInfo(shotData);
    }
    QCOMPARE(teamsTable.lastValidShotIndex(), 1);

    QList<Team*> teams = teamsTable.findChildren<Team*>();
    QVERIFY(teams.size() > 0);
    QList<Competitor*> competitors = teams.first()->findChildren<Competitor*>();
    QVERIFY(competitors.size() > 0);
    QList<ShotEdit*> shotEdits = competitors.first()->findChildren<ShotEdit*>();
    QVERIFY(shotEdits.size() > 0);

    for (int i = 0; i < competitors.size(); i++) {
        QList<QLineEdit*> lineEdits = competitors.at(i)->findChildren<QLineEdit*>();
        QCOMPARE(competitors.at(i)->id(), 11);
        QCOMPARE(lineEdits.first()->placeholderText(), "Nimi");
        lineEdits.first()->setText(QString("Competitor%1").arg(i));
        QCOMPARE(competitors.at(i)->name(), QString("Competitor%1").arg(i));
    }
    QCOMPARE(teams.first()->teamName(), "Competitor0");

    QVector<TeamsTable::Result> expected3;
    expected3.append(TeamsTable::Result { "Competitor0", "10,7", "3", "5", 50 });
    expected3.append(TeamsTable::Result { "", "10,7", "3", "6,5", 65 });
    expected3.append(TeamsTable::Result { "", "10,7", "3", "6,5", 65 });
    expected3.append(TeamsTable::Result { "", "9,8", "1", "2", 20 });

    auto actual3 = teamsTable.getCurrentResults();
    for(int i = 0; i < 4; i++) {
        QCOMPARE(actual3.value(i + 1).name, expected3.at(i).name);
        QCOMPARE(actual3.value(i + 1).shotValue, expected3.at(i).shotValue);
        QCOMPARE(actual3.value(i + 1).seriesOrPoints, expected3.at(i).seriesOrPoints);
        QCOMPARE(actual3.value(i + 1).totalScore, expected3.at(i).totalScore);
    }
}

void TeamsTableTest::test_readSiusShotWithOffset()
{
    QJsonObject json = QJsonDocument::fromJson(QString("{\"event\": \"60l Õhupüss\",\"teams\": 2,\"membersInTeam\": 1,\"relaysTogether\": 1,\"shots\": [17],\"scoringWithPoints\": true}").toUtf8()).object();

                                                                                                                                                                                                        TeamsTable teamsTable;
    teamsTable.createLayout(json, true);

    QCOMPARE(teamsTable.lastValidShotIndex(), -1);

    QList<Competitor*> competitors = teamsTable.findChildren<Competitor*>();
    foreach(Competitor *competitor, competitors) {
        QSpinBox *siusOffset = competitor->findChild<QSpinBox*>();
        siusOffset->setValue(-15);
    }

    QVector<SiusShotData> shot1;
    shot1.append(SiusShotData(11, 0, 16, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot1.append(SiusShotData(21, 0, 16, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;107;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));

    foreach(SiusShotData shotData, shot1) {
        teamsTable.readSiusInfo(shotData);
    }
    QCOMPARE(teamsTable.lastValidShotIndex(), 0);

    QVector<TeamsTable::Result> expected1;
    expected1.append(TeamsTable::Result { "", "10,1", "0", "0", 0 });
    expected1.append(TeamsTable::Result { "", "10,7", "2", "2", 20 });
    auto actual1 = teamsTable.getCurrentResults();
    for(int i = 0; i < expected1.size(); i++) {
        QCOMPARE(actual1.value(i+1).name, expected1.at(i).name);
        QCOMPARE(actual1.value(i+1).shotValue, expected1.at(i).shotValue);
        QCOMPARE(actual1.value(i+1).seriesOrPoints, expected1.at(i).seriesOrPoints);
        QCOMPARE(actual1.value(i+1).totalScore, expected1.at(i).totalScore);
    }
}

void TeamsTableTest::test_returnSortedResults()
{
    QJsonObject json = QJsonDocument::fromJson(QString("{\"event\": \"60l Õhupüss\",\"teams\": 4,\"membersInTeam\": 1,\"relaysTogether\": 1,\"shots\": [10, 5],\"scoringWithPoints\": true}").toUtf8()).object();

                                                                                                                                                                                                           TeamsTable teamsTable;
    teamsTable.createLayout(json, true);

    QCOMPARE(teamsTable.lastValidShotIndex(), -1);

    QVector<SiusShotData> shot1;
    shot1.append(SiusShotData(11, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot1.append(SiusShotData(21, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;107;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot1.append(SiusShotData(31, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;105;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot1.append(SiusShotData(41, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;98;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));

    teamsTable.readSiusInfo(shot1.first());
    QCOMPARE(teamsTable.lastValidShotIndex(), 0);
    TeamsTable::Result expected1 = { "", "10,1", "", "0", 0 };
    TeamsTable::Result actual1 = teamsTable.getSortedResults().last();
    QCOMPARE(actual1.shotValue, expected1.shotValue);
    QCOMPARE(actual1.seriesOrPoints, expected1.seriesOrPoints);
    QCOMPARE(actual1.totalScore, expected1.totalScore);

    foreach(SiusShotData shotData, shot1) {
        teamsTable.readSiusInfo(shotData);
    }
    QVector<TeamsTable::Result> expected2;
    expected2.append(TeamsTable::Result { "", "9,8", "1", "1", 98 });
    expected2.append(TeamsTable::Result { "", "10,1", "2", "2", 101 });
    expected2.append(TeamsTable::Result { "", "10,5", "3", "3", 105 });
    expected2.append(TeamsTable::Result { "", "10,7", "4", "4", 107 });

    auto actual2 = teamsTable.getSortedResults();
    int i = 0;
    foreach(auto currentResult, actual2) {
        QCOMPARE(currentResult.shotValue, expected2.at(i).shotValue);
        QCOMPARE(currentResult.seriesOrPoints, expected2.at(i).seriesOrPoints);
        QCOMPARE(currentResult.totalScore, expected2.at(i).totalScore);
        i++;
    }

    QList<Competitor*> competitors = teamsTable.findChildren<Competitor*>();
    competitors.last()->setActive(false);

    QVector<SiusShotData> shot2;
    shot2.append(SiusShotData(11, 0, 12, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;103;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot2.append(SiusShotData(21, 0, 12, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;106;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot2.append(SiusShotData(31, 0, 12, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;100;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));

    foreach(SiusShotData shotData, shot2) {
        teamsTable.readSiusInfo(shotData);
    }
    QCOMPARE(teamsTable.lastValidShotIndex(), 11);
    QVector<TeamsTable::Result> expected3;
    expected3.append(TeamsTable::Result { "", "", "", "1", 0 });
    expected3.append(TeamsTable::Result { "", "10,0", "2", "5", 100 });
    expected3.append(TeamsTable::Result { "", "10,3", "3", "5", 103 });
    expected3.append(TeamsTable::Result { "", "10,6", "4", "8", 106 });

    auto actual3 = teamsTable.getSortedResults();
    i = 0;
    foreach(auto currentResult, actual3) {
        QCOMPARE(currentResult.shotValue, expected3.at(i).shotValue);
        QCOMPARE(currentResult.seriesOrPoints, expected3.at(i).seriesOrPoints);
        QCOMPARE(currentResult.totalScore, expected3.at(i).totalScore);
        i++;
    }

    SiusShotData shot3(21, 0, 13, Lask("_SHOT;17;18;13;60;31;10:05:39.28;3;1;512;10;106;0;16;0.00128;-0.00261;900;0;0;655.35;387153707;64;559;0"));
    teamsTable.readSiusInfo(shot3);
    QCOMPARE(teamsTable.lastValidShotIndex(), 12);
    TeamsTable::Result expected4 = { "", "10,6", "", "8", 106 };
    auto actual4 = teamsTable.getSortedResults();
    QCOMPARE(actual4.last().shotValue, expected4.shotValue);
    QCOMPARE(actual4.last().seriesOrPoints, expected4.seriesOrPoints);
    QCOMPARE(actual4.last().totalScore, expected4.totalScore);
}

QTEST_MAIN(TeamsTableTest)

#include "tst_teamstabletest.moc"

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
    void test_lastResultAndSum();
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

    Competitor *competitor = new Competitor(shotsConf);
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
    QJsonObject savedConf = QJsonDocument::fromJson(QString("{\"Series\": [{\"Shots\": [\"10,5\",\"10,8\",\"9,2\",\"10,0\",\"\"],\"Sum\": \"0,0\"},{\"Shots\": [\"\",\"\",\"\",\"\",\"\"],\"Sum\": \"0,0\"},{\"Shots\": [\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"],\"Sum\": \"0,0\"}],\"id\": 536,\"nameEdit\": \"Pedaal P.\"}").toUtf8()).object();

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

void CompetitorTest::test_lastResultAndSum()
{
    QJsonArray conf = {5, 5, 14};
    Competitor *competitor = new Competitor(conf);
    competitor->setShot(0, "10,4");
    competitor->sum();
    QCOMPARE(competitor->lastResult(), "10,4");
    QCOMPARE(competitor->lastSum(), "10,4");

    competitor->setShot(1, "8,3");
    competitor->sum();
    QCOMPARE(competitor->lastResult(), "18,7");
    QCOMPARE(competitor->lastSum(), "18,7");

    competitor->setShot(5, "10,0");
    competitor->sum();
    QCOMPARE(competitor->lastResult(), "10,0");
    QCOMPARE(competitor->lastSum(), "28,7");

    competitor->setShot(6, "5,8");
    competitor->sum();
    QCOMPARE(competitor->lastResult(), "15,8");
    QCOMPARE(competitor->lastSum(), "34,5");

    competitor->setShot(10, "10,9");
    competitor->sum();
    QCOMPARE(competitor->lastResult(), "10,9");
    QCOMPARE(competitor->lastSum(), "45,4");
}

void CompetitorTest::test_toJson()
{
    QJsonObject savedConf = QJsonDocument::fromJson(QString("{\"Series\": [{\"Shots\": [\"10,5\",\"10,8\",\"9,2\",\"10,0\",\"\"],\"Sum\": \"40,5\"},{\"Shots\": [\"\",\"\",\"\",\"\",\"\"],\"Sum\": \"0,0\"},{\"Shots\": [\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"],\"Sum\": \"40,5\"}],\"id\": 536,\"nameEdit\": \"Pedaal P.\"}").toUtf8()).object();

    Competitor *competitor = new Competitor(savedConf);
    QCOMPARE(competitor->toJson(), savedConf);
}

QTEST_MAIN(CompetitorTest)

#include "tst_competitortest.moc"

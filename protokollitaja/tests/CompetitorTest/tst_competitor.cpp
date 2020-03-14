#include <QtTest>

#include "laskur.h"

bool verbose = false;
bool veryVerbose = false;

class CompetitorTest : public QObject
{
    Q_OBJECT

public:
    CompetitorTest();
    ~CompetitorTest();

private slots:
    void test_isFinished();
    void test_seriesMissingShots();

};

CompetitorTest::CompetitorTest()
{

}

CompetitorTest::~CompetitorTest()
{

}

void CompetitorTest::test_isFinished()
{
    QString eventType = "60l õhupüss";
    bool writeAssistant = false;
    bool withDecimals = true;
    int sorting = 1;

    Laskur competitor(nullptr, 4, 0, 0, &writeAssistant, &withDecimals, 35, &sorting, &eventType, 10, nullptr);
    Lask shot("_SHOT;9;10;36;60;74;10:43:56.17;3;31;7;94;0;0;49;-0.00187626;0.00347202;900;0;0;655.35;98903519;61;450;0");

    QVERIFY(competitor.isFinished() == false);

    competitor.lasud[3][9]->set(&shot);
    QVERIFY(competitor.isFinished() == true);
}

void CompetitorTest::test_seriesMissingShots()
{
    QString eventType = "60l õhupüss";
    bool writeAssistant = false;
    bool withDecimals = true;
    int sorting = 1;

    Laskur competitor(nullptr, 4, 0, 0, &writeAssistant, &withDecimals, 35, &sorting, &eventType, 10, nullptr);
    Lask shot("_SHOT;9;10;36;60;74;10:43:56.17;3;31;7;94;0;0;49;-0.00187626;0.00347202;900;0;0;655.35;98903519;61;450;0");

    competitor.lasud[0][0]->set(&shot);
    QVERIFY(competitor.seeriad[0]->palette().base().color() == Qt::white);
    competitor.liida();
    QVERIFY(competitor.seeriad[0]->palette().base().color() == Qt::yellow);

    for(int i = 0; i < 10; i++) {
        competitor.lasud[0][i]->set(&shot);
    }
    competitor.liida();
    QVERIFY(competitor.seeriad[0]->palette().base().color() == Qt::white);

    competitor.lasud[1][4]->set(&shot);
    QVERIFY(competitor.seeriad[1]->palette().base().color() == Qt::white);
    competitor.liida();
    QVERIFY(competitor.seeriad[1]->palette().base().color() == Qt::yellow);

    competitor.lasud[3][9]->set(&shot);
    QVERIFY(competitor.seeriad[3]->palette().base().color() == Qt::white);
    competitor.liida();
    QVERIFY(competitor.seeriad[3]->palette().base().color() == Qt::red);
    QVERIFY(competitor.seeriad[1]->palette().base().color() == Qt::red);

    for(int i = 0; i < 10; i++) {
        competitor.lasud[1][i]->set(&shot);
    }
    competitor.liida();
    QVERIFY(competitor.seeriad[1]->palette().base().color() == Qt::white);
}

QTEST_MAIN(CompetitorTest)

#include "tst_competitor.moc"

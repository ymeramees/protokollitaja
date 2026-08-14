#include <QtTest>

#include "duelview.h"
#include "leht.h"

bool verbose = false;
bool veryVerbose = false;

class DuelViewTest : public QObject
{
    Q_OBJECT

public:
    DuelViewTest();
    ~DuelViewTest();

private slots:
    void test_gunTypes();
    void test_shotValues();
    void test_targetViewsAreCreatedForEveryCompetitor();
    void test_targetViewsAreKeptForTheSameMatch();
};

DuelViewTest::DuelViewTest()
{

}

DuelViewTest::~DuelViewTest()
{

}

namespace {

Leht* createDuelSheet(Andmebaas *dataBase, LiikmeteValikKast *membersBox, bool *writeAssistant, int *sorting, int pairs)
{
    Leht *sheet = new Leht(dataBase, 4, 0, 1, writeAssistant, "40 AR Duel", TargetTypes::AirRifle,
                           QualificationEvents::AirRifle40, false, sorting, nullptr, Leht::Duel, membersBox, 1, 10);
    for(int i = 0; i < pairs; i++)
        sheet->addDuelPair(i * 2 + 1, i * 2 + 2);
    return sheet;
}

}

void DuelViewTest::test_gunTypes()
{
    QCOMPARE(DuelView::gunType(TargetTypes::AirRifle), 0);
    QCOMPARE(DuelView::gunType(TargetTypes::RunningTarget10m), 0);
    QCOMPARE(DuelView::gunType(TargetTypes::Other), 0);
    QCOMPARE(DuelView::gunType(TargetTypes::AirPistol), 1);
    QCOMPARE(DuelView::gunType(TargetTypes::FreePistol), 1);
    QCOMPARE(DuelView::gunType(TargetTypes::SmallboreRifle), 2);
    QCOMPARE(DuelView::gunType(TargetTypes::Rifle300m), 2);
}

void DuelViewTest::test_shotValues()
{
    Lask innerTen(105, 1000, 2000, true);   // 10,5
    Lask ten(102, 3000, 4000);   // 10,2
    Lask nine(94, 10000, 10000);   // 9,4
    Lask emptyShot;

    // With full rings the decimal value is added in parenthesis and inner tens are marked
    QCOMPARE(DuelCompetitorView::shotValue(&innerTen, false), QString("10* (10,5)"));
    QCOMPARE(DuelCompetitorView::shotValue(&ten, false), QString("10 (10,2)"));
    QCOMPARE(DuelCompetitorView::shotValue(&nine, false), QString("9 (9,4)"));

    // With decimals the value itself is enough
    QCOMPARE(DuelCompetitorView::shotValue(&ten, true), QString("10,2"));
    QCOMPARE(DuelCompetitorView::shotValue(&innerTen, true), QString("10,5*"));

    QCOMPARE(DuelCompetitorView::shotValue(&emptyShot, false), QString(""));
    QCOMPARE(DuelCompetitorView::shotValue(nullptr, true), QString(""));
}

void DuelViewTest::test_targetViewsAreCreatedForEveryCompetitor()
{
    Andmebaas dataBase;
    LiikmeteValikKast membersBox;
    bool writeAssistant = false;
    int sorting = 0;

    Leht *sheet = createDuelSheet(&dataBase, &membersBox, &writeAssistant, &sorting, 3);

    DuelView view;
    view.setHeading("Test Competition", sheet->duelName(), "14.08.2026 Tallinn");
    view.showMatch(sheet->duelPairs, TargetTypes::AirRifle);

    QList<DuelCompetitorView*> competitorViews = view.findChildren<DuelCompetitorView*>();
    QCOMPARE(competitorViews.count(), 6);   // Both competitors of every pair have their own target view

    // The views are created in pair order, the competitor of the first team first
    QCOMPARE(competitorViews[0]->competitor(), sheet->duelPairs[0]->left());
    QCOMPARE(competitorViews[1]->competitor(), sheet->duelPairs[0]->right());
    QCOMPARE(competitorViews[4]->competitor(), sheet->duelPairs[2]->left());
    QCOMPARE(competitorViews[5]->competitor(), sheet->duelPairs[2]->right());

    delete sheet;
}

void DuelViewTest::test_targetViewsAreKeptForTheSameMatch()
{
    Andmebaas dataBase;
    LiikmeteValikKast membersBox;
    bool writeAssistant = false;
    int sorting = 0;

    Leht *sheet = createDuelSheet(&dataBase, &membersBox, &writeAssistant, &sorting, 2);
    Leht *anotherSheet = createDuelSheet(&dataBase, &membersBox, &writeAssistant, &sorting, 2);

    DuelView view;
    view.showMatch(sheet->duelPairs, TargetTypes::AirRifle);
    DuelCompetitorView *firstView = view.findChildren<DuelCompetitorView*>().first();

    // Updating the same match keeps the target views, so that the shots already drawn are kept
    view.showMatch(sheet->duelPairs, TargetTypes::AirRifle);
    QCOMPARE(view.findChildren<DuelCompetitorView*>().count(), 4);
    QCOMPARE(view.findChildren<DuelCompetitorView*>().first(), firstView);

    // Another match needs new target views
    view.showMatch(anotherSheet->duelPairs, TargetTypes::AirRifle);
    QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);   // The old views are deleted later
    QList<DuelCompetitorView*> competitorViews = view.findChildren<DuelCompetitorView*>();
    QCOMPARE(competitorViews.count(), 4);
    QCOMPARE(competitorViews[0]->competitor(), anotherSheet->duelPairs[0]->left());

    delete sheet;
    delete anotherSheet;
}

QTEST_MAIN(DuelViewTest)

#include "tst_duelview.moc"

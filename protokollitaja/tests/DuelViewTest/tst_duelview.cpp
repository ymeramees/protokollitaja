#include <QtTest>

#include "duelview.h"
#include "leht.h"
#include "target.h"

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
    void test_sightingShotsAreShownUntilTheCompetitionBegins();
    void test_targetViewsAreCreatedForEveryCompetitor();
    void test_targetViewsAreKeptForTheSameMatch();
    void test_targetsAreZoomedAccordingToTheFurthestShot();
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

void shootFirstSeries(Laskur *competitor, const QList<QPointF> &shotsInMm)
{
    for(int i = 0; i < shotsInMm.count() && i < competitor->lasud[0].count(); i++){
        competitor->lasud[0][i]->set10Lask(100);
        competitor->lasud[0][i]->setMmX(float(shotsInMm[i].x()));
        competitor->lasud[0][i]->setMmY(float(shotsInMm[i].y()));
    }
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

/**
 * The sighting shots are shown on the targets as well, so that the spectators can already follow
 * the competitors. The target is cleared once the first competition shot arrives, which can be
 * seen from the zoom level: only the competition shots are taken into account after that.
 */
void DuelViewTest::test_sightingShotsAreShownUntilTheCompetitionBegins()
{
    const int wholeTarget = 856;   // Width of the whole air rifle target with its margins, in the target image's px
    const int zoomedTo10mm = 576;   // (10 mm * 16 px + 2 * 32 px) * 2 + 32 px * 4
    const int zoomedTo2mm = 320;   // (2 mm * 16 px + 2 * 32 px) * 2 + 32 px * 4

    Andmebaas dataBase;
    LiikmeteValikKast membersBox;
    bool writeAssistant = false;
    int sorting = 0;

    Leht *sheet = createDuelSheet(&dataBase, &membersBox, &writeAssistant, &sorting, 1);
    Laskur *competitor = sheet->duelPairs[0]->left();

    DuelView view;
    view.showMatch(sheet->duelPairs, TargetTypes::AirRifle);
    QList<Target*> targets = view.findChildren<Target*>();
    QCOMPARE(targets[0]->zoomedWidth(), wholeTarget);   // Without shots the whole target is shown

    competitor->addSightingShot(Lask(100, 10000, 0, false, QTime(10, 0, 0), false));   // 10,0 at 10 mm
    view.showMatch(sheet->duelPairs, TargetTypes::AirRifle);
    QCOMPARE(targets[0]->zoomedWidth(), zoomedTo10mm);   // The sighting shots are drawn onto the target

    shootFirstSeries(competitor, QList<QPointF>() << QPointF(2.0, 0.0));
    view.showMatch(sheet->duelPairs, TargetTypes::AirRifle);
    QCOMPARE(targets[0]->zoomedWidth(), zoomedTo2mm);   // The sighting shots have been wiped off the target

    delete sheet;
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

/**
 * Every competitor's target has to be zoomed automatically, according to that competitor's
 * furthest shot, so that the spectators can see the shots as big as possible.
 */
void DuelViewTest::test_targetsAreZoomedAccordingToTheFurthestShot()
{
    const int wholeTarget = 856;   // Width of the whole air rifle target with its margins, in the target image's px
    const int zoomedTo6mm = 448;   // (6 mm * 16 px + 2 * 32 px) * 2 + 32 px * 4

    Andmebaas dataBase;
    LiikmeteValikKast membersBox;
    bool writeAssistant = false;
    int sorting = 0;

    Leht *sheet = createDuelSheet(&dataBase, &membersBox, &writeAssistant, &sorting, 1);

    DuelView view;
    view.showMatch(sheet->duelPairs, TargetTypes::AirRifle);
    QList<Target*> targets = view.findChildren<Target*>();
    QCOMPARE(targets.count(), 2);
    QCOMPARE(targets[0]->zoomedWidth(), wholeTarget);   // Without shots the whole target is shown

    shootFirstSeries(sheet->duelPairs[0]->left(), QList<QPointF>() << QPointF(1.5, -2.0) << QPointF(-6.0, 3.0)
                                                                  << QPointF(0.5, 1.0));
    view.showMatch(sheet->duelPairs, TargetTypes::AirRifle);

    QCOMPARE(targets[0]->zoomedWidth(), zoomedTo6mm);   // Zoomed according to the furthest shot
    QCOMPARE(targets[1]->zoomedWidth(), wholeTarget);   // The other competitor has not shot yet

    delete sheet;
}

QTEST_MAIN(DuelViewTest)

#include "tst_duelview.moc"

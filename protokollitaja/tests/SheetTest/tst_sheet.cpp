#include <QtTest>

#include <memory>

#include "leht.h"
#include "spectatorpage.h"
//#include "protokollitaja.h"

bool verbose = false;
bool veryVerbose = false;

class SheetTest : public QObject
{
    Q_OBJECT

public:
    SheetTest();
    ~SheetTest();

private slots:
    void test_exportStartList();
    void test_duelPairsAreCreated();
    void test_duelPointsByTotals();
    void test_duelExportJson();
    void test_duelIsNotSorted();
    void test_duelFileRoundTrip();
    void test_sheetsOnPageUsesFreeRows();
    void test_sheetsOnPageRequiresSameLayout();
    void test_sheetsOnPageStopsAtUnsuitableSheet();
    void test_sheetsOnPageFirstSheetTooBig();
    void test_combinedPageRows();
    void test_combinedPageSkipsHiddenSheets();
    void test_combinedPageFallsBackToSingleSheet();

};

SheetTest::SheetTest()
{

}

SheetTest::~SheetTest()
{

}

namespace {

void setSeries(Laskur *competitor, const QStringList &series)
{
    for (int i = 0; i < series.count() && i < competitor->seeriad.count(); i++)
        competitor->seeriad[i]->setText(series[i]);
    competitor->liida();
}

SpectatorPage::SheetInfo combinableSheet(int competitorRows, int seriesCount = 4, int subtotalStep = 0)
{
    SpectatorPage::SheetInfo info;
    info.combinable = true;
    info.seriesCount = seriesCount;
    info.subtotalStep = subtotalStep;
    info.competitorRows = competitorRows;
    return info;
}

struct SheetFactory {
    Andmebaas dataBase;
    LiikmeteValikKast membersBox;
    bool writeAssistant = false;
    int sorting = 0;

    std::unique_ptr<Leht> sheet(const QString &name, int competitors)
    {
        auto sheet = std::make_unique<Leht>(&dataBase, 4, 0, 1, &writeAssistant, name, TargetTypes::AirRifle, QualificationEvents::AirRifle40, false, &sorting, nullptr, Leht::Individual, &membersBox, 1, 10);
        for (int i = 0; i < competitors; i++)
            sheet->uusLaskur(i + 1);
        return sheet;
    }
};

void setCompetitor(Laskur *competitor, const QString &lastName, const QStringList &series)
{
    competitor->perekNimi->setText(lastName);
    setSeries(competitor, series);
}

}

void SheetTest::test_exportStartList()
{
    Andmebaas dataBase;
    LiikmeteValikKast membersBox;
    QualificationEvents::EventType eventType = QualificationEvents::AirRifle40;
    bool writeAssistant = false;
    bool withDecimals = true;
    int sorting = 1;

    Leht sheet = Leht(&dataBase, 4, 0, 1, &writeAssistant, "40 AR", TargetTypes::AirRifle, eventType, withDecimals, &sorting, nullptr, Leht::Individual, &membersBox, 1, 10);

    sheet.uusLaskur(13);

    // Laskur competitor(nullptr, 4, 0, 0, &writeAssistant, &withDecimals, 35, &sorting, &eventType, 10, nullptr);
    // Lask shot("_SHOT;9;10;36;60;74;10:43:56.17;3;31;7;94;0;0;49;-0.00187626;0.00347202;900;0;0;655.35;98903519;61;450;0");

    QCOMPARE(sheet.laskurid.count(), 1);
    QCOMPARE(sheet.laskurid[0]->id, 13);

    sheet.laskurid[0]->linnuke->setChecked(true);
    sheet.laskurid[0]->eesNimi->setText("Isås");
        sheet.laskurid[0]->perekNimi->setText("STRÖM");
        sheet.laskurid[0]->klubi->setText("Ülenurme GSK");

    QString expectedData = ";13;Isås;STRÖM;Ülenurme GSK;1;1;40";
    auto exportData = sheet.exportStartList();
    QVERIFY(exportData.has_value());
    QCOMPARE(exportData.value(), expectedData);
    // competitor.lasud[3][9]->set(&shot);
    // QVERIFY(competitor.isFinished() == true);
}

void SheetTest::test_duelPairsAreCreated()
{
    Andmebaas dataBase;
    LiikmeteValikKast membersBox;
    bool writeAssistant = false;
    int sorting = 0;

    Leht sheet = Leht(&dataBase, 4, 0, 1, &writeAssistant, "40 AR Duel", TargetTypes::AirRifle, QualificationEvents::AirRifle40, false, &sorting, nullptr, Leht::Duel, &membersBox, 1, 10);

    QCOMPARE(sheet.pageType(), Leht::Duel);
    QCOMPARE(sheet.voistk, false);

    sheet.addDuelPair(1, 2);
    sheet.addDuelPair(3, 4);

    QCOMPARE(sheet.duelPairs.count(), 2);
    QCOMPARE(sheet.laskurid.count(), 4);
    // Competitors are kept in pair order, left one first
    QCOMPARE(sheet.laskurid[0]->id, 1);
    QCOMPARE(sheet.laskurid[1]->id, 2);
    QCOMPARE(sheet.laskurid[2]->id, 3);
    QCOMPARE(sheet.laskurid[3]->id, 4);
    QCOMPARE(sheet.duelPairs[1]->left(), sheet.laskurid[2]);
    QCOMPARE(sheet.duelPairs[1]->right(), sheet.laskurid[3]);

    // Without results there are no points yet
    QVERIFY(!sheet.duelPairs[0]->leftPoints().has_value());
    QVERIFY(!sheet.duelPairs[0]->rightPoints().has_value());
    QCOMPARE(sheet.leftMatchPoints(), 0);
    QCOMPARE(sheet.rightMatchPoints(), 0);

    // Removing one competitor of a pair removes the whole pair
    sheet.duelPairs[0]->right()->linnuke->setChecked(true);
    sheet.eemaldaLaskur();
    QCOMPARE(sheet.duelPairs.count(), 1);
    QCOMPARE(sheet.laskurid.count(), 2);
    QCOMPARE(sheet.laskurid[0]->id, 3);
}

void SheetTest::test_duelPointsByTotals()
{
    Andmebaas dataBase;
    LiikmeteValikKast membersBox;
    bool writeAssistant = false;
    int sorting = 0;

    Leht sheet = Leht(&dataBase, 4, 0, 1, &writeAssistant, "40 AR Duel", TargetTypes::AirRifle, QualificationEvents::AirRifle40, false, &sorting, nullptr, Leht::Duel, &membersBox, 1, 10);

    sheet.addDuelPair(1, 2);
    sheet.addDuelPair(3, 4);
    sheet.addDuelPair(5, 6);
    sheet.addDuelPair(7, 8);

    // Only the higher total wins
    setSeries(sheet.duelPairs[0]->left(), {"98", "97", "99", "96"});
    setSeries(sheet.duelPairs[0]->right(), {"95", "97", "99", "96"});

    // Equal totals give no points to either of them, not even inner tens decide the winner
    setSeries(sheet.duelPairs[1]->left(), {"97", "97", "97", "97"});
    setSeries(sheet.duelPairs[1]->right(), {"97", "97", "97", "97"});
    sheet.duelPairs[1]->left()->kumned->setText("3");
    sheet.duelPairs[1]->right()->kumned->setText("5");

    // Completely equal results, no winner
    setSeries(sheet.duelPairs[2]->left(), {"90", "90", "90", "90"});
    setSeries(sheet.duelPairs[2]->right(), {"90", "90", "90", "90"});

    // An excluded competitor loses the pair, even with the higher total
    setSeries(sheet.duelPairs[3]->left(), {"98", "98", "98", "98"});
    sheet.duelPairs[3]->left()->markus->setText("DSQ");
    setSeries(sheet.duelPairs[3]->right(), {"80", "80", "80", "80"});

    sheet.updateDuelPoints();

    QCOMPARE(sheet.duelPairs[0]->leftPoints().value_or(-1), 1);
    QCOMPARE(sheet.duelPairs[0]->rightPoints().value_or(-1), 0);
    QCOMPARE(sheet.duelPairs[1]->leftPoints().value_or(-1), 0);
    QCOMPARE(sheet.duelPairs[1]->rightPoints().value_or(-1), 0);
    QCOMPARE(sheet.duelPairs[2]->leftPoints().value_or(-1), 0);
    QCOMPARE(sheet.duelPairs[2]->rightPoints().value_or(-1), 0);
    QCOMPARE(sheet.duelPairs[3]->leftPoints().value_or(-1), 0);
    QCOMPARE(sheet.duelPairs[3]->rightPoints().value_or(-1), 1);

    QCOMPARE(sheet.leftMatchPoints(), 1);
    QCOMPARE(sheet.rightMatchPoints(), 1);
}

void SheetTest::test_duelExportJson()
{
    Andmebaas dataBase;
    LiikmeteValikKast membersBox;
    bool writeAssistant = false;
    int sorting = 0;

    Leht sheet = Leht(&dataBase, 4, 0, 1, &writeAssistant, "40 AR Duel", TargetTypes::AirRifle, QualificationEvents::AirRifle40, false, &sorting, nullptr, Leht::Duel, &membersBox, 3, 10);

    sheet.leftTeamName->setText("Estonia");
    sheet.rightTeamName->setText("Latvia");
    sheet.addDuelPair(11, 12);
    sheet.addDuelPair(13, 14);

    setSeries(sheet.duelPairs[0]->left(), {"98", "97", "99", "96"});
    setSeries(sheet.duelPairs[0]->right(), {"95", "97", "99", "96"});
    setSeries(sheet.duelPairs[1]->left(), {"90", "91", "92", "93"});
    setSeries(sheet.duelPairs[1]->right(), {"94", "91", "92", "93"});

    QCOMPARE(sheet.duelName(), QString("Estonia : Latvia"));

    QJsonObject json = sheet.toExportJson();

    QVERIFY(json.contains("duels"));
    QJsonArray duels = json["duels"].toArray();
    QCOMPARE(duels.count(), 1);

    QJsonObject duel = duels[0].toObject();
    QCOMPARE(duel["name"].toString(), QString("Estonia : Latvia"));
    QCOMPARE(duel["leftPoints"].toInt(), 1);
    QCOMPARE(duel["rightPoints"].toInt(), 1);

    QJsonArray pairs = duel["pairs"].toArray();
    QCOMPARE(pairs.count(), 2);

    // Pairs must carry the whole competitor object on each side, not just a reference to one
    QJsonObject firstPair = pairs[0].toObject();
    QVERIFY(firstPair["left"].isObject());
    QVERIFY(firstPair["right"].isObject());
    QCOMPARE(firstPair["left"].toObject()["id"].toString(), QString("11"));
    QCOMPARE(firstPair["left"].toObject()["localId"].toString(), QString("11"));
    QCOMPARE(firstPair["right"].toObject()["id"].toString(), QString("12"));
    QCOMPARE(firstPair["right"].toObject()["localId"].toString(), QString("12"));
    QCOMPARE(firstPair["leftPoints"].toInt(), 1);
    QCOMPARE(firstPair["rightPoints"].toInt(), 0);

    QJsonObject secondPair = pairs[1].toObject();
    QCOMPARE(secondPair["left"].toObject()["id"].toString(), QString("13"));
    QCOMPARE(secondPair["leftPoints"].toInt(), 0);
    QCOMPARE(secondPair["rightPoints"].toInt(), 1);

    // Competitors are embedded in the pairs, so the top-level "competitors" array must be empty
    QJsonArray competitors = json["competitors"].toArray();
    QCOMPARE(competitors.count(), 0);
}

void SheetTest::test_duelIsNotSorted()
{
    Andmebaas dataBase;
    LiikmeteValikKast membersBox;
    bool writeAssistant = false;
    int sorting = 0;

    Leht sheet = Leht(&dataBase, 4, 0, 1, &writeAssistant, "40 AR Duel", TargetTypes::AirRifle, QualificationEvents::AirRifle40, false, &sorting, nullptr, Leht::Duel, &membersBox, 1, 10);

    sheet.addDuelPair(1, 2);
    sheet.addDuelPair(3, 4);

    // The last competitor has the best result, but the order of the pairs must not change
    setSeries(sheet.duelPairs[0]->left(), {"90", "90", "90", "90"});
    setSeries(sheet.duelPairs[1]->right(), {"99", "99", "99", "99"});

    sheet.reasta(0);

    QCOMPARE(sheet.laskurid.count(), 4);
    QCOMPARE(sheet.laskurid[0]->id, 1);
    QCOMPARE(sheet.laskurid[3]->id, 4);
    QCOMPARE(sheet.reasLaskurid.count(), 4);
    QCOMPARE(sheet.reasLaskurid[0]->id, 1);
    QCOMPARE(sheet.reasLaskurid[3]->id, 4);
}

void SheetTest::test_duelFileRoundTrip()
{
    Andmebaas dataBase;
    LiikmeteValikKast membersBox;
    bool writeAssistant = false;
    int sorting = 0;

    Leht sheet = Leht(&dataBase, 4, 0, 1, &writeAssistant, "40 AR Duel", TargetTypes::AirRifle, QualificationEvents::AirRifle40, false, &sorting, nullptr, Leht::Duel, &membersBox, 1, 10);

    sheet.leftTeamName->setText("Estonia");
    sheet.rightTeamName->setText("Latvia");
    sheet.addDuelPair(11, 12);
    sheet.addDuelPair(13, 14);

    setSeries(sheet.duelPairs[0]->left(), {"98", "97", "99", "96"});
    setSeries(sheet.duelPairs[0]->right(), {"95", "97", "99", "96"});
    sheet.duelPairs[0]->left()->eesNimi->setText("Kai");
    sheet.duelPairs[0]->left()->perekNimi->setText("Kask");
    sheet.duelPairs[0]->right()->eesNimi->setText("Tiit");
    sheet.duelPairs[0]->right()->perekNimi->setText("Tamm");
    setSeries(sheet.duelPairs[1]->left(), {"90", "91", "92", "93"});
    setSeries(sheet.duelPairs[1]->right(), {"94", "91", "92", "93"});
    sheet.updateDuelPoints();

    // Simulates what kirjutaFail() writes for a duel page: only the file-JSON of the pairs, no points
    QJsonArray savedPairs;
    for (DuelPair *pair : sheet.duelPairs)
        savedPairs.append(pair->toFileJson());

    Leht loaded = Leht(&dataBase, 4, 0, 1, &writeAssistant, "40 AR Duel", TargetTypes::AirRifle, QualificationEvents::AirRifle40, false, &sorting, nullptr, Leht::Duel, &membersBox, 1, 10);
    loaded.leftTeamName->setText(sheet.leftTeamName->text());
    loaded.rightTeamName->setText(sheet.rightTeamName->text());
    for (const QJsonValue &pairJson : savedPairs)
        loaded.addDuelPair(pairJson.toObject());

    QCOMPARE(loaded.duelName(), QString("Estonia : Latvia"));
    QCOMPARE(loaded.duelPairs.count(), 2);

    // Competitor data (id, names, results) must survive the round trip
    QCOMPARE(loaded.duelPairs[0]->left()->id, 11);
    QCOMPARE(loaded.duelPairs[0]->left()->eesNimi->text(), QString("Kai"));
    QCOMPARE(loaded.duelPairs[0]->left()->perekNimi->text(), QString("Kask"));
    QCOMPARE(loaded.duelPairs[0]->left()->getSumma(), sheet.duelPairs[0]->left()->getSumma());
    QCOMPARE(loaded.duelPairs[0]->right()->id, 12);
    QCOMPARE(loaded.duelPairs[0]->right()->eesNimi->text(), QString("Tiit"));
    QCOMPARE(loaded.duelPairs[1]->left()->id, 13);
    QCOMPARE(loaded.duelPairs[1]->right()->id, 14);

    // Points are recalculated from the loaded results, not read from the file
    QCOMPARE(loaded.duelPairs[0]->leftPoints(), sheet.duelPairs[0]->leftPoints());
    QCOMPARE(loaded.duelPairs[0]->rightPoints(), sheet.duelPairs[0]->rightPoints());
    QCOMPARE(loaded.duelPairs[1]->leftPoints(), sheet.duelPairs[1]->leftPoints());
    QCOMPARE(loaded.duelPairs[1]->rightPoints(), sheet.duelPairs[1]->rightPoints());
    QCOMPARE(loaded.leftMatchPoints(), sheet.leftMatchPoints());
    QCOMPARE(loaded.rightMatchPoints(), sheet.rightMatchPoints());
}

void SheetTest::test_sheetsOnPageUsesFreeRows()
{
    // 9 rows for the first sheet, then 11 rows with the separator and heading, 5 rows are left
    QCOMPARE(SpectatorPage::sheetsOnPage({combinableSheet(9), combinableSheet(9), combinableSheet(3)}, 25), (QList<int>{0, 1, 2}));
    QCOMPARE(SpectatorPage::sheetsOnPage({combinableSheet(9), combinableSheet(9), combinableSheet(4)}, 25), (QList<int>{0, 1}));
    QCOMPARE(SpectatorPage::sheetsOnPage({combinableSheet(9), combinableSheet(15)}, 25), (QList<int>{0}));
    QCOMPARE(SpectatorPage::sheetsOnPage({combinableSheet(9), combinableSheet(14)}, 25), (QList<int>{0, 1}));
    // The next sheet is not skipped for a smaller one after it, to keep the order of the sheets
    QCOMPARE(SpectatorPage::sheetsOnPage({combinableSheet(9), combinableSheet(15), combinableSheet(1)}, 25), (QList<int>{0}));
}

void SheetTest::test_sheetsOnPageRequiresSameLayout()
{
    QCOMPARE(SpectatorPage::sheetsOnPage({combinableSheet(2, 4), combinableSheet(2, 6)}, 25), (QList<int>{0}));
    QCOMPARE(SpectatorPage::sheetsOnPage({combinableSheet(2, 4, 0), combinableSheet(2, 4, 1)}, 25), (QList<int>{0}));
    QCOMPARE(SpectatorPage::sheetsOnPage({combinableSheet(2, 4, 1), combinableSheet(2, 4, 1)}, 25), (QList<int>{0, 1}));
}

void SheetTest::test_sheetsOnPageStopsAtUnsuitableSheet()
{
    SpectatorPage::SheetInfo teamSheet;    // Not combinable
    SpectatorPage::SheetInfo hiddenSheet = combinableSheet(2);
    hiddenSheet.shown = false;

    QCOMPARE(SpectatorPage::sheetsOnPage({combinableSheet(2), teamSheet, combinableSheet(2)}, 25), (QList<int>{0}));
    QCOMPARE(SpectatorPage::sheetsOnPage({combinableSheet(2), hiddenSheet, combinableSheet(2)}, 25), (QList<int>{0, 2}));
}

void SheetTest::test_sheetsOnPageFirstSheetTooBig()
{
    // 25 competitors take the whole page
    QCOMPARE(SpectatorPage::sheetsOnPage({combinableSheet(25), combinableSheet(0)}, 25), (QList<int>{0}));
    QCOMPARE(SpectatorPage::sheetsOnPage({combinableSheet(30), combinableSheet(1)}, 25), (QList<int>{0}));

    SpectatorPage::SheetInfo teamSheet;
    QCOMPARE(SpectatorPage::sheetsOnPage({teamSheet, combinableSheet(1)}, 25), (QList<int>{0}));
}

void SheetTest::test_combinedPageRows()
{
    SheetFactory factory;
    auto first = factory.sheet("AR Men", 3);
    auto second = factory.sheet("AR Women", 3);

    setCompetitor(first->laskurid[0], "Slow", {"90", "90", "90", "90"});
    setCompetitor(first->laskurid[1], "Fast", {"99", "98", "99", "98"});
    setCompetitor(first->laskurid[2], "Middle", {"95", "95", "95", "95"});
    setCompetitor(second->laskurid[0], "Lower", {"92", "92", "92", "92"});
    setCompetitor(second->laskurid[1], "Higher", {"97", "97", "97", "97"});
    // The third competitor has no results, so is not shown

    const auto page = SpectatorPage::combinedPage({first.get(), second.get()}, 25);

    QCOMPARE(page.sheetIndexes, (QList<int>{0, 1}));
    QCOMPARE(page.consumedSheets, 2);
    QCOMPARE(page.competitorRows, 5);
    QCOMPARE(page.maxTimeMs, first->maxTime());
    QCOMPARE(page.rows.count(), 25);
    QCOMPARE(page.title, QString("AR Men"));
    QCOMPARE(page.headingRows, (QSet<int>{4}));

    QCOMPARE(page.rows[0][0], QString("1."));
    QCOMPARE(page.rows[0][2], QString("Fast"));
    QCOMPARE(page.rows[0][7], QString("99"));
    QCOMPARE(page.rows[0][14], first->laskurid[1]->getSumma());
    QCOMPARE(page.rows[1][2], QString("Middle"));
    QCOMPARE(page.rows[2][0], QString("3."));
    QCOMPARE(page.rows[2][2], QString("Slow"));

    QStringList blankRow;
    blankRow.fill(" ", SpectatorPage::ColumnCount);
    QCOMPARE(page.rows[3], blankRow);

    QCOMPARE(page.rows[4][1], QString("AR Women"));
    QCOMPARE(page.rows[5][0], QString("1."));
    QCOMPARE(page.rows[5][2], QString("Higher"));
    QCOMPARE(page.rows[6][0], QString("2."));
    QCOMPARE(page.rows[6][2], QString("Lower"));
    QCOMPARE(page.rows[7], blankRow);
    QCOMPARE(page.rows[24], blankRow);
}

void SheetTest::test_combinedPageSkipsHiddenSheets()
{
    SheetFactory factory;
    auto first = factory.sheet("First", 2);
    auto hidden = factory.sheet("Hidden", 2);
    auto third = factory.sheet("Third", 2);
    hidden->naidata = false;

    const auto page = SpectatorPage::combinedPage({first.get(), hidden.get(), third.get()}, 25);

    QCOMPARE(page.sheetIndexes, (QList<int>{0, 2}));
    QCOMPARE(page.consumedSheets, 3);
    QCOMPARE(page.title, QString("First"));
    QCOMPARE(page.rows[2][1], QString(" "));
    QCOMPARE(page.rows[3][1], QString("Third"));
}

void SheetTest::test_combinedPageFallsBackToSingleSheet()
{
    SheetFactory factory;
    auto first = factory.sheet("Small", 3);
    auto big = factory.sheet("Big", 30);
    auto last = factory.sheet("Last", 1);

    QCOMPARE(SpectatorPage::combinedPage({first.get(), big.get(), last.get()}, 25).sheetIndexes, (QList<int>{0}));
    QCOMPARE(SpectatorPage::combinedPage({big.get(), first.get()}, 25).sheetIndexes, (QList<int>{0}));
    QCOMPARE(SpectatorPage::combinedPage({first.get()}, 25).sheetIndexes, (QList<int>{0}));
    QVERIFY(SpectatorPage::combinedPage({big.get(), first.get()}, 25).rows.isEmpty());
}

QTEST_MAIN(SheetTest)

#include "tst_sheet.moc"

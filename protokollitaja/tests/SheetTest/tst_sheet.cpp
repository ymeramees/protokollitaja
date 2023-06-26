#include <QtTest>

#include "leht.h"
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

};

SheetTest::SheetTest()
{

}

SheetTest::~SheetTest()
{

}

void SheetTest::test_exportStartList()
{
    Andmebaas dataBase;
    LiikmeteValikKast membersBox;
    QString eventType = "40l õhupüss";
    bool writeAssistant = false;
    bool withDecimals = true;
    int sorting = 1;

    Leht sheet = Leht(&dataBase, 4, 0, 1, &writeAssistant, "40 AR", 3, eventType, withDecimals, &sorting, nullptr, false, &membersBox, 1, 10);

    sheet.uusLaskur(13);

    // Laskur competitor(nullptr, 4, 0, 0, &writeAssistant, &withDecimals, 35, &sorting, &eventType, 10, nullptr);
    // Lask shot("_SHOT;9;10;36;60;74;10:43:56.17;3;31;7;94;0;0;49;-0.00187626;0.00347202;900;0;0;655.35;98903519;61;450;0");

    QCOMPARE(sheet.laskurid.count(), 1);
    QCOMPARE(sheet.laskurid[0]->id, 13);

    sheet.laskurid[0]->linnuke->setChecked(true);
    sheet.laskurid[0]->eesNimi->setText("Isås");
        sheet.laskurid[0]->perekNimi->setText("STRÖM");
        sheet.laskurid[0]->klubi->setText("Ülenurme GSK");

    QString expectedData = ";13;Isås;STRÖM;Ülenurme GSK;40l õhupüss;1;40";
    auto exportData = sheet.exportStartList();
    QVERIFY(exportData.has_value());
    QCOMPARE(exportData.value(), expectedData);
    // competitor.lasud[3][9]->set(&shot);
    // QVERIFY(competitor.isFinished() == true);
}

QTEST_MAIN(SheetTest)

#include "tst_sheet.moc"

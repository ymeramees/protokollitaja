#include <QtTest>
#include <QCoreApplication>
#include <QPushButton>
#include <QTableWidget>

#include "finalsfileexport.h"

class finalsfileexport : public QObject
{
    Q_OBJECT

public:
    finalsfileexport();
    ~finalsfileexport();

private slots:
    void test_drawStartPositionsWith5();
    void test_drawStartPositionsWith8();

};

finalsfileexport::finalsfileexport()
{

}

finalsfileexport::~finalsfileexport()
{

}

void finalsfileexport::test_drawStartPositionsWith5()
{
    QVector<QStringList> finalsTable; //Each "row": target, ID, screen name, result, first name, name, club
    finalsTable << (QStringList() << "3" << "400233" << "PAKIRAAM P." << "620,9" << "Paavel" << "PAKIRAAM" << "Champions Club");
    finalsTable << (QStringList() << "" << "400235" << "KUUSEPUU A." << "618,4" << "Aita Leida" << "KUUSEPUU" << "Masters Club");
    finalsTable << (QStringList() << "10" << "400218" << "PEDAAL P." << "617,3" << "Peeter" << "PEDAAL" << "SK Võit");
    finalsTable << (QStringList() << "13" << "40047" << "MÕTTETU M." << "617,2" << "Mees" << "MÕTTETU" << "Aa LK");
    finalsTable << (QStringList() << "" << "40025" << "KREE T." << "615,8" << "Tiit" << "KREE" << "SK Võit");

    FinalsFileExport finalsFileExport(finalsTable, "fileLocation", "Test Competition", "Test Event", "Today, Here and there", 0);
    finalsFileExport.setRelay(11);

    QTableWidget* competitorsTable = finalsFileExport.findChild<QTableWidget*>("finalsCompetitorsTable");
    QVERIFY(competitorsTable->rowCount() == 5);

    QPushButton* drawButton = finalsFileExport.findChild<QPushButton*>("drawButton");
    QTest::mouseClick(drawButton, Qt::LeftButton);
}

void finalsfileexport::test_drawStartPositionsWith8()
{
    QVector<QStringList> finalsTable; //Each "row": target, ID, screen name, result, first name, name, club
    finalsTable << (QStringList() << "3" << "400233" << "PAKIRAAM P." << "620,9" << "Paavel" << "PAKIRAAM" << "Champions Club");
    finalsTable << (QStringList() << "" << "400235" << "KUUSEPUU A." << "618,4" << "Aita Leida" << "KUUSEPUU" << "Masters Club");
    finalsTable << (QStringList() << "10" << "400218" << "PEDAAL P." << "617,3" << "Peeter" << "PEDAAL" << "SK Võit");
    finalsTable << (QStringList() << "13" << "40047" << "MÕTTETU M." << "617,2" << "Mees" << "MÕTTETU" << "Aa LK");
    finalsTable << (QStringList() << "" << "40025" << "KREE T." << "615,8" << "Tiit" << "KREE" << "SK Võit");
    finalsTable << (QStringList() << "10" << "400219" << "PEET T." << "615,3" << "Tee" << "PEET" << "SK Jõud");
    finalsTable << (QStringList() << "24" << "400147" << "REKSIA A." << "613,2" << "Anu" << "REKSIA" << "Uugametsa LK");
    finalsTable << (QStringList() << "" << "400225" << "JAAVA V." << "612,8" << "Vassiili" << "JAAVA" << "LK Kümme");

    FinalsFileExport finalsFileExport(finalsTable, "fileLocation", "Test Competition", "Test Event", "Today, Here and there", 0);
    finalsFileExport.setRelay(12);

    QTableWidget* competitorsTable = finalsFileExport.findChild<QTableWidget*>("finalsCompetitorsTable");
    QVERIFY(competitorsTable->rowCount() == 8);

    QPushButton* drawButton = finalsFileExport.findChild<QPushButton*>("drawButton");
    QTest::mouseClick(drawButton, Qt::LeftButton);
}

QTEST_MAIN(finalsfileexport)

#include "tst_finalsfileexport.moc"

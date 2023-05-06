#include <QtTest>

#include "simplekllfilerw.h"
#include "competitionsettings.h"

class KllFileRWTest : public QObject
{
    Q_OBJECT

public:
    KllFileRWTest();
    ~KllFileRWTest();

private slots:
    void test_writeAndReadInitialKllFile();

};

KllFileRWTest::KllFileRWTest()
{

}

KllFileRWTest::~KllFileRWTest()
{

}

void KllFileRWTest::test_writeAndReadInitialKllFile()
{
    QWidget *parentPlaceholder = new QWidget;
    CompetitionSettings data;
    data.competitionName = "Test Competition";
    data.startDate = QDate::currentDate().addDays(1);
    data.endDate = QDate::currentDate().addDays(3);
    data.place = "Somewhere nice";
    data.fileName = "Test Competition.kll";

    SimpleKllFileRW::writeInitialKll(data.fileName, data, parentPlaceholder);

    CompetitionSettings dataToCompare = SimpleKllFileRW::readCompetitionSettingsFromKll(data.fileName, parentPlaceholder);

    QCOMPARE(dataToCompare.competitionName, data.competitionName);
    QCOMPARE(dataToCompare.startDate, data.startDate);
//    QCOMPARE(dataToCompare.endDate, data.endDate);
    QCOMPARE(dataToCompare.place, data.place);
    QCOMPARE(dataToCompare.fileName, data.fileName);
}

QTEST_MAIN(KllFileRWTest)

#include "tst_kllfilerwtest.moc"

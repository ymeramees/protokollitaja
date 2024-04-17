#include <QtTest>
#include <QCoreApplication>

#include <QTextStream>
#include <QComboBox>
#include "lehelugeja.h"
#include "version.h"

QString programmiNimi = VER_INTERNALNAME_STR; //"Protolehelugeja 0.3 Beta"; // TODO uuendada nime
QString versioon = VER_PRODUCTVERSION_STR; //"0.3.0 Beta";
QString aasta = VER_AASTA;
QString argument;
bool verbose = false;
bool veryVerbose = false;

class IntegrationTest : public QObject
{
    Q_OBJECT

public:
    IntegrationTest();
    ~IntegrationTest();

private slots:
    void test_outputMessage();

};

IntegrationTest::IntegrationTest()
{

}

IntegrationTest::~IntegrationTest()
{

}

void IntegrationTest::test_outputMessage()
{
    Lehelugeja lehelugeja(true);

    QLineEdit* seriesBox = lehelugeja.findChild<QLineEdit*>("seeria1Edit");
    QApplication::setActiveWindow(seriesBox);

    QTest::mouseClick(seriesBox, Qt::LeftButton);
    for(int i = 0; i < 10; i++)
        lehelugeja.readShot(
                    Lask("_SHOT;9;10;36;60;73;10:43:18.64;3;31;7;96;0;0;48;-0.00197546;-0.00262108;900;0;0;655.35;98899783;61;450;0")
                    );

    qDebug() << "Series result: " << seriesBox->text();
    QVERIFY(seriesBox->text().compare("90") == 0);
}

QTEST_MAIN(IntegrationTest)

#include "tst_integrationtest.moc"

#include <QtTest>
#include <QCoreApplication>

#include "protofinaal.h"
#include "siusshotdata.h"

bool verbose = true;

class ProtofinaalTest : public QObject
{
    Q_OBJECT

public:
    ProtofinaalTest();
    ~ProtofinaalTest();

private slots:
    void sanityCheck();

};

ProtofinaalTest::ProtofinaalTest()
{
    QFile confFile("conf.json");

    if(confFile.open(QIODevice::WriteOnly)){
        QJsonObject jsonObj;

        jsonObj["lastFile"] = "test.fin";
        jsonObj["windowXLocation"] = 0;
        jsonObj["windowYLocation"] = 0;
        QJsonDocument jsonDoc(jsonObj);
        confFile.write(jsonDoc.toJson());
        confFile.close();
    }

//    QFile file(":/templates/spectatorView_template.html");
//    if(file.open(QIODevice::ReadOnly)) {
//        QByteArray dump = file.readAll();
//        qDebug() << "contents: " << dump;
//    } else {
//        qDebug() << "error: " << file.error();
//    }
    QFile::copy(":/templates/spectatorView_template.html", "spectatorView_template.html");
    QFile::copy(":/templates/60ohupuss2022.json", "60ohupuss2022.json");
}

ProtofinaalTest::~ProtofinaalTest()
{

}

void ProtofinaalTest::sanityCheck()
{

//    InitialDialog *initialDialog = finaal.findChild<InitialDialog*>();

//    QTimer::singleShot(150, [=](){
//        QWidgetList allToplevelWidgets = QApplication::topLevelWidgets();
//        QCOMPARE(allToplevelWidgets.length(), 4);

//        QWindow* initialDialog = QApplication::modalWindow();
//        QCOMPARE(initialDialog->title(), "Dialog");
////            initialDialog->setCompetitionName("Test Competition");
////            initialDialog->setTimePlace("today here");
////            initialDialog->accept();
//        QPushButton *forwardButton = initialDialog->findChild<QPushButton*>("forwardButton");
//        QVERIFY(initialDialog->inherits("QDialog"));
////        QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
////        QCoreApplication::postEvent(initialDialog, event);
//    });

    Protofinaal finaal; //("Test Competition", "today here");
    QTest::qWait(200);

//    SiusShotData shot1(11, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0"));

//    QTest::mouseClick(forwardButton, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10), 100);


//    qApp->processEvents();

    TeamsTable2022 *teamsTable = finaal.findChild<TeamsTable2022*>();
    QVERIFY(teamsTable);
    QList<Team2022*> teams = teamsTable->findChildren<Team2022*>();
    QVERIFY(teams.size() > 0);
    QList<Competitor2022*> competitors = teams.first()->findChildren<Competitor2022*>();
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

    QVector<SiusShotData> shot1;
    shot1.append(SiusShotData(11, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot1.append(SiusShotData(21, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;107;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot1.append(SiusShotData(31, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;107;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot1.append(SiusShotData(41, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;98;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));

    foreach(SiusShotData shotData, shot1) {
        teamsTable->readSiusInfo(shotData);
    }

    QVector<TeamsTable2022::Result> expected1;
    expected1.append(TeamsTable2022::Result { "Competitor0", "10,1", "2", "2" });
    expected1.append(TeamsTable2022::Result { "Teine", "10,7", "3,5", "3,5" });
    expected1.append(TeamsTable2022::Result { "Kolmas", "10,7", "3,5", "3,5" });
    expected1.append(TeamsTable2022::Result { "Neljas", "9,8", "1", "1" });

    auto actual1 = teamsTable->getCurrentResults();
    for(int i = 0; i < 4; i++) {
        QCOMPARE(actual1.value(i + 1).name, expected1.at(i).name);
        QCOMPARE(actual1.value(i + 1).shotValue, expected1.at(i).shotValue);
        QCOMPARE(actual1.value(i + 1).points, expected1.at(i).points);
        QCOMPARE(actual1.value(i + 1).totalPoints, expected1.at(i).totalPoints);
    }

    QTest::qWait(1000);
//    finaal.updateSpectatorWindow();
//    finaal.readSiusInfo(shot1);
}

QTEST_MAIN(ProtofinaalTest)

#include "tst_protofinaaltest.moc"

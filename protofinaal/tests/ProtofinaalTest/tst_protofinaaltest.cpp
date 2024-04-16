#include <QtTest>
#include <QCoreApplication>

#include "protofinaal.h"
#include "siusshotdata.h"

bool verbose = false;

class ProtofinaalTest : public QObject
{
    Q_OBJECT

public:
    ProtofinaalTest();
    ~ProtofinaalTest();

private slots:
    void sanityCheckWithPoints();
    void sanityCheckWithShots();
    void test_readProtokollitajaGeneratedFile();
    void test_readSiusShotWithOffsetWithPoints();
    void test_readSiusShotWithOffsetWithShots();
    void closeInitialDialog();
    void closeMessageBox();
    void setupCompetitionFile(QString);

private:
    CommonSettings m_originalSettings;
};

ProtofinaalTest::ProtofinaalTest() : m_originalSettings("Protofinaal", "Protofinaali conf")
{
//    QFile file(":/templates/spectatorView_template.html");
//    if(file.open(QIODevice::ReadOnly)) {
//        QByteArray dump = file.readAll();
//        qDebug() << "contents: " << dump;
//    } else {
//        qDebug() << "error: " << file.error();
//    }
    QFile::copy(":/templates/spectatorView_template.html", "spectatorView_template.html");
}

ProtofinaalTest::~ProtofinaalTest()
{
    m_originalSettings.writeSettings();
}

void ProtofinaalTest::closeInitialDialog()
{
    QWidgetList allToplevelWidgets = QApplication::topLevelWidgets();

    foreach (QWidget *w, allToplevelWidgets) {
        QPushButton *forwardButton = w->findChild<QPushButton*>("forwardButton");
        if (forwardButton) {
            QVERIFY(forwardButton);
            QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
            QCoreApplication::postEvent(forwardButton, event);
            QTimer::singleShot(50, this, SLOT(closeMessageBox()));
        }
    }
}

void ProtofinaalTest::closeMessageBox()
{
    QWidgetList allToplevelWidgets = QApplication::topLevelWidgets();

    QWidget* widget = QApplication::activeModalWidget();
    if (widget){
        QVERIFY(widget->inherits("QMessageBox"));
        QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
        QCoreApplication::postEvent(widget, event);
    }
}

void ProtofinaalTest::setupCompetitionFile(QString templateName)
{
    QFile::remove("test.fin");
    QFile::copy(QString(":/%1.fin").arg(templateName), "test.fin");
}

void ProtofinaalTest::test_readProtokollitajaGeneratedFile()
{
    setupCompetitionFile("testFromProtokollitaja");

    QFile openTest("readProtokollitajaGeneratedFile.fin");
    if (!openTest.open(QIODevice::ReadOnly))
        qDebug() << "Unable to open test competition file: " << openTest.error();

    QTimer::singleShot(210, this, SLOT(closeInitialDialog()));

    Protofinaal finaal("test.fin");
    QTest::qWait(200);

    TeamsTable *teamsTable = finaal.findChild<TeamsTable*>();
    QVERIFY(teamsTable);
    QList<Team*> teams = teamsTable->findChildren<Team*>();
    QCOMPARE(teams.size(), 8);
    QList<Competitor*> firstTeamCompetitors = teams.first()->findChildren<Competitor*>();
    QCOMPARE(firstTeamCompetitors.size(), 1);
    QList<ShotEdit*> shotEdits = firstTeamCompetitors.first()->findChildren<ShotEdit*>();
    QCOMPARE(shotEdits.size(), 24);

    QCOMPARE(teams.first()->teamName(), "HÕUM S.");
    QCOMPARE(teams.last()->teamName(), "");
    QCOMPARE(firstTeamCompetitors.first()->id(), 1601);
    QCOMPARE(firstTeamCompetitors.first()->name(), "HÕUM S.");
    QCOMPARE(firstTeamCompetitors.first()->qualificationResult(), "391,8");

    QList<Competitor*> fifthTeamCompetitors = teams.at(5)->findChildren<Competitor*>();
    QCOMPARE(fifthTeamCompetitors.size(), 1);
    QCOMPARE(fifthTeamCompetitors.first()->id(), 7701);
    QCOMPARE(fifthTeamCompetitors.first()->name(), "STRÖM E.");
    QCOMPARE(fifthTeamCompetitors.first()->qualificationResult(), "409,3");

    QFile::remove("readProtokollitajaGeneratedFile.fin");
}

void ProtofinaalTest::test_readSiusShotWithOffsetWithPoints()
{
    setupCompetitionFile("testWithPoints");

    QTimer::singleShot(210, this, SLOT(closeInitialDialog()));

    Protofinaal finaal("test.fin");
    QTest::qWait(200);

    TeamsTable *teamsTable = finaal.findChild<TeamsTable*>();
    QVERIFY(teamsTable);
    QList<Team*> teams = teamsTable->findChildren<Team*>();
    QVERIFY(teams.size() > 0);
    QCOMPARE(teamsTable->lastValidShotIndex(), -1);

    foreach(Team *team, teams) {
        Competitor *competitor = team->findChild<Competitor*>();
        QSpinBox *offsetBox = competitor->findChild<QSpinBox*>();
        offsetBox->setValue(-15);
    }

    QVector<SiusShotData> shot1;
    shot1.append(SiusShotData(11, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot1.append(SiusShotData(21, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;107;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot1.append(SiusShotData(31, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;9;97;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot1.append(SiusShotData(41, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;98;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));

    foreach(SiusShotData shotData, shot1) {
        teamsTable->readSiusInfo(shotData);
    }
    QCOMPARE(teamsTable->lastValidShotIndex(), -1);

    QVector<SiusShotData> shot2;
    shot2.append(SiusShotData(11, 0, 16, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot2.append(SiusShotData(21, 0, 16, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;107;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot2.append(SiusShotData(31, 0, 16, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;9;97;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot2.append(SiusShotData(41, 0, 16, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;98;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));

    foreach(SiusShotData shotData, shot2) {
        teamsTable->readSiusInfo(shotData);
    }

    QCOMPARE(teamsTable->lastValidShotIndex(), 0);
    QVector<TeamsTable::Result> expected1;
    expected1.append(TeamsTable::Result { "Esimene", "10,1", "3", "3", 101 });
    expected1.append(TeamsTable::Result { "Teine", "10,7", "4", "4", 107 });
    expected1.append(TeamsTable::Result { "Kolmas", "9,7", "1", "1", 97 });
    expected1.append(TeamsTable::Result { "Neljas", "9,8", "2", "2", 98 });

    auto actual1 = teamsTable->getCurrentResults();
    for(int i = 0; i < 4; i++) {
        QCOMPARE(actual1.value(i + 1).name, expected1.at(i).name);
        QCOMPARE(actual1.value(i + 1).shotValue, expected1.at(i).shotValue);
        QCOMPARE(actual1.value(i + 1).seriesOrPoints, expected1.at(i).seriesOrPoints);
        QCOMPARE(actual1.value(i + 1).totalScore, expected1.at(i).totalScore);
        QCOMPARE(actual1.value(i + 1).total10Score, expected1.at(i).total10Score);
    }

    QTest::qWait(500);
}

void ProtofinaalTest::test_readSiusShotWithOffsetWithShots()
{
    setupCompetitionFile("testWithShots");

    QTimer::singleShot(210, this, SLOT(closeInitialDialog()));

    Protofinaal finaal("test.fin");
    QTest::qWait(200);

    TeamsTable *teamsTable = finaal.findChild<TeamsTable*>();
    QVERIFY(teamsTable);
    QList<Team*> teams = teamsTable->findChildren<Team*>();
    QVERIFY(teams.size() > 0);
    QCOMPARE(teamsTable->lastValidShotIndex(), -1);

    foreach(Team *team, teams) {
        Competitor *competitor = team->findChild<Competitor*>();
        QSpinBox *offsetBox = competitor->findChild<QSpinBox*>();
        offsetBox->setValue(-15);
    }

    QVector<SiusShotData> shot1;
    shot1.append(SiusShotData(11, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot1.append(SiusShotData(21, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;107;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot1.append(SiusShotData(31, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;9;97;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot1.append(SiusShotData(41, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;98;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));

    foreach(SiusShotData shotData, shot1) {
        teamsTable->readSiusInfo(shotData);
    }
    QCOMPARE(teamsTable->lastValidShotIndex(), -1);

    QVector<SiusShotData> shot2;
    shot2.append(SiusShotData(11, 0, 16, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot2.append(SiusShotData(21, 0, 16, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;107;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot2.append(SiusShotData(31, 0, 16, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;9;97;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));
    shot2.append(SiusShotData(41, 0, 16, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;98;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0")));

    foreach(SiusShotData shotData, shot2) {
        teamsTable->readSiusInfo(shotData);
    }

    QCOMPARE(teamsTable->lastValidShotIndex(), 0);
    QVector<TeamsTable::Result> expected1;
    expected1.append(TeamsTable::Result { "Esimene", "10,1", "0", "10,1" });
    expected1.append(TeamsTable::Result { "Teine", "10,7", "0", "10,7" });
    expected1.append(TeamsTable::Result { "Kolmas", "9,7", "0", "9,7" });
    expected1.append(TeamsTable::Result { "Neljas", "9,8", "0", "9,8" });

    auto actual1 = teamsTable->getCurrentResults();
    for(int i = 0; i < 4; i++) {
        QCOMPARE(actual1.value(i + 1).name, expected1.at(i).name);
        QCOMPARE(actual1.value(i + 1).shotValue, expected1.at(i).shotValue);
//        QCOMPARE(actual1.value(i + 1).points, expected1.at(i).points);
        QCOMPARE(actual1.value(i + 1).totalScore, expected1.at(i).totalScore);
    }

    QTest::qWait(500);
}

void ProtofinaalTest::sanityCheckWithPoints()
{
    setupCompetitionFile("testWithPoints");

    QTimer::singleShot(210, this, SLOT(closeInitialDialog()));
//    QTimer::singleShot(250, this, SLOT(closeMessageBox()));

    Protofinaal finaal("test.fin");
    QTest::qWait(200);

//    SiusShotData shot1(11, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0"));

//    QTest::mouseClick(forwardButton, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10), 100);


//    qApp->processEvents();

    TeamsTable *teamsTable = finaal.findChild<TeamsTable*>();
    QVERIFY(teamsTable);
    QList<Team*> teams = teamsTable->findChildren<Team*>();
    QVERIFY(teams.size() > 0);
    QList<Competitor*> competitors = teams.first()->findChildren<Competitor*>();
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

    QVector<TeamsTable::Result> expected1;
    expected1.append(TeamsTable::Result { "Competitor0", "10,1", "2", "2", 101 });
    expected1.append(TeamsTable::Result { "Teine", "10,7", "3,5", "3,5", 107 });
    expected1.append(TeamsTable::Result { "Kolmas", "10,7", "3,5", "3,5", 107 });
    expected1.append(TeamsTable::Result { "Neljas", "9,8", "1", "1", 98 });

    auto actual1 = teamsTable->getCurrentResults();
    for(int i = 0; i < 4; i++) {
        QCOMPARE(actual1.value(i + 1).name, expected1.at(i).name);
        QCOMPARE(actual1.value(i + 1).shotValue, expected1.at(i).shotValue);
        QCOMPARE(actual1.value(i + 1).seriesOrPoints, expected1.at(i).seriesOrPoints);
        QCOMPARE(actual1.value(i + 1).totalScore, expected1.at(i).totalScore);
        QCOMPARE(actual1.value(i + 1).total10Score, expected1.at(i).total10Score);
    }

    QTest::qWait(500);
//    finaal.updateSpectatorWindow();
//    finaal.readSiusInfo(shot1);
}

void ProtofinaalTest::sanityCheckWithShots()
{
    setupCompetitionFile("testWithShots");

    QTimer::singleShot(210, this, SLOT(closeInitialDialog()));

    Protofinaal finaal("test.fin");
    QTest::qWait(200);

    TeamsTable *teamsTable = finaal.findChild<TeamsTable*>();
    QVERIFY(teamsTable);
    QList<Team*> teams = teamsTable->findChildren<Team*>();
    QVERIFY(teams.size() > 0);
    QList<Competitor*> competitors = teams.first()->findChildren<Competitor*>();
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

    QVector<TeamsTable::Result> expected1;
    expected1.append(TeamsTable::Result { "Competitor0", "10,1", "0", "10,1" });
    expected1.append(TeamsTable::Result { "Teine", "10,7", "0", "10,7" });
    expected1.append(TeamsTable::Result { "Kolmas", "10,7", "0", "10,7" });
    expected1.append(TeamsTable::Result { "Neljas", "9,8", "0", "9,8" });

    auto actual1 = teamsTable->getCurrentResults();
    for(int i = 0; i < 4; i++) {
        QCOMPARE(actual1.value(i + 1).name, expected1.at(i).name);
        QCOMPARE(actual1.value(i + 1).shotValue, expected1.at(i).shotValue);
//        QCOMPARE(actual1.value(i + 1).points, expected1.at(i).points);
        QCOMPARE(actual1.value(i + 1).totalScore, expected1.at(i).totalScore);
    }

    QTest::qWait(500);
}

QTEST_MAIN(ProtofinaalTest)

#include "tst_protofinaaltest.moc"

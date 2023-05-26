#include <QtTest>

#include "startlistwriter.h"

class StartListWriterTest : public QObject
{
    Q_OBJECT

public:
    StartListWriterTest();
    ~StartListWriterTest();

private slots:
    void test_writeInbandStartListInCorrectFormat();
    void test_writeSiusStartListInCorrectFormat();

};

StartListWriterTest::StartListWriterTest()
{

}

StartListWriterTest::~StartListWriterTest()
{

}

void StartListWriterTest::test_writeInbandStartListInCorrectFormat()
{
    QVector<StartListWriter::StartListCompetitor> competitorsList;
    // Each "row": target, ID, first name, name, club, result
    competitorsList.append(
                StartListWriter::StartListCompetitor{"1", "101", "Donald", "Duck", "Duckburg Shooting Club", "0", 1, ""}
                );
    competitorsList.append(
                StartListWriter::StartListCompetitor{"2", "102", "Scrooge","McDuck", "Duckburg Shooting Club", "0", 1, "M"}
                );
    competitorsList.append(
                StartListWriter::StartListCompetitor{"3", "103", "Huey","Duck", "Duckburg Shooting Club", "0", 3, "JM"}
                );
    competitorsList.append(
                StartListWriter::StartListCompetitor{"4", "104", "Dewey","Duck", "Duckburg Shooting Club", "0", 3, "JM"}
                );
    competitorsList.append(
                StartListWriter::StartListCompetitor{"5", "105", "Louie","Duck", "Duckburg Shooting Club", "0", 3, "JM"}
                );

    QString expectedOutput = QString::fromUtf8("Erä,Rata,Etunimi,Sukunimi,Seura,Laji,Sarja\n"
            "0,1,Donald,Duck,Duckburg Shooting Club,Ilmakivääri,\n"
            "0,2,Scrooge,McDuck,Duckburg Shooting Club,Ilmakivääri,M\n"
            "0,3,Huey,Duck,Duckburg Shooting Club,Ilmapistooli,JM\n"
            "0,4,Dewey,Duck,Duckburg Shooting Club,Ilmapistooli,JM\n"
            "0,5,Louie,Duck,Duckburg Shooting Club,Ilmapistooli,JM\n");

    QTimer::singleShot(200, [=](){
        QWidgetList allToplevelWidgets = QApplication::topLevelWidgets();
        QCOMPARE(allToplevelWidgets.length(), 1);

        QWidget* widget = QApplication::activeModalWidget();
        if (widget){
            QVERIFY(widget->inherits("QMessageBox"));
            QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
            QCoreApplication::postEvent(widget, event);
        }
    });

    QString filename = "./test.csv";
    StartListWriter *writer = new StartListWriter(competitorsList, filename, StartListWriter::INBAND, this);
    writer->deleteLater();

    QFile file(filename);
    bool fileCorrect = true;
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
//        in.setCodec("UTF-8"); // TODO Not needed in Qt 6.5
        QString contents = in.readAll();
        QCOMPARE(contents, expectedOutput);
    } else {
        qDebug() << "Unable to open file: " << filename;
        fileCorrect = false;
    }
    QVERIFY(fileCorrect);
}

void StartListWriterTest::test_writeSiusStartListInCorrectFormat()
{
    QVector<StartListWriter::StartListCompetitor> competitorsList;
    // Each "row": target, ID, first name, name, club, result
    competitorsList.append(
                StartListWriter::StartListCompetitor{"1", "101", "Donald", "Duck", "Duckburg Shooting Club", "0", 1, ""}
                );
    competitorsList.append(
                StartListWriter::StartListCompetitor{"2", "102", "Scrooge","McDuck", "Duckburg Shooting Club", "0", 1, "M"}
                );
    competitorsList.append(
                StartListWriter::StartListCompetitor{"3", "103", "Huey","Duck", "Duckburg Shooting Club", "0", 3, "JM"}
                );

    QString expectedOutput = ";101;\"Duck\";\"Donald\";\"Duck D.\";;0;0;\"Duckburg Shooting Club\";;1;0;;0;0;0;0;\n"
            ";102;\"McDuck\";\"Scrooge\";\"McDuck S.\";;0;0;\"Duckburg Shooting Club\";;2;0;;0;0;0;0;\n"
            ";103;\"Duck\";\"Huey\";\"Duck H.\";;0;0;\"Duckburg Shooting Club\";;3;0;;0;0;0;0;\n";

    QTimer::singleShot(200, [=](){
        QWidgetList allToplevelWidgets = QApplication::topLevelWidgets();
        QCOMPARE(allToplevelWidgets.length(), 1);

        QWidget* widget = QApplication::activeModalWidget();
        if (widget){
            QVERIFY(widget->inherits("QMessageBox"));
            QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
            QCoreApplication::postEvent(widget, event);
        }
    });

    QString filename = "./test.csv";
    StartListWriter *writer = new StartListWriter(competitorsList, filename, StartListWriter::SIUS, this);
    writer->deleteLater();

    QFile file(filename);
    bool fileCorrect = true;
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString contents = in.readAll();
        QCOMPARE(contents, expectedOutput);
    } else {
        qDebug() << "Unable to open file: " << filename;
        fileCorrect = false;
    }
    QVERIFY(fileCorrect);
}

QTEST_MAIN(StartListWriterTest)

#include "tst_startlistwritertest.moc"

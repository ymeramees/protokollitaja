#include <QtTest>
#include <QDate>

#include "initialdialog.h"

class InitialDialogTest : public QObject
{
    Q_OBJECT

public:
    InitialDialogTest();
    ~InitialDialogTest();

private slots:
    void acceptCorrectlyFilledDialog();
    void notAcceptEmptyCompetitionName();
    void notAcceptEmptyFileName();
    void notAcceptEmptyCompetitionNorFileName();
    void notAcceptStartDateAfterEndDate();

};

InitialDialogTest::InitialDialogTest()
{

}

InitialDialogTest::~InitialDialogTest()
{

}

void InitialDialogTest::acceptCorrectlyFilledDialog()
{
    InitialDialog dialog;
    QPushButton *forwardButton = dialog.findChild<QPushButton*>("forwardButton");

    CompetitionSettings settings;
    settings.competitionName = "Test competition";
    settings.place = "Test place";
    settings.fileName = "Test.kll";
    settings.startDate = QDate(2021, 4, 19);
    settings.endDate = QDate(2021, 4, 20);
    dialog.setData(settings);

    QTimer::singleShot(100, [=](){
        QWidgetList allToplevelWidgets = QApplication::topLevelWidgets();
        QCOMPARE(allToplevelWidgets.length(), 2);

        QWidget* widget = QApplication::activeModalWidget();
        if (widget){
            QVERIFY(widget->inherits("QDialog"));
            QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
            QCoreApplication::postEvent(widget, event);
        }
    });

    QTimer::singleShot(250, [=](){
        QWidgetList allToplevelWidgets = QApplication::topLevelWidgets();
        QCOMPARE(allToplevelWidgets.length(), 2);

        QWidget* widget = QApplication::activeModalWidget();
        if (widget){
            QVERIFY(widget->inherits("QMessageBox"));
            QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
            QCoreApplication::postEvent(widget, event);
        }
    });

    QTest::mouseClick(forwardButton, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10), 100);

    QCOMPARE(dialog.result(), 1); // The dialog should be accepted
}

void InitialDialogTest::notAcceptEmptyCompetitionName()
{
    InitialDialog dialog;

    QLineEdit* fileNameBox = dialog.findChild<QLineEdit*>("fileNameEdit");
    fileNameBox->setText("Test.kll");

    QPushButton *forwardButton = dialog.findChild<QPushButton*>("forwardButton");

    QTimer::singleShot(200, [=](){
        QWidgetList allToplevelWidgets = QApplication::topLevelWidgets();
        QCOMPARE(allToplevelWidgets.length(), 2);

        QWidget* widget = QApplication::activeModalWidget();
        if (widget){
            QVERIFY(widget->inherits("QMessageBox"));
            QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
            QCoreApplication::postEvent(widget, event);
        }
    });

    QTest::mouseClick(forwardButton, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10), 100);

    QCOMPARE(dialog.result(), 0); // The dialog should not be accepted
    QCOMPARE(dialog.fileName(), "Test.kll");
}

void InitialDialogTest::notAcceptEmptyFileName()
{
    InitialDialog dialog;

    QLineEdit* voistluseNimiBox = dialog.findChild<QLineEdit*>("competitionNameEdit");
    voistluseNimiBox->setText("Test competition");

    QPushButton *forwardButton = dialog.findChild<QPushButton*>("forwardButton");

    QTimer::singleShot(200, [=](){
        QWidgetList allToplevelWidgets = QApplication::topLevelWidgets();
        QCOMPARE(allToplevelWidgets.length(), 2);

        QWidget* widget = QApplication::activeModalWidget();
        if (widget){
            QVERIFY(widget->inherits("QMessageBox"));
            QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
            QCoreApplication::postEvent(widget, event);
        }
    });

    QTest::mouseClick(forwardButton, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10), 100);

    QCOMPARE(dialog.result(), 0); // The dialog should not be accepted
    QCOMPARE(dialog.competitionName(), "Test competition");
}

void InitialDialogTest::notAcceptEmptyCompetitionNorFileName()
{
    InitialDialog dialog;
    QPushButton *forwardButton = dialog.findChild<QPushButton*>("forwardButton");

    QTimer::singleShot(200, [=](){
        QWidgetList allToplevelWidgets = QApplication::topLevelWidgets();
        QCOMPARE(allToplevelWidgets.length(), 2);

        QWidget* widget = QApplication::activeModalWidget();
        if (widget){
            QVERIFY(widget->inherits("QMessageBox"));
            QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
            QCoreApplication::postEvent(widget, event);
        }
    });

    QTest::mouseClick(forwardButton, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10), 100);

    QCOMPARE(dialog.result(), 0); // The dialog should not be accepted
}

void InitialDialogTest::notAcceptStartDateAfterEndDate()
{
    InitialDialog dialog;
    QPushButton *forwardButton = dialog.findChild<QPushButton*>("forwardButton");

    CompetitionSettings settings;
    settings.competitionName = "Test competition";
    settings.place = "Test place";
    settings.fileName = "Test.kll";
    settings.startDate = QDate(2021, 4, 19);
    settings.endDate = QDate(2021, 4, 18);
    dialog.setData(settings);

    QTimer::singleShot(200, [=](){
        QWidgetList allToplevelWidgets = QApplication::topLevelWidgets();
        QCOMPARE(allToplevelWidgets.length(), 2);

        QWidget* widget = QApplication::activeModalWidget();
        if (widget){
            QVERIFY(widget->inherits("QMessageBox"));
            QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
            QCoreApplication::postEvent(widget, event);
        }
    });

    QTest::mouseClick(forwardButton, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10), 100);

    QCOMPARE(dialog.result(), 0); // The dialog should not be accepted
}

QTEST_MAIN(InitialDialogTest)

#include "tst_initialdialogtest.moc"

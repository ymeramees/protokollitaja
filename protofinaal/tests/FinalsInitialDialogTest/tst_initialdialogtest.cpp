#include <QtTest>
#include <QCoreApplication>
#include <QPushButton>

#include "initialdialog.h"

class InitialDialogTest : public QObject
{
    Q_OBJECT

public:
    InitialDialogTest();
    ~InitialDialogTest();

private slots:
    void notAcceptEmptyCompetitionName();
    void notAcceptEmptyFileName();
    void notAcceptEmptyCompetitionNorFileName();

};

InitialDialogTest::InitialDialogTest()
{

}

InitialDialogTest::~InitialDialogTest()
{

}

void InitialDialogTest::notAcceptEmptyCompetitionName()
{
    InitialDialog dialog;
    dialog.setFileName("Test.fin");
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
    QCOMPARE(dialog.fileName(), "Test.fin");
}

void InitialDialogTest::notAcceptEmptyFileName()
{
    InitialDialog dialog;
    dialog.setCompetitionName("Test");
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
    QCOMPARE(dialog.competitionName(), "Test");
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

QTEST_MAIN(InitialDialogTest)

#include "tst_initialdialogtest.moc"

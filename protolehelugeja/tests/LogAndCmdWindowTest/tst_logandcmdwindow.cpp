#include <QtTest>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>

#include "logandcmdwindow.h"

class LogAndCmdWindowTest : public QObject
{
    Q_OBJECT

public:
    LogAndCmdWindowTest();
    ~LogAndCmdWindowTest();

private slots:
    void test_append();
    void test_cmdEditSendWithReturnKey();
    void test_cmdEditSendWithSendButton();
    void test_comPortChange();
    void test_scoringMachineTypeChange();

};

LogAndCmdWindowTest::LogAndCmdWindowTest()
{

}

LogAndCmdWindowTest::~LogAndCmdWindowTest()
{

}

void LogAndCmdWindowTest::test_append()
{
    LogAndCmdWindow logWindow;
    QString textToBeAppended = "Some text to be appended.";
    logWindow.append(textToBeAppended);

    QTextEdit* logView = logWindow.findChild<QTextEdit*>("logView");

    QVERIFY(logView->toPlainText().compare(textToBeAppended) == 0);
}

void LogAndCmdWindowTest::test_cmdEditSendWithReturnKey()
{
    LogAndCmdWindow logWindow;
    QSignalSpy spy(&logWindow, SIGNAL(receivedCommand(QString)));

    QLineEdit* cmdEdit = logWindow.findChild<QLineEdit*>("cmdEdit");
    QTest::keyClicks(cmdEdit, "ACK", Qt::NoModifier, 50);
    QTest::keyClick(cmdEdit, Qt::Key_Return, Qt::NoModifier, 100);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("ACK") == 0);
}

void LogAndCmdWindowTest::test_cmdEditSendWithSendButton()
{
    LogAndCmdWindow logWindow;
    QSignalSpy spy(&logWindow, SIGNAL(receivedCommand(QString)));

    QLineEdit* cmdEdit = logWindow.findChild<QLineEdit*>("cmdEdit");
    QTest::keyClicks(cmdEdit, "ACK", Qt::NoModifier, 50);

    QPushButton* sendButton = logWindow.findChild<QPushButton*>("sendButton");
    QTest::mouseClick(sendButton, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10), 100);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("ACK") == 0);
}

void LogAndCmdWindowTest::test_comPortChange()
{
    LogAndCmdWindow logWindow;
    QSignalSpy spy(&logWindow, SIGNAL(changeVariable(QString)));

    QLineEdit* cmdEdit = logWindow.findChild<QLineEdit*>("cmdEdit");
    QTest::keyClicks(cmdEdit, "COM21", Qt::NoModifier, 50);
    QTest::keyClick(cmdEdit, Qt::Key_Return, Qt::NoModifier, 100);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("comPort;COM21") == 0);
}

void LogAndCmdWindowTest::test_scoringMachineTypeChange()
{
    LogAndCmdWindow logWindow;
    QSignalSpy spy(&logWindow, SIGNAL(changeVariable(QString)));

    QLineEdit* cmdEdit = logWindow.findChild<QLineEdit*>("cmdEdit");
    QTest::keyClicks(cmdEdit, "scoringMachineType=RMIV", Qt::NoModifier, 50);
    QTest::keyClick(cmdEdit, Qt::Key_Return, Qt::NoModifier, 100);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> statusMessages = spy.takeFirst();
    QVERIFY(statusMessages.at(0).toString().compare("scoringMachineType;RMIV") == 0);
}

QTEST_MAIN(LogAndCmdWindowTest)

#include "tst_logandcmdwindow.moc"

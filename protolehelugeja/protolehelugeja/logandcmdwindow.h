#ifndef LOGANDCMDWINDOW_H
#define LOGANDCMDWINDOW_H

#include <QWidget>

namespace Ui {
class LogAndCmdWindow;
}

class LogAndCmdWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LogAndCmdWindow(QWidget *parent = nullptr);
    ~LogAndCmdWindow();
    void append(QString textToAppend, bool received = false);

protected:

private:
    Ui::LogAndCmdWindow *ui;

private slots:
    void sendText();

signals:
//    void changeBool(QString variable, bool value);
    void changeVariable(QString variableAndValue);  // Variable name and value separated by ';'
    void receivedCommand(QString cmd);
};

#endif // LOGANDCMDWINDOW_H

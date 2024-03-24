#ifndef INITIALDIALOG_H
#define INITIALDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>

#include "finalsformats.h"

namespace Ui {
class InitialDialog;
}

class InitialDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InitialDialog(QString, QWidget *parent = 0);
    ~InitialDialog();
    QString competitionName();
    QString fileName();
    QString eventName();
    QString eventType();
    QJsonObject eventConf();
    QString timePlace();
    void setCompetitionName(QString competitionName);
    void setFileName(QString fileName);
    void setEventName(QString newName);
    void setEventType(QString newType);
    void setTimePlace(QString timePlace);

public slots:
    void newFile();
    void openFile();

private slots:
    void forward();

private:
    Ui::InitialDialog *ui;
    FinalsFormats m_finalsFormats;

signals:
    void updateMe();
};

#endif // INITIALDIALOG_H

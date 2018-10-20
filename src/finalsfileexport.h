#ifndef FINALSFILEEXPORT_H
#define FINALSFILEEXPORT_H

#include <QFile>
#include <QDebug>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>

#include "startlistwriter.h"

namespace Ui {
class FinalsFileExport;
}

class FinalsFileExport : public QDialog
{
    Q_OBJECT

public:
    explicit FinalsFileExport(QVector<QStringList> inputTable, QString competitionFileLocation, QString competitionName, QString eventName, QWidget *parent = 0);
    ~FinalsFileExport();

public slots:
    QString getFinalsFileName();
    int getRelay();
    void setRelay(int);

protected:
    void accept();
//    void closeEvent(QCloseEvent *event);

private:
    QString competitionFileLocation;
    QString competitionName;
    QString eventName;
    QString finalsFileName;
    Ui::FinalsFileExport *ui;

private slots:
    bool writeFinalsFile();
    bool writeFinalsStartListFile();
};

#endif // FINALSFILEEXPORT_H

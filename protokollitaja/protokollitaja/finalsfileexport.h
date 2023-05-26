#ifndef FINALSFILEEXPORT_H
#define FINALSFILEEXPORT_H

#include <QFile>
#include <QTime>
#include <QDebug>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QRandomGenerator>

#include "startlistwriter.h"

namespace Ui {
class FinalsFileExport;
}

class FinalsFileExport : public QDialog
{
    Q_OBJECT

public:
    explicit FinalsFileExport(
            QVector<QStringList> inputTable,
            QString competitionFileLocation,
            QString competitionName,
            QString eventName,
            int eventType,
            QWidget *parent = 0
            );
    ~FinalsFileExport();

public slots:
    QString getFinalsFileName();
    int getRelay();
    void setRelay(int);

protected:
    void accept();
//    void closeEvent(QCloseEvent *event);

private:
    QString m_competitionFileLocation;
    QString m_competitionName;
    QString m_eventName;
    int m_eventType;
    QString finalsFileName;
    Ui::FinalsFileExport *ui;

private slots:
    void drawStartPositions();
    bool sortCompetitors();
    bool writeFinalsFile();
    bool writeFinalsStartListFile();
};

#endif // FINALSFILEEXPORT_H

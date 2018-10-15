#ifndef FINALSFILEEXPORT_H
#define FINALSFILEEXPORT_H

#include <QFile>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>

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
};

#endif // FINALSFILEEXPORT_H

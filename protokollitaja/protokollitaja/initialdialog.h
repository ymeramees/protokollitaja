#ifndef ALGUSEVALIK_H
#define ALGUSEVALIK_H

#include <QtGlobal>
#include <QDialog>
#include <QStandardPaths>

#include <QInputDialog>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QCloseEvent>
#include "ui_initialdialog.h"

#include "simplekllfilerw.h"
#include "competitionsettings.h"

class InitialDialog : public QDialog
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *event);

public:
    InitialDialog(QWidget *parent = 0);
    ~InitialDialog();
    void setCompetitionName(const QString newName);
    void setCountry(const QString newCountry);
    void setData(CompetitionSettings data);
    void setEndDate(const QDate endDate);
    void setFileName(QString newName);
    void setStartDate(const QDate startDate);
    QDate endDate() const;
    QDate startDate() const;
    QString competitionName() const;
    QString country() const;
    QString fileName() const;
    QString place() const;

public slots:
    void avamine();
    void edasi();
    void muutus(QString);
    void newCompetition();

private slots:

private:
    Ui::InitialDialogClass ui;
    void resetDates();
};

#endif // ALGUSEVALIK_H

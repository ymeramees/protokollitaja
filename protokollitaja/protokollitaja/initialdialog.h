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
    Ui::InitialDialogClass ui;
    void setData(CompetitionSettings data);
    void setStartDate(const QDate startDate);
    void setEndDate(const QDate endDate);
    QDate endDate() const;
    QDate startDate() const;
    QString competitionName() const;
    QString fileName() const;
    QString place() const;

public slots:
    void avamine();
    void edasi();
    void muutus(QString);
    void newCompetition();

private slots:

private:
    void resetDates();
};

#endif // ALGUSEVALIK_H

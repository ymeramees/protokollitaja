#ifndef SHOTSWINDOW_H
#define SHOTSWINDOW_H

#include <QDialog>
//#include <QTextStream>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>

#include "shotstablemodel.h"
#include "competitorshot.h"
#include "lask.h"

extern bool verbose;

namespace Ui {
class ShotsWindow;
}

class ShotsWindow : public QDialog
{
    Q_OBJECT

public:
    int aktiivneSeeria;
//    QList<QList<Lask*> > lasud; //Iga seeria lasud + koordinaadid
    explicit ShotsWindow(QVector<CompetitorShot> *shots, /*int, int ls = 10,*/ QWidget *parent = 0);
    void muudaPealkirja(QString);
    ~ShotsWindow();

public slots:
    bool deleteSelectedShot();
    bool ignoreSelectedShot();
    bool insertShot(int value = -999);
    void insertPenalty();
    void kirjutaLasud();    //Kirjutab tabelis olevad lasu väärtused laskudesse
    void loeLasud();    //Loeb laskude väärtused ja kirjutab tabelisse
    void moveDown();
    void moveUp();

private:
    Ui::ShotsWindow *ui;
    void closeEvent(QCloseEvent *event);
//    bool m_withDecimals;
    int m_noOfShots;
    int m_noOfSeries;
//    QVector<Lask> *m_shots = nullptr;
    ShotsTableModel m_shotsTableModel;

private slots:
    void markCompetition(bool);
};

#endif // SHOTSWINDOW_H

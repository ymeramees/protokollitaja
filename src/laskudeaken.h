#ifndef LASKUDEAKEN_H
#define LASKUDEAKEN_H

#include <QDialog>
#include <QMessageBox>

#include "lask.h"

namespace Ui {
class LaskudeAken;
}

class LaskudeAken : public QDialog
{
    Q_OBJECT

public:
    bool kumnendikega;
    int aktiivneSeeria;
    int laskudeArv;
    int seeriateArv;
    QList<QList<Lask*> > lasud; //Iga seeria lasud + koordinaadid
    explicit LaskudeAken(bool, int, int ls = 10, QWidget *parent = 0);
    void muudaPealkirja(QString);
    ~LaskudeAken();

public slots:
    void kirjutaLasud();    //Kirjutab tabelis olevad lasu väärtused laskudesse
    void loeLasud();    //Loeb laskude väärtused ja kirjutab tabelisse

private:
    Ui::LaskudeAken *ui;
    void closeEvent(QCloseEvent *event);

private slots:
    void paremale();
    void vasakule();
};

#endif // LASKUDEAKEN_H

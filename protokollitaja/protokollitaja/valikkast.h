#ifndef VALIKKAST_H
#define VALIKKAST_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QDialog>
#else
#include <QtGui/QDialog>
#endif
#include <QCloseEvent>
#include <QMessageBox>
#include "ui_valikkast.h"
#include "commonsettings.h"

class ValikKast : public QDialog
{
    Q_OBJECT

public:
    int relv;   //defineeritud järgmiselt
    enum Relv {Muu = 0, Ohupuss = 1, Ohupustol = 2, Sportpuss = 3, Spordipustol = 4, Puss = 5, Pustol = 6};
    QString harjutus;   // TODO To be changed to int
    ValikKast(CommonSettings *settings, QWidget *parent = 0);
    ~ValikKast();
    Ui::ValikKastClass ui;

protected:

public slots:
	void muudaEkraaniNimi(int);
        void muudaHarjutus(int);
	void voimalda(int indeks);
	void sulge();
};

#endif // VALIKKAST_H

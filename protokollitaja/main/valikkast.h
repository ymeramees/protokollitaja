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

class ValikKast : public QDialog
{
    Q_OBJECT

public:
    int relv;   //defineeritud järgmiselt
    enum {Muu = 0, Ohupuss = 1, Ohupustol = 2, Sportpuss = 3, Spordipustol = 4, Puss = 5, Pustol = 6};
    QString harjutus;
    ValikKast(QWidget *parent = 0);
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

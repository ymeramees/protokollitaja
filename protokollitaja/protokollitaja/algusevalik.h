#ifndef ALGUSEVALIK_H
#define ALGUSEVALIK_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QDialog>
#include <QStandardPaths>
#else
#include <QtGui/QDialog>
#include <QDesktopServices>
#endif

#include <QInputDialog>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include "ui_algusevalik.h"

class AlguseValik : public QDialog
{
    Q_OBJECT

public:
	//QString failiNimi;
    AlguseValik(QWidget *parent = 0);
    ~AlguseValik();
    Ui::AlguseValikClass ui;

public slots:
	void avamine();
        void edasi();
	void muutus(QString);
	void uus();
};

#endif // ALGUSEVALIK_H

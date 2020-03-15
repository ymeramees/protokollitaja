#ifndef LIIKMETEVALIKKAST_H
#define LIIKMETEVALIKKAST_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QDialog>
#else
#include <QtGui/QDialog>
#endif
#include <QMessageBox>

#include "ui_liikmetevalikkast.h"

class LiikmeteValikKast : public QDialog
{
    Q_OBJECT

public:
	int maxArv;
	QList<int> jalgitavad;
    LiikmeteValikKast(QWidget *parent = 0);
    ~LiikmeteValikKast();
    Ui::LiikmeteValikKastClass ui;

public slots:
	void eemalda();
	void lisa();
// TODO vaata kas läks vaja seda
signals:
	void lisaNr();
};

#endif // LIIKMETEVALIKKAST_H

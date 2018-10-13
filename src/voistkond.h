#ifndef VOISTKOND_H_
#define VOISTKOND_H_

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui/QWidget>
#endif

#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QDebug>
#include <QList>

#include "liikmetevalikkast.h"

class Voistkond : public QWidget
{
    Q_OBJECT

public:
	int vArv;
	int keskmLask;
	int *viimaneIndex;
	struct Liige{
		QString eesNimi;
		QString perekNimi;
		QString klubi;
		QString summa;
        QString harjutus;
//		QLabel *silt;
        QLineEdit *silt;
	};
	QLineEdit *nimi;
	QLineEdit *summa;
	QLineEdit *markus;
	QHBoxLayout *hKast;
	QCheckBox *linnuke;
	QPushButton *muudaNupp;
	QList<int> *jalgitavad;
	QList<Liige*> voistlejad;
	LiikmeteValikKast *lValik;
	Voistkond(LiikmeteValikKast *lV, int vA, QList<int> *j, int *vI, QWidget *parent = 0);
	virtual ~Voistkond();
	bool vaiksem(Voistkond *v) const;

public slots:
	void liida();
	void otsiNime();
	void naitaLiikmeteValikKast();

signals:
	void uuendaLiikmeid();
};

#endif /* VOISTKOND_H_ */

#ifndef LEHT_H
#define LEHT_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui/QWidget>
#endif

#include <QSignalMapper>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QKeyEvent>
#include <QList>

#include "andmebaas.h"
#include "laskur.h"
#include "voistkond.h"
#include "liikmetevalikkast.h"

class Leht : public QWidget
{
    Q_OBJECT

public:
    struct VaruLaskur{
        bool linnuke;
        QString rajaNr;
        QString sifriAlgus;
        QString sifriLopp;
		QString eesNimi;
		QString perekNimi;
		QString sunniAasta;
		QString klubi;
		QStringList seeriad;
		QList<int> lisaLasud;
		QString summa;
		QString finaal;
		QString kumned;
		QString markus;
	};
	struct Voistleja{
		QString nimi;
		QString perekNimi;
		QString klubi;
		QString summa;
		QString silt;
	};
	struct VaruVoistkond{
		QString nimi;
		QList<Voistleja> voistejad;
		QString summa;
		QString markus;
	};
	bool *kirjutusAbi;
	bool voistk;
	bool naidata;
	bool voibKontrollida;
	bool alustamine;
    bool kumnendikega;
	int leheIndeks;
	int viimaneIndex;
	int abi;
    int laskudeArv;
	int seeriateArv;
    int vSummadeSamm;
	int maxAeg, minAeg;
    int relv;
    int *jarjestamine;  //Kas sorteerimine käib kümnete arvu järgi või viimase seeria järgi
    QString harjutus;
	QString ekraaniNimi;
	QVBoxLayout* vKast;
	QList<int> jalgitavad;
	QList<Laskur*> laskurid;
    QList<Laskur*> reasLaskurid;    //Pointerid lehel olevatele laskuritele
	QList<Voistkond*> voistkonnad;
	QList<Voistkond*> reasVoistkonnad;
    Andmebaas* andmebaas;
	LiikmeteValikKast *lValik;
	//Leht *nimedeLeht;
    Leht(Andmebaas*, int, int, int, bool*, QString, int, QString, bool kum, int*, QWidget *parent = 0, bool v = false, /* Leht *ind = 0
            ,*/ LiikmeteValikKast *lV = 0, int lI = 0, int ls = 10);
    ~Leht();
    void sorteeri(int t);
    void reasta(int t);

public slots:
	void eemaldaLaskur();
    void idMuudatus(int, Laskur*);  //Edastab laskurist tulnud signaali
    void kontrolliKordusi();
    void lubaKontrollimist(const QString);
    void naitaSifrit();
	void uuendaLiikmeteKast();
    void uusLaskur(int);
	void vajutaTab();
    void vajutaTab2(int);
    void siusiReset(int connectionIndex);  //siusDatast lugemise progressi nullimine
	void teataMuudatusest(const QString);
	void teataMuudatusest();

signals:
    void idMuutus(int, Laskur*);    //Teatab, et taheti laskuri ID'd muuta
	void uuendaLiikmeid();
	void uuendaVoistkondi();
	void muudatus();
};

#endif // LEHT_H

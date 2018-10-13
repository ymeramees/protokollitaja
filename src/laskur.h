#ifndef LASKUR_H
#define LASKUR_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui/QWidget>
#include <QtGui/QMenu>
#include <QtGui/QContextMenuEvent>
#endif

#include <QLineEdit>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QList>
#include <QTimer>

#include "lisalaskudeaken.h"
#include "laskudeaken.h"
#include "andmebaas.h"
#include "lask.h"

class Laskur : public QWidget
{
    Q_OBJECT

public:
	bool *kirjutusAbi;
	bool muudetud;
    bool *kumnendikega;
    bool onLehelugejaLaskur;    //Märgistamaks ära laskurit, kellele parasjagu loetakse lehti
    bool onVorguLaskur; //Märgistamaks ära laskurit, kellele lehelugejas loetakse lehti
    bool voistlus;  //Kas on proovi või võistluslasud
	enum {Puudub = 0, Puss = 1, Pustol = 2};
	int abi;
    int id; //Laskuri ID
    int jrkArv; //Laskuri järjekorra nr lehel
    int keskmLask;
    int laskudeArv;
    int mitmesVoistlus; //Siusist lugemisel mitmendad võistluslasud (asend või teine pool) tulevad
	int seeriateArv;
    int vSummadeSamm;
    int *jarjestamine;  //Kas sorteerimine käib kümnete arvu järgi või viimase seeria järgi
    enum {KumneteArvuga = 0, ViimaseSeeriaga = 1};  //Täisarvudega lugemise puhul järjestamise variandid
	Andmebaas* andmebaas;
    QCheckBox *linnuke;
	QHBoxLayout *hKast;
    QLabel *sidekriips;
    QLineEdit *rajaNr;
    QLineEdit *sifriAlgus;
    QLineEdit *sifriLopp;
    QPushButton *sifriNupp;
	QLineEdit *eesNimi;
	QLineEdit *perekNimi;
	QLineEdit *sunniAasta;
	QLineEdit *klubi;
    QList<QLineEdit*> seeriad;
	QList<QLineEdit*> vSummad;
    QList<QList<Lask*> > lasud; //Iga seeria lasud + koordinaadid
    QList<int> lisaLasud;   //Väärtused 10kordsetena

	QLineEdit *finaal;
    QLabel *koguSumma;
	QLineEdit *kumned;
	QLineEdit *markus;
	QPushButton *lisaLNupp;
    QString eelmineRida;    //Eelmine Siusist tulnud rida
	QTimer *arvutaja;
	LisaLaskudeAken *lisaAken;
    Laskur(Andmebaas*, int, int, int, bool*, bool*, int, int*,int ls = 10, QWidget *parent = 0);
    ~Laskur();
//    bool operator<(const Laskur &l) const;
    QString getSumma();
    bool vaiksem(Laskur *l, int t) const;
    void set(const Laskur *l);
    void setSumma(QString);

public slots:
    void liida(); //laskude summeerimine
    void siusiReset();   //Siusist lugemise progressi nullimine

private slots:
    void enterVajutatud();
    bool eventFilter(QObject* object, QEvent* event);
    void lisaLAken();
    void markuseMuutus();
    void muutus(QString);
    void muutus2(QString);
    void muutus4(QString);
    void muutus5();
    void naitaIdAken();
    void naitaLaskudeAkent();
    void naitaSifrit();
    void teataMuudatusest(QString);
    void teataMuudatusest();
    int vaiksemNimega(Laskur*) const;   //Tagastab kas laskur on nime järgi väiksem (-1), suurem (1) või võrdne (0)
    int vaiksemRajaga(Laskur*) const;   //Tagastab kas laskur on rajanumbri järgi väiksem (-1), suurem (1) või võrdne (0)
    void vajutaTab();

signals:
    void enter(int);
    void idMuutus(int, Laskur*);    //Teatab, et taheti ID'd muuta
    void muudatus();
    void sifrimuutus();

private:
    QAction *idAct;
    QAction *laskudeAkenAct;
    QLineEdit *summa;
    QMenu *popup;
    LaskudeAken *laskudeAken;
    void contextMenuEvent(QContextMenuEvent *event);
};

#endif // LASKUR_H

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
#include "duelpair.h"
#include "laskur.h"
#include "voistkond.h"
#include "liikmetevalikkast.h"
#include "qualificationevents.h"

class Leht : public QWidget
{
    Q_OBJECT

public:
    enum PageType {
        Individual = 0,
        Team = 1,
        Duel = 2
    };
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
    TargetTypes::TargetType m_targetType;
    int *jarjestamine;  //Kas sorteerimine käib kümnete arvu järgi või viimase seeria järgi
	QString ekraaniNimi;
    std::optional<QString> exportStartList();
	QVBoxLayout* vKast;
	QList<int> jalgitavad;
	QList<Laskur*> laskurid;
    QList<Laskur*> reasLaskurid;    //Pointerid lehel olevatele laskuritele
	QList<Voistkond*> voistkonnad;
	QList<Voistkond*> reasVoistkonnad;
    QList<DuelPair*> duelPairs; //Duellimatši paarid, ainult duelli lehel
    QLineEdit *leftTeamName = nullptr;
    QLineEdit *rightTeamName = nullptr;
    Andmebaas* andmebaas;
	LiikmeteValikKast *lValik;
	//Leht *nimedeLeht;
    Leht(Andmebaas*, int, int, int, bool*, QString, TargetTypes::TargetType target, QualificationEvents::EventType eventType, bool kum, int*, QWidget *parent = 0, PageType pageType = Individual, /* Leht *ind = 0
            ,*/ LiikmeteValikKast *lV = 0, int lI = 0, int ls = 10);
    ~Leht();
    void addDuelPair(int leftId, int rightId);
    void addDuelPair(QJsonObject pairJson);
    QString duelName() const;
    QualificationEvents::EventType eventType() const;
    std::optional<QJsonObject> finalsData();
    int leftMatchPoints() const;
	int maxTime() const;
	int minTime() const;
    PageType pageType() const;
    int rightMatchPoints() const;
    void setEventType(const QualificationEvents::EventType newEventType);
    void setFinalsData(QJsonObject finalsData);
	void setMaxTimeMs(const int newTime);
	void setMinTimeMs(const int newTime);
    void setToBeUploaded(bool newStatus);
    void sorteeri(int t);
    void reasta(int t);
    bool toBeUploaded();
    QJsonObject toExportJson();
    QString getEventType();

public slots:
    void deleteAllShotsFromSelectedCompetitors();    // For testing purposes
	void eemaldaLaskur();
    void idMuudatus(int, Laskur*);  //Edastab laskurist tulnud signaali
    void kontrolliKordusi();
    void lubaKontrollimist(const QString);
    void naitaSifrit();
    void updateDuelPoints();
	void uuendaLiikmeteKast();
    void uusLaskur(int);
    void uusLaskur(QJsonObject);
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

private:
    QualificationEvents::EventType m_eventType;
    PageType m_pageType = Individual;
    bool m_toBeUploaded = true;  //Shows if results on this page should be uploaded to web or not
	int m_maxTime, m_minTime;	// Time limits for how long the results are shown on spectator screen, in ms
    std::optional<QJsonObject> m_finals = std::nullopt;
    QLabel *m_matchScore = nullptr;
    void connectCompetitorSignals(Laskur *las);
    Laskur* createCompetitor(int id);
    Laskur* createCompetitor(QJsonObject json);
    void createDuelHeader();
    void updateMaximumHeight();
};

#endif // LEHT_H

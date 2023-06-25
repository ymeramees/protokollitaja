#ifndef PROTOKOLLITAJA_H
#define PROTOKOLLITAJA_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QMainWindow>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#else
#include <QtGui/QMainWindow>
#include <QProgressDialog>
#endif
//#include <QNetworkInterface>
#include <QListIterator>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QToolButton>
#include <QMessageBox>
//#include <QHostAddress>
//#include <QTcpServer>
//#include <QTcpSocket>
#include <QTabWidget>
#include <QtNetwork>
#include <QShortcut>
#include <QTextEdit>
//#include <QWebView>
#include <QProcess>
#include <QAction>
#include <QList>
#include <QMenu>

#include "andmebaas.h"
#include "laskur.h"
#include "leht.h"
#include "initialdialog.h"
#include "datauploader.h"
#include "valikkast.h"
#include "seadedkast.h"
#include "commonsettings.h"
#include "sifriseade.h"
#include "importaken.h"
#include "tulemuseaken.h"
#include "lehelugejaaken.h"
#include "startlistwriter.h"
#include "finalsfileexport.h"
#include "liikmetevalikkast.h"
#include "siusdataconnections.h"
#include "protolehelugejaserver.h"
#include "uhendumiseaken.h"
#include "updatechecker.h"
#include "xlslib.h"

class Protokollitaja : public QMainWindow
{
    Q_OBJECT

protected:
	void closeEvent(QCloseEvent *event);

public:
	bool voibSulgeda;
	bool kirjutusAbi;
	bool kasNaidataTul;
    bool uhendusAutoriseeritud;
    bool autoUuendus;   //Kas uuenduste kontroll on automaatne (true) või menüüst valitud (false)
	bool voibUuendadaNimekirja;
    bool voistlus;   //Kasutatakse SiusDatast info vastuvõtmisel, näitab, kas finaali võistluslasud on alanud
    enum TargetType {Muu = 0, Ohupuss = 1, Ohupustol = 2, Sportpuss = 3, Spordipustol = 4, Puss = 5, Pustol = 6};
        //enum {vOhupuss = 0, sOhupuss = 1, vOhupustol = 2, sOhupustol = 3, lam = 4, vStandard = 5, sStandard = 6, vabap = 7, spordikas = 8, sumat = 9, olump = 10};
    enum RankingOptions {KumneteArvuga = 0, ViimaseSeeriaga = 1};  //Täisarvudega lugemise puhul järjestamise variandid
    int m_ranking;   //Täisarvudega lugemisel, millist järjestamist kasutada
	int abi;
    int laskuriId;  //Suurim kasutusel olev laskuri ID
    int leheIndeks;
    int parool;
    int lasuVSiusis;   //Mitmes lahter on lasu väärtus SiusDatatast tulnud reas
    int lasuNrSiusis;   //Mitmes lahter on lasu jrk number SiusDatatast tulnud reas
    ProtolehelugejaServer *server;  //For accepting connections from Protolehelugeja
    QFile *logi;    //Siusist tulnud info põhjal tehtud muudatuste logi
    QFile *siusLogi;    //Siusist saabunud võrguliikluse logi
	QString m_competitionName;
    QString m_place;
	QString seeFail;
    QString finaaliFailiNimi;   //Tehtava finaali faili nimi täies pikkuses
    QString ipAadress;
//    QString eelmineRida;    //SiusDatast loetud eelmise lasu rida, vaja, et aru saada, kas võistlus on alanud
    QString siusiBuffer;    //SiusDatast saabunud info vaheladu
    QTextStream logiValja;  //Logifaili kirjutamiseks
	Andmebaas andmebaas;
    QProcess *finaal;
    QProcess *protoUuendaja;
	QMenu *failMenu;
	QMenu *tooriistadMenu;
	QMenu *tulemusedMenu;
	QMenu *abiMenu;
    QAction *avaAct;
    QAction *eelvaadeAct;
    QAction *eemaldaLaskurAct;
    QAction *eemaldaMargidAct;
    QAction *eemaldaTabAct;
    QAction *eksportCSVAct;
    QAction *eksportInbandStartListAct;
    QAction *eksportSiusStartListAct;
    QAction *eksportFSiusStartListAct;
    QAction *eksportTXTAct;
    QAction *eksportXLSAct;
    QAction *finaalAct;
    QAction *finaaliFailAct;
    QAction *impordiFinaalAct;
    QAction *importAct;
    QAction *kaivitaServerAct;
    QAction *kontrolliUuendusiAct;  //Programmi uuenduste olemasolu kontrollimine ja programmi uuendamine
    QAction *kopeeriLaskuridAct;
    QAction *kopeeriVahAct;
    QAction *kopeeriValitudVahAct;
    QAction *lehelugejaAct;
    QAction *liigutaAct;
    QAction *margiKoikAct;
    QAction *naitaSeadedAct;
    QAction *naitaTulAkenAct;
    QAction *naitaWAbiAct;
    QAction *prindiAct;
    QAction *programmistAct;
    QAction *reastaAct;
    QAction *reastaPAct;
    QAction *reastaRAct;
    QAction *reastaSAct;
    QAction *reastaSiAct;
    QAction *salvestaAct;
    QAction *salvestaKuiAct;
    QAction *seiskaServerAct;
    QAction *sendCompetitorsToRangeAct;
    QAction *sifriLisaAct;
    QAction *taiendaBaasAct;
    QAction *uhenduSiusDatagaAct;
    QAction *uusAct;
    QAction *uusLaskurAct;
    QAction *uusTabAct;
    QAction *uploadAct;
    QAction *valjuAct;
	QToolBar *toolBar;
	QTabWidget *tabWidget;
	//QToolButton *uusNupp;
	QTimer *sulgeja;
	QTimer *naitaja;
	QTimer *viiLopuni;
	QTimer *salvestaja;
    QTimer uploadTimer;
	QTimer *voistkondadele;
    QUdpSocket *udpSocket;  //Broadcasti saatmiseks, et leida võrgust Protokollitaja arvuti
	QLabel *prindiEelvaade;
	QPixmap *pilt;
	QPixmap *pilt2;
	//QPixmap *lPilt;
//    QTcpServer *server; //Server, kuhu ühendub Protolehelugeja
//    QTcpSocket *socket; //Protolehelugejaga suhtlemiseks
    QTcpSocket *siusDataSocket; //SiusDataga suhtlemiseks
    quint16 blockSize;  //Protolehelugejaga suhtlemisel paketi suurus
    QMessageBox teatekast;
    Leht *lehelugejaLeht;   //Pointer lehele, millele loetakse parasjagu tulemusi
//    Leht *vorguLeht;    //Pointer lehele, millel oleva laskuri siffer saadeti viimati Protolehelugejale
    Laskur *lehelugejaLaskur;   //Pointer laskurile, kellele loetakse parasjagu tulemusi
//    Laskur *vorguLaskur;    //Pointer laskurile, kelle siffer saadeti viimati Protolehelugejale
    InitialDialog *aValik;
	ValikKast *valik;
	SeadedKast *seaded;
    SifriSeade *sifriLisaAken;
    SiusDataConnections *siusDataConnections = nullptr;
	ImportAken *importAken;
    TulemuseAken *tulemus;
    LehelugejaAken *lehelugejaAken;
	LiikmeteValikKast *lValik;
    UhendumiseAken *uhendumiseAken; //SiusDataga ühendumiseks
    Protokollitaja(QWidget *parent = 0);
    ~Protokollitaja();
    //bool lessThan(const Laskur*, const Laskur*);
    QJsonObject toExportJson();

#ifdef QT_DEBUG    // Functionality for testing purposes
    QAction *deleteAllShotsAct;

private slots:
    void deleteAllShots();
#endif

private slots:
    void algseaded();   //Tekitab või taastab algsed seaded
    void autosave();
    void ava();
    void checkForUpdates();   //Automaatne uuenduste kontroll
    void checkForUpdates(bool autoCheck);  // Check for newer versions of the program
    void eelvaade();
    void eemaldaLaskur();
    void eemaldaMargid();
    void eemaldaTab();
    void eksportCSV();
    void exportStartList(StartListWriter::StartListType type);    // For qualification event
    void exportFinalsSiusStartList();   // For finals
    void eksportTXT();
    void eksportXLS();
    void finaaliFail();
    void finaalValmis(const int exitCode, const QProcess::ExitStatus exitStatus);
    void impordiFinaal();
    void import();
    void kaivitaFinaal();
    void kaivitaServer();
    void kasNaitaTul(bool);
    void kirjutaFail(QString);
    void kirjutaSeaded();
    void kontrolliIdKordusi();   //Kuna see funktsioon peaks midagi muutma ainult, kui avatakse vana faili, siis lisab see uued ID'd automaatselt
    void kontrolliIdKordust(int, Laskur*);   //Kontrollib laskuri uue ID varasemat olemasolu
    void kopeeriLaskurid();
    void kopeeriVah();
    void kopeeriValitudVah();
    void lehedLoetud();
    void lehelugeja();
    void liiguta();
    void loeBroadcast();    //Loeb võrgust tulnud broadcasti
    void loefail();
    void loeFinaaliFail(QString);
    void loeSeaded();
//    void loeSiusDatast();   //Võtab vastu lasud SiusDatast
    void margi();
    void muudaSalvestamist();
    void muudaTab(const QModelIndex&);
    void naitaInfot();
    void naitaSeaded();
    void naitaTul();
    void naitaTulAken();
    void naitaWAbi();
    void peataProgress();   //Sulgeb progressi akna, kui see on ees
    void prindi();
    void prindi2();
    void readShotInfo(QString data, int socketIndex);
    void readSiusInfo(SiusShotData shotData);
    void reasta();
    void reastaP();
    void reastaR();
    void reastaS();
    void reastaSi();
    void receivedVersionInfo(bool updateExists, QString versionString);
    void dataUploaderFinished(bool success, QString reply, QString errorString);
    void saadaVorku(QString, int socketIndex);
    void salvesta();
    void salvestaKui();
    void seiskaServer();
    void sendCompetitorsToRange();
    void sifriLisa();
    void statusBarInfoChanged(QString newStatusInfo);
    void sulge();
    void sulgeUhendus();
    void taiendaAndmebaas();
    void uuendaJalgitavaid();
    void uuendaLehelugejat(QString);
    void uuendaLehelugejatSifriga(int);
    void uuendaLiikmeteKast();
    void uuendaLiikmeteNimekirja(int);
    void uuendaSeaded();
    void uuendaVoistkondi();
    void uuendaVorkuSifriga(int, int socketIndex);
    void uhenduSiusDataga();
    void uhendusSiusigaKatkes(int connectionIndex);
    void uploadResults();   //Upload results to Protokollitaja web page
    void uus();
    void uusLaskur();    //Uue laskuri loomine,  koos uue ID'ga
    void uusLaskur(int);   //Uue laskuri loomine, koos olemasoleva ID'ga (kasutatakse näiteks faili avamisel
    void uusLaskur(QJsonObject);    // Create new competitor from json
    void uusTab();
    void viiLoppu();

private:
    void setDataFromInitialDialog();
    QString timeAndPlaceString();
    DataUploader *dataUploader = nullptr;
    QDate m_startDate;
    QDate m_endDate;
    QProgressDialog *progress;  //Näitab, et SiusDatast alles andmed tulevad
    CommonSettings m_settings;
    QString webCompetitionId = "";
    QString m_country = "Estonia";
    QString m_restHeaderData = "";
    QTimer *progressTimer;  //Timer, et progressi aken mõne aja pärast kinni panna
};

#endif // PROTOKOLLITAJA_H

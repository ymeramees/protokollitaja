#ifndef LEHELUGEJA_H
#define LEHELUGEJA_H

#include <QMainWindow>
#include <QMessageBox>
//#include <QHostAddress>
#include <QInputDialog>
#include <QProgressDialog>
#include <QSerialPortInfo>
#include <QDataStream>
//#include <QTcpSocket>
#include <QtNetwork>
#include <QShortcut>
#include <QKeyEvent>
#include <QAction>
#include <QPainter>
#include <QImage>
#include <QTimer>
#include <QDebug>

#include "lask.h"
#include "target.h"
#include "targettypes.h"
#include "logandcmdwindow.h"
#include "qualificationevents.h"
#include "scoringmachineconnection.h"

// For testing
#ifdef QT_DEBUG
#include "selftests.h"
#endif

namespace Ui {
class Lehelugeja;
}

class Lehelugeja : public QMainWindow
{
    Q_OBJECT

    friend class IntegrationTest;

public:
    explicit Lehelugeja(bool inTest, QWidget *parent = nullptr);
    ~Lehelugeja();
    int scoringMachineType() const;

private:
    int lask;   //Loetava lasu järjekorra nr seerias
    int seeria; //Loetava seeria summa
    int parool; //Protokollitajaga ühendusmisel küsitav parool
    int laskudeArv;     //Laskude arv igas seerias, praegu ainult 10
    int seeriateArv;    //Mitu seeriat on loetava laskuri harjutuses
    int saatmiseEtapp;  //Näitab mitmendas etapis saatmine parasjagu on: 0 - algseis/ACK tulnud (lõpp), 1 - ENQ saadetud, 2 - STX saabunud, 3 - tekst saadetud, 4 - saabus tekst, vaja vastata ACK
    int aktiivseSeeriaNr;   //Seeria järjekorra nr (0-5), kuhu loetakse tulemus
    bool broadcastSaabunud;
    bool lugemismasinaValimine; //Näitab, kas alles määratakse kindlaks, milline lugemismasin ühendatud on
    bool oliLiigneLask;
    bool vahemuses;
    bool oliKahtlane;
    bool uhendusAutoriseeritud; //Vajalik parooli kasutamiseks Protokollitajaga ühendumisel
    bool uusLugemismasin;   //Kas ühendatud on uus või vana lugemismasin
    bool voibSulgeda;   //Kontroll, kas ühendus on suletud ja Protokollitajale teade saadetud, enne kui programm sulgeda
    bool kaabelLeitud;  //Kontrollimaks, kas käivitamisel leiti lugemismasina kaabel või ei
    LogAndCmdWindow logi;
    QAction *programmistAct;
    QAction *uhenduServerigaAct;
    QAction *valjuAct;
    QByteArray saadetis;    //Lugemismasinasse saadetav tekst
    QMenu *abiMenu;
    QMenu *failMenu;
    quint16 blockSize;  //Protokollitajaga suhtlemisel paketi suurus
    QTcpSocket *socket; //Protokollitajaga suhtlemiseks
    QList<QSerialPortInfo> pordid;
    QImage *pilt;
    QList<Lask*> seriesShots;   // Shots in current series that is being read
    QList<QList<Lask*> > lasud; //Iga seeria lasud + koordinaadid
    QList<QLineEdit*> seeriad;
    QLineEdit *aktiivneSeeria;  //Kast, kuhu pannakse loetud seeria
    QPainter *painter;
    QProgressDialog *progress;
    QSize markleheSuurus;  //Eelmine märklehe suurus repaintEvent'i jaoks
    QString aadress;    //Protokollitaja serveri IP aadress
    QString eelminePakett;  //vältimaks ühe info korduvat saatmist
    QTimer *broadcastiSaatja;   //Timer, mis käivitab IP küsimise broadcasti saatmise
    QTimer *otsija; //Portide otsimiseks
    QTimer *fookus; //Viib kursori, peale uue seeria kasti aktiivseks tegemist, sifri kasti
    QTimer *sulgeja;    //Vajalik viivitus programmi sulgemisel, et jõuaks teate Protokollitajale saata
    QTimer *seadistaja; //Viivis peale lehtede lugemist, enne kui uuesti seadistatakse
    QUdpSocket *udpSocket;  //Broadcasti saatmiseks, et leida võrgust Protokollitaja arvuti
    QUdpSocket *udpSocket2;  //Kuna windows ei suuta WaitForBytesWritten funktsiooni kasutada, on vaja teist socketit
    ScoringMachineConnection scoringMachCon;
    Target target;
    Ui::Lehelugeja *ui;

public slots:
    void changeVariable(QString variableAndValue);
    void processCommand(QString command);

private slots:
    void restartScoring();
    int CRC(QByteArray *s);  //Arvutab lugemismasina ühenduse jaoks kontrollnumbri
    void fookusMuutus(QWidget *vana, QWidget *uus);    //Vaatab, kas mõni seeria kastidest sai fookuse, kui sai, muudab, et sinna loetaks tulemus
    void fookusSifrile();   //Viib kursori, peale uue seeria kasti aktiivseks tegemist, sifri kasti
//    void joonistaLask(QPointF p, bool kasMM);    //Joonistab lasu lehele, bool näitab, kas on vana lugemismasina koordinaadid või mm
    void drawTarget();
    void hakkaOtsima();
    void sumAndEndSeries();
    void liigneLask();
    void loeBroadcast();    //Loeb võrgust tulnud broadcasti
    void loeVorgust();
    void naitaInfot();
    void naitaViga(QAbstractSocket::SocketError viga);
    void otsiPorti();
    void paluSalvestada();
    void peidaNimi();
    void peidaNupud();
#ifdef QT_DEBUG
    void runSelfTests();
#endif
    void saada(QString);
    void saadaBroadcast();
    void saadaParool(); //Vaja ühenduse loomise jätkamiseks
    void saadaVorku(QString);
    void seadista();
    void seeriaLoetud();
    void startScoring();
    void sulgeUhendus();
    void uhenda();
    void uhenduServeriga(QString); //Ühendumine serveriga
    void uhenduUuesti();    //Et muuta serveri IP'd ja uuesti ühenduda
    void updateLog(QString);
    void updatePorts(bool);
    void uuendaSifriga();

protected slots:
    void readShot(Lask shot);
//signals:
//    void lehedLoetud();
//    void salvesta();
//    void uutSifrit(int);

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
//    void paintEvent(QPaintEvent * event);
};

#endif // LEHELUGEJA_H

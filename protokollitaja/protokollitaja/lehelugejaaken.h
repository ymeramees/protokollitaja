#ifndef LEHELUGEJAAKEN_H
#define LEHELUGEJAAKEN_H

#include <QPainter>
#include <QImage>
#include <QTimer>
#include <QDebug>
#include <QKeyEvent>
#include <QShortcut>
#include <QSerialPort>
#include <QSerialPortInfo>
//#include <qextserialport/qextserialbase.h>
//#include <qextserialbase.h>
//#include <qextserialport/qextserialport.h>
//#include <qextserialport.h>
#include "lask.h"
#include "ui_lehelugejaaken.h"
//class QextSerialPort;

class LehelugejaAken : public QDialog {
    Q_OBJECT

public:
    bool oliLiigneLask;
    bool vahemuses;
    bool oliKahtlane;
    bool uhendatud; //Vajalik, et aru saada, kas lugemismasin on ühendatud või mitte, et uus nupuvajutus midagi ära ei rikuks.
    bool kaabelLeitud;  //Kontrollimaks, kas käivitamisel leiti lugemismasina kaabel või ei
    int lask;   //Loetava lasu järjekorra nr seerias
    int seeria; //Loetava seeria summa
    int aktiivseSeeriaNr;   //Seeria järjekorra nr (0-5), kuhu loetakse tulemus
    QSerialPort serial;
    QList<QSerialPortInfo> pordid;
    QImage *pilt;
    QList<Lask*> seeriaLasud;   //Loetava seeria lasud
    QList<QList<Lask*> > lasud; //Iga seeria lasud + koordinaadid
    QList<QLineEdit*> seeriad;
    QLineEdit *aktiivneSeeria = nullptr;  //Kast, kuhu pannakse loetud seeria
    QPainter *painter;
    QTimer *timer;
    QTimer *otsija;
    QTimer *fookus; //Viib kursori, peale uue seeria kasti aktiivseks tegemist, sifri kasti
    QTimer *seadistaja; //Viivis peale lehtede lugemist, enne kui uuesti seadistatakse
    Ui::LehelugejaAken m_ui;
    LehelugejaAken(QWidget *parent = 0, Qt::WindowFlags f = Qt::Widget);

public slots:
    void alustaUuesti();
    void fookusMuutus(QWidget *vana, QWidget *uus);    //Vaatab, kas mõni seeria kastidest sai fookuse, kui sai, muudab, et sinna
                                                       //loetaks tulemus
    void fookusSifrile();   //Viib kursori, peale uue seeria kasti aktiivseks tegemist, sifri kasti
    void joonistaLeht();
    void hakkaOtsima();
    void liigneLask();
    void loe();
    void otsiPorti();
    void paluSalvestada();
    void peidaNimi();
    void peidaNupud();
    void saada(QString);
    void saadaTekst();
    void seeriaLoetud();
    void sulgeUhendus();
    void uhenda();
    void uuendaNimega(QString);
    void uuendaPorte();
    void uuendaSifriga();

private slots:
    void seadista();

signals:
    void lehedLoetud();
    void salvesta();
    void uutSifrit(int);
    void uutNime(QString);

protected:
    void changeEvent(QEvent *e);
};

#endif // LEHELUGEJAAKEN_H

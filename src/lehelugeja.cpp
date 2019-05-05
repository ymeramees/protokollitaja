#include "lehelugeja.h"
#include "ui_lehelugeja.h"

/////////////////////////////////////////////////////////////////////////////
/// ToDo list:
/// Teha Protokollitaja lehelugemise aken sarnaseks sellega siin
/// Teha leht sportpüssi jaoks
/// Teha logi puhtamaks?
/// Kontrollida uue ja vana masina valimist................................OK
/// Uuest masinast tulnud lasu koordinaadid on 0, 0........................OK
/// Seeria summa vaja lisada...............................................OK
/// Tab order segamini.....................................................OK
/// Peale lugemise lõppu teha sifrikast aktiivseks.........................OK
/// Lisada salvestamise käsk...............................................OK
/// Kui muudetakse samaaegselt Protokollitajas seeriaid, kaob see info.....OK
/// Programmist väljumisel sulgeda ka lugemismasina ühendus................OK
/////////////////////////////////////////////////////////////////////////////

extern QString programmiNimi;
extern QString versioon;
extern QString aasta;
extern bool verbose;

#define EOT 0x04
#define ENQ 0x05 	// Anfrage
#define ACK 0x06
#define STX 0x02	// Start Text
#define NAK 0x15
#define CR 0x0d

char enq = ENQ;
char ack = ACK;

Lehelugeja::Lehelugeja(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Lehelugeja)
{
    ui->setupUi(this);

#ifdef PROOV
    ui->laskudeBox->setRange(1, 10);    // For testing purposes
#endif

    broadcastSaabunud = false;
//    markleheSuurus = ui->silt->size();
    oliLiigneLask = false;
    vahemuses = false;
    oliKahtlane = false;
//    uhendatud = false;
    kaabelLeitud = false;
    lask = 0;
    seeria = 0;
    blockSize = 0;
    laskudeArv = 10;
    saatmiseEtapp = 2;  //kui on vana masinaga ühendus, siis see ei muutu ja võib teksti saata, kuna: 0 - algseis/ACK tulnud (lõpp), 1 - ENQ saadetud, 2 - STX saabunud, 3 - tekst saadetud, 4 - saabus tekst, vaja vastata ACK
    seeriateArv = 0;
    aktiivseSeeriaNr = 0;
    uhendusAutoriseeritud = false;
    uusLugemismasin = false;    //Kõigepealt proovime, kas on vana masin
    lugemismasinaValimine = false;  //Algul kohe mingit otsimist ei käi
//#ifdef PROOV
//    uusLugemismasin = true;
//#endif
    voibSulgeda = false;
    aadress = "127.0.0.1";  //Esialgse IP aadressi määramine, mida näidatakse aadressiakna esmakordsel kuvamisel, teistel kordadel,
                                           //kui ei ole aadress õige, näidatakse akna uuesti kuvamisel eelmist sisestatud aadressi

    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(loeVorgust()));
    connect(socket, SIGNAL(connected()), this, SLOT(saadaParool())); //Kui ühendus serverisse on loodud, jätkab ühenduse vormistamisega
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(naitaViga(QAbstractSocket::SocketError)));

    udpSocket = new QUdpSocket(this);
    connect(udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(naitaViga(QAbstractSocket::SocketError)));
    udpSocket2 = new QUdpSocket(this);
    udpSocket2->bind(45744, QUdpSocket::ShareAddress);
    connect(udpSocket2, SIGNAL(readyRead()), this, SLOT(loeBroadcast()));
    connect(udpSocket2, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(naitaViga(QAbstractSocket::SocketError)));

    programmistAct = new QAction("Programmist", this);
    programmistAct->setStatusTip("Kuvab infot programmi kohta");
    connect(programmistAct, SIGNAL(triggered()), this, SLOT(naitaInfot()));
    uhenduServerigaAct = new QAction(tr("Ühendu serveriga"), this);
    uhenduServerigaAct->setStatusTip(tr("Ühendu protokollitaja serveriga"));
    connect(uhenduServerigaAct, SIGNAL(triggered()), this, SLOT(uhenduUuesti()));
    valjuAct = new QAction(QIcon(":/images/exit.png"), tr("&Välju"), this);
    valjuAct->setShortcut(tr("Ctrl+q"));
    valjuAct->setStatusTip(tr("Välju programmist"));
    connect(valjuAct, SIGNAL(triggered()), this, SLOT(close()));

    failMenu = ui->menuBar->addMenu("&Fail");
    failMenu->addAction(uhenduServerigaAct);
    failMenu->addAction(valjuAct);

//#ifdef PROOV
//    QAction *liidaSeeriaAct = new QAction("Liida seeria", this);
//    liidaSeeriaAct->setStatusTip("Liidab seeria suvaliste tulemustega testimiseks");
//    connect(liidaSeeriaAct, SIGNAL(triggered()), this, SLOT(liidaSeeria()));
//    failMenu->addAction(liidaSeeriaAct);
//#endif

    abiMenu = ui->menuBar->addMenu("&Abi");
    abiMenu->addAction(programmistAct);

    connect(ui->liigneNupp, SIGNAL(clicked()), this, SLOT(liigneLask()));
    connect(ui->otsiCOMNupp, SIGNAL(clicked()), this, SLOT(hakkaOtsima()));
    connect(ui->resetNupp, SIGNAL(clicked()), this, SLOT(restartScoring()));
    connect(ui->peidaNupp, SIGNAL(clicked()), this, SLOT(peidaNimi()));
    connect(ui->peida2Nupp, SIGNAL(clicked()), this, SLOT(peidaNupud()));
    connect(ui->saadaNupp, SIGNAL(clicked()), this, SLOT(saadaTekst()));
    connect(ui->seadistaNupp, SIGNAL(clicked()), this, SLOT(seadista()));
    connect(ui->sulgeUhendusNupp, SIGNAL(clicked()), this, SLOT(sulgeUhendus()));
    connect(ui->tekstiEdit, SIGNAL(returnPressed()), this, SLOT(saadaTekst()));
    connect(ui->uhendaNupp, SIGNAL(clicked()), this, SLOT(uhenda()));
    connect(ui->seeria1Edit, SIGNAL(editingFinished()), this, SLOT(seeriaLoetud()));
    connect(ui->seeria2Edit, SIGNAL(editingFinished()), this, SLOT(seeriaLoetud()));
    connect(ui->seeria3Edit, SIGNAL(editingFinished()), this, SLOT(seeriaLoetud()));
    connect(ui->seeria4Edit, SIGNAL(editingFinished()), this, SLOT(seeriaLoetud()));
    connect(ui->seeria5Edit, SIGNAL(editingFinished()), this, SLOT(seeriaLoetud()));
    connect(ui->seeria6Edit, SIGNAL(editingFinished()), this, SLOT(seeriaLoetud()));
    connect(ui->seeria7Edit, SIGNAL(editingFinished()), this, SLOT(seeriaLoetud()));
    connect(ui->seeria8Edit, SIGNAL(editingFinished()), this, SLOT(seeriaLoetud()));
    connect(ui->seeria9Edit, SIGNAL(editingFinished()), this, SLOT(seeriaLoetud()));
    connect(ui->seeria10Edit, SIGNAL(editingFinished()), this, SLOT(seeriaLoetud()));
    connect(ui->seeria11Edit, SIGNAL(editingFinished()), this, SLOT(seeriaLoetud()));
    connect(ui->seeria12Edit, SIGNAL(editingFinished()), this, SLOT(seeriaLoetud()));
    connect(ui->sifriEdit, SIGNAL(returnPressed()), this, SLOT(uuendaSifriga()));
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(fookusMuutus(QWidget*,QWidget*)));

    (void) new QShortcut(QKeySequence(QKeySequence::Save), this, SLOT(paluSalvestada()));

    seeriad << ui->seeria1Edit;
    seeriad << ui->seeria2Edit;
    seeriad << ui->seeria3Edit;
    seeriad << ui->seeria4Edit;
    seeriad << ui->seeria5Edit;
    seeriad << ui->seeria6Edit;
    seeriad << ui->seeria7Edit;
    seeriad << ui->seeria8Edit;
    seeriad << ui->seeria9Edit;
    seeriad << ui->seeria10Edit;
    seeriad << ui->seeria11Edit;
    seeriad << ui->seeria12Edit;

    for(int i = 6; i < seeriad.count(); i++)
        seeriad[i]->hide();
    aktiivneSeeria = nullptr;

    for(int j = 0; j < laskudeArv; j++){
        seriesShots << new Lask(/*this*/);  //Loetava seeria lasud
    }

    for(int i = 0; i < seeriad.count(); i++){
        QList<Lask*> seeriaLasud;
        for(int j = 0; j < 10; j++){
            seeriaLasud << new Lask(/*this*/);  //Laskuri seeria lasud
        }
        lasud << seeriaLasud;   //Seeria lasud laskuri laskude hulka
    }

    pilt = new QImage(600, 600, QImage::Format_ARGB32);
    painter = new QPainter(pilt);
    painter->translate(300, 300);

//    ui->silt->setBackgroundRole(QPalette::Base);
//    ui->silt->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
//    ui->silt->setScaledContents(true);

    startScoring();

//    serial = new QextSerialPort();
    broadcastiSaatja = new QTimer(this);
    broadcastiSaatja->setInterval(150);
    broadcastiSaatja->setSingleShot(true);
    connect(broadcastiSaatja, SIGNAL(timeout()), this, SLOT(saadaBroadcast()));
//    timer = new QTimer(this);
//    timer->setInterval(100);
//    timer->setSingleShot(false);
//    connect(timer, SIGNAL(timeout()), this, SLOT(loe()));
    otsija = new QTimer(this);
    otsija->setInterval(97);
    otsija->setSingleShot(false);
    connect(otsija, SIGNAL(timeout()), this, SLOT(otsiPorti()));
    fookus = new QTimer(this);
    fookus->setInterval(100);
    fookus->setSingleShot(true);
    connect(fookus, SIGNAL(timeout()), this, SLOT(fookusSifrile()));
    sulgeja = new QTimer(this);
    sulgeja->setInterval(50);
    sulgeja->setSingleShot(true);
    connect(sulgeja, SIGNAL(timeout()), this, SLOT(close()));
//    saatja = new QTimer(this);  //Viivis enne saatmist, et uus lugemismasin jõuaks reageerida
//    saatja->setInterval(20);
//    saatja->setSingleShot(true);
//    connect(saatja, SIGNAL(timeout()), this, SLOT(saada()));
    seadistaja = new QTimer(this);
    seadistaja->setInterval(1000);
    seadistaja->setSingleShot(true);
    connect(seadistaja, SIGNAL(timeout()), this, SLOT(seadista()));

    if(verbose)
        scoringMachCon.setLogLevel(ScoringMachineConnection::Verbose);

    connect(&scoringMachCon, &ScoringMachineConnection::shotRead, this, &Lehelugeja::readShot);
    connect(&scoringMachCon, &ScoringMachineConnection::connectionStatusChanged, this, &Lehelugeja::updateLog);
    connect(&scoringMachCon, &ScoringMachineConnection::dataSent, this, &Lehelugeja::updateLog);

    // Make sure things are clear
//    serial->close();

    uuendaPorte();

    ui->nimeBox->hide();
    ui->nimeSilt->hide();
    ui->comPort->hide();
    ui->otsiCOMNupp->hide();
    ui->seadistaNupp->hide();

    progress = new QProgressDialog(tr("Protokollitaja serveri otsimine..."), "Loobu", 0, 0, this);
    progress->setWindowModality(Qt::WindowModal);
    progress->show();

    showMaximized();
    broadcastiSaatja->start();
#ifdef PROOV
    QMessageBox::information(this, "Teade", "Debug versioon!", QMessageBox::Ok);
#endif
}

void Lehelugeja::restartScoring()
{
    lask = 0;
    seeria = 0;
    startScoring();
    seadistaja->start();
}

void Lehelugeja::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Lehelugeja::closeEvent(QCloseEvent *event)
{
    if(!voibSulgeda){
        saadaVorku(tr("Käsk:Vabastada"));
        sulgeja->start();
        voibSulgeda = true;
        event->ignore();
    }else{
        sulgeUhendus();
        socket->disconnectFromHost();
        event->accept();
    }
}

int Lehelugeja::CRC(QByteArray* s)
{
    int crc = 0;
    for(int i = 0; i < s->size(); i++)
        crc ^=s->at(i);
    return crc;
}

void Lehelugeja::fookusMuutus(QWidget *vana, QWidget *uus)
{
    Q_UNUSED(vana);
    Q_UNUSED(uus);

    for(int i = 0; i < seeriad.count(); i++)
        if(seeriad[i]->hasFocus()){
            if(aktiivneSeeria != nullptr)
                aktiivneSeeria->setStyleSheet(this->styleSheet());
            seeriad[i]->setStyleSheet("border: 3px solid black");
            aktiivneSeeria = seeriad[i];
            aktiivseSeeriaNr = i;
        }
}

//int Lehelugeja::eraldaLask(QString rida)
//{
    //QStringList read = tekst.split(';');    //Näiteks: SCH=1;7.2;2973.0;164.7;G#CR
//}

void Lehelugeja::fookusSifrile()
{
    ui->sifriEdit->setFocus();  //Kui lehed loetud, teeb sifri akna aktiivseks, et saaks uuesti sifrit sisestada
    ui->sifriEdit->selectAll();
}

void Lehelugeja::hakkaOtsima()
{
    // TODO to be implemented
    if(scoringMachCon.connected()){
        otsija->stop();
        QMessageBox::warning(this, "Protokollitaja", tr("Ühendus juba loodud! Kui on mingi jama ja tahate"
                "uuesti ühendada, tuleb enne vajutada \"Sulge ühendus\"."), QMessageBox::Ok);
        return;
    }

    uuendaPorte();
//    otsija->start();
}

void Lehelugeja::joonistaLask(QPointF p, bool kasMM)
{
//    int x = 0, y = 0;
//    if(lask == 0){
//        joonistaLeht();
//    }
//    painter->setBrush(Qt::red);
//    painter->setPen(Qt::black);
//    if(ui->leheCombo->currentIndex() == 0){ //Õhupüssi lehed
////        x = list.at(3).toFloat() * 100;
////        y = list.at(4).toFloat() * 100;
//        if(kasMM){  //Uue masina puhul 1 mm = 4 ühikut lehel
//            x = qRound(qreal(p.x() * 8));
//            y = qRound(qreal(p.y() * -8));  //Koordinaadid on peegelpildis
//        }else{  //Vana masina puhul on vaja teha järgnev tehe:
//            x = qRound(qreal(p.x() * 20 / 100));
//            y = qRound(qreal(p.y() * 20 / 100));
//        }
////        ui->logi->append(QString("x = %1, y = %2\n").arg(x).arg(y));
//        painter->drawEllipse(QPoint(x, y), 18, 18);
//    }else{  //Õhupüstoli leht
////        x = list.at(3).toFloat() * 100;
////        y = list.at(4).toFloat() * 100;
//        if(kasMM){  //Uue masina puhul 1 mm = 2 ühikut lehel
//            x = qRound(qreal(p.x() * 4));
//            y = qRound(qreal(p.y() * -4));  //Koordinaadid on peegelpildis
//        }else{  //Vana masina puhul on vaja teha järgnev tehe:
//            x = qRound(qreal(x * 32 / 100));
//            y = qRound(qreal(y * 32 / 100));
//        }
////        ui->logi->append(QString("x = %1, y = %2\n").arg(x).arg(y));
//        painter->drawEllipse(QPoint(x, y), 9, 9);
//    }
//    painter->setPen(Qt::black); //Seadistus lasu teksti joonistamiseks
//    QFont font;
//    font.setPointSize(16);
//    painter->setFont(font);

//    if(ui->kumnendikegaBox->isChecked()){  //Kui on kümnendikega lugemine, joonistada komakohaga arv,
////        il = qRound(fl * 10);
//        painter->drawText(260, -260 + 18 * lask, seeriaLasud[lask]->getSLask());
//    }else{  //kui täisarvudega, siis täisarv
////        il = fl;
////        il *= 10;
//        painter->drawText(260, -260 + 18 * lask, seeriaLasud[lask]->getSLask().left(seeriaLasud[lask]->getSLask().indexOf(",")));
//    }
//    ui->silt->setPixmap(QPixmap::fromImage(*pilt).scaled(ui->silt->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
//    this->update();
}

void Lehelugeja::drawTarget()
{
    target.init(ui->leheCombo->currentIndex());
    target.setZoomEnabled(false);
    target.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->centralGrid->replaceWidget(ui->placeHolderWidget, &target);

//    if(ui->leheCombo->currentIndex() == 0 || ui->leheCombo->currentIndex() == 2){   //Õhupüss ja sportpüss
//        QFont font;
//        font.setPointSize(12);
//        painter->setFont(font);
//        pilt->fill(0);
//        painter->setBrush(Qt::white);
//        painter->setPen(Qt::black);
//        //painter->rotate(180);
//        painter->drawEllipse(QPoint(0, 0), 182, 182);   //kordaja on 4 (1 mm = 4 punkti)
//        painter->drawEllipse(QPoint(0, 0), 162, 162);
//        painter->drawEllipse(QPoint(0, 0), 142, 142);
//        painter->setBrush(Qt::black);
//        painter->setPen(Qt::white);
//        painter->drawEllipse(QPoint(0, 0), 122, 122);
//        painter->drawEllipse(QPoint(0, 0), 102, 102);
//        painter->drawEllipse(QPoint(0, 0), 82, 82);
//        painter->drawEllipse(QPoint(0, 0), 62, 62);
//        painter->drawEllipse(QPoint(0, 0), 42, 42);
//        painter->drawEllipse(QPoint(0, 0), 22, 22);
//        painter->drawEllipse(QPoint(0, 0), 2, 2);
//        painter->drawText(-36, 5, "8");
//        painter->drawText(28, 5, "8");
//        painter->drawText(-56, 5, "7");
//        painter->drawText(48, 5, "7");
//        painter->drawText(-76, 5, "6");
//        painter->drawText(70, 5, "6");
//        painter->drawText(-97, 5, "5");
//        painter->drawText(88, 5, "5");
//        painter->drawText(-116, 5, "4");
//        painter->drawText(110, 5, "4");
//        painter->setPen(Qt::black);
//        painter->drawText(-136, 5, "3");
//        painter->drawText(129, 5, "3");
//        painter->drawText(-156, 5, "2");
//        painter->drawText(150, 5, "2");
//        painter->drawText(-176, 5, "1");
//        painter->drawText(169, 5, "1");
//        //painter->setBrush(Qt::red);
//        //painter->setPen(Qt::NoPen);
//        //painter->drawEllipse(QPoint(190, 162), 10, 10);
//        //painter->drawEllipse(QPoint(211, 247), 10, 10);
//    }else{  //Õhupüstol
//        QFont font;
//        font.setPointSize(12);
//        painter->setFont(font);
//        pilt->fill(0);
//        painter->setBrush(Qt::white);
//        painter->setPen(Qt::black);
//        //painter->rotate(180);
//        painter->drawEllipse(QPoint(0, 0), 311, 311);   //Kordaja on 2 (1 mm = 2 punkti)
//        painter->drawEllipse(QPoint(0, 0), 279, 279);
//        painter->drawEllipse(QPoint(0, 0), 247, 247);
//        painter->drawEllipse(QPoint(0, 0), 215, 215);
//        painter->drawEllipse(QPoint(0, 0), 183, 183);
//        painter->drawEllipse(QPoint(0, 0), 151, 151);
//        painter->setBrush(Qt::black);
//        painter->setPen(Qt::white);
//        painter->drawEllipse(QPoint(0, 0), 119, 119);
//        painter->drawEllipse(QPoint(0, 0), 87, 87);
//        painter->drawEllipse(QPoint(0, 0), 55, 55);
//        painter->drawEllipse(QPoint(0, 0), 23, 23);
//        painter->drawEllipse(QPoint(0, 0), 10, 10);
//        painter->drawText(-73, 5, "8");
//        painter->drawText(68, 5, "8");
//        painter->drawText(-105, 5, "7");
//        painter->drawText(100, 5, "7");
//        painter->setPen(Qt::black);
//        painter->drawText(-137, 5, "6");
//        painter->drawText(132, 5, "6");
//        painter->drawText(-169, 5, "5");
//        painter->drawText(164, 5, "5");
//        painter->drawText(-201, 5, "4");
//        painter->drawText(196, 5, "4");
//        painter->drawText(-233, 5, "3");
//        painter->drawText(228, 5, "3");
//        painter->drawText(-265, 5, "2");
//        painter->drawText(260, 5, "2");
//        painter->drawText(-297, 5, "1");
//        painter->drawText(292, 5, "1");
//    }
//    ui->silt->setPixmap(QPixmap::fromImage(*pilt).scaled(ui->silt->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void Lehelugeja::sumAndEndSeries()
{
//#ifdef PROOV    //For quick and dirty testing purposes
//    for(int i = 0; i < laskudeArv; i++){
//        seeriaLasud[i]->setX(qrand() % 100);
//        seeriaLasud[i]->setY(qrand() % 100);
//        seeriaLasud[i]->setLask(qrand() % 10);
//    }
//#endif
    float fl = 0;
    if(ui->kumnendikegaBox->isChecked()){  //Kui on kümnendikega lugemine, on vaja liita komakohaga lasu väärtus
        for(int i = 0; i < laskudeArv; i++){
            seeria += seriesShots[i]->get10Lask();
        }
        fl = seeria;    //Kasutame float muutujat seeria jagamiseks 10'ga, seeria enda jagamisel kaovad komakohad ära
        if(aktiivneSeeria != nullptr){
            aktiivneSeeria->setText(QString("%1").arg(double(fl / 10)));
            aktiivneSeeria->setText(aktiivneSeeria->text().replace('.', ','));
        }
    }else{  //Täisarvudega lugemise puhul on vaja liita täisarvuline lasu väärtus
        for(int i = 0; i < laskudeArv; i++){
            seeria += seriesShots[i]->getILask();
            fl = seeria * 10;   //Logisse mineva veateate jaoks
        }
        if(aktiivneSeeria != nullptr){
            aktiivneSeeria->setText(QString("%1").arg(seeria));
        }
    }

    if(aktiivneSeeria != nullptr){
        target.setResult(aktiivneSeeria->text());

        for(int i = 0; i < seriesShots.count(); i++)    //Loetud laskude kirjutamine aktiivsesse seeriasse
            lasud[aktiivseSeeriaNr][i]->set(seriesShots[i]);
        ui->logi->append(ui->nimeBox->currentText() + QString(" %1. seeria: %2").arg(aktiivneSeeria->objectName().at(6)).arg(double(fl / 10)));

        aktiivneSeeria->setFocus();
        QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
        QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyRelease, Qt::Key_Tab, Qt::NoModifier));
    } else {
        QMessageBox::critical(this, "Viga", "Puudus aktiivne seeria kast!", QMessageBox::Ok);
        ui->logi->append("Viga!:" + ui->nimeBox->currentText() + QString(" tundmatu seeria: %1").arg(double(fl / 10)));
    }

    fookus->start();

    seeria = 0;
    lask = 0;
    seadistaja->start();
}

void Lehelugeja::liigneLask()
{
//    QString s = "111111111";
//    if(ui->leheCombo->currentIndex() < 2)
//        s.replace(0, 1, QString("%1").arg(ui->leheCombo->currentIndex() + 1));
//    else s.replace(0, 1, "6");
//    s.replace(8, 1, QString("%1").arg(ui->laskudeBox->value() + 1));
////    if(ui->kumnendikegaBox->isChecked())   //Kas lugeda laske kümnendiku täpsusega või mitte
////        s.replace(3, 1, "2");
//    saada(s);
//    oliLiigneLask = true;   //Siis loe() funktsioon teab, et tuleb uuesti seadistada
}

void Lehelugeja::loe()
{
//    if(uusLugemismasin){    //Kui on uus masin, siis loetakse teise funktsiooniga
//        loe2();
//        return;
//    }
//    static bool esimeneKord = true; //Vajalik, et vana masinat proovitaks kaks korda seadistada. Esimesele seadistamisele ei pruugi see

//    if(serial->bytesAvailable()>0){
//        static QString buffer;
//        QString tekst;
//        //char buffer[256];
//        //int rec = serial->readLine(buffer, 255);
//        //buffer[rec]='\0';
//        buffer.append(serial->readAll());
//        if(buffer.contains(13)){
//            tekst = buffer.left(buffer.indexOf(13) + 1);
//            buffer.remove(0, buffer.indexOf(13) + 1);
//        }else return;

//        uhendatud = true;   //Kui saabus tekst, siis järelikult on ühendus loodud?
//        lugemismasinaValimine = false;  //Vana masin leitud, rohkem ei ole vaja valida
//        ui->logi->append(tr("vastuvõtt:"));

//        if(!tekst.contains("START") && !tekst.contains("SCHEIBE") && !tekst.contains("Keine") && tekst.contains(';'))
//            ui->logi->append(QString("     %1. lask").arg(lask + 1));
//        ui->logi->append(tekst);
//        /*if(tekst.startsWith('H', Qt::CaseInsensitive)){
//                saada("611101112");
//                return;
//        }*/
//        if(!tekst.contains("TART") && !tekst.contains("SCHEIBE") && !tekst.contains("Keine") && tekst.contains(';')){
//                QStringList list = tekst.split(';', QString::KeepEmptyParts);
//                if(list.size() < 5){
//                    ui->logi->append(tr("See rida oli liiga lühike!"));
//                    return;
//                }
//                int x = 0, y = 0, il = 0;
//                float fl = 0;
//                if(lask == 0){
//                    joonistaLeht();
//                }
//                ui->logi->append(QString("list.count() = %1").arg(list.count()));
//                //ui->logi->append(list.at(3));
//                //ui->logi->append(list.at(4));
//                if(ui->leheCombo->currentIndex() == 0){ //Õhupüssi lehed
//                    x = list.at(3).toFloat() * 100;
//                    y = list.at(4).toFloat() * 100;
//                    x = qRound(qreal(x * 20 / 100));
//                    y = qRound(qreal(y * 20 / 100));
//                    ui->logi->append(QString("x = %1, y = %2\n").arg(x).arg(y));
//                    painter->setBrush(Qt::red);
//                    painter->setPen(Qt::black);
//                    painter->drawEllipse(QPoint(x, y), 18, 18);
//                }else{  //Õhupüstoli leht
//                    x = list.at(3).toFloat() * 100;
//                    y = list.at(4).toFloat() * 100;
//                    x = qRound(qreal(x * 32 / 100));
//                    y = qRound(qreal(y * 32 / 100));
//                    ui->logi->append(QString("x = %1, y = %2\n").arg(x).arg(y));
//                    painter->setBrush(Qt::red);
//                    painter->setPen(Qt::black);
//                    painter->drawEllipse(QPoint(x, y), 9, 9);
//                }
//                /*bool onnestus = false;    //Neid ridu pole enam vaja
//                fl = list.at(1).toFloat(&onnestus);
//                if(!onnestus){
//                    //QMessageBox::critical(this, "Viga", "Ütle Laurile, et lugemismasinast tulnud punkt on vaja asendada komaga.", QMessageBox::Ok);
//                    QString rida = list.at(1);
//                    rida.replace('.', ',');
//                    fl = rida.toFloat(&onnestus);
//                }*/
//                seriesShots[lask]->setX(x);
//                seriesShots[lask]->setY(y);
//                seriesShots[lask]->setLask(list.at(1));

//                lask++;
//                painter->setPen(Qt::black);
//                QFont font;
//                font.setPointSize(16);
//                painter->setFont(font);

//                if(ui->kumnendikegaBox->isChecked()){  //Kui on kümnendikega lugemine, liita seeriale komakohaga arv,
//                    il = qRound(fl * 10);              //kui täisarvudega, siis liita täisarv
//                    painter->drawText(260, -295 + 18 * lask, list.at(1));
//                }else{
//                    il = fl;
//                    il *= 10;
//                    painter->drawText(260, -295 + 18 * lask, list.at(1).left(list.at(1).indexOf('.')));
//                }
//                seeria += il;

//                if(list.count() > 6){
//                    ui->logi->append(list.at(8));
//                    ui->logi->append(list.at(9));
//                    if(ui->leheCombo->currentIndex() == 0){ //Õhupüssi lehed
//                        x = list.at(8).toFloat() * 100;
//                        y = list.at(9).toFloat() * 100;
//                        x = qRound(qreal(x * 20 / 100));
//                        y = qRound(qreal(y * 20 / 100));
//                        ui->logi->append(QString("x = %1, y = %2\n").arg(x).arg(y));
//                        painter->setBrush(Qt::red);
//                        painter->setPen(Qt::black);
//                        painter->drawEllipse(QPoint(x, y), 18, 18);
//                    }else{  //Õhupüstoli leht
//                        x = list.at(8).toFloat() * 100;
//                        y = list.at(9).toFloat() * 100;
//                        x = qRound(qreal(x * 32 / 100));
//                        y = qRound(qreal(y * 32 / 100));
//                        ui->logi->append(QString("x = %1, y = %2\n").arg(x).arg(y));
//                        painter->setBrush(Qt::red);
//                        painter->setPen(Qt::black);
//                        painter->drawEllipse(QPoint(x, y), 9, 9);
//                    }
//                    /*onnestus = false; //Neid ridu ei ole enam vaja
//                    fl = list.at(6).toFloat();
//                    if(!onnestus){
//                        //QMessageBox::critical(this, "Viga", "Ütle Laurile, et lugemismasinast tulnud punkt on vaja asendada komaga.", QMessageBox::Ok);
//                        QString rida = list.at(6);
//                        rida.replace('.', ',');
//                        fl = rida.toFloat(&onnestus);
//                    }*/
//                    seriesShots[lask]->setX(x);
//                    seriesShots[lask]->setY(y);
//                    seriesShots[lask]->setLask(list.at(6));

//                    lask++;
//                    painter->setPen(Qt::black);

//                    if(ui->kumnendikegaBox->isChecked()){  //Kui on kümnendikega lugemine, liita seeriale komakohaga arv,
//                        il = qRound(fl * 10);              //kui täisarvudega, siis liita täisarv
//                        painter->drawText(260, -295 + 18 * lask, list.at(6));
//                    }else{
//                        il = fl;
//                        il *= 10;
//                        painter->drawText(260, -295 + 18 * lask, list.at(6).left(list.at(6).indexOf('.')));
//                    }
//                    seeria += il;
//                }
//                if(list.count() > 12){
//                    ui->logi->append(list.at(13));
//                    ui->logi->append(list.at(14));
//                    if(ui->leheCombo->currentIndex() == 0){ //Õhupüssi lehed
//                        x = list.at(13).toFloat() * 100;
//                        y = list.at(14).toFloat() * 100;
//                        x = qRound(qreal(x * 20 / 100));
//                        y = qRound(qreal(y * 20 / 100));
//                        ui->logi->append(QString("x = %1, y = %2\n").arg(x).arg(y));
//                        painter->setBrush(Qt::red);
//                        painter->setPen(Qt::black);
//                        painter->drawEllipse(QPoint(x, y), 18, 18);
//                    }else{  //Õhupüstoli leht
//                        x = list.at(13).toFloat() * 100;
//                        y = list.at(14).toFloat() * 100;
//                        x = qRound(qreal(x * 32 / 100));
//                        y = qRound(qreal(y * 32 / 100));
//                        ui->logi->append(QString("x = %1, y = %2\n").arg(x).arg(y));
//                        painter->setBrush(Qt::red);
//                        painter->setPen(Qt::black);
//                        painter->drawEllipse(QPoint(x, y), 9, 9);
//                    }
//                    /*onnestus = false; //Neid ridu ei ole enam vaja
//                    fl = list.at(11).toFloat();
//                    if(!onnestus){
//                        //QMessageBox::critical(this, "Viga", "Ütle Laurile, et lugemismasinast tulnud punkt on vaja asendada komaga.", QMessageBox::Ok);
//                        QString rida = list.at(11);
//                        rida.replace('.', ',');
//                        fl = rida.toFloat(&onnestus);
//                    }*/
//                    seriesShots[lask]->setX(x);
//                    seriesShots[lask]->setY(y);
//                    seriesShots[lask]->setLask(list.at(11));

//                    lask++;
//                    painter->setPen(Qt::black);

//                    if(ui->kumnendikegaBox->isChecked()){  //Kui on kümnendikega lugemine, liita seeriale komakohaga arv,
//                        il = qRound(fl * 10);              //kui täisarvudega, siis liita täisarv
//                        painter->drawText(260, -295 + 18 * lask, list.at(11));
//                    }else{
//                        il = fl;
//                        il *= 10;
//                        painter->drawText(260, -295 + 18 * lask, list.at(11).left(list.at(11).indexOf('.')));
//                    }
//                    seeria += il;
//                }
//                if(list.contains(";M", Qt::CaseSensitive))
//                    oliKahtlane = true;
////                ui->silt->setPixmap(QPixmap::fromImage(*pilt).scaled(ui->silt->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
//                this->update();
//                if(lask == laskudeArv /*10*/){ //Seeria sai täis, vaja kirjutada väärtus märgitud lahtrisse
//                    if(aktiivneSeeria != 0){
//                        fl = seeria;    //Kasutame lasu float muutujat seeria jagamiseks 10'ga, seeria enda jagamisel kaovad komakohad ära
//                        //ui->logi->append(QString("Seeria: %1, fl: %2, fl/10: %3").arg(seeria).arg(fl).arg(fl/10));
//                        aktiivneSeeria->setText(QString("%1").arg(fl / 10));
//                        aktiivneSeeria->setText(aktiivneSeeria->text().replace('.', ','));
//                        for(int i = 0; i < seriesShots.count(); i++)    //Loetud laskude kirjutamine aktiivsesse seeriasse
//                            lasud[aktiivseSeeriaNr][i]->set(seriesShots[i]);
//                        ui->logi->append(ui->nimeBox->currentText() + QString(" %1. seeria: %2").arg(aktiivneSeeria->objectName().at(6)).arg(fl / 10));
//                    }else{
//                        QMessageBox::critical(this, "Viga", "Puudus aktiivne seeria kast!", QMessageBox::Ok);
//                        ui->logi->append("Viga!:" + ui->nimeBox->currentText() + QString(" tundmatu seeria: %1").arg(fl / 10));
//                    }
//                    /*for(int k = 0; k < seeriad.count(); k++)
//                        if(seeriad[k]->hasFocus()){
//                            fl = seeria;    //Kasutame lasu float muutujat seeria jagamiseks 10'ga, seeria enda jagamisel kaovad komakohad ära
//                            //ui->logi->append(QString("Seeria: %1, fl: %2, fl/10: %3").arg(seeria).arg(fl).arg(fl/10));
//                            seeriad[k]->setText(QString("%1").arg(fl / 10));
//                            seeriad[k]->setText(seeriad[k]->text().replace('.', ','));
//                            ui->logi->append(ui->nimeBox->currentText() + QString(" %1. seeria: %2").arg(k + 1).arg(fl / 10));
//                        }*/
////                    if(!ui->sifriEdit->text().isEmpty() && ui->sifriEdit->text() != ui->sifriLabel->text().left(ui->sifriEdit->text().length())){   //Kui on
////                        QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier)); //juba uus siffer valmis sisestatud, võib kohe
////                        QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyRelease, Qt::Key_Return, Qt::NoModifier));   //automaatselt seadistada uue sifri
////                    }else{
//                    aktiivneSeeria->setFocus();
//                    QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
//                    QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyRelease, Qt::Key_Tab, Qt::NoModifier));
//                    fookus->start();
////                    }
//                    seeria = 0;
//                    lask = 0;
//                    seadistaja->start();
//                }
//                if(oliKahtlane){ //Kui on vaja kontrollida, viskab teate
//                    QMessageBox::warning(this, "Teade", tr("Lehes olid kahtlased lasud, vaja kontrollida ning "
//                            "seejärel kaks korda Neustart vajutada. Alles siis vajutage siin \"OK\""), QMessageBox::Ok);
//                    oliKahtlane = false;
//                }
//                return;
//        }else{
//            if(vahemuses && tekst.contains("SCHEIBE")){  //Seadistab tagasi normaalseks
//                /*QString s = "111111111";
//                if(ui->leheCombo->currentIndex() < 2)  //Relva tüübi kindlaks tegemine
//                    s.replace(0, 1, QString("%1").arg(ui->leheCombo->currentIndex() + 1));
//                else s.replace(0, 1, "6");
//                s.replace(8, 1, QString("%1").arg(ui->laskudeBox->value()));
//                saada(s);*/
//                seadistaja->start();
//                vahemuses = false;
//            }else if(oliLiigneLask && tekst.contains("SCHEIBE")){  //Seadistab järgmiseks vähema laskude arvu peale
//                                                                   //või tagasi normaalseks
//                QString s = "111111111";
//                if(ui->leheCombo->currentIndex() < 2)  //Relva tüübi kindlaks tegemine
//                    s.replace(0, 1, QString("%1").arg(ui->leheCombo->currentIndex() + 1));
//                else s.replace(0, 1, "6");

//                //Kui oli ainult 1 lask lehte, siis järgmine leht on nagunii tühi ja nii võib tagasi normaalseks seadistada
//                if(ui->laskudeBox->value() == 1)
//                    s.replace(8, 1, QString("%1").arg(ui->laskudeBox->value()));
//                //Kui oli 2 lasku lehte, on järgmisse vaja 1
//                else{
//                    s.replace(8, 1, QString("%1").arg(ui->laskudeBox->value() - 1));
//                    vahemuses = true;
//                }
//                if(ui->kumnendikegaBox->isChecked())   //Kas lugeda laske kümnendiku täpsusega või mitte
//                    s.replace(3, 1, "2");
//                saada(s);
//                oliLiigneLask = false;
//            }
//        }
//    }else if(lugemismasinaValimine){
//        if(!esimeneKord) //Vana masin ei pruugi esimene kord vastata, tasub teist korda veel proovida
//            uusLugemismasin = true; //Kui teksti vastu ei tulnud, võib arvata, et tegu on uue lugemismasinaga
//        esimeneKord = false;
//        uhenda();
//    }
}

void Lehelugeja::loe2()
{
//    if(serial->bytesAvailable()>0){
//        static QString buffer;
//        QString tekst;
//        //char buffer[256];
//        //int rec = serial->readLine(buffer, 255);
//        //buffer[rec]='\0';
//        buffer.append(serial->readAll());

//#ifdef PROOV
//            qDebug() << QString("Midagi saabus: %1").arg(buffer);
//#endif
//        if(buffer.contains(CR)){
//            tekst = buffer.left(buffer.indexOf(CR) + 1);
//            tekst.replace(STX, "");
//#ifdef PROOV
//            qDebug() << QString("saabus: %1").arg(tekst.replace(CR, "CR"));
//#endif
//            if(/*saatmiseEtapp == 0 &&*/ tekst.contains("SCH=")){ //Kui ei ole saatmine pooleli ja saabub mingi tekst, siis lugemismasin saadab midagi
//                QStringList read = tekst.split(';');    //SCH=1;7.2;2973.0;164.7;G#CR
//                if(read.count() >= 4){
//                    bool onnestus = false;
//                    QLineF koordinaadid = QLineF::fromPolar(read.at(2).left(read.at(2).indexOf('.')).toInt(), read.at(3).toDouble(&onnestus) - 90);   //QLineF'i 0 on kella 3'e suunas, Disag'il otse üles
//                    if(!onnestus)   //Kui punktiga ei õnnestu double'iks teha, peab olema koma
//                        koordinaadid = QLineF::fromPolar(read.at(2).toInt(), read.value(3).replace(".", ",").toDouble(&onnestus) - 90);
//                    if(!onnestus)   //Kui ikka ei õnnestunud, siis on mingi jama
//                        ui->logi->append(tr("Rida vigane, ei õnnestu koordinaate lugeda!"));

//                    seriesShots[lask]->setX(koordinaadid.p2().x() / 100);   //Lasu koordinaadid kõigepealt siin lisada
//                    seriesShots[lask]->setY(koordinaadid.p2().y() / 100);
//                    seriesShots[lask]->setLask(read.at(1));  //Seejärel lasu väärtus
//#ifdef PROOV
//            qDebug() << "JoonistaLask:" << seriesShots[lask]->X() << ", " << seriesShots[lask]->Y();
//#endif
//                    joonistaLask(koordinaadid.p2() / 100, true);    //Ühikuks mm
//                    lask++;
//                    if(lask == laskudeArv)  //Kui seeria on täis, on vaja see kokku liita ja ära saata
//                        sumAndEndSeries();
//                }else
//                    ui->logi->append(tr("Rida liiga lühike!"));
//                saatmiseEtapp = 4;  //Et saadetaks vastuseks kinnitus
//                saada("");  //Saadab ACK'i;
//                seadistaja->setInterval(600);   //Siin võib kiiremini seadistada
//                seadistaja->start();    //Vaja seadistada, et järgmist lehte vastu võetaks
//            }/*else if(tekst.contains("WSC")){
//                seadistaja->setInterval(300);   //Siin võib kiiremini seadistada
//                seadistaja->start();    //Vaja seadistada, et järgmist lehte vastu võetaks
//            }*/
////            saatmiseEtapp = 4;  //Et saadetaks vastuseks kinnitus
////            saada("");  //Saadab ACK'i;

////            bytes = rida.toLatin1();
//            buffer.remove(0, buffer.indexOf(CR) + 1);
//        }else if(buffer.contains(STX)){
//            tekst = buffer.left(buffer.indexOf(STX) + 1);
//            tekst.replace(STX, "STX");
////            ui->logi->append("Vastuvõtt: STX");
//            if(saatmiseEtapp == 1){
//#ifdef PROOV
//        qDebug() << "saabus: STX\n";
//#endif
//                saatmiseEtapp = 2;  //STX saabunud
//#ifdef PROOV
//        qDebug() << "saatmiseEtapp = 2";
//#endif
//                saada("");  //Kui vastati STX, siis ilmselt on masin valmis teksti vastuvõtmiseks
////            bytes = rida.toLatin1();
//            }
//            buffer.remove(0, buffer.indexOf(STX) + 1);
//        }else if(buffer.contains(NAK)){
//            tekst = buffer.left(buffer.indexOf(NAK) + 1);
//            tekst.replace(NAK, "NAK");
//            if(saatmiseEtapp == 3){
//#ifdef PROOV
//        qDebug() << "saabus: NAK\n";
//#endif
//                saatmiseEtapp = 0;  //NAK saabunud ja midagi tuksis -> algseis
//#ifdef PROOV
//        qDebug() << "saatmiseEtapp = 0";
//#endif
////            bytes = rida.toLatin1();
//            }
//            buffer.remove(0, buffer.indexOf(NAK) + 1);
//        }else if(buffer.contains(ACK)){
//            tekst = buffer.left(buffer.indexOf(ACK) + 1);
//            tekst.replace(ACK, "ACK");
////            ui->logi->append("Vastuvõtt: ACK");
////            bytes = rida.toLatin1();
//            if(saatmiseEtapp == 3){
//#ifdef PROOV
//        qDebug() << "saabus: ACK";
//#endif
//                saatmiseEtapp = 0;  //ACK saabunud -> algseis
//#ifdef PROOV
//        qDebug() << "saatmiseEtapp = 0";
//#endif
////                saada("");  //Kui vastati ACK, siis on masin teksti vastu võtnud
////            bytes = rida.toLatin1();
//            }
//            buffer.remove(0, buffer.indexOf(ACK) + 1);
//        }else{
//#ifdef PROOV
//        qDebug() << "saabus: midagi muud";
//#endif
//            ui->logi->append(tr("Vastuvõtt: %1").arg(buffer));
//            return;
//        }
//        ui->logi->append(tr("Vastuvõtt: %1").arg(tekst));
//    }
}

void Lehelugeja::loeBroadcast()
{
#ifdef PROOV
    qDebug() << "Loe broadcast:";
#endif

    if(socket->state() == QTcpSocket::ConnectedState)   //If already connected, no need to read broadcasts
        return;

    while (udpSocket2->hasPendingDatagrams()){
        QByteArray datagram;
        QHostAddress protoAadress;
        datagram.resize(int(udpSocket2->pendingDatagramSize()));
        udpSocket2->readDatagram(datagram.data(), datagram.size(), &protoAadress);
        if(verbose)
            QTextStream(stdout) << "Saabus broadcast: " << datagram.data();

//        statusLabel->setText(tr("Received datagram: \"%1\"").arg(datagram.data()));
//        QMessageBox::information(this, tr("Teade"), tr("Saabus broadcastitud datagram: \"%1\"").arg(datagram.data()), QMessageBox::Ok);
        QString rida(datagram);
        if(rida.startsWith("Protok:")){
            broadcastSaabunud = true;
//            datagram = "Protok:";
            rida.remove(0, 7);
#ifdef PROOV
            qDebug() << "Broadcasti rida: " << rida;
#endif
            QStringList ipAadressid = rida.split(";", QString::SkipEmptyParts);
            //Mõnes arvutis on mitu IPv4 aadressi, saata tuleb kõik, mis ei alga 255 või 127'ga
            socket->abort();

            socket->connectToHost(protoAadress, 50005); //Kõigepealt proovida broadcasti saatja aadressi
            if(socket->waitForConnected(500)){
                aadress = protoAadress.toString();    //Serveri IP uuendamine
                progress->accept();
                return; //Ühendus olemas, võib funktsioonist väljuda, vastasel juhul proovitakse nimekirjast järgmist aadressi
            }

            for(int i = 0; i < ipAadressid.count(); ++i){   // Then try address not starting with 192.to prefer direct cable connections
                if(!ipAadressid.at(i).startsWith("192.")){
                    socket->connectToHost(ipAadressid.at(i), 50005);
                    if(socket->waitForConnected(500)){
                        aadress = ipAadressid.at(i);    //Serveri IP uuendamine
                        progress->accept();
                        return; //Ühendus olemas, võib funktsioonist väljuda, vastasel juhul proovitakse nimekirjast järgmist aadressi
                    }
                }
            }

            //Kui eelmine tulemust ei andnud, siis proovida kõiki aadresse
            for(int i = 0; i < ipAadressid.count(); ++i){
                if(progress->wasCanceled()) return;  //Ühenduse loomise katkestamine
                if(verbose)
                    QTextStream(stdout) << QString("ipAadressid.at(%1) = %2").arg(i).arg(ipAadressid.at(i));
                socket->connectToHost(ipAadressid.at(i), 50005);
                if(socket->waitForConnected(500)){
                    aadress = ipAadressid.at(i);    //Serveri IP uuendamine
                    progress->accept();
//                    uhenduServeriga(aadress);   //Serveriga ühendumine (ei ole vaja, kuna ühendus on juba loodud ju)
                    return; //Ühendus olemas, võib funktsioonist väljuda, vastasel juhul proovitakse nimekirjast järgmist aadressi
                }
            }
//            datagram = "Protok:" + QByteArray()
//            udpSocket->writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, 45743);
        }
    }
}

void Lehelugeja::loeVorgust()   //Protokollitajast tulev info/käsk
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_8);

    if (blockSize == 0){
        if (socket->bytesAvailable() < qint64(sizeof(quint16))){
            return; //Ei ole paketi suurust tähistav quint16 veel kohale jõudnud
        }
        in >> blockSize;
    }

    if (socket->bytesAvailable() < blockSize){
        return; //Ei ole kogu pakett veel kohale jõudnud
    }

    QString sisse;
    sisse.clear();
    in >> sisse;

    if(sisse.startsWith("Viga:")){    //Saadeti veateade
        sisse.remove(0, 5);
        QMessageBox::critical(this, "Viga!", sisse, QMessageBox::Ok);
        return;
    }else if(sisse.startsWith("Hoiatus:")){ //Saadeti hoiatuse teade
        sisse.remove(0, 8);
        QMessageBox::critical(this, "Protolehelugeja", sisse, QMessageBox::Ok);
        return;
    }
    if(uhendusAutoriseeritud){  //Kui on autoriseeritud ühendus, saadetakse ilmselt muud infot
        blockSize = 0;
    }else{  //Kui on autoriseerimata ühendus, peab saabuma kinnitus "OK", et parool oli õige
//        sisse.clear();
//        in >> sisse;
        blockSize = 0;
        if(sisse == "OK"){
            saadaVorku("Versioon:3");
        }else if(sisse == "Versioon OK"){
            uhendusAutoriseeritud = true;
            saadaVorku("Tere");
        }
        return;
    }
    if(sisse.startsWith("Laskur:")){    //Saadeti laskur, kellele tulemusi lugema hakata
        //"Laskur:siffer - siffer;Eesnimi;Perekonnanimi;seeriate arv;laskude arv;seeriad;selle seeria lasud; x; y; summa;
        //aktiivne seeria;harjutus;lasku lehes;kümnendikega lugemine (true/false)
        if(verbose)
            QTextStream(stdout) << "Protolehelugeja::loeVorgust(): sisse = " << sisse << endl;
        sisse.remove(0, 7);

        ui->nimeBox->clear();
        for(int k = 0; k < seeriad.count(); k++){
            seeriad[k]->show();
            seeriad[k]->clear();
            seeriad[k]->setEnabled(true);
            for(int j = 0; j < 10; j++){
                lasud[k][j]->clear();    //Lasud puhtaks
            }
        }

        ui->summaEdit->setEnabled(true);
        restartScoring();

        QStringList pakett = sisse.split(";");
        ui->sifriLabel->setText(pakett.takeFirst());
        ui->nimeBox->addItem(QString("%1 %2").arg(pakett.takeFirst()).arg(pakett.takeFirst()));
        seeriateArv = pakett.takeFirst().toInt();
        /*laskudeArv = */pakett.takeFirst().toInt();    //Ei ole veel kasutusel, on alati 10
        for(int i = 0; i < seeriateArv; i++){
            seeriad[i]->setText(pakett.takeFirst());
            for(int j = 0; j < laskudeArv; j++){
                lasud[i][j]->setLask(pakett.takeFirst());
                lasud[i][j]->setX(pakett.takeFirst());
                lasud[i][j]->setY(pakett.takeFirst());
            }
        }
        ui->summaEdit->setText(pakett.takeFirst());
        int aktiivneSeeria = pakett.takeFirst().toInt();
        if(pakett.at(0).contains(tr("Õhupüss")))
            ui->leheCombo->setCurrentIndex(0);
        else if(pakett.at(0).contains(tr("Õhupüstol")))
        ui->leheCombo->setCurrentIndex(1);
        else if(pakett.at(0).contains("Standard", Qt::CaseInsensitive)
                || pakett.at(0).contains("Lamades", Qt::CaseInsensitive))
        ui->leheCombo->setCurrentIndex(2);
        pakett.takeFirst();
        ui->laskudeBox->setValue(pakett.takeFirst().toInt());
        if(pakett.takeFirst() == "true")
            ui->kumnendikegaBox->setChecked(true);
        else
            ui->kumnendikegaBox->setChecked(false);

//        ui->sifriLabel->setText(pakett.at(0));
//        ui->nimeBox->addItem(QString("%1 %2").arg(pakett.at(1)).arg(pakett.at(2)));
//        seeriateArv = pakett.at(3).toInt();
////        laskudeArv = pakett.at(4).toInt();    Ei ole veel kasutusel, on alati 10
//        for(int i = 0; i < seeriateArv; i++){
//            seeriad[i]->setText(pakett.at(5 + i + i*3*laskudeArv));
//            for(int j = 0; j < laskudeArv; j++){
//                lasud[i][j]->setLask(pakett.at(6 + i + (i+1)*3*j));
//                lasud[i][j]->setX(pakett.at(7 + i + (i+1)*3*j).toInt());
//                lasud[i][j]->setY(pakett.at(8 + i + (i+1)*3*j).toInt());
//            }
//        }
//        ui->summaEdit->setText(pakett.at(5 + seeriateArv + seeriateArv*3*laskudeArv));
//        int aktiivneSeeria = pakett.at(6 + seeriateArv + seeriateArv*3*laskudeArv).toInt();
//        if(pakett.at(7 + seeriateArv + seeriateArv*3*laskudeArv).contains(tr("Õhupüss")))
//            ui->leheCombo->setCurrentIndex(0);
//        else if(pakett.at(7 + seeriateArv + seeriateArv*3*laskudeArv).contains(tr("Õhupüstol")))
//        ui->leheCombo->setCurrentIndex(1);
//        else if(pakett.at(7 + seeriateArv + seeriateArv*3*laskudeArv).contains("Standard", Qt::CaseInsensitive)
//                || pakett.at(7 + seeriateArv + seeriateArv*3*laskudeArv).contains("Lamades", Qt::CaseInsensitive))
//        ui->leheCombo->setCurrentIndex(2);
//        ui->laskudeBox->setValue(pakett.at(8 + seeriateArv + seeriateArv*3*laskudeArv).toInt());
//        if(pakett.at(9 + seeriateArv + seeriateArv*3*laskudeArv) == "true")
//            ui->kumnendikegaBox->setChecked(true);
//        else
//            ui->kumnendikegaBox->setChecked(false);

        for(int j = seeriateArv; j < seeriad.count(); j++)
            seeriad[j]->hide();
//        drawTarget();
//        seadista();
        seeriad[aktiivneSeeria]->selectAll();
        seeriad[aktiivneSeeria]->setFocus();
    }else if(sisse.startsWith("Summa:")){    //Saadeti praeguse laskuri summa
        sisse.remove(0, 6);
        ui->summaEdit->setText(sisse);
    }
}

void Lehelugeja::naitaInfot()
{
    QMessageBox::about(this, programmiNimi, "Versioon: " + versioon + "\n\n" + programmiNimi + " on "
                    "vabavaraline programm, mis on loodud kasutamiseks laskevõistlustel koos Protokollitajaga "
                    "lehtede lugemiseks. Kasutamine omal vastutusel. Autor ei võta endale mingit vastutust "
                    "kahjude eest, mis programmi kasutamisega võivad kaasneda! Tegu on beta versiooniga ehk siis "
                    "alles katsetamisjärgus oleva programmiga.\n\nMuidu kasutage, seda nii kuidas ise heaks "
                    "arvate. Välja arvatud programmi looja maine ning heaolu kahjustamise või rahalise kasu "
                    "saamise eesmärkidel. Samuti pole kasutamine lubatud juhul, kui autor on selle ära keelanud!"
                    "\n\nKüsimused, ettepanekud, leitud vead, arvamused jms võib saata allolevale e-postile\n\n"
                    "Autor: Ümeramees\nymeramees@gmail.com\nTallinn " + aasta);
}

void Lehelugeja::naitaViga(QAbstractSocket::SocketError viga)
{
    switch (viga) {
        case QAbstractSocket::RemoteHostClosedError:
            QMessageBox::information(this, tr("Protolehelugeja"), tr("Ühendus katkestatud! Proovige uuesti ühenduda."));
            break;
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::information(this, tr("Protolehelugeja"), tr("Serverit ei leitud, palun kontrollige aardessi ja porti!"));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::information(this, tr("Protolehelugeja"), tr("Server keeldus ühendusest! Kontrollige, "
                    "et server töötaks ja et aadress ja port õiged oleksid"));
            break;
        default:
            QMessageBox::information(this, tr("Protolehelugeja"), tr("Viga: %1.").arg(socket->errorString()));
        }
}

void Lehelugeja::otsiPorti()
{
    // TODO to be implemented
    //ui->logi->clear();
    if(scoringMachCon.connected()){
        otsija->stop();
        QMessageBox::warning(this, "Protokollitaja", tr("Ühendus juba loodud! Kui on mingi jama ja tahate uuesti "
                             "ühendada, tuleb enne vajutada \"Sulge ühendus\"."), QMessageBox::Ok);
        return;
    }
//    static int i = 0;
//    loe();
//    serial->close();
//    if(ui->logi->toPlainText().split(QRegExp("\\n")).last().contains("H")){
//        otsija->stop();
//        uhenda();
//        i = 0;
//        return;
//    }
//    otsija->stop();
//    if(i >= ui->comPort->count()){
//        i = 0;
//        return;
//    }
//    ui->comPort->setCurrentIndex(i);
//    serial->setPortName(ui->comPort->currentText());
//    serial->setBaudRate(BAUD2400);
//    serial->setDataBits(DATA_8 /*(DataBitsType)(3)*/ );
//    serial->setParity(PAR_NONE /*(ParityType)(0)*/ );
//    serial->setStopBits(STOP_1 /*(StopBitsType)(0)*/ );
//    serial->setFlowControl(FLOW_OFF /*(FlowType)(0)*/ );
//    serial->setTimeout(100);
//    serial->open(QIODevice::ReadWrite);

//    serial->setDtr(true);
//    serial->setRts(true);

//    saada(QString("%1. korda: %2").arg(++i).arg(ui->comPort->currentText()));
//    serial->flush();
//    otsija->start();
}

void Lehelugeja::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

//    if(!pilt->isNull() && ui->silt->size() != markleheSuurus){
//        ui->silt->setPixmap(QPixmap::fromImage(*pilt).scaled(ui->silt->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
//        markleheSuurus = ui->silt->size();
//    }
}

void Lehelugeja::paluSalvestada()
{
    saadaVorku(tr("Käsk:Salvestada"));
}

void Lehelugeja::peidaNimi()
{
    if(ui->nimeBox->isVisible()){
        ui->nimeBox->hide();
        ui->nimeSilt->hide();
        ui->peidaNupp->setText(">>");
    }else{
        ui->nimeBox->show();
        ui->nimeSilt->show();
        ui->peidaNupp->setText("<<");
    }
}

void Lehelugeja::peidaNupud()
{
    if(ui->comPort->isVisible()){
        ui->comPort->hide();
        ui->otsiCOMNupp->hide();
        ui->seadistaNupp->hide();
        ui->peida2Nupp->setText(">>");
    }else{
        ui->comPort->show();
        ui->otsiCOMNupp->show();
        ui->seadistaNupp->show();
        ui->peida2Nupp->setText("<<");
    }
}

void Lehelugeja::readShot(Lask shot)
{
    if(lask == 0) {
        startScoring();
    }

    target.drawAShot(shot);
    seriesShots[lask]->set(&shot);

    if(ui->kumnendikegaBox->isChecked()) {
        ui->silt->setText(ui->silt->text() + "\n" + shot.getSLask());
    }else {
        ui->silt->setText(ui->silt->text() + "\n" + shot.getSLask()
                          .left(shot.getSLask().indexOf(",")));
    }

    if(shot.isInnerTen())
        ui->silt->setText(ui->silt->text() + "*");

    lask++;
    if(lask == laskudeArv) {
        sumAndEndSeries();
    }
}

void Lehelugeja::saada()
{
//    switch (saatmiseEtapp) {    //Olenevalt millises etapis parasjagu ollakse (vana masina puhul on see alati 2)
//    case 0:
//        serial->write(&enq);
//#ifdef PROOV
//        qDebug() << "saadan: ENQ";
//#endif
//        ui->logi->append("<font color=blue>ENQ</font>");
//        serial->flush();
//        saatmiseEtapp = 1;  //ENQ saadetud
//#ifdef PROOV
//        qDebug() << "saatmiseEtapp = 1";
//#endif
//        return;
//    case 2:
//        if(uusLugemismasin)
//            saadetis.append(char(CRC(&saadetis)));
//        saadetis.append(CR);
//        //serial->writeData(saadetis, saadetis.size());
//        serial->write(saadetis);
//#ifdef PROOV
//        qDebug() << QString("saadan: %1").arg(QString(saadetis));
//#endif
//        ui->logi->append("<font color=blue>"+saadetis+"</font>");
//        serial->flush();
//        if(uusLugemismasin){
//            saatmiseEtapp = 3;  //tekst saadetud
//#ifdef PROOV
//        qDebug() << "saatmiseEtapp = 3";
//#endif
//        }
//        break;
//    case 4:
//        serial->write(&ack);
//#ifdef PROOV
//        qDebug() << "saadan: ACK";
//#endif
//        ui->logi->append("<font color=blue>ACK</font>");
//        serial->flush();
//        saatmiseEtapp = 0;  //ACK saadetud
//#ifdef PROOV
//        qDebug() << "saatmiseEtapp = 0";
//#endif
//        return;
//    default:
//        break;
//    }//0 - algseis/ACK tulnud (lõpp), 1 - ENQ saadetud, 2 - STX saabunud, 3 - tekst saadetud, 4 - saabus tekst, vaja vastata ACK
}

void Lehelugeja::saada(QString s)
{
    scoringMachCon.sendToMachine(s);

//    if(!s.isEmpty())    //Kui s on tühi, siis ilmselt on tegu saatmise teise? etapiga
//        saadetis = s.toLatin1();
//    saatja->start();

    /*switch (saatmiseEtapp) {    //Olenevalt millises etapis parasjagu ollakse (vana masina puhul on see alati 2)
    case 0:
        serial->write(&enq);
#ifdef PROOV
        qDebug() << "saadan: ENQ";
#endif
        ui->logi->append("<font color=blue>ENQ</font>");
        serial->flush();
        saatmiseEtapp = 1;  //ENQ saadetud
#ifdef PROOV
        qDebug() << "saatmiseEtapp = 1";
#endif
        return;
        break;
    case 2:
        if(uusLugemismasin)
            saadetis.append(CRC(&saadetis));
        saadetis.append(CR);
        //serial->writeData(saadetis, saadetis.size());
        serial->write(saadetis);
#ifdef PROOV
        qDebug() << QString("saadan: %1").arg(QString(saadetis));
#endif
        ui->logi->append("<font color=blue>"+saadetis+"</font>");
        serial->flush();
        if(uusLugemismasin){
            saatmiseEtapp = 3;  //tekst saadetud
#ifdef PROOV
        qDebug() << "saatmiseEtapp = 3";
#endif
        }
        break;
    case 4:
        serial->write(&ack);
#ifdef PROOV
        qDebug() << "saadan: ACK";
#endif
        ui->logi->append("<font color=blue>ACK</font>");
        serial->flush();
        saatmiseEtapp = 0;  //ACK saadetud
#ifdef PROOV
        qDebug() << "saatmiseEtapp = 0";
#endif
        return;
        break;
    default:
        break;
    }*///0 - algseis/ACK tulnud (lõpp), 1 - ENQ saadetud, 2 - STX saabunud, 3 - tekst saadetud, 4 - saabus tekst, vaja vastata ACK
}

void Lehelugeja::saadaBroadcast()
{
    static bool broadcastSaadetud = false;  //Et broadcast saadetaks ainult üks kord
    if(!broadcastSaadetud){
        statusBar()->showMessage(tr("IP küsimus saadetud"), 2000);
        QByteArray datagram = "Protok?";
//        udpSocket->connectToHost(QHostAddress::Broadcast, 45743);
        udpSocket->writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, 45743);
//        udpSocket->waitForBytesWritten();
        udpSocket->disconnectFromHost();
        broadcastSaadetud = true;
        broadcastiSaatja->setInterval(10000);
        broadcastiSaatja->start();
    }else if(!broadcastSaabunud){
        progress->close();
        QMessageBox::warning(this, "Protolehelugeja", tr("Protokollitaja serverit ei leitud!\nKontrollige võrguühenduse olemasolu."), QMessageBox::Ok);
    }
}

void Lehelugeja::saadaParool()
{
    progress->accept(); //Otsimise progressiaken kinni
    parool = QInputDialog::getInt(this, "Sisestage parool", "Parool:");
//    saadaVorku(QString("%1").arg(parool));
    QByteArray block;
    QDataStream valja(&block, QIODevice::WriteOnly);
    valja.setVersion(QDataStream::Qt_4_8);
    valja << quint16(0);
//    valja << "Parool:";
    valja << parool;
    valja.device()->seek(0);
    valja << quint16(ulong(block.size()) - sizeof(quint16));
    if(verbose)
        QTextStream(stdout) << QString("saadaVorku(): %1").arg(parool) << endl;
    socket->write(block);
    block.clear();
}

void Lehelugeja::saadaTekst()
{
    if(ui->tekstiEdit->text().isEmpty())
        return;
    else if(ui->tekstiEdit->text().contains("uuenda porte", Qt::CaseInsensitive)){
        ui->logi->append("Uuenda porte:");
        uuendaPorte();
    }else if(ui->tekstiEdit->text().left(3) == "COM" && ui->tekstiEdit->text().length() <= 5 && ui->tekstiEdit->text().length() >= 4){
        ui->logi->append("Lisatud: " + ui->tekstiEdit->text());
        ui->comPort->addItem(ui->tekstiEdit->text());
        ui->comPort->setCurrentIndex(ui->comPort->count() - 1);
    }else if(ui->tekstiEdit->text() == "uusLugemismasin=true"){
        ui->logi->append(tr("Kasutusel uue lugemismasinaga ühendus"));
//        uusLugemismasin = true;
        scoringMachCon.setScoringMachineType(ScoringMachineConnection::RMIV);
//        saatmiseEtapp = 0;
    }else if(ui->tekstiEdit->text() == "uusLugemismasin=false"){
        ui->logi->append(tr("Kasutusel vana lugemismasinaga ühendus"));
//        uusLugemismasin = false;
        scoringMachCon.setScoringMachineType(ScoringMachineConnection::RMIII);
//        saatmiseEtapp = 2;
        // TODO to be fixed:
    }else if(ui->tekstiEdit->text() == "ACK"){
//        saatmiseEtapp = 4;
        saada("");  //Saadab ACK'i
    }else{
//        if(uusLugemismasin)
//            saatmiseEtapp = 0;
        saada(ui->tekstiEdit->text());
    }
    ui->tekstiEdit->clear();
}

void Lehelugeja::saadaVorku(QString saadetis)
{
    blockSize = 0;
    QByteArray block;
    QDataStream valja(&block, QIODevice::WriteOnly);
    valja.setVersion(QDataStream::Qt_4_8);
    valja << quint16(0);
    valja << saadetis;
    valja.device()->seek(0);
    valja << quint16(ulong(block.size()) - sizeof(quint16));
    if(verbose)
        QTextStream(stdout) << "saadaVorku(): " << block << endl;
    socket->write(block);
    block.clear();
}

void Lehelugeja::seadista()
{
    seadistaja->setInterval(1000);  // Restore initial value

    scoringMachCon.setTargetType(ui->leheCombo->currentIndex());
    scoringMachCon.setNoOfShotsPerTarget(ui->laskudeBox->value());
    scoringMachCon.setNotOfShotsPerSeries(laskudeArv);

    scoringMachCon.sendSettings();
//    if(uusLugemismasin){
//        seadista2();
//        return;
//    }
//    QString s = "111111111";
//    if(ui->leheCombo->currentIndex() < 2)
//        s.replace(0, 1, QString("%1").arg(ui->leheCombo->currentIndex() + 1)); //Milliseid lehti loetakse (harjutus)
//    else s.replace(0, 1, "6");
//    s.replace(8, 1, QString("%1").arg(ui->laskudeBox->value()));   //Mitu lasku lehes on
////    if(ui->kumnendikegaBox->isChecked())   //Kas lugeda laske kümnendiku täpsusega või mitte
//        s.replace(3, 1, "2");
//    saada(s);
}

//void Lehelugeja::seadista2(){
//    saatmiseEtapp = 0;
////    QString s = "SCH=KK50;TEA=KT;RIA=ZR;SSC=2;SZI=10;SGE=10;"; //"SCH=LP;TEA=KT;TEG=1500;SSC=2;SGE=10;";//"SCH=LGES;TEA=ZT;SSC=1;SZI=10;SGE=10;KSD;";
//    QString s;
//    switch(ui->leheCombo->currentIndex()){  //Lehe tüübi (harjutuse) valimine, 0 - õhupüss, 1 - õhupüstol, 2 - sportpüss
//    case 0 : s = "SCH=LGES;";
//        break;
//    case 1 : s = "SCH=LP;";
//        break;
//    case 2 : s = "SCH=KK50;";
//        break;
//    default : s = "SCH=LGES;";
//    }
//    s.append("TEA=KT;RIA=ZR;"); //Teiler täisarvudega ja väärtus kümnendikega
//    s.append(QString("SSC=%1;").arg(ui->laskudeBox->value()));  //Laskude arv lehes
//    s.append(QString("SGE=%1;SZI=%1;").arg(laskudeArv));    //Laskude arv seerias ja vahesummas
//#ifdef PROOV
////    s.append("KSD;");
//    qDebug() << "Seadistamine: " << s;
//#endif
////    if(!ui->trukkimiseBox->isChecked())   //Kas trükkida lehele lasud ja tulemused või ei
////        s.append("KSD;");
//    saada(s);
//}

void Lehelugeja::seeriaLoetud()
{
//    emit lehedLoetud();
    QString pakett = "Laskur:"; //Protokollitajasse saadetav tulemustega pakett
            //OLD: "Laskur:siffer - siffer;Eesnimi;Perekonnanimi;seeriate arv;laskude arv;seeriad;selle seeria lasud; x; y; summa;
            //aktiivne seeria;harjutus;lasku lehes;kümnendikega lugemine (true/false)
    //NEW: "Laskur:siffer - siffer;Eesnimi;Perekonnanimi;seeriate arv;laskude arv;loetud seeria nr (0-5); loetud seeria; loetud seeria lasud; x; y;
    pakett.append(ui->sifriLabel->text() + ";");
    pakett.append("Eesnimi;Perekonnanimi;");
    pakett.append(QString("%1;").arg(seeriateArv));
    pakett.append(QString("%1;").arg(laskudeArv));
    pakett.append(QString("%1;").arg(aktiivseSeeriaNr));
    pakett.append(QString("%1;").arg(aktiivneSeeria->text()));

    for(int j = 0; j < laskudeArv; j++){
        pakett.append(QString("%1;%2;%3;").arg(double(lasud[aktiivseSeeriaNr][j]->getFLask()))
                      .arg(double(lasud[aktiivseSeeriaNr][j]->X()))
                      .arg(double(lasud[aktiivseSeeriaNr][j]->Y())));
    }

//    for(int i = 0; i < seeriateArv; i++){
//        pakett.append(seeriad[i]->text() + ";");
//        for(int j = 0; j < laskudeArv; j++){
//            pakett.append(QString("%1;%2;%3;").arg(lasud[i][j]->getFLask()).arg(lasud[i][j]->X()).arg(lasud[i][j]->Y()));
//        }
//    }
//    pakett.append(ui->summaEdit->text() + ";");
//    pakett.append(";harjutus;;;");  //Paketi kõiki lahtreid ei ole vaja, jäävad tühjaks
    if(pakett != eelminePakett && !aktiivneSeeria->text().isEmpty()){    //Empty series not to be sent
        saadaVorku(pakett);
        if(verbose)
            QTextStream(stdout) << "saadaVorku(pakett): " << pakett << "\n";
    }
    eelminePakett = pakett;
    bool kasKoik = true;    //Kui on kõik seeriad loetud, tuleb muutmine keelata, et vältida ülekirjutamist Protokollitajas
    for(int i = 0; i < seeriateArv; i++)
        if(seeriad[i]->text().isEmpty())
            kasKoik = false;
    if(kasKoik){
        for(int i = 0; i < seeriateArv; i++)
            seeriad[i]->setEnabled(false);
        ui->summaEdit->setEnabled(false);
    }
//    fookus->start();  //See siin pigem tekitab probleeme
}

void Lehelugeja::startScoring()
{
    for(int j = 0; j < 10; j++){
        seriesShots[j]->clear();    // Loetava seeria lasud puhtaks
    }
    drawTarget();
    ui->silt->setText("");
}

void Lehelugeja::sulgeUhendus()
{
    scoringMachCon.closeConnection();
//    saada("EXIT");
//    saadetis = QString("EXIT").toLatin1();
//    saatmiseEtapp = 2;  //Vajalik kindluse mõttes
//    saada();
//    serial->close();
//    timer->stop();
//    uhendatud = false;
}

void Lehelugeja::uhenda()
{
//    if(uusLugemismasin){
//        uhenda2();
//        return;
//    }
//    if(uhendatud){
////        QMessageBox::warning(this, "Protolehelugeja", tr("Ühendus juba loodud! Kui on mingi jama ja tahate uuesti ühendada, tuleb enne vajutada \"Sulge ühendus\"."), QMessageBox::Ok);
//        saada("V");
//        return;
//    }
    if(!kaabelLeitud)   //Kui lugemismasina kaablit algul ei leitud, siis äkki vahepeal on see külge ühendatud
        uuendaPorte();

    ui->logi->append("Ühendamine: " + ui->comPort->currentText());

    scoringMachCon.setTargetType(ui->leheCombo->currentIndex());
    scoringMachCon.setNoOfShotsPerTarget(ui->laskudeBox->value());
    scoringMachCon.setNotOfShotsPerSeries(laskudeArv);
    scoringMachCon.setPortName(ui->comPort->currentText());

    scoringMachCon.connectToMachine();

//    serial->setPortName(ui->comPort->currentText());
//    serial->setBaudRate(BAUD2400);
//    serial->setDataBits(DATA_8 /*(DataBitsType)(3)*/ );
//    serial->setParity(PAR_NONE /*(ParityType)(0)*/ );
//    serial->setStopBits(STOP_1 /*(StopBitsType)(0)*/ );
//    serial->setFlowControl(FLOW_OFF /*(FlowType)(0)*/ );
//    serial->setTimeout(100);
//    serial->open(QIODevice::ReadWrite);

//    serial->setDtr(true);
//    serial->setRts(true);

//    saada("V");
//    serial->flush();
//    timer->start();
//    lugemismasinaValimine = true;
//    uhendatud = true;
}

//void Lehelugeja::uhenda2()
//{
//    serial->flush();
//    serial->close();

//    serial->setPortName(ui->comPort->currentText());
//    serial->setBaudRate(BAUD9600);
//    serial->setDataBits(DATA_8 /*(DataBitsType)(3)*/ );
//    serial->setParity(PAR_NONE /*(ParityType)(0)*/ );
//    serial->setStopBits(STOP_1 /*(StopBitsType)(0)*/ );
//    serial->setFlowControl(/*FLOW_HARDWARE FLOW_XONXOFF*/ FLOW_OFF /*(FlowType)(0)*/ );
//    serial->setTimeout(100);
//    serial->open(QIODevice::ReadWrite);

//    saatmiseEtapp = 0;
//    saada("SNR=");
////    serial->flush();
//    timer->start();
//    uhendatud = true;
//    QByteArray s = "ACK:";
//    s.append(ACK);
//    s.append(" NAK:");
//    s.append(NAK);
//    s.append(" CR");
//    s.append(CR);
//    s.append(" STX:");
//    s.append(STX);
//    s.append(" EOT:");
//    s.append(EOT);
//    s.append(" ENQ:");
//    s.append(ENQ);
//    ui->logi->append(QString(s));
//}

void Lehelugeja::uhenduServeriga(QString ip)
{

    socket->connectToHost(ip, 50005);  //Kui aadress ei ole tühi ning ei vajutatud Cancel, toimub ühendamine

    //Edasi peaks jätkuma funktsiooniga saadaParool();

//    QMessageBox::information(this, tr("Protolehelugeja"), tr("Järgmine saatmine"));
}

void Lehelugeja::uhenduUuesti()
{
    blockSize = 0;
    uhendusAutoriseeritud = false;
    socket->abort();
    QRegExp iPkontroll("^[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}$");  //Kontrollmuster, et sisestatud oleks ikkagi IP aadress

    bool ok = true;
    do{
        aadress = QInputDialog::getText(this, "Sisestage serveri IP aadress", "IP:", QLineEdit::Normal, aadress, &ok);
//  socket->connectToHost(QHostAddress(QInputDialog::getText(this, "Sisestage serveri IP aadress", "IP:", QLineEdit::Normal,
//            "0.0.0.0")), 50005);
//    socket->connectToHost(QHostAddress("192.168.11.131"), 50005);
        if(!ok) break;
    }while(iPkontroll.indexIn(aadress) == -1);  //Tsükkel käib nii kaua, kuni vajutatakse Cancel või on sisestatud korrektne IP aadress
    if(aadress.isEmpty() || !ok) return;
    uhenduServeriga(aadress);
}

void Lehelugeja::updateLog(QString updateInfo)
{
    ui->logi->append(updateInfo);
}

void Lehelugeja::uuendaSifriga()
{
    saadaVorku("Siffer:" + ui->sifriEdit->text());
    fookus->start();
}

void Lehelugeja::uuendaPorte()
{
    kaabelLeitud = false;
    pordid = QextSerialEnumerator::getPorts();
    ui->comPort->clear();
    foreach(QextPortInfo info, pordid){
        ui->comPort->addItem(info.portName);
        ui->logi->append(QString("%1, %2").arg(info.portName).arg(info.friendName));
        if((info.friendName.contains("Prolific", Qt::CaseInsensitive) || info.friendName.contains("serial", Qt::CaseInsensitive))
                && info.friendName.contains("USB", Qt::CaseInsensitive)){
            ui->comPort->setCurrentIndex(ui->comPort->count() - 1);
            kaabelLeitud = true;
        }
    }
    if(!kaabelLeitud) QMessageBox::warning(this, "Protokollitaja", "Lugemismasina kaablit ei leitud!", QMessageBox::Ok);
}

Lehelugeja::~Lehelugeja()
{
    delete ui;
}

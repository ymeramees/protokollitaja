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

    abiMenu = ui->menuBar->addMenu("&Abi");
    abiMenu->addAction(programmistAct);

    ui->centralGrid->replaceWidget(ui->logiPlaceholder, &logi);
    connect(&logi, &LogAndCmdWindow::receivedCommand, this, &Lehelugeja::processCommand);
    connect(&logi, &LogAndCmdWindow::changeVariable, this, &Lehelugeja::changeVariable);


    connect(ui->liigneNupp, SIGNAL(clicked()), this, SLOT(liigneLask()));
    connect(ui->otsiCOMNupp, SIGNAL(clicked()), this, SLOT(hakkaOtsima()));
    connect(ui->resetNupp, SIGNAL(clicked()), this, SLOT(restartScoring()));
    connect(ui->peidaNupp, SIGNAL(clicked()), this, SLOT(peidaNimi()));
    connect(ui->peida2Nupp, SIGNAL(clicked()), this, SLOT(peidaNupud()));
    connect(ui->seadistaNupp, SIGNAL(clicked()), this, SLOT(seadista()));
    connect(ui->sulgeUhendusNupp, SIGNAL(clicked()), this, SLOT(sulgeUhendus()));
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
        seriesShots << new Lask();  //Loetava seeria lasud
    }

    for(int i = 0; i < seeriad.count(); i++){
        QList<Lask*> seeriaLasud;
        for(int j = 0; j < 10; j++){
            seeriaLasud << new Lask();  //Laskuri seeria lasud
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

    broadcastiSaatja = new QTimer(this);
    broadcastiSaatja->setInterval(150);
    broadcastiSaatja->setSingleShot(true);
    connect(broadcastiSaatja, SIGNAL(timeout()), this, SLOT(saadaBroadcast()));
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
    seadistaja = new QTimer(this);
    seadistaja->setInterval(1000);
    seadistaja->setSingleShot(true);
    connect(seadistaja, SIGNAL(timeout()), this, SLOT(seadista()));

    if(verbose)
        scoringMachCon.setLogLevel(ScoringMachineConnection::Verbose);

    connect(&scoringMachCon, &ScoringMachineConnection::shotRead, this, &Lehelugeja::readShot);
    connect(&scoringMachCon, &ScoringMachineConnection::connectionStatusChanged, this, &Lehelugeja::updateLog);
    connect(&scoringMachCon, &ScoringMachineConnection::dataSent, this, &Lehelugeja::updateLog);

    updatePorts();

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

void Lehelugeja::changeVariable(QString variableAndValue)
{
    QStringList variableValueList = variableAndValue.split(';');

    if(variableValueList.length() < 2){
        logi.append(tr("Vigane käsklus: ") + variableAndValue);
        return;
    }
    QString variableName = variableValueList.takeFirst();
    QString value = variableValueList.takeFirst();

    if(variableName.compare("comPort") == 0){
        ui->comPort->addItem(value);
        ui->comPort->setCurrentIndex(ui->comPort->count() - 1);
    }else if(variableName.compare("scoringMachineType") == 0){
        if(value.compare("RMIV") == 0){
            scoringMachCon.setScoringMachineType(ScoringMachineConnection::RMIV);
            logi.append(tr("Lugemismasina tüüp: RMIV"));
        }else if(value.compare("RMIII") == 0){
            scoringMachCon.setScoringMachineType(ScoringMachineConnection::RMIII);
            logi.append(tr("Lugemismasina tüüp: RMIII"));
        }else {
            logi.append(tr("Tundmatu lugemismasina tüüp: ") + value);
        }
    }else{
        logi.append(tr("Tundmatu muutuja: ") + variableName);
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
    Q_UNUSED(vana)
    Q_UNUSED(uus)

    for(int i = 0; i < seeriad.count(); i++)
        if(seeriad[i]->hasFocus()){
            if(aktiivneSeeria != nullptr)
                aktiivneSeeria->setStyleSheet(this->styleSheet());
            seeriad[i]->setStyleSheet("border: 3px solid black");
            aktiivneSeeria = seeriad[i];
            aktiivseSeeriaNr = i;
        }
}

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

    updatePorts();
//    otsija->start();
}

void Lehelugeja::joonistaLask(QPointF p, bool kasMM)
{
}

void Lehelugeja::drawTarget()
{
    target.init(ui->leheCombo->currentIndex());
    target.setZoomEnabled(false);
    target.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->centralGrid->replaceWidget(ui->placeholderWidget, &target);
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
        logi.append(ui->nimeBox->currentText() + QString(" %1. seeria: %2").arg(aktiivneSeeria->objectName().at(6)).arg(double(fl / 10)));

        aktiivneSeeria->setFocus();
        QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
        QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyRelease, Qt::Key_Tab, Qt::NoModifier));
    } else {
        QMessageBox::critical(this, "Viga", "Puudus aktiivne seeria kast!", QMessageBox::Ok);
        logi.append("Viga!:" + ui->nimeBox->currentText() + QString(" tundmatu seeria: %1").arg(double(fl / 10)));
    }

    fookus->start();

    seeria = 0;
    lask = 0;
    seadistaja->start();
}

void Lehelugeja::liigneLask()
{
    // TODO To be implemented
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

        QString rida(datagram);
        if(rida.startsWith("Protok:")){
            broadcastSaabunud = true;
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
                    return; //Ühendus olemas, võib funktsioonist väljuda, vastasel juhul proovitakse nimekirjast järgmist aadressi
                }
            }
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

        for(int j = seeriateArv; j < seeriad.count(); j++)
            seeriad[j]->hide();
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
}

//void Lehelugeja::paintEvent(QPaintEvent *event)
//{
//    Q_UNUSED(event)
//}

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

void Lehelugeja::processCommand(QString command)
{
    if(command.isEmpty())
        return;
    else if(command.contains("uuenda porte", Qt::CaseInsensitive)){
        logi.append(tr("Uuenda porte:"));
        updatePorts();
    }else if(command == "ACK"){
        saada("");  //Saadab ACK'i
    }else{
        saada(command);
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

void Lehelugeja::saada(QString s)
{
    scoringMachCon.sendToMachine(s);
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

int Lehelugeja::scoringMachineType() const
{
    return scoringMachCon.scoringMachineType();
}

void Lehelugeja::seadista()
{
    seadistaja->setInterval(1000);  // Restore initial value

    scoringMachCon.setTargetType(ui->leheCombo->currentIndex());
    scoringMachCon.setNoOfShotsPerTarget(ui->laskudeBox->value());
    scoringMachCon.setNotOfShotsPerSeries(laskudeArv);

    scoringMachCon.sendSettings();
}

void Lehelugeja::seeriaLoetud()
{

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
        pakett.append(QString("%1;%2;%3;").arg(lasud[aktiivseSeeriaNr][j]->getSLask())
                      .arg(double(lasud[aktiivseSeeriaNr][j]->X()))
                      .arg(double(lasud[aktiivseSeeriaNr][j]->Y())));
    }

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
}

void Lehelugeja::uhenda()
{
//        QMessageBox::warning(this, "Protolehelugeja", tr("Ühendus juba loodud! Kui on mingi jama ja tahate uuesti ühendada, tuleb enne vajutada \"Sulge ühendus\"."), QMessageBox::Ok);
    if(!kaabelLeitud)   //Kui lugemismasina kaablit algul ei leitud, siis äkki vahepeal on see külge ühendatud
        updatePorts();

    logi.append("Ühendamine: " + ui->comPort->currentText());

    scoringMachCon.setTargetType(ui->leheCombo->currentIndex());
    scoringMachCon.setNoOfShotsPerTarget(ui->laskudeBox->value());
    scoringMachCon.setNotOfShotsPerSeries(laskudeArv);
    scoringMachCon.setPortName(ui->comPort->currentText());

    scoringMachCon.connectToMachine();
}

void Lehelugeja::uhenduServeriga(QString ip)
{
    socket->connectToHost(ip, 50005);  //Kui aadress ei ole tühi ning ei vajutatud Cancel, toimub ühendamine
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
    logi.append(updateInfo, true);
}

void Lehelugeja::uuendaSifriga()
{
    saadaVorku("Siffer:" + ui->sifriEdit->text());
    fookus->start();
}

void Lehelugeja::updatePorts()
{
    kaabelLeitud = false;
    pordid = QSerialPortInfo::availablePorts();
    ui->comPort->clear();
    foreach(QSerialPortInfo info, pordid){
        ui->comPort->addItem(info.portName());
        logi.append(QString("%1, %2").arg(info.portName()).arg(info.description()));
        if((info.description().contains("Prolific", Qt::CaseInsensitive) || info.description().contains("serial", Qt::CaseInsensitive))
                && info.description().contains("USB", Qt::CaseInsensitive)){
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

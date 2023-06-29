#include <QMessageBox>
#include "lehelugejaaken.h"

#define CR QChar(0x0d)

LehelugejaAken::LehelugejaAken(QWidget *parent , Qt::WindowFlags f) :
    QDialog(parent, f){
    oliLiigneLask = false;
    vahemuses = false;
    oliKahtlane = false;
    uhendatud = false;
    kaabelLeitud = false;
    lask = 0;
    seeria = 0;
    aktiivseSeeriaNr = 0;

    m_ui.setupUi(this);
    connect(m_ui.liigneNupp, SIGNAL(clicked()), this, SLOT(liigneLask()));
    connect(m_ui.otsiCOMNupp, SIGNAL(clicked()), this, SLOT(hakkaOtsima()));
    connect(m_ui.resetNupp, SIGNAL(clicked()), this, SLOT(alustaUuesti()));
    connect(m_ui.peidaNupp, SIGNAL(clicked()), this, SLOT(peidaNimi()));
    connect(m_ui.peida2Nupp, SIGNAL(clicked()), this, SLOT(peidaNupud()));
    connect(m_ui.saadaNupp, SIGNAL(clicked()), this, SLOT(saadaTekst()));
    connect(m_ui.seadistaNupp, SIGNAL(clicked()), this, SLOT(seadista()));
    connect(m_ui.sulgeUhendusNupp, SIGNAL(clicked()), this, SLOT(sulgeUhendus()));
    connect(m_ui.tekstiEdit, SIGNAL(returnPressed()), this, SLOT(saadaTekst()));
    connect(m_ui.uhendaNupp, SIGNAL(clicked()), this, SLOT(uhenda()));
    connect(m_ui.seeria1Edit, SIGNAL(editingFinished()), this, SLOT(seeriaLoetud()));
    connect(m_ui.seeria2Edit, SIGNAL(editingFinished()), this, SLOT(seeriaLoetud()));
    connect(m_ui.seeria3Edit, SIGNAL(editingFinished()), this, SLOT(seeriaLoetud()));
    connect(m_ui.seeria4Edit, SIGNAL(editingFinished()), this, SLOT(seeriaLoetud()));
    connect(m_ui.seeria5Edit, SIGNAL(editingFinished()), this, SLOT(seeriaLoetud()));
    connect(m_ui.seeria6Edit, SIGNAL(editingFinished()), this, SLOT(seeriaLoetud()));
    connect(m_ui.nimeBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(uuendaNimega(QString)));
    connect(m_ui.sifriEdit, SIGNAL(returnPressed()), this, SLOT(uuendaSifriga()));
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(fookusMuutus(QWidget*,QWidget*)));

    (void) new QShortcut(QKeySequence(QKeySequence::Save), this, SLOT(paluSalvestada()));

    seeriad << m_ui.seeria1Edit;
    seeriad << m_ui.seeria2Edit;
    seeriad << m_ui.seeria3Edit;
    seeriad << m_ui.seeria4Edit;
    seeriad << m_ui.seeria5Edit;
    seeriad << m_ui.seeria6Edit;

    for(int j = 0; j < 10; j++){
        seeriaLasud << new Lask(/*this*/);  //Loetava seeria lasud
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

    m_ui.silt->setBackgroundRole(QPalette::Base);
    m_ui.silt->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_ui.silt->setScaledContents(true);

    joonistaLeht();

//    serial = QSerialPort();
    timer = new QTimer(this);
    timer->setInterval(100);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(loe()));
    otsija = new QTimer(this);
    otsija->setInterval(97);
    otsija->setSingleShot(false);
    connect(otsija, SIGNAL(timeout()), this, SLOT(otsiPorti()));
    fookus = new QTimer(this);
    fookus->setInterval(50);
    fookus->setSingleShot(true);
    connect(fookus, SIGNAL(timeout()), this, SLOT(fookusSifrile()));
    seadistaja = new QTimer(this);
    seadistaja->setInterval(40);
    seadistaja->setSingleShot(true);
    connect(seadistaja, SIGNAL(timeout()), this, SLOT(seadista()));

    // Make sure things are clear
    serial.close();

    uuendaPorte();

    m_ui.nimeBox->hide();
    m_ui.nimeSilt->hide();
    m_ui.comPort->hide();
    m_ui.otsiCOMNupp->hide();
    m_ui.seadistaNupp->hide();
}

void LehelugejaAken::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui.retranslateUi(this);
        break;
    default:
        break;
    }
}

void LehelugejaAken::alustaUuesti()
{
    lask = 0;
    seeria = 0;
    for(int j = 0; j < 10; j++){
        seeriaLasud[j]->clear();    //Loetava seeria lasud puhtaks
    }
    joonistaLeht();
}

void LehelugejaAken::fookusMuutus(QWidget *vana, QWidget *uus)
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

void LehelugejaAken::fookusSifrile()
{
    seadista();
    m_ui.sifriEdit->setFocus();  //Kui lehed loetud, teeb sifri akna aktiivseks, et saaks uuesti sifrit sisestada
    m_ui.sifriEdit->selectAll();
}

void LehelugejaAken::hakkaOtsima()
{
    if(uhendatud){
        otsija->stop();
        QMessageBox::warning(this, "Protokollitaja", tr("Ühendus juba loodud! Kui on mingi jama ja tahate"
                "uuesti ühendada, tuleb enne vajutada \"Sulge ühendus\"."), QMessageBox::Ok);
        return;
    }

    uuendaPorte();
    otsija->start();
}

void LehelugejaAken::joonistaLeht()
{
    if(m_ui.leheCombo->currentIndex() == 0 || m_ui.leheCombo->currentIndex() == 2){
        QFont font;
        font.setPointSize(12);
        painter->setFont(font);
        pilt->fill(0);
        painter->setBrush(Qt::white);
        painter->setPen(Qt::black);
        //painter->rotate(180);
        painter->drawEllipse(QPoint(0, 0), 182, 182);
        painter->drawEllipse(QPoint(0, 0), 162, 162);
        painter->drawEllipse(QPoint(0, 0), 142, 142);
        painter->setBrush(Qt::black);
        painter->setPen(Qt::white);
        painter->drawEllipse(QPoint(0, 0), 122, 122);
        painter->drawEllipse(QPoint(0, 0), 102, 102);
        painter->drawEllipse(QPoint(0, 0), 82, 82);
        painter->drawEllipse(QPoint(0, 0), 62, 62);
        painter->drawEllipse(QPoint(0, 0), 42, 42);
        painter->drawEllipse(QPoint(0, 0), 22, 22);
        painter->drawEllipse(QPoint(0, 0), 2, 2);
        painter->drawText(-36, 5, "8");
        painter->drawText(28, 5, "8");
        painter->drawText(-56, 5, "7");
        painter->drawText(48, 5, "7");
        painter->drawText(-76, 5, "6");
        painter->drawText(70, 5, "6");
        painter->drawText(-97, 5, "5");
        painter->drawText(88, 5, "5");
        painter->drawText(-116, 5, "4");
        painter->drawText(110, 5, "4");
        painter->setPen(Qt::black);
        painter->drawText(-136, 5, "3");
        painter->drawText(129, 5, "3");
        painter->drawText(-156, 5, "2");
        painter->drawText(150, 5, "2");
        painter->drawText(-176, 5, "1");
        painter->drawText(169, 5, "1");
        //painter->setBrush(Qt::red);
        //painter->setPen(Qt::NoPen);
        //painter->drawEllipse(QPoint(190, 162), 10, 10);
        //painter->drawEllipse(QPoint(211, 247), 10, 10);
    }else{
        QFont font;
        font.setPointSize(12);
        painter->setFont(font);
        pilt->fill(0);
        painter->setBrush(Qt::white);
        painter->setPen(Qt::black);
        //painter->rotate(180);
        painter->drawEllipse(QPoint(0, 0), 311, 311);
        painter->drawEllipse(QPoint(0, 0), 279, 279);
        painter->drawEllipse(QPoint(0, 0), 247, 247);
        painter->drawEllipse(QPoint(0, 0), 215, 215);
        painter->drawEllipse(QPoint(0, 0), 183, 183);
        painter->drawEllipse(QPoint(0, 0), 151, 151);
        painter->setBrush(Qt::black);
        painter->setPen(Qt::white);
        painter->drawEllipse(QPoint(0, 0), 119, 119);
        painter->drawEllipse(QPoint(0, 0), 87, 87);
        painter->drawEllipse(QPoint(0, 0), 55, 55);
        painter->drawEllipse(QPoint(0, 0), 23, 23);
        painter->drawEllipse(QPoint(0, 0), 10, 10);
        painter->drawText(-73, 5, "8");
        painter->drawText(68, 5, "8");
        painter->drawText(-105, 5, "7");
        painter->drawText(100, 5, "7");
        painter->setPen(Qt::black);
        painter->drawText(-137, 5, "6");
        painter->drawText(132, 5, "6");
        painter->drawText(-169, 5, "5");
        painter->drawText(164, 5, "5");
        painter->drawText(-201, 5, "4");
        painter->drawText(196, 5, "4");
        painter->drawText(-233, 5, "3");
        painter->drawText(228, 5, "3");
        painter->drawText(-265, 5, "2");
        painter->drawText(260, 5, "2");
        painter->drawText(-297, 5, "1");
        painter->drawText(292, 5, "1");
    }
    m_ui.silt->setPixmap(QPixmap::fromImage(*pilt));
}

void LehelugejaAken::liigneLask()
{
    QString s = "111111111";
    if(m_ui.leheCombo->currentIndex() < 2)
        s.replace(0, 1, QString("%1").arg(m_ui.leheCombo->currentIndex() + 1));
    else s.replace(0, 1, "6");
    s.replace(8, 1, QString("%1").arg(m_ui.laskudeBox->value() + 1));
    if(m_ui.kumnendikegaBox->isChecked())   //Kas lugeda laske kümnendiku täpsusega või mitte
        s.replace(3, 1, "2");
    saada(s);
    oliLiigneLask = true;   //Siis loe() funktsioon teab, et tuleb uuesti seadistada
}

void LehelugejaAken::loe()
{
    if(serial.bytesAvailable()>0){
        static QString buffer;
        QString tekst;
        //char buffer[256];
        //int rec = serial.readLine(buffer, 255);
        //buffer[rec]='\0';
        buffer.append(serial.readAll());
        if(buffer.contains(CR)){
            tekst = buffer.left(buffer.indexOf(CR) + 1);
            buffer.remove(0, buffer.indexOf(CR) + 1);
        }else return;

        m_ui.logi->append(tr("vastuvõtt:"));

        if(!tekst.contains("START") && !tekst.contains("SCHEIBE") && !tekst.contains("Keine") && tekst.contains(';'))
            m_ui.logi->append(tr("     %1. lask").arg(lask + 1));
        m_ui.logi->append(tekst);
        /*if(tekst.startsWith('H', Qt::CaseInsensitive)){
                saada("611101112");
                return;
        }*/
        if(!tekst.contains("TART") && !tekst.contains("SCHEIBE") && !tekst.contains("Keine") && tekst.contains(';')){
                QStringList list = tekst.split(';', Qt::KeepEmptyParts);
                if(list.size() < 5){
                    m_ui.logi->append(tr("See rida oli liiga lühike!"));
                    return;
                }
                int x = 0, y = 0, il = 0;
                float fl = 0;
                if(lask == 0){
                    joonistaLeht();
                }
                m_ui.logi->append(QString("list.count() = %1").arg(list.count()));
                //m_ui.logi->append(list.at(3));
                //m_ui.logi->append(list.at(4));
                if(m_ui.leheCombo->currentIndex() == 0){    //Õhupüssi lehed
                    x = list.at(3).toFloat() * 100;
                    y = list.at(4).toFloat() * 100;
                    x = qRound(qreal(x * 20 / 100));
                    y = qRound(qreal(y * 20 / 100));
                    m_ui.logi->append(QString("x = %1, y = %2\n").arg(x).arg(y));
                    painter->setBrush(Qt::red);
                    painter->setPen(Qt::black);
                    painter->drawEllipse(QPoint(x, y), 18, 18);
                }else{
                    x = list.at(3).toFloat() * 100;
                    y = list.at(4).toFloat() * 100;
                    x = qRound(qreal(x * 32 / 100));
                    y = qRound(qreal(y * 32 / 100));
                    m_ui.logi->append(QString("x = %1, y = %2\n").arg(x).arg(y));
                    painter->setBrush(Qt::red);
                    painter->setPen(Qt::black);
                    painter->drawEllipse(QPoint(x, y), 9, 9);
                }
                bool onnestus = false;
                fl = list.at(1).toFloat(&onnestus);
                if(!onnestus){
                    //QMessageBox::critical(this, "Viga", "Ütle Laurile, et lugemismasinast tulnud punkt on vaja asendada komaga.", QMessageBox::Ok);
                    QString rida = list.at(1);
                    rida.replace('.', ',');
                    fl = rida.toFloat(&onnestus);
                }
                seeriaLasud[lask]->setMmX(x);
                seeriaLasud[lask]->setMmY(y);
                seeriaLasud[lask]->setLask(fl);

                lask++;
                painter->setPen(Qt::black);
                QFont font;
                font.setPointSize(16);
                painter->setFont(font);

                if(m_ui.kumnendikegaBox->isChecked()){  //Kui on kümnendikega lugemine, liita seeriale komakohaga arv,
                    il = qRound(fl * 10);               //kui täisarvudega, siis liita täisarv
                    painter->drawText(260, -295 + 18 * lask, list.at(1));
                }else{
                    il = fl;
                    il *= 10;
                    painter->drawText(260, -295 + 18 * lask, list.at(1).left(list.at(1).indexOf('.')));
                }
                seeria += il;

                if(list.count() > 6){
                    m_ui.logi->append(list.at(8));
                    m_ui.logi->append(list.at(9));
                    if(m_ui.leheCombo->currentIndex() == 0){    //Õhupüssi lehed
                        x = list.at(8).toFloat() * 100;
                        y = list.at(9).toFloat() * 100;
                        x = qRound(qreal(x * 20 / 100));
                        y = qRound(qreal(y * 20 / 100));
                        m_ui.logi->append(QString("x = %1, y = %2\n").arg(x).arg(y));
                        painter->setBrush(Qt::red);
                        painter->setPen(Qt::black);
                        painter->drawEllipse(QPoint(x, y), 18, 18);
                    }else{
                        x = list.at(8).toFloat() * 100;
                        y = list.at(9).toFloat() * 100;
                        x = qRound(qreal(x * 32 / 100));
                        y = qRound(qreal(y * 32 / 100));
                        m_ui.logi->append(QString("x = %1, y = %2\n").arg(x).arg(y));
                        painter->setBrush(Qt::red);
                        painter->setPen(Qt::black);
                        painter->drawEllipse(QPoint(x, y), 9, 9);
                    }
                    onnestus = false;
                    fl = list.at(6).toFloat();
                    if(!onnestus){
                        //QMessageBox::critical(this, "Viga", "Ütle Laurile, et lugemismasinast tulnud punkt on vaja asendada komaga.", QMessageBox::Ok);
                        QString rida = list.at(6);
                        rida.replace('.', ',');
                        fl = rida.toFloat(&onnestus);
                    }
                    seeriaLasud[lask]->setMmX(x);
                    seeriaLasud[lask]->setMmY(y);
                    seeriaLasud[lask]->setLask(fl);

                    lask++;
                    painter->setPen(Qt::black);

                    if(m_ui.kumnendikegaBox->isChecked()){  //Kui on kümnendikega lugemine, liita seeriale komakohaga arv,
                        il = qRound(fl * 10);               //kui täisarvudega, siis liita täisarv
                        painter->drawText(260, -295 + 18 * lask, list.at(6));
                    }else{
                        il = fl;
                        il *= 10;
                        painter->drawText(260, -295 + 18 * lask, list.at(6).left(list.at(6).indexOf('.')));
                    }
                    seeria += il;
                }
                if(list.count() > 12){
                    m_ui.logi->append(list.at(13));
                    m_ui.logi->append(list.at(14));
                    if(m_ui.leheCombo->currentIndex() == 0){    //Õhupüssi lehed
                        x = list.at(13).toFloat() * 100;
                        y = list.at(14).toFloat() * 100;
                        x = qRound(qreal(x * 20 / 100));
                        y = qRound(qreal(y * 20 / 100));
                        m_ui.logi->append(QString("x = %1, y = %2\n").arg(x).arg(y));
                        painter->setBrush(Qt::red);
                        painter->setPen(Qt::black);
                        painter->drawEllipse(QPoint(x, y), 18, 18);
                    }else{
                        x = list.at(13).toFloat() * 100;
                        y = list.at(14).toFloat() * 100;
                        x = qRound(qreal(x * 32 / 100));
                        y = qRound(qreal(y * 32 / 100));
                        m_ui.logi->append(QString("x = %1, y = %2\n").arg(x).arg(y));
                        painter->setBrush(Qt::red);
                        painter->setPen(Qt::black);
                        painter->drawEllipse(QPoint(x, y), 9, 9);
                    }
                    onnestus = false;
                    fl = list.at(11).toFloat();
                    if(!onnestus){
                        //QMessageBox::critical(this, "Viga", "Ütle Laurile, et lugemismasinast tulnud punkt on vaja asendada komaga.", QMessageBox::Ok);
                        QString rida = list.at(11);
                        rida.replace('.', ',');
                        fl = rida.toFloat(&onnestus);
                    }
                    seeriaLasud[lask]->setMmX(x);
                    seeriaLasud[lask]->setMmY(y);
                    seeriaLasud[lask]->setLask(fl);

                    lask++;
                    painter->setPen(Qt::black);

                    if(m_ui.kumnendikegaBox->isChecked()){  //Kui on kümnendikega lugemine, liita seeriale komakohaga arv,
                        il = qRound(fl * 10);               //kui täisarvudega, siis liita täisarv
                        painter->drawText(260, -295 + 18 * lask, list.at(11));
                    }else{
                        il = fl;
                        il *= 10;
                        painter->drawText(260, -295 + 18 * lask, list.at(11).left(list.at(11).indexOf('.')));
                    }
                    seeria += il;
                }
                if(list.contains(";M", Qt::CaseSensitive))
                    oliKahtlane = true;
                m_ui.silt->setPixmap(QPixmap::fromImage(*pilt));
                this->update();
                if(lask == 10){ //Seeria sai täis, vaja kirjutada väärtus aktiivsesse lahtrisse
                    if(aktiivneSeeria != nullptr){
                        fl = seeria;    //Kasutame lasu float muutujat seeria jagamiseks 10'ga, seeria enda jagamisel kaovad komakohad ära
                        //m_ui.logi->append(QString("Seeria: %1, fl: %2, fl/10: %3").arg(seeria).arg(fl).arg(fl/10));
                        aktiivneSeeria->setText(QString("%1").arg(fl / 10));
                        aktiivneSeeria->setText(aktiivneSeeria->text().replace('.', ','));
                        for(int i = 0; i < seeriaLasud.count(); i++)    //Loetud laskude kirjutamine aktiivsesse seeriasse
                            lasud[aktiivseSeeriaNr][i]->set(seeriaLasud[i]);
                        m_ui.logi->append(m_ui.nimeBox->currentText() + QString(" %1. seeria: %2").arg(aktiivneSeeria->objectName().at(6)).arg(fl / 10));
                    }else{
                        QMessageBox::critical(this, "Viga", "Puudus aktiivne seeria kast!", QMessageBox::Ok);
                        m_ui.logi->append("Viga!:" + m_ui.nimeBox->currentText() + tr(" tundmatu seeria: %1").arg(fl / 10));
                    }
                    /*for(int k = 0; k < seeriad.count(); k++)
                        if(seeriad[k]->hasFocus()){
                            fl = seeria;    //Kasutame lasu float muutujat seeria jagamiseks 10'ga, seeria enda jagamisel kaovad komakohad ära
                            //m_ui.logi->append(QString("Seeria: %1, fl: %2, fl/10: %3").arg(seeria).arg(fl).arg(fl/10));
                            seeriad[k]->setText(QString("%1").arg(fl / 10));
                            seeriad[k]->setText(seeriad[k]->text().replace('.', ','));
                            m_ui.logi->append(m_ui.nimeBox->currentText() + QString(" %1. seeria: %2").arg(k + 1).arg(fl / 10));
                        }*/
//                    if(!m_ui.sifriEdit->text().isEmpty() && m_ui.sifriEdit->text() != m_ui.sifriLabel->text().left(m_ui.sifriEdit->text().length())){   //Kui on
//                        QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier)); //juba uus siffer valmis sisestatud, võib kohe
//                        QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyRelease, Qt::Key_Return, Qt::NoModifier));   //automaatselt seadistada uue sifri
//                    }else{
                        aktiivneSeeria->setFocus();
                        QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
                        QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyRelease, Qt::Key_Tab, Qt::NoModifier));
                        fookus->start();
//                    }
                    seeria = 0;
                    lask = 0;
                    seadistaja->start();
                }
                if(oliKahtlane){ //Kui on vaja kontrollida, viskab teate
                    QMessageBox::warning(this, "Teade", tr("Lehes olid kahtlased lasud, vaja kontrollida ning "
                            "seejärel kaks korda Neustart vajutada. Alles siis vajutage siin \"OK\""), QMessageBox::Ok);
                    oliKahtlane = false;
                }
                return;
        }else{
            if(vahemuses && tekst.contains("SCHEIBE")){  //Seadistab tagasi normaalseks
                /*QString s = "111111111";
                if(m_ui.leheCombo->currentIndex() < 2)  //Relva tüübi kindlaks tegemine
                    s.replace(0, 1, QString("%1").arg(m_ui.leheCombo->currentIndex() + 1));
                else s.replace(0, 1, "6");
                s.replace(8, 1, QString("%1").arg(m_ui.laskudeBox->value()));
                saada(s);*/
                seadistaja->start();
                vahemuses = false;
            }else if(oliLiigneLask && tekst.contains("SCHEIBE")){  //Seadistab järgmiseks vähema laskude arvu peale
                                                                   //või tagasi normaalseks
                QString s = "111111111";
                if(m_ui.leheCombo->currentIndex() < 2)  //Relva tüübi kindlaks tegemine
                    s.replace(0, 1, QString("%1").arg(m_ui.leheCombo->currentIndex() + 1));
                else s.replace(0, 1, "6");

                //Kui oli ainult 1 lask lehte, siis järgmine leht on nagunii tühi ja nii võib tagasi normaalseks seadistada
                if(m_ui.laskudeBox->value() == 1)
                    s.replace(8, 1, QString("%1").arg(m_ui.laskudeBox->value()));
                //Kui oli 2 lasku lehte, on järgmisse vaja 1
                else{
                    s.replace(8, 1, QString("%1").arg(m_ui.laskudeBox->value() - 1));
                    vahemuses = true;
                }
                if(m_ui.kumnendikegaBox->isChecked())   //Kas lugeda laske kümnendiku täpsusega või mitte
                    s.replace(3, 1, "2");
                saada(s);
                oliLiigneLask = false;
            }
        }
    }
}

void LehelugejaAken::otsiPorti()
{
    //m_ui.logi->clear();
    if(uhendatud){
        otsija->stop();
        QMessageBox::warning(this, "Protokollitaja", tr("Ühendus juba loodud! Kui on mingi jama ja tahate uuesti "
                             "ühendada, tuleb enne vajutada \"Sulge ühendus\"."), QMessageBox::Ok);
        return;
    }
    static int i = 0;
    loe();
    serial.close();
    if(m_ui.logi->toPlainText().split("\n").last().contains("H")){
        otsija->stop();
        uhenda();
        i = 0;
        return;
    }
    otsija->stop();
    if(i >= m_ui.comPort->count()){
        i = 0;
        return;
    }
    m_ui.comPort->setCurrentIndex(i);
    serial.setPortName(m_ui.comPort->currentText());
    serial.setBaudRate(QSerialPort::Baud2400);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
//    serial.setTimeout(100);   //TODO Not available in Qt 6.5
    serial.open(QIODevice::ReadWrite);

    serial.setDataTerminalReady(true);
    serial.setRequestToSend(true);

    saada(tr("%1. korda: %2").arg(++i).arg(m_ui.comPort->currentText()));
    serial.flush();
    otsija->start();
}

void LehelugejaAken::paluSalvestada()
{
    emit salvesta();
}

void LehelugejaAken::peidaNimi()
{
    if(m_ui.nimeBox->isVisible()){
        m_ui.nimeBox->hide();
        m_ui.nimeSilt->hide();
        m_ui.peidaNupp->setText(">>");
    }else{
        m_ui.nimeBox->show();
        m_ui.nimeSilt->show();
        m_ui.peidaNupp->setText("<<");
    }
}

void LehelugejaAken::peidaNupud()
{
    if(m_ui.comPort->isVisible()){
        m_ui.comPort->hide();
        m_ui.otsiCOMNupp->hide();
        m_ui.seadistaNupp->hide();
        m_ui.peida2Nupp->setText(">>");
    }else{
        m_ui.comPort->show();
        m_ui.otsiCOMNupp->show();
        m_ui.seadistaNupp->show();
        m_ui.peida2Nupp->setText("<<");
    }
}

void LehelugejaAken::saada(QString s)
{
    QByteArray saadetis = s.toLatin1();
    saadetis.append(13);
    //serial.writeData(saadetis, saadetis.size());
    serial.write(saadetis);
    m_ui.logi->append("<font color=blue>"+s+"</font>");
    serial.flush();
}

void LehelugejaAken::saadaTekst()
{
    if(m_ui.tekstiEdit->text().isEmpty())
        return;
    else if(m_ui.tekstiEdit->text().contains("uuenda porte", Qt::CaseInsensitive)){
        m_ui.logi->append("Uuenda porte:");
        uuendaPorte();
    }else if(m_ui.tekstiEdit->text().left(3) == "COM" && m_ui.tekstiEdit->text().length() == 4){
        m_ui.logi->append("Lisatud: " + m_ui.tekstiEdit->text());
        m_ui.comPort->addItem(m_ui.tekstiEdit->text());
        m_ui.comPort->setCurrentIndex(m_ui.comPort->count() - 1);
    }else saada(m_ui.tekstiEdit->text());
    m_ui.tekstiEdit->clear();
}

void LehelugejaAken::seadista()
{
    QString s = "111111111";
    if(m_ui.leheCombo->currentIndex() < 2)
        s.replace(0, 1, QString("%1").arg(m_ui.leheCombo->currentIndex() + 1)); //Milliseid lehti loetakse (harjutus)
    else s.replace(0, 1, "6");
    s.replace(8, 1, QString("%1").arg(m_ui.laskudeBox->value()));   //Mitu lasku lehes on
//    if(m_ui.kumnendikegaBox->isChecked())   //Kas lugeda laske kümnendiku täpsusega või mitte
        s.replace(3, 1, "2");
    saada(s);
}

void LehelugejaAken::seeriaLoetud()
{
    emit lehedLoetud();
//    fookus->start();  //See siin segab pigem, kuna viskab kohe fookuse sifrikasti
}

void LehelugejaAken::sulgeUhendus()
{
    saada("EXIT");
    serial.close();
    timer->stop();
    uhendatud = false;
}

void LehelugejaAken::uhenda()
{
    if(uhendatud){
        QMessageBox::warning(this, "Protokollitaja", tr("Ühendus juba loodud! Kui on mingi jama ja tahate uuesti "
                             "ühendada, tuleb enne vajutada \"Sulge ühendus\"."), QMessageBox::Ok);
        return;
    }
    if(!kaabelLeitud)   //Kui lugemismasina kaablit algul ei leitud, siis äkki vahepeal on see külge ühendatud
        uuendaPorte();

    m_ui.logi->append("Ühendamine: " + m_ui.comPort->currentText());
    serial.setPortName(m_ui.comPort->currentText());
    serial.setBaudRate(QSerialPort::Baud2400);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
//    serial.setTimeout(100);   // TODO Not available in Qt 6.5
    serial.open(QIODevice::ReadWrite);

    serial.setDataTerminalReady(true);
    serial.setRequestToSend(true);

    saada("V");
    serial.flush();
    timer->start();
    uhendatud = true;
}

void LehelugejaAken::uuendaNimega(QString s)
{
    emit uutNime(s);
}

void LehelugejaAken::uuendaSifriga()
{
    emit uutSifrit(m_ui.sifriEdit->text().toInt());
//    fookus->start();
}

void LehelugejaAken::uuendaPorte()
{
    pordid = QSerialPortInfo::availablePorts();
    m_ui.comPort->clear();
    foreach(QSerialPortInfo info, pordid){ //Kontroll, kas mõnes pordis on lugemismasina kaabel
        m_ui.comPort->addItem(info.portName());
        m_ui.logi->append(QString("%1, %2").arg(info.portName()).arg(info.description()));  // TODO To be checked if this still works
        if((info.description().contains("Prolific", Qt::CaseInsensitive) || info.description().contains("serial", Qt::CaseInsensitive))
                && info.description().contains("USB", Qt::CaseInsensitive)){
            m_ui.comPort->setCurrentIndex(m_ui.comPort->count() - 1);
            kaabelLeitud = true;
        }
    }
    if(!kaabelLeitud) QMessageBox::warning(this, "Protokollitaja", "Lugemismasina kaablit ei leitud!", QMessageBox::Ok);
}

#include "laskur.h"

extern bool verbose;
extern bool veryVerbose;

Laskur::Laskur(Andmebaas* baas, int s, int vs, int a, bool *k, bool *kum, int i, int *jar, int ls, QWidget *parent)
    : QWidget(parent)
{
    id = i;
        jrkArv = 0;
        lisaAken = nullptr;
        laskudeAken = nullptr;
        arvutaja = new QTimer(this);
        arvutaja->setInterval(60000);
        arvutaja->setSingleShot(false);
        connect(arvutaja, SIGNAL(timeout()), this, SLOT(liida()));
        arvutaja->start();
        laskudeArv = ls;
        seeriateArv = s;
        vSummadeSamm = vs;
        andmebaas = baas;
        abi = a;
        kirjutusAbi = k;
        kumnendikega = kum;
        jarjestamine = jar;
        keskmLask = 0;
        muudetud = false;
        onLehelugejaLaskur = false;
        onVorguLaskur = false;
        for(int i = 0; i < 24; i++)
                lisaLasud << -1;
        if(this->objectName().isEmpty()){
                this->setObjectName("laskurClass");
        }
        m_competitionStage = 0;
        m_competitionStarted = false;
        m_siusConnectionIndex = -1;
//  QMessageBox::information(this, "Teade", "Lasku::Laskur()", "OK");
//	connect(eesNimi, SIGNAL(textEdited(QString)), this, SLOT(muutus(QString)));
//	connect(perekNimi, SIGNAL(textEdited(QString)), this, SLOT(muutus2(QString)));
//	QMessageBox::information(this, "Teade", "Lasku::Laskur()3", "OK");

        hKast = new QHBoxLayout(this);
        hKast->setObjectName("hKast");
        linnuke = new QCheckBox(this);
        rajaNr = new QLineEdit(this);
        rajaNr->setMinimumHeight(28);
        rajaNr->setMaximumWidth(20);
        rajaNr->setToolTip("Raja number");
        connect(rajaNr, SIGNAL(returnPressed()), this, SLOT(vajutaTab()));
        connect(rajaNr, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
        sifriAlgus = new QLineEdit(this);
        sifriAlgus->setMinimumHeight(28);
        sifriAlgus->setMaximumWidth(50);
        sifriAlgus->setToolTip("Siffer");
        connect(sifriAlgus, SIGNAL(returnPressed()), this, SLOT(vajutaTab()));
        connect(sifriAlgus, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
        sidekriips = new QLabel(this);
        sidekriips->setText("-");
        sifriLopp = new QLineEdit(this);
        sifriLopp->setMinimumHeight(28);
        sifriLopp->setMaximumWidth(50);
        sifriLopp->setToolTip("Siffer");
        connect(sifriLopp, SIGNAL(returnPressed()), this, SLOT(enterVajutatud()));
        connect(sifriLopp, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
        sifriNupp = new QPushButton(this);
        sifriNupp->setText(">>");
        sifriNupp->setMinimumWidth(20);
        sifriNupp->setMaximumWidth(22);
        connect(sifriNupp, SIGNAL(clicked()), this, SLOT(naitaSifrit()));
        eesNimi = new QLineEdit(this);
        eesNimi->setMinimumHeight(28);
        eesNimi->setToolTip("Eesnimi");
        connect(eesNimi, SIGNAL(textEdited(QString)), this, SLOT(muutus(QString)));
        connect(eesNimi, SIGNAL(returnPressed()), this, SLOT(vajutaTab()));
        connect(eesNimi, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
        perekNimi = new QLineEdit(this);
        perekNimi->setMinimumHeight(28);
        perekNimi->setToolTip("Perekonnanimi");
        connect(perekNimi, SIGNAL(textEdited(QString)), this, SLOT(muutus2(QString)));
        connect(perekNimi, SIGNAL(returnPressed()), this, SLOT(vajutaTab()));
        connect(perekNimi, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
        sunniAasta = new QLineEdit(this);
        sunniAasta->setMinimumHeight(28);
        sunniAasta->setMaximumWidth(45);
        sunniAasta->setToolTip(tr("Sünniaasta"));
        connect(sunniAasta, SIGNAL(returnPressed()), this, SLOT(vajutaTab()));
        connect(sunniAasta, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
        klubi = new QLineEdit(this);
        klubi->setMinimumHeight(28);
        klubi->setMaxLength(12);
        klubi->setToolTip("Klubi");
        connect(klubi, SIGNAL(textEdited(QString)), this, SLOT(muutus4(QString)));
        connect(klubi, SIGNAL(returnPressed()), this, SLOT(vajutaTab()));
        connect(klubi, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
        for(int i = 0; i < seeriateArv; i++){
                seeriad << new QLineEdit(this);
                seeriad[i]->setMinimumHeight(28);
                seeriad[i]->setMaximumWidth(40);
                seeriad[i]->setToolTip("Seeriad");
                connect(seeriad[i], SIGNAL(editingFinished()), this, SLOT(liida()));
                connect(seeriad[i], SIGNAL(returnPressed()), this, SLOT(vajutaTab()));
                connect(seeriad[i], SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
                QList<Lask*> seeriaLasud;
                for(int j = 0; j < laskudeArv; j++){
                    seeriaLasud << new Lask(/*this*/);  //Seeria lasud
                }
                seeriaLasud << new Lask(/*this*/);  //Lisaks üks lask karistuse jaoks
                lasud << seeriaLasud/*.toVector().toList()*/;   //Seeria lasud laskuri laskude hulka
        }
        if(vSummadeSamm != 0){
                for(int i = 0; i < seeriateArv / vSummadeSamm; i++){
                        vSummad << new QLineEdit(this);
                        vSummad[i]->setMinimumHeight(28);
                        vSummad[i]->setMaximumWidth(50);
                        vSummad[i]->setText("0");
                        vSummad[i]->setToolTip("Vahesumma");
                        vSummad[i]->setStyleSheet("border: 1px solid grey");
                        vSummad[i]->setReadOnly(true);
                }
        }
        summa = new QLineEdit(this);
        summa->setText("0");
        summa->setMinimumHeight(28);
        summa->setMaximumWidth(50);
        summa->setToolTip("Summa");
        summa->setStyleSheet("border: 1px solid grey");
        summa->setReadOnly(true);
        connect(summa, SIGNAL(returnPressed()), this, SLOT(vajutaTab()));
        connect(summa, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
        finaal = new QLineEdit(this);
        finaal->setText("Fin");
        finaal->setMinimumHeight(28);
        finaal->setMaximumWidth(45);
        finaal->setToolTip("Finaali seeria");
        connect(finaal, SIGNAL(editingFinished()), this, SLOT(liida()));
        connect(finaal, SIGNAL(returnPressed()), this, SLOT(vajutaTab()));
        connect(finaal, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
        koguSumma = new QLabel(this);
        koguSumma->setText("0,0");
        koguSumma->setToolTip("Summa");
        kumned = new QLineEdit(this);
        kumned->setText("0");
        kumned->setMinimumHeight(28);
        kumned->setMaximumWidth(25);
        kumned->setToolTip(tr("Sisekümnete arv"));
        connect(kumned, SIGNAL(editingFinished()), this, SLOT(muutus5()));
        connect(kumned, SIGNAL(returnPressed()), this, SLOT(vajutaTab()));
        connect(kumned, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
        markus = new QLineEdit(this);
        markus->setMinimumHeight(28);
        markus->setMaximumWidth(85);
        markus->setText(tr("Märkused"));
        connect(markus, SIGNAL(editingFinished()), this, SLOT(markuseMuutus()));
        connect(markus, SIGNAL(returnPressed()), this, SLOT(vajutaTab()));
        connect(markus, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
        lisaLNupp = new QPushButton("Lis.", this);
        lisaLNupp->setMaximumWidth(25);
        lisaLNupp->setMinimumWidth(25);
        lisaLNupp->setToolTip("Lisalasud");
        connect(lisaLNupp, SIGNAL(clicked()), this, SLOT(lisaLAken()));
        hKast->addWidget(linnuke);
        hKast->addWidget(rajaNr);
        hKast->addWidget(sifriAlgus);
        hKast->addWidget(sidekriips);
        hKast->addWidget(sifriLopp);
        hKast->addWidget(sifriNupp);
        hKast->addWidget(eesNimi);
        hKast->addWidget(perekNimi);
        hKast->addWidget(sunniAasta);
        hKast->addWidget(klubi);
        int j = 0;
        //QMessageBox::information(this, "Teade", QString("seeriad:%1").arg(seeriad.size()), "OK");
        for(int i = 0; i < seeriad.size(); i++){
            //QMessageBox::information(this, "Teade", "Lasku::Laskur()2", "OK");
            hKast->addWidget(seeriad[i]);
            if(vSummadeSamm != 0){
                if(/*i != 0 &&*/ (i+1) % vSummadeSamm == 0){
                    if(j >= vSummad.size()){
                        QMessageBox::critical(this, "Viga!", "Ei ole nii palju vahesummasid!", "Selge");
                        return;
                    }
                    hKast->addWidget(vSummad[j]);
                    j++;
                }
            }
        }
        hKast->addWidget(summa);
        hKast->addWidget(finaal);
        hKast->addWidget(koguSumma);
        hKast->addWidget(kumned);
        hKast->addWidget(markus);
        hKast->addWidget(lisaLNupp);
        hKast->addStretch();
        rajaNr->hide();
        sifriAlgus->hide();
        sidekriips->hide();
        sifriLopp->hide();
//        this->setMaximumWidth(1200);

        laskudeAkenAct = new QAction(tr("Lasud..."), this);
        laskudeAkenAct->setStatusTip(tr("Ava laskuri laskude aken"));
        connect(laskudeAkenAct, SIGNAL(triggered()), this, SLOT(naitaLaskudeAkent()));
        idAct = new QAction(tr("ID..."), this);
        idAct->setStatusTip(tr("Vaata/muuda laskuri ID'd"));
        connect(idAct, SIGNAL(triggered()), this, SLOT(naitaIdAken()));

        popup = new QMenu(this);
        popup->addAction(laskudeAkenAct);
        popup->addAction(idAct);
}

int Laskur::competitionStage() const
{
    return m_competitionStage;
}

void Laskur::contextMenuEvent(QContextMenuEvent *event)
{
        popup->exec(event->globalPos());
}

void Laskur::enterVajutatud()
{
    emit enter(jrkArv);
}

bool Laskur::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);

    if(/*object == seeriad[0] &&*/ event->type() == QEvent::MouseButtonDblClick) {
        naitaLaskudeAkent();
        return false; // lets the event continue to the edit
    }
    return false;
}

bool Laskur::isCompetitionStarted() const
{
    return m_competitionStarted;
}

QString Laskur::getSumma()
{
return summa->text();
}

void Laskur::lisaLAken()
{
    QString llask;
    bool onnestus;
        double lask = 0;
        if(lisaAken == nullptr)
                lisaAken = new LisaLaskudeAken(this);
        for(int i = 0; i < lisaLasud.count(); i++){
                if(lisaLasud[i] != -1){
                        lask = lisaLasud[i];
                        lask /= 10;
                        QString::number(lask);
                        QString slask = QString("%1").arg(lask);
                        if(!slask.contains('.') && !slask.contains(','))
                            slask.append(",0");
                        lisaAken->ui.lisaLasud->addItem(slask);
                }
        }
        if(lisaLasud.count() == 0)
                QMessageBox::critical(this, "Teade", "lisalasud.count() = 0", "Selge");
        if(lisaAken->exec() == QDialog::Accepted){
                emit muudatus();
                lisaLasud.clear();
                int max = lisaAken->ui.lisaLasud->count();
                for(int i = 0; i < max; i++){
                    llask = lisaAken->ui.lisaLasud->takeItem(0)->text();
                    lask = llask.toDouble(&onnestus);
                        if(!onnestus){  //Kui arvuks tegemine ei õnnestunud on vaja asendada koma punktiga, kasutame sama QString seeria't
                            llask.replace(',', '.');
                            lask = llask.toDouble(&onnestus);
                        }
                        lask *= 10;
                        //QString::number(lask);
                        lisaLasud << qRound(lask);
                }
                if(lisaLasud.count() != 24)
                        for(int i = lisaLasud.count(); i < 24; i++)
                                lisaLasud << -1;
        }else lisaAken->ui.lisaLasud->clear();
}

void Laskur::markuseMuutus()
{
    if(markus->text().isEmpty())
        markus->setText(tr("Märkused"));
}

void Laskur::muutus(QString)
{

        if(abi == Puudub || !*kirjutusAbi || summa->text().toInt() != 0 || muudetud) return;
        if(!andmebaas->kirjutusabiPuss && !andmebaas->kirjutusabiPustol) return;
        if(eesNimi->cursorPosition() < eesNimi->text().length()) return;
                        //Kontrolliks kas abi on üldse võimalik.
        static QString eelmine = "";	//Kui on tekst sama, mis eelminegi, siis järelikult kustutatakse.
        if(!eesNimi->text().isEmpty()){
                QString tekst = eesNimi->text().toLower();
                tekst[0] = tekst.at(0).toUpper();
                if(tekst.count("-") > 1){
                        for(int i = 0; i < tekst.length(); i++)
                                if(tekst.length() > (tekst.lastIndexOf("-") + 1))
                                        tekst[tekst.indexOf("-", i) + 1] = tekst[tekst.indexOf("-", i) + 1].toUpper();
                }else if(tekst.contains("-") && tekst.length() > (tekst.indexOf("-") + 1)){
                        tekst[tekst.indexOf("-") + 1] = tekst[tekst.indexOf("-") + 1].toUpper();
                }
                if(tekst.count(" ") > 1){
                        for(int i = 0; i < tekst.length(); i++)
                                if(tekst.length() > (tekst.lastIndexOf(" ") + 1))
                                        tekst[tekst.indexOf(" ", i) + 1] = tekst[tekst.indexOf(" ", i) + 1].toUpper();
                }else if(tekst.contains(" ") && tekst.length() > (tekst.indexOf(" ") +1)){
                        tekst[tekst.indexOf(" ") + 1] = tekst[tekst.indexOf(" ") + 1].toUpper();
                }
                eesNimi->setText(tekst);
        }
        QString tekst = eesNimi->text();
        if(eelmine.length() > tekst.length() + 1) eelmine = "";
//	QMessageBox::information(this, "Teade", "eelmine = " + eelmine + " ja tekst = " + tekst, "Selge");
        if(eelmine != tekst && eelmine.length() < tekst.length()){//Pikkuste võrdlus on samuti vajalik
                if(abi == Puss){										//kustutamisest aru saamiseks.
//			QMessageBox::information(this, "Teade", "abi == puss", "Selge");
                        if(andmebaas->kirjutusabiPuss){
//				QMessageBox::information(this, "Teade", "Otsin püssilaskurite seast", "Selge");
                                for(int i = 0; i < andmebaas->nimekiriPuss.count(); i++){
                                        if(tekst == andmebaas->nimekiriPuss[i]->eesnimi.mid(0, tekst.length())){
                                                                //Võrdleb nime kastis olevat andmebaasi nimede algustega.
                                                eesNimi->setText(andmebaas->nimekiriPuss[i]->eesnimi);
                                                perekNimi->setText(andmebaas->nimekiriPuss[i]->perekonnanimi);
                                                sunniAasta->setText(andmebaas->nimekiriPuss[i]->sunniaasta);
                                                klubi->setText(andmebaas->nimekiriPuss[i]->klubi);
                                                eesNimi->setSelection(tekst.length(), andmebaas->nimekiriPuss[i]->eesnimi
                                                                .length()-tekst.length());
                                                i = andmebaas->nimekiriPuss.count();
                                                eelmine = tekst;
                                                return;
                                        }/*else{	eemaldasin vältimaks eesnimede parandamisel andmekadu
                                                perekNimi->setText("");
                                                sunniAasta->setText("");
                                                klubi->setText("");
                                        }*/
                                }
                        }
                        if(andmebaas->kirjutusabiPustol){
                                if(tekst == eesNimi->text()){
//					QMessageBox::information(this, "Teade", "Otsin püstolilaskurite seast", "Selge");
                                        for(int i = 0; i < andmebaas->nimekiriPustol.count(); i++){
                                                if(tekst == andmebaas->nimekiriPustol[i]->eesnimi.mid(0, tekst.length())){
                                                                        //Võrdleb nime kastis olevat andmebaasi nimede algustega.
                                                        eesNimi->setText(andmebaas->nimekiriPustol[i]->eesnimi);
                                                        perekNimi->setText(andmebaas->nimekiriPustol[i]->perekonnanimi);
                                                        sunniAasta->setText(andmebaas->nimekiriPustol[i]->sunniaasta);
                                                        klubi->setText(andmebaas->nimekiriPustol[i]->klubi);
                                                        eesNimi->setSelection(tekst.length(), andmebaas->nimekiriPustol[i]->eesnimi
                                                                        .length()-tekst.length());
                                                        i = andmebaas->nimekiriPustol.count();
                                                        eelmine = tekst;
                                                        return;
                                                }/*else{	eemaldasin vältimaks eesnimede parandamisel andmekadu
                                                        perekNimi->setText("");
                                                        sunniAasta->setText("");
                                                        klubi->setText("");
                                                }*/
                                        }
                                }
                        }
                }
                else if(abi == Pustol){
//			QMessageBox::information(this, "Teade", "abi == pustol", "Selge");
                        if(andmebaas->kirjutusabiPustol){
//				QMessageBox::information(this, "Teade", "Otsin püstolilaskurite seast", "Selge");
                                for(int i = 0; i < andmebaas->nimekiriPustol.count(); i++){
                                        if(tekst == andmebaas->nimekiriPustol[i]->eesnimi.mid(0, tekst.length())){
                                                                //Võrdleb nime kastis olevat andmebaasi nimede algustega.
                                                eesNimi->setText(andmebaas->nimekiriPustol[i]->eesnimi);
                                                perekNimi->setText(andmebaas->nimekiriPustol[i]->perekonnanimi);
                                                sunniAasta->setText(andmebaas->nimekiriPustol[i]->sunniaasta);
                                                klubi->setText(andmebaas->nimekiriPustol[i]->klubi);
                                                eesNimi->setSelection(tekst.length(), andmebaas->nimekiriPustol[i]->eesnimi
                                                                .length()-tekst.length());
                                                i = andmebaas->nimekiriPustol.count();
                                                eelmine = tekst;
                                                return;
                                        }/*else{	eemaldasin vältimaks eesnimede parandamisel andmekadu
                                                perekNimi->setText("");
                                                sunniAasta->setText("");
                                                klubi->setText("");
                                        }*/
                                }
                        }
                        if(andmebaas->kirjutusabiPuss){
                                if(tekst == eesNimi->text()){
//					QMessageBox::information(this, "Teade", "Otsin püssilaskurite seast", "Selge");
                                        for(int i = 0; i < andmebaas->nimekiriPuss.count(); i++){
                                                if(tekst == andmebaas->nimekiriPuss[i]->eesnimi.mid(0, tekst.length())){
                                                                        //Võrdleb nime kastis olevat andmebaasi nimede algustega.
                                                        eesNimi->setText(andmebaas->nimekiriPuss[i]->eesnimi);
                                                        perekNimi->setText(andmebaas->nimekiriPuss[i]->perekonnanimi);
                                                        sunniAasta->setText(andmebaas->nimekiriPuss[i]->sunniaasta);
                                                        klubi->setText(andmebaas->nimekiriPuss[i]->klubi);
                                                        eesNimi->setSelection(tekst.length(), andmebaas->nimekiriPuss[i]->eesnimi
                                                                        .length()-tekst.length());
                                                        i = andmebaas->nimekiriPuss.count();
                                                        eelmine = tekst;
                                                        return;
                                                }/*else{	eemaldasin vältimaks eesnimede parandamisel andmekadu
                                                        perekNimi->setText("");
                                                        sunniAasta->setText("");
                                                        klubi->setText("");
                                                }*/
                                        }
                                }
                        }
//			eelmine = tekst;
                }
        }
        eelmine = tekst;
}

void Laskur::muutus2(QString)
{
    if(abi == Puudub || !*kirjutusAbi) return;
    if(perekNimi->cursorPosition() < perekNimi->text().length()){
            int pos = perekNimi->cursorPosition();
            perekNimi->setText(perekNimi->text().toUpper());
            perekNimi->setCursorPosition(pos);
            return;
    }
    if(!andmebaas->kirjutusabiPuss && !andmebaas->kirjutusabiPustol) return;    //Kontrolliks kas abi on üldse võimalik.
            static QString eelmine = "";
            QString tekst = perekNimi->text().toUpper();
            perekNimi->setText(tekst);
            if(eelmine.length() > tekst.length() + 1) eelmine = "";
//		QMessageBox::information(this, "Teade", "eelmine = " + eelmine + " ja tekst = " + tekst, "Selge");
            if(eelmine != tekst && eelmine.length() < tekst.length()){
                    if(abi == Puss){
                            if(andmebaas->kirjutusabiPuss){
//					QMessageBox::information(this, "Teade", "Otsin püssilaskurite seast", "Selge");
                                    for(int i = 0; i < andmebaas->nimekiriPuss.count(); i++){
                                            if(eesNimi->text() == andmebaas->nimekiriPuss[i]->eesnimi &&
                                                            tekst == andmebaas->nimekiriPuss[i]->perekonnanimi.mid(0, tekst.length())){
                                                    perekNimi->setText(andmebaas->nimekiriPuss[i]->perekonnanimi);
                                                    sunniAasta->setText(andmebaas->nimekiriPuss[i]->sunniaasta);
                                                    klubi->setText(andmebaas->nimekiriPuss[i]->klubi);
                                                    perekNimi->setSelection(tekst.length(), andmebaas->nimekiriPuss[i]->
                                                                    perekonnanimi.length()-tekst.length());
                                                    i = andmebaas->nimekiriPuss.count();
                                                    eelmine = tekst;
                                                    return;
                                            }else{
                                                    sunniAasta->setText("");
                                                    klubi->setText("");
                                            }
                                    }
                            }
                            if(andmebaas->kirjutusabiPustol){
                                    if(tekst.length() == perekNimi->text().length()){
//						QMessageBox::information(this, "Teade", "Otsin püstolilaskurite seast", "Selge");
                                            for(int i = 0; i < andmebaas->nimekiriPustol.count(); i++){
                                                    if(eesNimi->text() == andmebaas->nimekiriPustol[i]->eesnimi &&
                                                                    tekst == andmebaas->nimekiriPustol[i]->perekonnanimi.mid(0,
                                                                                    tekst.length())){
                                                            perekNimi->setText(andmebaas->nimekiriPustol[i]->perekonnanimi);
                                                            sunniAasta->setText(andmebaas->nimekiriPustol[i]->sunniaasta);
                                                            klubi->setText(andmebaas->nimekiriPustol[i]->klubi);
                                                            perekNimi->setSelection(tekst.length(), andmebaas->nimekiriPustol[i]->
                                                                            perekonnanimi.length()-tekst.length());
                                                            i = andmebaas->nimekiriPustol.count();
                                                            eelmine = tekst;
                                                            return;
                                                    }else{
                                                            sunniAasta->setText("");
                                                            klubi->setText("");
                                                    }
                                            }
                                    }
                            }
//				eelmine = tekst;
                    }
                    if(abi == Pustol){
                            if(andmebaas->kirjutusabiPustol){
                                    for(int i = 0; i < andmebaas->nimekiriPustol.count(); i++){
                                            if(eesNimi->text() == andmebaas->nimekiriPustol[i]->eesnimi &&
                                                            tekst == andmebaas->nimekiriPustol[i]->perekonnanimi.mid(0,
                                                                            tekst.length())){
                                                    perekNimi->setText(andmebaas->nimekiriPustol[i]->perekonnanimi);
                                                    sunniAasta->setText(andmebaas->nimekiriPustol[i]->sunniaasta);
                                                    klubi->setText(andmebaas->nimekiriPustol[i]->klubi);
                                                    perekNimi->setSelection(tekst.length(), andmebaas->nimekiriPustol[i]->
                                                                    perekonnanimi.length()-tekst.length());
                                                    i = andmebaas->nimekiriPustol.count();
                                                    eelmine = tekst;
                                                    return;
                                            }else{
                                                    sunniAasta->setText("");
                                                    klubi->setText("");
                                            }
                                    }
                            }
                            if(andmebaas->kirjutusabiPuss){
                                    if(tekst.length() == perekNimi->text().length()){
                                            for(int i = 0; i < andmebaas->nimekiriPuss.count(); i++){
                                                    if(eesNimi->text() == andmebaas->nimekiriPuss[i]->eesnimi &&
                                                                    tekst == andmebaas->nimekiriPuss[i]->perekonnanimi.mid(0,
                                                                                    tekst.length())){
                                                            perekNimi->setText(andmebaas->nimekiriPuss[i]->perekonnanimi);
                                                            sunniAasta->setText(andmebaas->nimekiriPuss[i]->sunniaasta);
                                                            klubi->setText(andmebaas->nimekiriPuss[i]->klubi);
                                                            perekNimi->setSelection(tekst.length(), andmebaas->nimekiriPuss[i]->
                                                                            perekonnanimi.length()-tekst.length());
                                                            i = andmebaas->nimekiriPuss.count();
                                                            eelmine = tekst;
                                                            return;
                                                    }else{
                                                            sunniAasta->setText("");
                                                            klubi->setText("");
                                                    }
                                            }
                                    }
                            }
//				eelmine = tekst;
                    }
            }
            eelmine = tekst;
}

void Laskur::liida() //Laskude summeerimine
{
        int sum = 0;    //Summa täisarvuna
        double fsum = 0; //Summa ujukomakohaarvuna
        int ssum = 0;   //Seeria summa täisarvuna
        double fssum = 0;  //Seeria summa ujukomakohaarvuna
        int lastudSeeriad = 0;
        int lastudLasud = 0;
        int siseKumneid = 0;
//        bool komaga = false;
        bool onnestus = false;
        bool onLasud = false;
        QString seeria;

        for(int i = 0; i < seeriad.size(); i++){
            ssum = 0;
            onLasud = false;
            for(int j = 0; j < lasud[i].count(); j++)   //Liitmisel liidetakse ka karistus juurde
                if(lasud[i][j]->getILask() != -999){
//                    qDebug() << "lasud[" << i << "][" << j << "] = " << lasud[i][j]->getILask();
                    if(*kumnendikega){
                        ssum += qRound(lasud[i][j]->getFLask() * 10);
                    }else{
                        ssum += lasud[i][j]->getILask() * 10;
                    }
                    if(lasud[i][j]->isInnerTen())
                        siseKumneid++;
                    onLasud = true;
                    lastudLasud++;
                }
            if(onLasud){
                fssum = ssum;
                seeriad[i]->setText(QString("%1").arg(fssum / 10));
                seeriad[i]->setText(seeriad[i]->text().replace('.', ','));
                if(*kumnendikega && !seeriad[i]->text().contains(',') && !seeriad[i]->text().contains('.'))
                    seeriad[i]->setText(seeriad[i]->text() + ",0");
                seeriad[i]->setReadOnly(true);
                seeriad[i]->installEventFilter(this);
                kumned->setText(QString("%1").arg(siseKumneid));
                kumned->setReadOnly(true);
//                kumned->setEnabled(false);
//                connect(seeriad[i], SIGNAL(doubleClicked()), this, SLOT(naitaLaskudeAkent()));
            }else{
                seeriad[i]->setReadOnly(false);
                kumned->setReadOnly(false);
            }
            if(!seeriad[i]->text().isEmpty()){
                if(!onLasud)
                    lastudLasud += 10;  //Kui on käsitsi seeria kirjutatud, siis ilmselt on 10 lasku lastud
                sum += qRound(seeriad[i]->text().toFloat(&onnestus) * 10);
                if(!onnestus){  //Kui arvuks tegemine ei õnnestunud on vaja asendada koma punktiga
                    seeria = seeriad[i]->text();
                    seeria.replace(',', '.');
                    sum += qRound(seeria.toFloat(&onnestus) * 10);
                }
                lastudSeeriad++;
                seeriad[i]->installEventFilter(this);
//                if(seeriad[i]->text().contains(','))
//                    komaga = true;
            }
        }
        siseKumneid = 0;
        fsum = sum;
        summa->setText(QString("%1").arg(fsum / 10));
        if(*kumnendikega && !summa->text().contains(',') &&!summa->text().contains('.'))
            summa->setText(summa->text() + ",0");

        if(lastudSeeriad == 0){
                keskmLask = 0;
                koguSumma->setText("0,0");
                for(int i = 0; i < vSummad.count(); i++)
                    vSummad[i]->setText("0");
                return;
        }
//        if(finaal->text() == "Fin")
        koguSumma->setText(summa->text());  //Kuna finaali põhisummale ei liideta
//        else{
//            koguSumma->setText(QString("%1").arg((fsum + finaal->text().toFloat(&onnestus) * 10) / 10));
//            if(!onnestus){  //Kui arvuks tegemine ei õnnestunud on vaja asendada koma punktiga, kasutame sama QString seeria't
//                seeria = finaal->text();
//                seeria.replace(',', '.');
//                koguSumma->setText(QString("%1").arg((fsum + seeria.toFloat(&onnestus) * 10) / 10));
//            }
//        }
        sum *= 1000;
        if(finaal->text().isEmpty()) finaal->setText("Fin");    //Kui lahter on tühi, tuleb taastada algseis
        if(finaal->text() != "Fin"){
            sum += qRound(finaal->text().toFloat(&onnestus) * 10000);
            if(!onnestus){  //Kui arvuks tegemine ei õnnestunud on vaja asendada koma punktiga, kasutame sama QString seeria't
                seeria = finaal->text();
                seeria.replace(',', '.');
                sum += qRound(seeria.toFloat(&onnestus) * 10000);
            }
        }
        keskmLask = sum / lastudLasud;
//            lastudSeeriad *= 10;
//            keskmLask = sum / lastudSeeriad;
        if(finaal->text().contains('.') || koguSumma->text().contains('.') || summa->text().contains('.')){
                QString uus = finaal->text();
                uus.replace('.', ',');
                finaal->setText(uus);
                uus = koguSumma->text();
                uus.replace('.', ',');
                koguSumma->setText(uus);
                uus = summa->text();
                uus.replace('.', ',');
                summa->setText(uus);
        }
        if(finaal->text() != "Fin" && !finaal->text().contains(',', Qt::CaseInsensitive))
                finaal->setText(finaal->text().append(",0"));
        if(finaal->text() != "Fin" && !koguSumma->text().contains(',', Qt::CaseInsensitive))
                koguSumma->setText(koguSumma->text().append(",0"));
        if(vSummadeSamm == 0) return;
        int x = 0;
        for(int i = 0; i < seeriateArv / vSummadeSamm; i++){
                sum = 0;
                for(int j = 0; j < vSummadeSamm; j++){
                        sum += qRound(seeriad[x]->text().toFloat(&onnestus) * 10);
                        if(!onnestus){  //Kui arvuks tegemine ei õnnestunud on vaja asendada koma punktiga, kasutame sama QString seeria't
                            seeria = seeriad[x]->text();
                            seeria.replace(',', '.');
                            sum += qRound(seeria.toFloat(&onnestus) * 10);
                        }
                        x++;
                }
                fsum = sum;
                vSummad[i]->setText(QString("%1").arg(fsum / 10));
                if(*kumnendikega && !vSummad[i]->text().contains(',') &&!vSummad[i]->text().contains('.'))
                    vSummad[i]->setText(vSummad[i]->text() + ",0");
        }
//        if(summa->text().compare(eelmineSumma) != 0)
//            emit muudatus();
}

void Laskur::muutus4(QString)
{
        if(abi == Puudub || !*kirjutusAbi) return;
        if(!andmebaas->kirjutusabiPuss && !andmebaas->kirjutusabiPustol) return;
        if(klubi->cursorPosition() < klubi->text().length()) return;
        static QString eelmine = "";
        QString tekst = klubi->text();
        if(eelmine.length() > tekst.length() + 1) eelmine = "";
        if(eelmine != tekst && eelmine.length() < tekst.length()){
                if(abi == Puss){
                        if(andmebaas->kirjutusabiPuss){
                                for(int i = 0; i < andmebaas->nimekiriPuss.count(); i++){
                                        if(eesNimi->text() == andmebaas->nimekiriPuss[i]->eesnimi &&
                                                        perekNimi->text() == andmebaas->nimekiriPuss[i]->perekonnanimi &&
                                                        sunniAasta->text() == andmebaas->nimekiriPuss[i]->sunniaasta)
                                                return;
                                }
                        }
                        if(andmebaas->kirjutusabiPustol){
                                if(tekst.length() == perekNimi->text().length()){
                                        for(int i = 0; i < andmebaas->nimekiriPustol.count(); i++){
                                                if(eesNimi->text() == andmebaas->nimekiriPustol[i]->eesnimi &&
                                                                perekNimi->text() == andmebaas->nimekiriPustol[i]->perekonnanimi&&
                                                                sunniAasta->text() == andmebaas->nimekiriPustol[i]->sunniaasta)
                                                        return;
                                        }
                                }
                        }
                }
                if(abi == Pustol){
                        if(andmebaas->kirjutusabiPustol){
                                for(int i = 0; i < andmebaas->nimekiriPustol.count(); i++){
                                        if(eesNimi->text() == andmebaas->nimekiriPustol[i]->eesnimi &&
                                                        perekNimi->text() == andmebaas->nimekiriPustol[i]->perekonnanimi&&
                                                        sunniAasta->text() == andmebaas->nimekiriPustol[i]->sunniaasta)
                                                return;
                                }
                        }
                        if(andmebaas->kirjutusabiPuss){
                                if(tekst.length() == perekNimi->text().length()){
                                        for(int i = 0; i < andmebaas->nimekiriPuss.count(); i++){
                                                if(eesNimi->text() == andmebaas->nimekiriPuss[i]->eesnimi &&
                                                                perekNimi->text() == andmebaas->nimekiriPuss[i]->perekonnanimi &&
                                                                sunniAasta->text() == andmebaas->nimekiriPuss[i]->sunniaasta)
                                                        return;
                                        }
                                }
                        }
                }
                for(int i = 0; i < andmebaas->nimekiriPuss.count(); i++){
                        if(tekst == andmebaas->nimekiriPuss[i]->klubi.mid(0, tekst.length())){
                                klubi->setText(andmebaas->nimekiriPuss[i]->klubi);
                                klubi->setSelection(tekst.length(), andmebaas->nimekiriPuss[i]->klubi.length()-tekst
                                                .length());
                                i = andmebaas->nimekiriPuss.count();
                        }
                }
                if(klubi->text().length() == tekst.length())
                        for(int i = 0; i < andmebaas->nimekiriPustol.count(); i++){
                                if(tekst == andmebaas->nimekiriPustol[i]->klubi.mid(0, tekst.length())){
                                        klubi->setText(andmebaas->nimekiriPustol[i]->klubi);
                                        klubi->setSelection(tekst.length(), andmebaas->nimekiriPustol[i]->klubi.length()-tekst
                                                        .length());
                                        i = andmebaas->nimekiriPustol.count();
                                }
                        }
        }
        eelmine = tekst;
}

void Laskur::muutus5()
{
        bool ok = false;
        kumned->text().toInt(&ok);
        if(kumned->text().isEmpty() || !ok) kumned->setText("0");
        liida();
}

void Laskur::naitaIdAken()
{
    bool ok = false;
    int uusId = QInputDialog::getInt(this, "Sisestage uus laskuri ID", "ID:", this->id, 0, 9999, 1, &ok);
    if(ok)
        emit idMuutus(uusId, this);
//        this->id = uusId;
}

void Laskur::naitaLaskudeAkent()
{
//#ifdef PROOV
//    qDebug() << "naitaLaskudeAkent()";
//#endif
    if(!laskudeAken){
        laskudeAken = new LaskudeAken(kumnendikega, seeriateArv, laskudeArv, this);
    }

    laskudeAken->muudaPealkirja(eesNimi->text() + " " + perekNimi->text());

    for(int i = 0; i < seeriateArv; i++){
        if(seeriad[i]->hasFocus())
            laskudeAken->aktiivneSeeria = i;    //Teeb laskude aknas aktiivseks selle seeria, millele klikiti
        for(int j = 0; j < lasud[0].count(); j++){    //Karistuse lask on vaja ka kopeerida, seetõttu on lasud.count, mitte laskudeArv
            laskudeAken->lasud[i][j]->set(lasud[i][j]);
        }
    }
    laskudeAken->loeLasud();

    if(laskudeAken->exec() == QDialog::Accepted){
        laskudeAken->kirjutaLasud();
        emit muudatus();
        for(int i = 0; i < seeriateArv; i++)
            for(int j = 0; j < lasud[0].count(); j++){ //Karistuse lask on vaja ka kopeerida
                lasud[i][j]->set(laskudeAken->lasud[i][j]);
            }
    }
    liida();
}

void Laskur::naitaSifrit()
{
    emit sifrimuutus();
}

/*bool Laskur::operator<(const Laskur &l) const
{
        if(this->keskmLask != l.keskmLask){
                if(this->keskmLask < l.keskmLask)
                        return true;
                else return false;
        }
        if(this->kumned->text() != l.kumned->text()){
                if(this->kumned->text().toInt() < l.kumned->text().toInt())
                        return true;
                else return false;
        }
        if(this->summa->text().toInt() == l.summa->text().toInt()){
                for(int i = 0; i < seeriateArv; i++){
                        if(this->seeriad[seeriateArv-(i+1)]->text().isEmpty() &&
                                        !l.seeriad[l.seeriateArv-(i+1)]->text().isEmpty())
                                return true;
                        else if(!this->seeriad[seeriateArv-(i+1)]->text().isEmpty() &&
                                        l.seeriad[l.seeriateArv-(i+1)]->text().isEmpty())
                                return false;
                        if(!this->seeriad[seeriateArv-(i+1)]->text().isEmpty() &&
                                        !l.seeriad[l.seeriateArv-(i+1)]->text().isEmpty() &&
                                        this->seeriad[seeriateArv-(i+1)]->text().toInt() !=
                                        l.seeriad[l.seeriateArv-(i+1)]->text().toInt()){
                                if(this->seeriad[seeriateArv-(i+1)]->text().toInt() <
                                                l.seeriad[l.seeriateArv-(i+1)]->text().toInt())
                                        return true;
                                else if(this->seeriad[seeriateArv-(i+1)]->text().toInt() >
                                        l.seeriad[l.seeriateArv-(i+1)]->text().toInt())
                                        return false;
                        }
                }
                if(this->perekNimi->text().localeAwareCompare(l.perekNimi->text()) < 0)
                                return true;
                else if(this->perekNimi->text().localeAwareCompare(l.perekNimi->text()) > 0)
                        return false;
                else{
                        if(this->eesNimi->text().localeAwareCompare(l.eesNimi->text()) < 0)
                                return true;
                        else return false;
                }
        }
        if(this->summa->text().toInt() < l.summa->text().toInt())
                return true;
        else if(this->summa->text().toInt() > l.summa->text().toInt())
                return false;
}*/

bool Laskur::vaiksem(Laskur *l, int t) const    //Kas see laskur on väiksem, kui teine
{
    if(veryVerbose)
        QTextStream(stdout) << "Laskur::vaiksem(): t = " << t << ", " << this->perekNimi->text() << ", " << l->perekNimi->text() << endl;
    QString seeria;
    bool onnestus = false;
    //Kõigepealt viiakse diskvalifitseeritud laskurid lõppu
    if(this->markus->text().contains("DSQ", Qt::CaseInsensitive) && !l->markus->text().contains("DSQ", Qt::CaseInsensitive)){
        if(l->summa->text() == "0" || l->summa->text() == "0,0")
            return false;
        else return true;
    }
    if(!this->markus->text().contains("DSQ", Qt::CaseInsensitive) && l->markus->text().contains("DSQ", Qt::CaseInsensitive)){
        if(this->summa->text() == "0" || this->summa->text() == "0,0")
            return true;
        else return false;
    }
    //Seejärel katkestanud laskurid
    if(this->markus->text().contains("DNF", Qt::CaseInsensitive) && !l->markus->text().contains("DNF", Qt::CaseInsensitive)){
        if(l->summa->text() == "0" || l->summa->text() == "0,0")
            return false;
        else return true;
    }
    if(!this->markus->text().contains("DNF", Qt::CaseInsensitive) && l->markus->text().contains("DNF", Qt::CaseInsensitive)){
        if(this->summa->text() == "0" || this->summa->text() == "0,0")
            return true;
        else return false;
    }
    //Ning siis v.a. laskurid
    if(this->markus->text().contains("V.A", Qt::CaseInsensitive) && !l->markus->text().contains("V.A", Qt::CaseInsensitive)){
        if(l->summa->text() == "0" || l->summa->text() == "0,0")
            return false;
        else return true;
    }
    if(!this->markus->text().contains("V.A", Qt::CaseInsensitive) && l->markus->text().contains("V.A", Qt::CaseInsensitive)){
        if(this->summa->text() == "0" || this->summa->text() == "0,0")
            return true;
        else return false;
    }

    switch(t){
    case 1: {   //1 - perekonnanimede järgi reastamine
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Perekonnanimede järgi - " << this->perekNimi->text() << ", " << l->perekNimi->text() << endl;
        if(this->perekNimi->text().localeAwareCompare(l->perekNimi->text()) > 0)
            return true;
        else if(this->perekNimi->text().localeAwareCompare(l->perekNimi->text()) < 0)
            return false;
        else{
            if(this->eesNimi->text().localeAwareCompare(l->eesNimi->text()) > 0)
                return true;
            else return false;
        }
    }
    case 2: { //2 - sünniaastate järgi reastamine
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Sünniaastate järgi - " << this->perekNimi->text() << ", " << l->perekNimi->text() << endl;
        if(this->sunniAasta->text().toInt() < l->sunniAasta->text().toInt())
            return true;
        else if(this->sunniAasta->text().toInt() > l->sunniAasta->text().toInt())
            return false;
        else{
            if(this->perekNimi->text().localeAwareCompare(l->perekNimi->text()) > 0)
                return true;
            else if(this->perekNimi->text().localeAwareCompare(l->perekNimi->text()) < 0)
                return false;
            else{
                if(this->eesNimi->text().localeAwareCompare(l->eesNimi->text()) > 0)
                    return true;
                else return false;
            }
        }
    }
    case 3: { //3 - raja nr'ite järgi reastamine
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Raja nr'ite järgi - " << this->perekNimi->text() << ", " << l->perekNimi->text() << endl;

        int tulemus = vaiksemRajaga(l);

        switch(tulemus){    //Kui on 0, siis läheb edasi nimede võrdlemise juurde
        case -1 : return true;
        case 1 : return false;
        }

        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Raja nr'id samad" << endl;
        tulemus = vaiksemNimega(l);

        switch(tulemus){
        case -1 : return true;
        case 1 : return false;
        default: return false;
        }
    }
    case 4: { //4 - sifrite järgi reastamine
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Sifrite järgi - " << this->perekNimi->text() << ", " << l->perekNimi->text() << endl;
        if(this->sifriAlgus->text().toInt() < l->sifriAlgus->text().toInt()){
            if(this->sifriAlgus->text().isEmpty()) return true;
            else return false;
        }
        else if(this->sifriAlgus->text().toInt() > l->sifriAlgus->text().toInt()){
            if(l->sifriAlgus->text().isEmpty()) return false;
            else return true;
        }
        else{
            if(this->perekNimi->text().localeAwareCompare(l->perekNimi->text()) > 0)
                return true;
            else if(this->perekNimi->text().localeAwareCompare(l->perekNimi->text()) < 0)
                return false;
            else{
                if(this->eesNimi->text().localeAwareCompare(l->eesNimi->text()) > 0)
                    return true;
                else return false;
            }
        }
    }
    default: {   //Kui ei ole eritingimusega sorteerimist, vaadatakse kõigepealt finaali seeriat
//            liida();
//            l->liida();
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Üldine järgi - " << this->perekNimi->text() << ", " << l->perekNimi->text() << endl;
        if(this->finaal->text() != "Fin" && l->finaal->text() != "Fin"){
            float ksum = this->finaal->text().toFloat(&onnestus);
            if(!onnestus){  //Kui arvuks tegemine ei õnnestunud on vaja asendada koma punktiga, kasutame sama QString seeria't
                seeria = this->finaal->text();
                seeria.replace(',', '.');
                ksum = seeria.toFloat(&onnestus);
            }
            ksum *= 10;
            float ksum2 = l->finaal->text().toFloat(&onnestus);
            if(!onnestus){  //Kui arvuks tegemine ei õnnestunud on vaja asendada koma punktiga, kasutame sama QString seeria't
                seeria = l->finaal->text();
                seeria.replace(',', '.');
                ksum2 = seeria.toFloat(&onnestus);
            }
            ksum2 *= 10;
            int sum = int(ksum), sum2 = int(ksum2);
            if(sum != sum2){
                if(sum < sum2)
                    return true;
                else return false;
            }else{  //Kui finaaliseeriad on võrdsed, peavad olema tehtud lisalasud
#ifdef PROOV
                qDebug() << "Laskur::vaiksem(), finaalid võrdsed, this->lisaLasud.count() = " << this->lisaLasud.count() << ", l->lisaLasud.count() = " << l->lisaLasud.count();
#endif
                if(this->lisaLasud.count() == 0 && l->lisaLasud.count() != 0)   //Juhul, kui finaali seeriad on võrdsed, aga millegipärast lisalaske ei ole
                    return true;
                if(l->lisaLasud.count() == 0)
                    return false;
#ifdef PROOV
                qDebug() << "Laskur::vaiksem(), vaadatakse lisalaske";
#endif
                for(int k = 0; k < this->lisaLasud.count(); k++){
                    if(this->lisaLasud[k] != l->lisaLasud[k]){
                        if(this->lisaLasud[k] < l->lisaLasud[k])
                            return true;
                        else return false;
                    }
                }
            }
            //Kui ühel laskuril on finaali seeria ja teisel ei ole, on ees see, kellel on seeria, kuna ta oli finaalis
        }else if(this->finaal->text() != "Fin" && l->finaal->text() == "Fin"){
            return false;
        }else if(this->finaal->text() == "Fin" && l->finaal->text() != "Fin"){
            return true;
        }

        if(this->keskmLask != l->keskmLask){    //Järgmiseks vaadatakse keskmist lasku
            if(this->keskmLask < l->keskmLask)
                return true;
            else return false;
        }
        if(!*kumnendikega && *jarjestamine == KumneteArvuga){ //Kui loetakse täisarvudega, siis vaadatakse kõigepealt sisekümneid
            if(this->kumned->text() != l->kumned->text()){  //Kui keskmine lask on võrdne ja loetakse täisarvudega, vaadatakse
                if(this->kumned->text().toInt() < l->kumned->text().toInt())                        //sisekümnete arvu
                    return true;
                else return false;
            }
        }
        //Kui ka sisekümned ja summad on võrdsed, vaadatakse seeriaid alustades viimasest ja liikudes ettepoole
//                if(this->summa->text().toInt() == l->summa->text().toInt()){
        for(int i = 0; i < seeriateArv; i++){
            if(this->seeriad[seeriateArv-(i+1)]->text().isEmpty() && !l->seeriad[l->seeriateArv-(i+1)]->text().isEmpty())
                return true;
            else if(!this->seeriad[seeriateArv-(i+1)]->text().isEmpty() && l->seeriad[l->seeriateArv-(i+1)]->text().isEmpty())
                return false;
            if(!this->seeriad[seeriateArv-(i+1)]->text().isEmpty() && !l->seeriad[l->seeriateArv-(i+1)]->text().isEmpty() && this->seeriad[seeriateArv-(i+1)]->text().replace(',',"").replace('.',"").toInt() != l->seeriad[l->seeriateArv-(i+1)]->text().replace(',',"").replace('.',"").toInt()){
                if(this->seeriad[seeriateArv-(i+1)]->text().replace(',',"").replace('.',"").toInt() < l->seeriad[l->seeriateArv-(i+1)]->text().replace(',',"").replace('.',"").toInt())
                    return true;
                else if(this->seeriad[seeriateArv-(i+1)]->text().replace(',',"").replace('.',"").toInt() > l->seeriad[l->seeriateArv-(i+1)]->text().replace(',',"").replace('.',"").toInt())
                    return false;
            }
        }

        if(*kumnendikega || *jarjestamine == ViimaseSeeriaga){  //Kui on komakohtadega seeriad võrdsed või täisarvudega seeriate puhul on valitud viimase seeria järgi reastamine
            if(this->kumned->text() != l->kumned->text()){  //Kui loetakse komakohtadega, vaadatakse kõigepealt seeriaid ja siis alles sisekümnete arvu
                if(this->kumned->text().toInt() < l->kumned->text().toInt())
                    return true;
                else return false;
            }
        }

        int tulemus = vaiksemNimega(l);

        switch(tulemus){    //Kui on 0, siis läheb edasi sifrite võrdlemise juurde
        case -1 : return true;
        case 1 : return false;
        }

        //Kui mõlemad perekonnanimed on tühjad, reastatakse sifri järgi
        if(this->perekNimi->text().isEmpty() && l->perekNimi->text().isEmpty()){
            if(this->sifriAlgus->text().isEmpty() && !l->sifriAlgus->text().isEmpty())
                return true;
            else if(!this->sifriAlgus->text().isEmpty() && l->sifriAlgus->text().isEmpty())
                return false;
            else if(!this->sifriAlgus->text().isEmpty() && !l->sifriAlgus->text().isEmpty()){
                if(this->sifriAlgus->text().toInt() < l->sifriAlgus->text().toInt())
                    return false;
                else return true;
            }
            //Kui kummalgi sifrit ei ole, reastatakse rajanr'i järgi
            else if(this->sifriAlgus->text().isEmpty() && l->sifriAlgus->text().isEmpty() && (!this->rajaNr->text().isEmpty() || !l->rajaNr->text().isEmpty())){
                if(this->rajaNr->text().toInt() < l->rajaNr->text().toInt())
                    return true;
                else if(this->rajaNr->text().toInt() > l->rajaNr->text().toInt())
                    return false;
            }
        }
//                }

        //Kui kõik eelnev ei andnud tulemust, siis vaadatakse summat (peaks olema väga harv juhus)
        if(this->summa->text().toInt() < l->summa->text().toInt())
            return true;
        else if(this->summa->text().toInt() > l->summa->text().toInt())
            return false;

                //Kõige lõpus tagastatakse "false" et vältida lõputut sorteerimist võrdsete laskurite puhul
        return false;
    }
    }
    return false;
}

void Laskur::nextCompetitionStage()
{
    m_competitionStage++;
}

QString Laskur::previousSiusRow() const
{
    return m_previousSiusRow;
}

void Laskur::resetCompetitionStage()
{
    m_competitionStage = 0;
}

void Laskur::set(const Laskur *l)
{
    //Laskuri kopeerimise funktsioon
    int seeriaid = 0;
    int laske = 0;
    if(this->seeriateArv > l->seeriateArv){ //Kui selle laskuri seeriate arv on suurem, kui teisel, tuleb üleliigsed seeriad tühjendada
        seeriaid = l->seeriateArv;
        for(int i = 0; i < seeriateArv; i++)
            this->seeriad[i]->setText("");
    }else seeriaid = seeriateArv;

    if(this->laskudeArv > l->laskudeArv){   //Kui selle laskuri laskude arv on suurem, kui teisel, tuleb üleliigsed lasud tühjendada
        laske = l->laskudeArv;
        for(int i = 0; i < seeriateArv; i++)
            for(int j = 0; j < lasud[0].count(); j++){
                this->lasud[i][j]->clear();
            }
    }else laske = laskudeArv;

    this->onLehelugejaLaskur = l->onLehelugejaLaskur;
    this->onVorguLaskur = l->onVorguLaskur;
    this->m_competitionStage = l->competitionStage();
    this->m_competitionStarted = l->isCompetitionStarted();
    this->m_previousSiusRow = l->previousSiusRow();
    this->m_siusConnectionIndex = l->siusConnectionIndex();
    this->linnuke->setChecked(l->linnuke->isChecked());
    this->rajaNr->setText(l->rajaNr->text().trimmed());
    this->sifriAlgus->setText(l->sifriAlgus->text().trimmed());
    this->sifriLopp->setText(l->sifriLopp->text().trimmed());
    if(l->id != 0)  //Kui ID == 0, siis on ilmselt tegu importimisega
        this->id = l->id;
    this->eesNimi->setText(l->eesNimi->text().trimmed());
    this->perekNimi->setText(l->perekNimi->text().trimmed());
    this->sunniAasta->setText(l->sunniAasta->text().trimmed());
    this->klubi->setText(l->klubi->text().trimmed());
    this->summa->setText(l->summa->text().trimmed());
    this->finaal->setText(l->finaal->text().trimmed());
    this->kumned->setText(l->kumned->text().trimmed());
    this->markus->setText(l->markus->text().trimmed());
    for(int i = 0; i < seeriaid; i++){
        this->seeriad[i]->setText(l->seeriad[i]->text().trimmed());
        for(int j = 0; j < laske; j++)
            this->lasud[i][j]->set(l->lasud[i][j]);
    }
    this->lisaLasud.clear();
    for(int i = 0; i < 24; i++)
        this->lisaLasud << l->lisaLasud[i];
    this->liida();
}

void Laskur::setCompetitionStarted(bool competitionStatus)
{
    m_competitionStarted = competitionStatus;
}

void Laskur::setPreviousSiusRow(QString row)
{
    m_previousSiusRow = row;
}

void Laskur::setSiusConnectionIndex(int newIndex)
{
    m_siusConnectionIndex = newIndex;
}

void Laskur::setSumma(QString s)
{
    summa->setText(s);
}

int Laskur::siusConnectionIndex() const
{
    return m_siusConnectionIndex;
}

void Laskur::siusiReset(int siusConnectionIndex)
{
    if(siusConnectionIndex == m_siusConnectionIndex){
        previousSiusRow().clear();
        m_competitionStage = 0;
        m_competitionStarted = false;
        this->m_siusConnectionIndex = -1;
    }
}

void Laskur::teataMuudatusest(const QString)
{
    emit muudatus();
}

void Laskur::teataMuudatusest()
{
    emit muudatus();
}

QJsonObject Laskur::toExportJson()
{
    QJsonObject json;
    json["id"] = QString("%1").arg(id);
    json["firstName"] = eesNimi->text();
    json["lastName"] = perekNimi->text();
    json["birthYear"] = sunniAasta->text();
    json["club"] = klubi->text();
    QJsonArray seriesArray;
    int index = 0;
    foreach (QList<Lask*> series, lasud){
        QJsonObject seriesJson;
        QJsonArray shotsArray;
        foreach (Lask *shot, series)
            shotsArray.append(shot->toJson());
        seriesJson["seriesShots"] = shotsArray;
        seriesJson["seriesSum"] = seeriad.at(index)->text();
        index++;
        seriesArray.append(seriesJson);
    }
    json["series"] = seriesArray;
    json["totalResult"] = summa->text();
    json["innerTens"] = kumned->text();
    json["finals"] = finaal->text();
    if(markus->text() == "Märkused")
        json["remarks"] = "";
    else
        json["remarks"] = markus->text();
    return json;
}

int Laskur::vaiksemNimega(Laskur *l) const //Tagastab kas laskur on nime järgi väiksem (-1), suurem (1) või võrdne (0)
{
    if(veryVerbose)
        QTextStream(stdout) << "Laskur::vaiksem(): Perekonnanimede võrdlus - " << this->perekNimi->text() << ", " << l->perekNimi->text() << endl;
    //Tühja perekonnanimega laskur reastatakse tahapoole
    if(this->perekNimi->text().isEmpty() && !l->perekNimi->text().isEmpty()){
        return -1;
    }else if(l->perekNimi->text().isEmpty() && !this->perekNimi->text().isEmpty()){
        return 1;
    }else if(this->perekNimi->text().localeAwareCompare(l->perekNimi->text()) > 0){
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Perekonnanimede võrdlus - return true" << endl;
        return -1;
    }else if(this->perekNimi->text().localeAwareCompare(l->perekNimi->text()) < 0){
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Perekonnanimede võrdlus - return false" << endl;
        return 1;
    }else{
        if(this->eesNimi->text().localeAwareCompare(l->eesNimi->text()) > 0){
            if(veryVerbose)
                QTextStream(stdout) << "Laskur::vaiksem(): Eesnimede võrdlus - return true" << endl;
            return -1;
        }else return 1;
    }
    return 0;   //Kui jõudis siiani, siis ilmselt on võrdsed
}

int Laskur::vaiksemRajaga(Laskur *l) const   //Tagastab kas laskur on raja järgi väiksem (-1), suurem (1) või võrdne (0), kusjuures väiksem rajanr on eespool
{
    bool onnestus1, onnestus2 = false;
    if(this->rajaNr->text().isEmpty() && !l->rajaNr->text().isEmpty())
        return -1;  //Kui üks on tühi, läheb see tahapoole
    else if(!this->rajaNr->text().isEmpty() && l->rajaNr->text().isEmpty())
        return 1;   //Kui üks on tühi, läheb see tahapoole

    this->rajaNr->text().toInt(&onnestus1);
//        if(veryVerbose)
//            QTextStream(stdout) << "Laskur::vaiksem(): this->rajaNr->text().toInt(&onnestus1); " << this->rajaNr->text().toInt(&onnestus1) << this->perekNimi->text() << endl;
    l->rajaNr->text().toInt(&onnestus2);
//        if(veryVerbose)
//            QTextStream(stdout) << "Laskur::vaiksem(): l->rajaNr->text().toInt(&onnestus2); " << l->rajaNr->text().toInt(&onnestus2) << l->perekNimi->text() << endl;
    if(onnestus1 && !onnestus2 && !l->rajaNr->text().isEmpty()){
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Arv vs täht" << endl;
        return -1;   //Kui üks kast on arvuga ja teine tähega on väiksem arvuga kast
    }else if(onnestus2 && !onnestus1 && !this->rajaNr->text().isEmpty()){
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Täht vs arv" << endl;
        return 1;
    }else if(!onnestus1 && !onnestus2 && this->rajaNr->text().localeAwareCompare(l->rajaNr->text()) > 0){
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Täht vs täht" << endl;
        return -1;  //Kui on tähed finaali jaoks
    }else if(!onnestus1 && !onnestus2 && this->rajaNr->text().localeAwareCompare(l->rajaNr->text()) < 0){
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Täht vs täht 2" << endl;
        return 1;
    }else if(onnestus1 && onnestus2){ //Kui mõlemas kastis on arvuline raja nr
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Arv vs arv" << endl;
        if(this->rajaNr->text().toInt() < l->rajaNr->text().toInt()){
            if(this->rajaNr->text().isEmpty()) return -1; //Rajanumbrite puhul on vastupidi, väiksem nr on eespool
            else{
                if(veryVerbose)
                    QTextStream(stdout) << "Laskur::vaiksem(): Arv vs arv, " << this->perekNimi->text() << "<" << l->perekNimi->text() << endl;
                return 1;
            }
//                return true;
        }else if(this->rajaNr->text().toInt() > l->rajaNr->text().toInt()){
            if(l->rajaNr->text().isEmpty()) return 1; //Rajanumbrite puhul on vastupidi, väiksem nr on eespool
            else{
                if(veryVerbose)
                    QTextStream(stdout) << "Laskur::vaiksem(): Arv vs arv, " << this->perekNimi->text() << ">" << l->perekNimi->text() << endl;
                return -1;
            }
//                return false;
        }
    }else if(l->rajaNr->text().isEmpty()){
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): l on tühi - " << l->perekNimi->text() << endl;
        return 1;
    }else if(this->rajaNr->text().isEmpty()){
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): This on tühi - " << this->perekNimi->text() << endl;
        return -1;
    }
    return 0;   //Kui jõudis siiani, siis ilmselt on võrdsed
}

void Laskur::vajutaTab()
{
    QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
    QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyRelease, Qt::Key_Tab, Qt::NoModifier));
}

Laskur::~Laskur()
{
    if(!laskudeAken){
        laskudeAken->deleteLater();
        laskudeAken = nullptr;
    }
    if(!lisaAken){
        lisaAken->deleteLater();
        lisaAken = nullptr;
    }
}

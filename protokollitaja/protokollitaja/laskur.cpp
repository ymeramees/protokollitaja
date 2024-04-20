#include "laskur.h"

extern bool verbose;
extern bool veryVerbose;

Laskur::Laskur(Andmebaas* baas, int s, int vs, int a, bool *k, bool *kum, int i, int *jar, QualificationEvents::EventType *eventType, int ls, QWidget *parent)
    : QWidget(parent)
{
    setupFields();
    id = i;
        laskudeArv = ls;
        seeriateArv = s;
        vSummadeSamm = vs;
        andmebaas = baas;
        abi = a;
        kirjutusAbi = k;
        kumnendikega = kum;
        jarjestamine = jar;
        m_eventType = eventType;
//	connect(eesNimi, SIGNAL(textEdited(QString)), this, SLOT(muutus(QString)));
//	connect(perekNimi, SIGNAL(textEdited(QString)), this, SLOT(muutus2(QString)));


        for(int i = 0; i < seeriateArv; i++){
                seeriad << new QLineEdit(this);
                seeriad[i]->setMinimumHeight(28);
                seeriad[i]->setMaximumWidth(40);
                seeriad[i]->setToolTip(tr("Seeriad"));
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
                        vSummad[i]->setToolTip(tr("Vahesumma"));
                        vSummad[i]->setStyleSheet("border: 1px solid grey");
                        vSummad[i]->setReadOnly(true);
                }
        }

    createLayout();
//        this->setMaximumWidth(1200);
}

Laskur::Laskur(
    QJsonObject jsonObj,
    Andmebaas* autocompleteDb,
    int vs,
    int autocompleteAvailable,
    bool *autocomplete,
    bool *withDecimals,
    int *sorting,
    QualificationEvents::EventType *eventType,
    int numberOfShots,
    QWidget *parent
): QWidget(parent){
    setupFields();
    andmebaas = autocompleteDb;
    vSummadeSamm = vs;
    abi = autocompleteAvailable;
    kirjutusAbi = autocomplete;
    kumnendikega = withDecimals;
    jarjestamine = sorting;
    m_eventType = eventType;

    id = jsonObj["id"].toInt();
    rajaNr->setText(jsonObj["targetNo"].toString());
    sifriAlgus->setText(jsonObj["sifferStart"].toString());
    sifriLopp->setText(jsonObj["sifferEnd"].toString());
    eesNimi->setText(jsonObj["firstName"].toString());
    perekNimi->setText(jsonObj["familyName"].toString());
    sunniAasta->setText(jsonObj["yearOfBirth"].toString());
    klubi->setText(jsonObj["club"].toString());

    QString finalsTotalString = jsonObj["finalsTotal"].toString();
    if (finalsTotalString == "Fin")
        finaal->setText("");
    else
        finaal->setText(finalsTotalString);


    summa->setText(jsonObj["total"].toString());
    kumned->setText(jsonObj["innerTens"].toString());

    QString remarksString = jsonObj["remarks"].toString();
    if (remarksString == "Märkused")
        markus->setText("");
    else
        markus->setText(remarksString);

    QJsonArray seriesArray = jsonObj["series"].toArray();
    seeriateArv = seriesArray.size();
    laskudeArv = numberOfShots;

    for (int i = 0; i < seriesArray.size(); i++) {
                QJsonObject seriesJson = seriesArray[i].toObject();
        seeriad << new QLineEdit(seriesJson["seriesSum"].toString(), this);
        seeriad[i]->setMinimumHeight(28);
        seeriad[i]->setMaximumWidth(40);
        seeriad[i]->setToolTip(tr("Seeriad"));
        connect(seeriad[i], SIGNAL(editingFinished()), this, SLOT(liida()));
        connect(seeriad[i], SIGNAL(returnPressed()), this, SLOT(vajutaTab()));
        connect(seeriad[i], SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
        QList<Lask*> seeriaLasud;
        QJsonArray shotsArray = seriesJson["shots"].toArray();
        for(int j = 0; j < shotsArray.size(); j++){
            seeriaLasud << new Lask(shotsArray[j].toObject());  // Series shots, including one for punishment
        }
        lasud << seeriaLasud;   //Seeria lasud laskuri laskude hulka
    }

    if (vSummadeSamm != 0) {
        for (int i = 0; i < seeriateArv / vSummadeSamm; i++) {
            vSummad << new QLineEdit(this);
            vSummad[i]->setMinimumHeight(28);
            vSummad[i]->setMaximumWidth(50);
            vSummad[i]->setText("0");
            vSummad[i]->setToolTip(tr("Vahesumma"));
            vSummad[i]->setStyleSheet("border: 1px solid grey");
            vSummad[i]->setReadOnly(true);
        }
    }

    QJsonArray shootOffShotsArray = jsonObj["shootOffShots"].toArray();
    for (int i = 0; i < shootOffShotsArray.size(); i++) {
        lisaLasud.append(shootOffShotsArray[i].toInt());
    }
    createLayout();
    liida();
}

int Laskur::competitionStage() const
{
    return m_competitionStage;
}

void Laskur::contextMenuEvent(QContextMenuEvent *event)
{
    popup->exec(event->globalPos());
}

void Laskur::createLayout()
{
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
    for(int i = 0; i < seeriad.size(); i++){
        hKast->addWidget(seeriad[i]);
        if(vSummadeSamm != 0){
            if(/*i != 0 &&*/ (i+1) % vSummadeSamm == 0){
                if(j >= vSummad.size()){
                    QMessageBox::critical(this, tr("Viga!"), tr("Ei ole nii palju vahesummasid!"), QMessageBox::Ok);
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
}

void Laskur::deleteAllShots()
{
    for(int i = 0; i < lasud.length(); i++)
        for(Lask *shot : lasud.at(i))
            shot->clear();

    for(QLineEdit *serie : seeriad)
        serie->setText("");
    liida();
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

bool Laskur::isFinished() const
{
    return lasud[seeriateArv - 1][laskudeArv - 1]->getILask() != -999;
}

QualificationEvents::EventType* Laskur::getEventType() const
{
    return m_eventType;
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
                QMessageBox::critical(this, tr("Teade"), "lisalasud.count() = 0", QMessageBox::Ok);
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
        if(eelmine != tekst && eelmine.length() < tekst.length()){//Pikkuste võrdlus on samuti vajalik
                if(abi == Puss){										//kustutamisest aru saamiseks.
                        if(andmebaas->kirjutusabiPuss){
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
                        if(andmebaas->kirjutusabiPustol){
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
    if(perekNimi->cursorPosition() < perekNimi->text().length()){
            int pos = perekNimi->cursorPosition();
            perekNimi->setText(perekNimi->text().toUpper());
            perekNimi->setCursorPosition(pos);
            return;
    }

    perekNimi->setText(perekNimi->text().toUpper());

    if(abi == Puudub || !*kirjutusAbi) return;
    if(!andmebaas->kirjutusabiPuss && !andmebaas->kirjutusabiPustol) return;    //Kontrolliks kas abi on üldse võimalik.
            static QString eelmine = "";
            QString tekst = perekNimi->text();
            if(eelmine.length() > tekst.length() + 1) eelmine = "";
            if(eelmine != tekst && eelmine.length() < tekst.length()){
                    if(abi == Puss){
                            if(andmebaas->kirjutusabiPuss){
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
        int shotsInTotal = 0;   // Total number of shots already existing
        int siseKumneid = 0;
//        bool komaga = false;
        bool onnestus = false;
        bool onLasud = false;
        QString seeria;
        QPalette shotsMissingBackground;
        shotsMissingBackground.setColor(QPalette::Base, Qt::yellow);
        QPalette shotsMissingFinishedBackground;
        shotsMissingFinishedBackground.setColor(QPalette::Base, Qt::red);
        QPalette standardBackground = eesNimi->palette();

        for(int i = 0; i < seeriad.size(); i++){
            ssum = 0;
            onLasud = false;
            int shotsInSeries = 0;  // Number of shots existing in the series
            for(int j = 0; j < lasud[i].count(); j++)   //Liitmisel liidetakse ka karistus juurde
                if(lasud[i][j]->getILask() != -999){
//                    qDebug() << "lasud[" << i << "][" << j << "] = " << lasud[i][j]->getILask();
                    if(*kumnendikega){
                        ssum += lasud[i][j]->get10Lask();
                    }else{
                        ssum += lasud[i][j]->getILask() * 10;
                    }
                    if(lasud[i][j]->isInnerTen())
                        siseKumneid++;
                    onLasud = true;
                    shotsInSeries++;
                }
            shotsInTotal += shotsInSeries;
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
                if(shotsInSeries < laskudeArv) {
                    if(isFinished())
                        seeriad[i]->setPalette(shotsMissingFinishedBackground);
                    else
                        seeriad[i]->setPalette(shotsMissingBackground);
                } else {
                    seeriad[i]->setPalette(standardBackground);
                }
//                kumned->setEnabled(false);
//                connect(seeriad[i], SIGNAL(doubleClicked()), this, SLOT(naitaLaskudeAkent()));
            }else{
                seeriad[i]->setReadOnly(false);
                seeriad[i]->setPalette(standardBackground);
                kumned->setReadOnly(false);
            }
            if(!seeriad[i]->text().isEmpty()){
                if(!onLasud)
                    shotsInTotal += 10;  //Kui on käsitsi seeria kirjutatud, siis ilmselt on 10 lasku lastud
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
        if(!finaal->text().isEmpty()){
            sum += qRound(finaal->text().toFloat(&onnestus) * 10000);
            if(!onnestus){  //Kui arvuks tegemine ei õnnestunud on vaja asendada koma punktiga, kasutame sama QString seeria't
                seeria = finaal->text();
                seeria.replace(',', '.');
                sum += qRound(seeria.toFloat(&onnestus) * 10000);
            }
        }
        keskmLask = sum / shotsInTotal;
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
        if(!finaal->text().isEmpty() && !finaal->text().contains(',', Qt::CaseInsensitive))
                finaal->setText(finaal->text().append(",0"));
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
        int uusId = QInputDialog::getInt(this, tr("Sisestage uus laskuri ID"), "ID:", this->id, 0, 9999, 1, &ok);
    if(ok)
        emit idMuutus(uusId, this);
//        this->id = uusId;
}

void Laskur::naitaLaskudeAkent()
{
//#ifdef QT_DEBUG
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
        QTextStream(stdout) << "Laskur::vaiksem(): t = " << t << ", " << this->perekNimi->text() << ", " << l->perekNimi->text() << Qt::endl;
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
    case 1: {   //1 - sorting by family names
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): by family names - " << this->perekNimi->text() << ", " << l->perekNimi->text() << Qt::endl;
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
    case 2: { //2 - sorting by birth years
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): by birth years - " << this->perekNimi->text() << ", " << l->perekNimi->text() << Qt::endl;
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
    case 3: { //3 - sorting by target numbers
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): by target numbers - " << this->perekNimi->text() << ", " << l->perekNimi->text() << Qt::endl;

        int tulemus = vaiksemRajaga(l);

        switch(tulemus){    //Kui on 0, siis läheb edasi nimede võrdlemise juurde
        case -1 : return true;
        case 1 : return false;
        }

        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Same target numbers" << Qt::endl;
        tulemus = vaiksemNimega(l);

        switch(tulemus){
        case -1 : return true;
        case 1 : return false;
        default: return false;
        }
    }
    case 4: { //4 - by paper target ciphers
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): by ciphers - " << this->perekNimi->text() << ", " << l->perekNimi->text() << Qt::endl;
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
            QTextStream(stdout) << "Laskur::vaiksem(): general - " << this->perekNimi->text() << ", " << l->perekNimi->text() << Qt::endl;
        if(!this->finaal->text().isEmpty() && !l->finaal->text().isEmpty()){
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
#ifdef QT_DEBUG
                qDebug() << "Laskur::vaiksem(), finals equal, this->lisaLasud.count() = " << this->lisaLasud.count() << ", l->lisaLasud.count() = " << l->lisaLasud.count();
#endif
                if(this->lisaLasud.count() == 0 && l->lisaLasud.count() != 0)   //Juhul, kui finaali seeriad on võrdsed, aga millegipärast lisalaske ei ole
                    return true;
                if(l->lisaLasud.count() == 0)
                    return false;
#ifdef QT_DEBUG
                qDebug() << "Laskur::vaiksem(), looking at additional shots";
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
        }else if(!this->finaal->text().isEmpty() && l->finaal->text().isEmpty()){
            return false;
        }else if(this->finaal->text().isEmpty() && !l->finaal->text().isEmpty()){
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

//bool Laskur::parseSiusCompetitionShot(QString row, QStringList rowParts, int shotFieldNoInSiusRow)
//{
//    if(rowParts.count() < 4){
//        return false;
////                logiValja << "\n#viga!: rowParts lõhki! rowParts.count() < 4\n";
//    }else if(rowParts.count() <= shotFieldNoInSiusRow){
//        return false;
////                logiValja << "\n#viga!: rowParts lõhki! rowParts.count() < lasuNrSiusis = " << lasuNrSiusis << "\n";
//    } else {

////            logiValja << "#" << rowParts[3] << ": " << competitionStage() * vSummadeSamm + rowParts[lasuNrSiusis].toInt() << ". Lask\n";
////            logiValja << "#SHOT: previousSiusRow(): " << previousSiusRow();
////                Lask newShot(row);

//        int seriesIndex = (competitionStage() * vSummadeSamm * 10 + rowParts[shotFieldNoInSiusRow].toInt() - 1) / 10;
//        int shotIndex = (rowParts[shotFieldNoInSiusRow].toInt() - 1) % 10;

//        //If competitor's last shot has data in it, then probably these results have already been read and it is better not to read them again, to avoid mistakes
//        if(seeriateArv > seriesIndex && lasud[seeriateArv - 1][laskudeArv - 1]->getILask() < 0){
//            //Check if series number and number of shots in each series is big enough
//            if(lasud.count() > seriesIndex && lasud[0].count() > shotIndex){
//                lasud[seriesIndex][shotIndex]->setSiusShot(row);
//                liida();
//                teataMuudatusest();
//                return true;

////                    logiValja << "#" << eesNimi->text() << " " << perekNimi->text() << " lask 1 = " << lasud[seriesIndex][(rowParts[lasuNrSiusis].toInt() - 1) % 10]->getSLask() << "\n";
//            }// else
////                    logiValja << "\n#viga!: laskur lõhki! seriesIndex = " << seriesIndex << ", (rowParts[lasuNrSiusis].toInt() - 1) % 10 = " << (rowParts[lasuNrSiusis].toInt() - 1) % 10 << "\n";
//        }
//    }
//    return false;   // If flow reaches here, then there is some error in row
//}

QString Laskur::previousSiusRow() const
{
    return m_previousSiusRow;
}

bool Laskur::readSiusShot(SiusShotData shotData)
{
    bool result = false;

    if(shotData.id == id && (shotData.socketIndex == siusConnectionIndex() || siusConnectionIndex() == -1)){ //To avoid different Sius connections reading into one competitor
        if(siusConnectionIndex() == -1)
            setSiusConnectionIndex(shotData.socketIndex);

        if(vSummadeSamm == 0 || (vSummadeSamm != 0 && shotData.siusShotNo <= vSummadeSamm * 10)) { // Ignore additional shots in all stages
            int seriesIndex = (competitionStage() * vSummadeSamm * 10 + shotData.siusShotNo - 1) / 10;
            int shotIndex = (shotData.siusShotNo - 1) % 10;

            // TODO: A solution to enable adding missing shots and ignoring wrong shots
            //Check if series number and number of shots in each series is big enough
            if(shotIndex >= 0 && lasud.count() > seriesIndex && lasud[0].count() > shotIndex){
                if (lasud[seriesIndex][shotIndex]->isEmpty()){
                    lasud[seriesIndex][shotIndex]->set(&shotData.shot);
                    liida();
                    teataMuudatusest();
                    result = true;
                } else if (lasud[seriesIndex][shotIndex]->getSLask().compare(shotData.shot.getSLask()) == 0 &&
                            lasud[seriesIndex][shotIndex]->shotTime() == shotData.shot.shotTime()) {
                    result = true;  // Shot already existing, ignore, but return true
                } else if (vSummadeSamm > 0 && findShotFromPreviousStages(shotData) != -1) {
                    setCompetitionStage(findShotFromPreviousStages(shotData));  // If old shot was received, then set competition stage to that stage, because most likely shots will continue from there
                    result = true;  // Shot already existing, ignore, but return true
                } else if (lasud[seriesIndex][shotIndex]->shotTime() < shotData.shot.shotTime()
                            && vSummadeSamm != 0){  // Increase competition stage only if there are more than 1 stages
                    nextCompetitionStage();
                    result = readSiusShot(shotData); // Try again with new competitionstage
                } else
                    result = false;
                //                    logiValja << "#" << eesNimi->text() << " " << perekNimi->text() << " lask 1 = " << lasud[seriesIndex][(rowParts[lasuNrSiusis].toInt() - 1) % 10]->getSLask() << "\n";
            }// else
            //                    logiValja << "\n#viga!: laskur lõhki! seriesIndex = " << seriesIndex << ", (rowParts[lasuNrSiusis].toInt() - 1) % 10 = " << (rowParts[lasuNrSiusis].toInt() - 1) % 10 << "\n";
        }
    }
//    QStringList rowParts = row.split(';');
//    if(rowParts.count() >= shotFieldNoInSiusRow){
//    if(rowParts[3].toInt() == id && (socketIndex == siusConnectionIndex() || siusConnectionIndex() == -1)){ //To avoid different Sius connections reading into one competitor
//        if(siusConnectionIndex() == -1)
//            setSiusConnectionIndex(socketIndex);

////        if(row.startsWith("_TOTL") && !isCompetitionStarted()){
////            if(rowParts.count() <=12)
////                return false;   // If row is not long enough, then it is probably an event total which we are not interested in
////            if(rowParts[12] != "0" && previousSiusRow().startsWith("_SHOT")){   // During sighting shots it is 0 and during competition it contains series result
////                setCompetitionStarted(true);
////                result = parseSiusCompetitionShot(previousSiusRow(), previousSiusRow().split(';'), shotFieldNoInSiusRow);
////            }
////            if(previousSiusRow().startsWith("_PRCH")){
////                if(isCompetitionStarted() && (!m_eventType->contains("Lamades") || !m_eventType->contains("Õhupüss") || !m_eventType->contains("Õhupüstol") || !m_eventType->contains("Vabapüstol"))){
////                    nextCompetitionStage();  //If there are sighting shots or only one competition, no point to risk with increasing competition stage
////                }
////                setCompetitionStarted(false);    //These are sighting shots
////                logiValja << "#GRPH: proovilasud: " << id << " " << eesNimi->text() << " " << perekNimi->text() << "\n";
////                logiValja << "#GRPH: competitionStage = " << competitionStage();
////            }else{
////                setCompetitionStarted(true); //If previous row does not start with _PRCH, then these are competition shots
////                logiValja << "#GRPH: algavad võistluslasud: " << id << " " << eesNimi->text() << " " << perekNimi->text() << "\n";
////            }
//        // Sighting shots: 32, 544
//        // Competition shots: 0, 512
//        QVector<int> sighterTypes = {32, 36, 37, 39, 544, 551, 1060};
//        QVector<int> competitionShotTypes = {0, 4, 5, 7, 512, 515, 519, 1028, 1029, 1036, 2304};
//        static bool checkTotal = false;

//        bool ok;    // If shot type cannot be converted to int, then something is wrong
//        int currentShotType = rowParts[9].toInt(&ok);

//        /*} else*/ if((row.startsWith("_SHOT") && ok && competitionShotTypes.contains(currentShotType) /*&& isCompetitionStarted()*/) ||
//                      (checkTotal && row.startsWith("_TOTL") && rowParts.count() > 12)){   //Competition shot row
////            result = parseSiusCompetitionShot(row, rowParts, shotFieldNoInSiusRow);
////            if (rowParts.count() < 4){
////                return false;
//        //                logiValja << "\n#viga!: rowParts lõhki! rowParts.count() < 4\n";
////            } else if(rowParts.count() <= shotFieldNoInSiusRow){
////                return false;
//        //                logiValja << "\n#viga!: rowParts lõhki! rowParts.count() < lasuNrSiusis = " << lasuNrSiusis << "\n";
////            } else {

//        //            logiValja << "#" << rowParts[3] << ": " << competitionStage() * vSummadeSamm + rowParts[lasuNrSiusis].toInt() << ". Lask\n";
//        //            logiValja << "#SHOT: previousSiusRow(): " << previousSiusRow();
//        //                Lask newShot(row);

//                int seriesIndex = (competitionStage() * vSummadeSamm * 10 + rowParts[shotFieldNoInSiusRow].toInt() - 1) / 10;
//                int shotIndex = (rowParts[shotFieldNoInSiusRow].toInt() - 1) % 10;

//                //If competitor's last shot has data in it, then probably these results have already been read and it is better not to read them again, to avoid mistakes
//                // Make sure new shot is within series and shots' list bounds
//                if(seeriateArv > seriesIndex && lasud[seeriateArv - 1][laskudeArv - 1]->getILask() < 0){
//                    if(lasud.count() > seriesIndex && lasud[0].count() > shotIndex){
//                        if(!lasud[seriesIndex][shotIndex]->isEmpty() && lasud[seriesIndex][shotIndex]->shotTime() < QTime::fromString(rowParts[6])){
//                            nextCompetitionStage(); // If shot no is same, but time is later, then it is probably next stage in the event
//                            seriesIndex = (competitionStage() * vSummadeSamm * 10 + rowParts[shotFieldNoInSiusRow].toInt() - 1) / 10;
//                        }
//                        lasud[seriesIndex][shotIndex]->setSiusShot(row);
//                        liida();
//                        teataMuudatusest();
//                        return true;

//        //                    logiValja << "#" << eesNimi->text() << " " << perekNimi->text() << " lask 1 = " << lasud[seriesIndex][(rowParts[lasuNrSiusis].toInt() - 1) % 10]->getSLask() << "\n";
//                    } else
//                        return false;
//        //                    logiValja << "\n#viga!: laskur lõhki! seriesIndex = " << seriesIndex << ", (rowParts[lasuNrSiusis].toInt() - 1) % 10 = " << (rowParts[lasuNrSiusis].toInt() - 1) % 10 << "\n";
////                }
//            }
//        }
//        result = true;
//    } else
//        result = false;

//        setPreviousSiusRow(row);
//    }
    return result;
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
    this->m_eventType = l->getEventType();
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

void Laskur::setCompetitionStage(int newStage)
{
    m_competitionStage = newStage;
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

void Laskur::setupFields()
{
    jrkArv = 0;
    lisaAken = nullptr;
    laskudeAken = nullptr;
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
    m_siusConnectionIndex = -1;

    arvutaja = new QTimer(this);
    arvutaja->setInterval(60000);
    arvutaja->setSingleShot(false);
    connect(arvutaja, SIGNAL(timeout()), this, SLOT(liida()));
    arvutaja->start();

    hKast = new QHBoxLayout(this);
    hKast->setObjectName("hKast");
    linnuke = new QCheckBox(this);
    rajaNr = new QLineEdit(this);
    rajaNr->setMinimumHeight(28);
    rajaNr->setMaximumWidth(20);
    rajaNr->setToolTip(tr("Raja number"));
    connect(rajaNr, SIGNAL(returnPressed()), this, SLOT(vajutaTab()));
    connect(rajaNr, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
    sifriAlgus = new QLineEdit(this);
    sifriAlgus->setMinimumHeight(28);
    sifriAlgus->setMaximumWidth(50);
    sifriAlgus->setToolTip(tr("Siffer"));
    connect(sifriAlgus, SIGNAL(returnPressed()), this, SLOT(vajutaTab()));
    connect(sifriAlgus, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
    sidekriips = new QLabel(this);
    sidekriips->setText("-");
    sifriLopp = new QLineEdit(this);
    sifriLopp->setMinimumHeight(28);
    sifriLopp->setMaximumWidth(50);
    sifriLopp->setToolTip(tr("Siffer"));
    connect(sifriLopp, SIGNAL(returnPressed()), this, SLOT(enterVajutatud()));
    connect(sifriLopp, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
    sifriNupp = new QPushButton(this);
    sifriNupp->setText(">>");
    sifriNupp->setMinimumWidth(20);
    sifriNupp->setMaximumWidth(22);
    connect(sifriNupp, SIGNAL(clicked()), this, SLOT(naitaSifrit()));
    eesNimi = new QLineEdit(this);
    eesNimi->setMinimumHeight(28);
    eesNimi->setToolTip(tr("Eesnimi"));
    connect(eesNimi, SIGNAL(textEdited(QString)), this, SLOT(muutus(QString)));
    connect(eesNimi, SIGNAL(returnPressed()), this, SLOT(vajutaTab()));
    connect(eesNimi, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
    perekNimi = new QLineEdit(this);
    perekNimi->setMinimumHeight(28);
    perekNimi->setToolTip(tr("Perekonnanimi"));
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
    klubi->setToolTip(tr("Klubi"));
    connect(klubi, SIGNAL(textEdited(QString)), this, SLOT(muutus4(QString)));
    connect(klubi, SIGNAL(returnPressed()), this, SLOT(vajutaTab()));
    connect(klubi, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
    summa = new QLineEdit(this);
    summa->setText("0");
    summa->setMinimumHeight(28);
    summa->setMaximumWidth(50);
    summa->setToolTip(tr("Summa"));
    summa->setStyleSheet("border: 1px solid grey");
    summa->setReadOnly(true);
    connect(summa, SIGNAL(returnPressed()), this, SLOT(vajutaTab()));
    connect(summa, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
    finaal = new QLineEdit(this);
    finaal->setPlaceholderText("Fin");
    finaal->setMinimumHeight(28);
    finaal->setMaximumWidth(45);
    finaal->setToolTip(tr("Finaali seeria"));
    connect(finaal, SIGNAL(editingFinished()), this, SLOT(liida()));
    connect(finaal, SIGNAL(returnPressed()), this, SLOT(vajutaTab()));
    connect(finaal, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
    koguSumma = new QLabel(this);
    koguSumma->setText("0,0");
    koguSumma->setToolTip(tr("Summa"));
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
    markus->setPlaceholderText(tr("Märkused"));
    connect(markus, SIGNAL(returnPressed()), this, SLOT(vajutaTab()));
    connect(markus, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
    lisaLNupp = new QPushButton("Lis.", this);
    lisaLNupp->setMaximumWidth(25);
    lisaLNupp->setMinimumWidth(25);
    lisaLNupp->setToolTip(tr("Lisalasud"));
    connect(lisaLNupp, SIGNAL(clicked()), this, SLOT(lisaLAken()));

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

int Laskur::findShotFromPreviousStages(const SiusShotData shotData) const
{
    if (vSummadeSamm != 0) {    // More than 1 stage
        for(int stage = competitionStage(); stage >= 0; stage--){
            int seriesIndex = (stage * vSummadeSamm * 10 + shotData.siusShotNo - 1) / 10;
            int shotIndex = (shotData.siusShotNo - 1) % 10;

            if (lasud[seriesIndex][shotIndex]->getSLask().compare(shotData.shot.getSLask()) == 0 &&
                    lasud[seriesIndex][shotIndex]->shotTime() == shotData.shot.shotTime())
                return stage;
        }
    }
    return -1;
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

    QJsonArray subtotalsArray;
    if(vSummadeSamm != 0) {
        for (int i = 0; i < vSummad.size(); i++){
//            int index = i * vSummadeSamm;
            QJsonObject subtotalJson;
            QJsonArray seriesArray;
            for (int index = i * vSummadeSamm; index < (i + 1) * vSummadeSamm; index++){
                QList<Lask*> seriesShots = lasud.at(index);
                QJsonObject seriesJson;
                QJsonArray shotsArray;
                foreach (Lask *shot, seriesShots)
                    shotsArray.append(shot->toJson());
                seriesJson["seriesShots"] = shotsArray;
                seriesJson["seriesSum"] = seeriad.at(index)->text();
//                index++;
                seriesArray.append(seriesJson);
            }
            subtotalJson["series"] = seriesArray;
            if(*m_eventType == QualificationEvents::Rifle3x20_50m || *m_eventType == QualificationEvents::Rifle3x40_50m ||
                *m_eventType == QualificationEvents::Rifle3x20_300m || *m_eventType == QualificationEvents::Rifle3x40_300m){
                switch (i) {
                case 0: {
                    subtotalJson["label"] = "Kneeling";
                    break;
                }
                case 1: {
                    subtotalJson["label"] = "Prone";
                    break;
                }
                case 2: {
                    subtotalJson["label"] = "Standing";
                    break;
                }
                default: {
                    subtotalJson["label"] = "Series";
                    break;
                }
                }
            } else if(*m_eventType == QualificationEvents::Pistol_25m){
                switch (i) {
                case 0: {
                    subtotalJson["label"] = "Precision";
                    break;
                }
                case 1: {
                    subtotalJson["label"] = "Rapid";
                    break;
                }
                default: {
                    subtotalJson["label"] = "Series";
                    break;
                }
                }
            } else if(*m_eventType == QualificationEvents::RapidFirePistol ||
                       *m_eventType == QualificationEvents::RunningTargetMixed_10m ||
                       *m_eventType == QualificationEvents::RunningTargetMixed_50m){
                switch (i) {
                case 0: {
                    subtotalJson["label"] = "Stage 1";
                    break;
                }
                case 1: {
                    subtotalJson["label"] = "Stage 2";
                    break;
                }
                default: {
                    subtotalJson["label"] = "Series";
                    break;
                }
                }
            } else if(*m_eventType == QualificationEvents::StandardPistol){
                switch (i) {
                case 0: {
                    subtotalJson["label"] = "150\'\'";
                    break;
                }
                case 1: {
                    subtotalJson["label"] = "20\'\'";
                    break;
                }
                case 2: {
                    subtotalJson["label"] = "10\'\'";
                    break;
                }
                default: {
                    subtotalJson["label"] = "Series";
                    break;
                }
                }
            } else if(*m_eventType == QualificationEvents::PistolCISM_25m){
                switch (i) {
                case 0: {
                    subtotalJson["label"] = "10\'\'";
                    break;
                }
                case 1: {
                    subtotalJson["label"] = "8\'\'";
                    break;
                }
                case 2: {
                    subtotalJson["label"] = "6\'\'";
                    break;
                }
                default: {
                    subtotalJson["label"] = "Series";
                    break;
                }
                }
            } else if(*m_eventType == QualificationEvents::RunningTarget_10m ||
                       *m_eventType == QualificationEvents::RunningTarget_50m){
                switch (i) {
                case 0: {
                    subtotalJson["label"] = "Slow run";
                    break;
                }
                case 1: {
                    subtotalJson["label"] = "Rapid run";
                    break;
                }
                default: {
                    subtotalJson["label"] = "Series";
                    break;
                }
                }
            } else {
                subtotalJson["label"] = "Series";
            }

            subtotalJson["subtotal"] = vSummad.at(i)->text();
            subtotalsArray.append(subtotalJson);
        }
    } else {
        QJsonObject subtotalJson;
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
        subtotalJson["series"] = seriesArray;
        subtotalJson["label"] = "Series";
        subtotalJson["subtotal"] = summa->text();
        subtotalsArray.append(subtotalJson);
    }
    json["subtotals"] = subtotalsArray;
    json["totalResult"] = summa->text();
    json["innerTens"] = kumned->text();
    json["finals"] = finaal->text();
    if(markus->text() == "Märkused")
        json["remarks"] = "";
    else
        json["remarks"] = markus->text();
    return json;
}

QJsonObject Laskur::toJson()
{
    QJsonObject competitorJson;
    competitorJson["targetNo"] = rajaNr->text();
    competitorJson["sifferStart"] = sifriAlgus->text();
    competitorJson["sifferEnd"] = sifriLopp->text();
    competitorJson["id"] = id;
    competitorJson["firstName"] = eesNimi->text()/*.toUtf8()*/;
    competitorJson["familyName"] = perekNimi->text()/*.toUtf8()*/;
    competitorJson["yearOfBirth"] = sunniAasta->text()/*.toUtf8()*/;
    competitorJson["club"] = klubi->text()/*.toUtf8()*/;

    QJsonArray seriesArray;
    for(int k = 0; k < seeriad.size(); k++){	//Kõik seeriad
        QJsonObject seriesJson;
        seriesJson["seriesSum"] = seeriad[k]->text()/*.toUtf8()*/;

        QJsonArray shotsArray;
        for(int l = 0; l < lasud[k].count(); l++){  //Karistuse lask tuleb ka kirjutada
            shotsArray.append(lasud[k][l]->toJson());
        }
        seriesJson["shots"] = shotsArray;
        seriesArray.append(seriesJson);
    }

    competitorJson["series"] = seriesArray;
    competitorJson["total"] = summa->text()/*.toUtf8()*/;
    competitorJson["finalsTotal"] = finaal->text()/*.toUtf8()*/;

    QJsonArray shootOffArray;
    for(int k = 0; k < 24; k++)
        shootOffArray.append(lisaLasud[k]);
    competitorJson["shootOffShots"] = shootOffArray;
    competitorJson["innerTens"] = kumned->text()/*.toUtf8()*/;

    if(markus->text() == "Märkused")
        competitorJson["remarks"] = "";
    else
        competitorJson["remarks"] = markus->text();

    return competitorJson;
}

int Laskur::vaiksemNimega(Laskur *l) const //Tagastab kas laskur on nime järgi väiksem (-1), suurem (1) või võrdne (0)
{
    if(veryVerbose)
        QTextStream(stdout) << "Laskur::vaiksem(): Perekonnanimede võrdlus - " << this->perekNimi->text() << ", " << l->perekNimi->text() << Qt::endl;
    //Tühja perekonnanimega laskur reastatakse tahapoole
    if(this->perekNimi->text().isEmpty() && !l->perekNimi->text().isEmpty()){
        return -1;
    }else if(l->perekNimi->text().isEmpty() && !this->perekNimi->text().isEmpty()){
        return 1;
    }else if(this->perekNimi->text().localeAwareCompare(l->perekNimi->text()) > 0){
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Perekonnanimede võrdlus - return true" << Qt::endl;
        return -1;
    }else if(this->perekNimi->text().localeAwareCompare(l->perekNimi->text()) < 0){
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Perekonnanimede võrdlus - return false" << Qt::endl;
        return 1;
    }else{
        if(this->eesNimi->text().localeAwareCompare(l->eesNimi->text()) > 0){
            if(veryVerbose)
                QTextStream(stdout) << "Laskur::vaiksem(): Eesnimede võrdlus - return true" << Qt::endl;
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
//            QTextStream(stdout) << "Laskur::vaiksem(): this->rajaNr->text().toInt(&onnestus1); " << this->rajaNr->text().toInt(&onnestus1) << this->perekNimi->text() << Qt::endl;
    l->rajaNr->text().toInt(&onnestus2);
//        if(veryVerbose)
//            QTextStream(stdout) << "Laskur::vaiksem(): l->rajaNr->text().toInt(&onnestus2); " << l->rajaNr->text().toInt(&onnestus2) << l->perekNimi->text() << Qt::endl;
    if(onnestus1 && !onnestus2 && !l->rajaNr->text().isEmpty()){
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Arv vs täht" << Qt::endl;
        return -1;   //Kui üks kast on arvuga ja teine tähega on väiksem arvuga kast
    }else if(onnestus2 && !onnestus1 && !this->rajaNr->text().isEmpty()){
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Täht vs arv" << Qt::endl;
        return 1;
    }else if(!onnestus1 && !onnestus2 && this->rajaNr->text().localeAwareCompare(l->rajaNr->text()) > 0){
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Täht vs täht" << Qt::endl;
        return -1;  //Kui on tähed finaali jaoks
    }else if(!onnestus1 && !onnestus2 && this->rajaNr->text().localeAwareCompare(l->rajaNr->text()) < 0){
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Täht vs täht 2" << Qt::endl;
        return 1;
    }else if(onnestus1 && onnestus2){ //Kui mõlemas kastis on arvuline raja nr
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): Arv vs arv" << Qt::endl;
        if(this->rajaNr->text().toInt() < l->rajaNr->text().toInt()){
            if(this->rajaNr->text().isEmpty()) return -1; //Rajanumbrite puhul on vastupidi, väiksem nr on eespool
            else{
                if(veryVerbose)
                    QTextStream(stdout) << "Laskur::vaiksem(): Arv vs arv, " << this->perekNimi->text() << "<" << l->perekNimi->text() << Qt::endl;
                return 1;
            }
//                return true;
        }else if(this->rajaNr->text().toInt() > l->rajaNr->text().toInt()){
            if(l->rajaNr->text().isEmpty()) return 1; //Rajanumbrite puhul on vastupidi, väiksem nr on eespool
            else{
                if(veryVerbose)
                    QTextStream(stdout) << "Laskur::vaiksem(): Arv vs arv, " << this->perekNimi->text() << ">" << l->perekNimi->text() << Qt::endl;
                return -1;
            }
//                return false;
        }
    }else if(l->rajaNr->text().isEmpty()){
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): l on tühi - " << l->perekNimi->text() << Qt::endl;
        return 1;
    }else if(this->rajaNr->text().isEmpty()){
        if(veryVerbose)
            QTextStream(stdout) << "Laskur::vaiksem(): This on tühi - " << this->perekNimi->text() << Qt::endl;
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

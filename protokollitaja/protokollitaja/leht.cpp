#include "leht.h"

extern bool verbose;

Leht::Leht(Andmebaas* baas, int s, int vs, int a, bool *k, QString eNimi, int r, QString h, bool kum, int *jar, QWidget *parent, bool v, LiikmeteValikKast *lV, int lI, int ls)
    : QWidget(parent)
{
    relv = r;
    harjutus = h;
	voistk = v;
	viimaneIndex = 0;
	naidata = true;
	voibKontrollida = true;
	alustamine = false;
    kumnendikega = kum;
    jarjestamine = jar;
	ekraaniNimi = eNimi;
    Q_UNUSED(ls);
    laskudeArv = 10; //ls; Laskude arv on praegu alati 10
	seeriateArv = s;
    vSummadeSamm = vs;
	andmebaas = baas;
	abi = a;
	leheIndeks = lI;
	kirjutusAbi = k;
	minAeg = 10000;
	maxAeg = 30000;
    lValik = lV;
	vKast = new QVBoxLayout(this);
//	setMaximumWidth(1000);
	setMinimumWidth(900);
	if(seeriateArv > 6){
//		setMaximumWidth(1200);
		setMinimumWidth(1200);
    }else if(vSummadeSamm > 0)
		setMinimumWidth(1000);
	//setGeometry(1, 1, 1000, 100);
}

void Leht::deleteAllShotsFromSelectedCompetitors()
{
    if(!voistk){
        for(Laskur *laskur : laskurid){
            if(laskur->linnuke->isChecked())
                laskur->deleteAllShots();
        }
    }
}

QString Leht::getEventType()
{
    return harjutus;
}

void Leht::idMuudatus(int uusId, Laskur* las)
{
    emit idMuutus(uusId, las);
}

void Leht::uusLaskur(int id)
{
    if(voistk){
        Voistkond *voistKond = new Voistkond(lValik, seeriateArv, &jalgitavad, &viimaneIndex, this);
        connect(voistKond->muudaNupp, SIGNAL(clicked()), voistKond, SLOT(naitaLiikmeteValikKast()));
        connect(voistKond->muudaNupp, SIGNAL(clicked()), this, SLOT(teataMuudatusest()));
        connect(voistKond, SIGNAL(uuendaLiikmeid()), this, SLOT(uuendaLiikmeteKast()));
        connect(voistKond->nimi, SIGNAL(textEdited(QString)), this, SLOT(teataMuudatusest(QString)));
        vKast->addWidget(voistKond);
        voistkonnad << voistKond;
        voistKond->show();
    }else{
        Laskur *las = new Laskur(andmebaas, seeriateArv, vSummadeSamm, abi, kirjutusAbi, &kumnendikega, id, jarjestamine, &harjutus, laskudeArv, this);
        connect(las, SIGNAL(sifrimuutus()), this, SLOT(naitaSifrit()));
        connect(las, SIGNAL(idMuutus(int,Laskur*)), this, SLOT(idMuudatus(int,Laskur*)));
        connect(las->eesNimi, SIGNAL(editingFinished()), this, SLOT(kontrolliKordusi()));
        connect(las->perekNimi, SIGNAL(editingFinished()), this, SLOT(kontrolliKordusi()));
        connect(las->klubi, SIGNAL(editingFinished()), this, SLOT(kontrolliKordusi()));
        connect(las->eesNimi, SIGNAL(textChanged(QString)), this, SLOT(lubaKontrollimist(QString)));
        connect(las->perekNimi, SIGNAL(textChanged(QString)), this, SLOT(lubaKontrollimist(QString)));
        connect(las->klubi, SIGNAL(textChanged(QString)), this, SLOT(lubaKontrollimist(QString)));
        connect(las, SIGNAL(muudatus()), this, SLOT(teataMuudatusest()));
        connect(las, SIGNAL(enter(int)), this, SLOT(vajutaTab2(int)));
        vKast->addWidget(las);
        laskurid << las;
        las->jrkArv = laskurid.count() - 1;
        if(laskurid.count() > 1)
            if(laskurid[0]->sifriAlgus->isVisible()){
                las->rajaNr->show();
                las->sifriAlgus->show();
                las->sidekriips->show();
                las->sifriLopp->show();
                las->sifriNupp->setText("<<");
            }
        las->show();
    }
    setMaximumHeight(vKast->count()*50+10);
}

void Leht::eemaldaLaskur()
{
	if(voistk){
		for(int i = 0; i < voistkonnad.count(); i++){
			if(voistkonnad[i]->linnuke->isChecked() == true){
				vKast->removeWidget(voistkonnad[i]);
				voistkonnad[i]->deleteLater();
				voistkonnad.removeAt(i);
				setMaximumHeight(vKast->count()*50+10);
				i--;
			}
		}
	}else{
		for(int i = 0; i < laskurid.count(); i++){
			if(laskurid[i]->linnuke->isChecked() == true){
				vKast->removeWidget(laskurid[i]);
				laskurid[i]->deleteLater();
				laskurid.removeAt(i);
				setMaximumHeight(vKast->count()*50+10);
				i--;
			}
		}
                for(int i = 0; i < laskurid.count(); i++){
                    laskurid[i]->jrkArv = i;
                }
	}
}

void Leht::setToBeUploaded(bool newStatus)
{
    m_toBeUploaded = newStatus;
}

void Leht::siusiReset(int connectionIndex)
{
    for(int i = 0; i < laskurid.count(); i++)
        laskurid[i]->siusiReset(connectionIndex);
}

void Leht::sorteeri(int t)
{
    if(verbose)
        QTextStream(stdout) << "Leht::sorteeri(): t = " << t << endl;

    if(laskurid.isEmpty() && voistkonnad.isEmpty()) return;
    int i = 0;
    if(voistk){ //Võistkondlik arvestus
        emit uuendaVoistkondi();
        reasVoistkonnad.clear();
        if(voistkonnad.count() < 2){
            reasVoistkonnad << voistkonnad[0];
            return;
        }
        for(int j = 0; j < voistkonnad.count(); j++)
            reasVoistkonnad << voistkonnad[j];
        for(i; i < (voistkonnad.count()-1) && i < (reasVoistkonnad.count() - 1); i++){
            if(reasVoistkonnad[i]->vaiksem(reasVoistkonnad.at(i+1))){
                Voistkond *vana = reasVoistkonnad[i];
                reasVoistkonnad[i] = reasVoistkonnad.at(i+1);
                reasVoistkonnad[i+1] = vana;
                i = -1;
            }
        }
    }else{  //Individuaalne arvestus
        reasLaskurid.clear();
        if(laskurid.count() < 2){   //Kui on ainult üks laskur
            reasLaskurid << laskurid[0];
            return;
        }
        //Kui on rohkem kui üks laskur lehel
        for(int j = 0; j < laskurid.count(); j++)
            reasLaskurid << laskurid[j];

        for(i; i < (laskurid.count()-1) && i < (reasLaskurid.count() - 1); i++){
            reasLaskurid[i]->liida();
            reasLaskurid.at(i+1)->liida();
            if(reasLaskurid[i]->vaiksem(reasLaskurid.at(i+1), t)){
                Laskur *vana = reasLaskurid[i];
                reasLaskurid[i] = reasLaskurid.at(i+1);
                reasLaskurid[i+1] = vana;
                i = -1;
            }
        }
    }
}

void Leht::reasta(int t)
{
	sorteeri(t);
	/*for(int i = 0; i < this->laskurid.count(); i++)
		this->vKast->removeWidget(laskurid[laskurid.count() - 1]);
	for(int i = 0; i < reasLaskurid.count(); i++)
		vKast->addWidget(reasLaskurid[i]);*/
    if(voistk){ //Kui on võistkondlik arvestus
		QList<VaruVoistkond> varuVoistkonnad;
		VaruVoistkond uus;
		Voistleja uusVoistleja;
		for(int i = 0; i < reasVoistkonnad.count(); i++){
			uus.voistejad.clear();
			uus.nimi = reasVoistkonnad[i]->nimi->text();
			for(int j = 0; j < seeriateArv; j++){
				uusVoistleja.nimi = reasVoistkonnad[i]->voistlejad[j]->eesNimi;
				uusVoistleja.perekNimi = reasVoistkonnad[i]->voistlejad[j]->perekNimi;
				uusVoistleja.klubi = reasVoistkonnad[i]->voistlejad[j]->klubi;
				uusVoistleja.summa = reasVoistkonnad[i]->voistlejad[j]->summa;
				uusVoistleja.silt = reasVoistkonnad[i]->voistlejad[j]->silt->text();
				uus.voistejad << uusVoistleja;
			}
			uus.summa = reasVoistkonnad[i]->summa->text();
			uus.markus = reasVoistkonnad[i]->markus->text();
			varuVoistkonnad << uus;
		}
		if(reasVoistkonnad.isEmpty()) return;
		for(int i = 0; i < voistkonnad.count() && i < reasVoistkonnad.count(); i++){
			voistkonnad[i]->nimi->setText(varuVoistkonnad[i].nimi);
			voistkonnad[i]->summa->setText(varuVoistkonnad[i].summa);
			voistkonnad[i]->markus->setText(varuVoistkonnad[i].markus);
			for(int j = 0; j < seeriateArv; j++){
				voistkonnad[i]->voistlejad[j]->eesNimi = varuVoistkonnad[i].voistejad[j].nimi;
				voistkonnad[i]->voistlejad[j]->perekNimi = varuVoistkonnad[i].voistejad[j].perekNimi;
				voistkonnad[i]->voistlejad[j]->klubi = varuVoistkonnad[i].voistejad[j].klubi;
				voistkonnad[i]->voistlejad[j]->summa = varuVoistkonnad[i].voistejad[j].summa;
				voistkonnad[i]->voistlejad[j]->silt->setText(varuVoistkonnad[i].voistejad[j].silt);
			}
			voistkonnad[i]->liida();
		}
    }else{  //Kui on individuaalne arvestus
//		QList<VaruLaskur> varuLaskurid;
//		VaruLaskur uus;
        if(reasLaskurid.isEmpty()) return;
        QList<Laskur*> varuLaskurid;
		for(int i = 0; i < reasLaskurid.count(); i++){
//            uus.linnuke = false;  //Neid ridu ei peaks olema vaja
//			uus.seeriad.clear();
//			uus.lisaLasud.clear();
//            uus.rajaNr.clear();
//            uus.sifriAlgus.clear();
//            uus.sifriLopp.clear();

//            uus.linnuke = reasLaskurid[i]->linnuke->isChecked();
//            uus.rajaNr = reasLaskurid[i]->rajaNr->text();
//            uus.sifriAlgus = reasLaskurid[i]->sifriAlgus->text();
//            uus.sifriLopp = reasLaskurid[i]->sifriLopp->text();
//            uus.eesNimi = reasLaskurid[i]->eesNimi->text();
//            uus.perekNimi = reasLaskurid[i]->perekNimi->text();
//            uus.sunniAasta = reasLaskurid[i]->sunniAasta->text();
//            uus.klubi = reasLaskurid[i]->klubi->text();
//            for(int j = 0; j < reasLaskurid[i]->seeriateArv; j++)
//                uus.seeriad << reasLaskurid[i]->seeriad[j]->text();
//            uus.summa = reasLaskurid[i]->summa->text();
//            uus.finaal = reasLaskurid[i]->finaal->text();
//            for(int j = 0; j < 10; j++)
//                uus.lisaLasud << reasLaskurid[i]->lisaLasud[j];
//            uus.kumned = reasLaskurid[i]->kumned->text();
//            uus.markus = reasLaskurid[i]->markus->text();
            Laskur* uus = new Laskur(andmebaas, seeriateArv, vSummadeSamm, abi, kirjutusAbi, &kumnendikega, 0, jarjestamine, &harjutus, laskudeArv, this);
            uus->set(reasLaskurid[i]);
			varuLaskurid << uus;
		}
		for(int i = 0; i < laskurid.count() && i < reasLaskurid.count(); i++){
//            laskurid[i]->linnuke->setChecked(varuLaskurid[i].linnuke);
//            laskurid[i]->rajaNr->setText(varuLaskurid[i].rajaNr);
//            laskurid[i]->sifriAlgus->setText(varuLaskurid[i].sifriAlgus);
//            laskurid[i]->sifriLopp->setText(varuLaskurid[i].sifriLopp);
//			laskurid[i]->eesNimi->setText(varuLaskurid[i].eesNimi);
//			laskurid[i]->perekNimi->setText(varuLaskurid[i].perekNimi);
//			laskurid[i]->sunniAasta->setText(varuLaskurid[i].sunniAasta);
//			laskurid[i]->klubi->setText(varuLaskurid[i].klubi);
//			laskurid[i]->summa->setText(varuLaskurid[i].summa);
//			laskurid[i]->finaal->setText(varuLaskurid[i].finaal);
//			laskurid[i]->kumned->setText(varuLaskurid[i].kumned);
//			laskurid[i]->markus->setText(varuLaskurid[i].markus);
//			for(int j = 0; j < laskurid[i]->seeriateArv; j++)
//				laskurid[i]->seeriad[j]->setText(varuLaskurid[i].seeriad[j]);
//			laskurid[i]->lisaLasud.clear();
//			for(int j = 0; j < 10; j++)
//				laskurid[i]->lisaLasud << varuLaskurid[i].lisaLasud[j];
//            laskurid[i]->liida();
            laskurid[i]->set(varuLaskurid[i]);
            varuLaskurid[i]->deleteLater();
		}
        for(int i = 0; i < laskurid.count(); i++){
            laskurid[i]->jrkArv = i;
        }
	}
    sorteeri(t);
}

void Leht::uuendaLiikmeteKast()
{
	emit uuendaLiikmeid();
}

void Leht::kontrolliKordusi()
{
	if(alustamine || voistk) return;
	QObject *saatja = this->sender();
	if(dynamic_cast<QLineEdit*>(saatja)->text().isEmpty()) return;
	/*QObject *uusSaatja = this->sender();
	if(saatja == uusSaatja)
		return;
	else saatja = uusSaatja;*/
	static int jrk = 0;
	int uusJrk = 0;
	for(int i = 0; i < laskurid.count(); i++){
		if(saatja == laskurid[i]->eesNimi || saatja == laskurid[i]->perekNimi || saatja == laskurid[i]->
				klubi){
			uusJrk = i;
		}
	}
	if(uusJrk == jrk && !voibKontrollida)
		return;
	else jrk = uusJrk;
	voibKontrollida = false;
	for(int j = 0; j < laskurid.count(); j++){
		if(jrk == j)
			j++;
		if(j >= laskurid.count()) return;
		if(laskurid[jrk]->eesNimi->text() == laskurid[j]->eesNimi->text() && laskurid[jrk]->
				perekNimi->text() == laskurid[j]->perekNimi->text() && laskurid[jrk]->
				klubi->text() == laskurid[j]->klubi->text())
			QMessageBox::warning(this, "Protokollitaja", "Selline laskur on juba olemas!", "Selge");
	}
}

void Leht::lubaKontrollimist(const QString)
{
	voibKontrollida = true;
}

void Leht::vajutaTab()
{
	QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
	QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyRelease, Qt::Key_Tab, Qt::NoModifier));
}

void Leht::vajutaTab2(int i)
{
    if(++i >= laskurid.count()){
        QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
        QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyRelease, Qt::Key_Tab, Qt::NoModifier));
    }else{
        laskurid[i]->sifriAlgus->setFocus();
    }
}

void Leht::teataMuudatusest(const QString)
{
	emit muudatus();
}

void Leht::teataMuudatusest()
{
    emit muudatus();
}

bool Leht::toBeUploaded()
{
    return m_toBeUploaded;
}

QJsonObject Leht::toExportJson()
{
    QJsonObject sheetJson;
    sheetJson["id"] = QString("%1").arg(leheIndeks);
    sheetJson["eventName"] = ekraaniNimi;
    sorteeri(0);
    QJsonArray competitorsArray;

    foreach (Laskur *competitor, reasLaskurid){
        competitorsArray.append(competitor->toExportJson());
    }
    sheetJson["competitors"] = competitorsArray;

    QJsonArray teamsArray;
    foreach (Voistkond *team, reasVoistkonnad){
        teamsArray.append(team->toExportJson());
    }
    sheetJson["teams"] = teamsArray;

    return sheetJson;
}

void Leht::naitaSifrit()
{
    if(laskurid.count() < 1) return;
    if(!laskurid[0]->sifriAlgus->isVisible()){
        for(int i = 0; i < laskurid.count(); i++){
            laskurid[i]->rajaNr->show();
            laskurid[i]->sifriAlgus->show();
            laskurid[i]->sidekriips->show();
            laskurid[i]->sifriLopp->show();
            laskurid[i]->sifriNupp->setText("<<");
        }
    }else{
        for(int i = 0; i < laskurid.count(); i++){
            laskurid[i]->rajaNr->hide();
            laskurid[i]->sifriAlgus->hide();
            laskurid[i]->sidekriips->hide();
            laskurid[i]->sifriLopp->hide();
            laskurid[i]->sifriNupp->setText(">>");
        }
    }
}

Leht::~Leht()
{
	/*nimedeLeht->jalgijad--;
	if(nimedeLeht->jalgijad < 0)
        nimedeLeht->jalgijad = 0;*/
}

#include "voistkond.h"

Voistkond::Voistkond(LiikmeteValikKast *lV, int vA, QList<int> *j, int *vI, QWidget *parent)
        : QWidget(parent)
{
        viimaneIndex = vI;
        keskmLask = 0;
        vArv = vA;
        lValik = lV;
        jalgitavad = j;
        hKast = new QHBoxLayout(this);
        hKast->setObjectName("hKast");
        linnuke = new QCheckBox(this);
        hKast->addWidget(linnuke);
        nimi = new QLineEdit(this);
        nimi->setMinimumHeight(28);
        nimi->setMaximumWidth(100 + this->width() / 5);
        hKast->addWidget(nimi);
        muudaNupp = new QPushButton(this);
        muudaNupp->setText("Liikmed");
        hKast->addWidget(muudaNupp);
        summa = new QLineEdit(this);
        summa->setText("0");
        summa->setMinimumHeight(28);
        summa->setMaximumWidth(55);
        summa->setReadOnly(true);
        summa->setStyleSheet("border: 1px solid grey");
        markus = new QLineEdit(this);
        markus->setMinimumHeight(28);
        markus->setMaximumWidth(85);
        markus->setText(tr("Märkused"));
        for(int i = 0; i < vArv; i++){
                voistlejad << new Liige;
                voistlejad[voistlejad.count() - 1]->eesNimi = tr("Võistleja 1");
                voistlejad[voistlejad.count() - 1]->harjutus = "Harjutus";
//                voistlejad[i]->silt = new QLabel(this);
                voistlejad[i]->silt = new QLineEdit(this);
                voistlejad[i]->silt->setText("0");
                voistlejad[i]->silt->setReadOnly(true);
                hKast->addWidget(voistlejad[i]->silt);
//                hKast->addStretch();
        }
        hKast->addWidget(summa);
        hKast->addWidget(markus);
        hKast->addStretch();
        //this->setMaximumWidth(800);
}

void Voistkond::otsiNime()
{

}

QJsonObject Voistkond::toExportJson()
{
    QJsonObject teamJson;
    teamJson["id"] =
    teamJson["teamName"] = nimi->text();

    QJsonArray teamMembers;
    foreach (Liige *teamCompetitor, voistlejad){
        QJsonObject competitorJson;
        competitorJson["firstName"] = teamCompetitor->eesNimi;
        competitorJson["lastName"] = teamCompetitor->perekNimi;
        competitorJson["club"] = teamCompetitor->klubi;
        competitorJson["event"] = teamCompetitor->harjutus;
        competitorJson["result"] = teamCompetitor->summa;
        teamMembers.append(competitorJson);
    }
    teamJson["teamMembers"] = teamMembers;
    teamJson["totalResult"] = summa->text();
    if(markus->text() == "Märkused")
        teamJson["remarks"] = "";
    else
        teamJson["remarks"] = markus->text();

    return teamJson;
}

bool Voistkond::vaiksem(Voistkond *v) const
{
//        if(this->keskmLask < v->keskmLask)
//                return true;
//        else if(this->keskmLask > v->keskmLask)
//                return false;
        //Võrdlemiseks on vaja komad eemaldada ning võrrelda täisarve
        if(this->summa->text().remove(',').remove('.').toInt() < v->summa->text().remove(',').remove('.').toInt())
                return true;
        else if(this->summa->text().remove(',').remove('.').toInt() > v->summa->text().remove(',').remove('.').toInt())
                return false;
        if(nimi->text().isEmpty() || v->nimi->text().isEmpty())
                return false;
        if(this->nimi->text().localeAwareCompare(v->nimi->text()) > 0)
                return true;
        else if(this->nimi->text().localeAwareCompare(v->nimi->text()) < 0)
                return false;
        return true;
}

void Voistkond::naitaLiikmeteValikKast()
{
        //QMessageBox::information(this, "Teade", "naitaLiikemeteValikkast()", QMessageBox::Ok);
        emit uuendaLiikmeid();
        //QMessageBox::information(this, "Teade", "naitaLiikemeteValikkast()1", QMessageBox::Ok);
        lValik->ui.liikmed->clear();
        //QMessageBox::information(this, "Teade", "naitaLiikemeteValikkast()2", QMessageBox::Ok);
        lValik->maxArv = vArv;
        for(int i = 0; i < vArv; i++){
            QString rida = voistlejad[i]->perekNimi + ", " + voistlejad[i]->eesNimi + ", " + voistlejad[i]->klubi
                    + ", " + voistlejad[i]->harjutus;
            if(voistlejad[i]->eesNimi != tr("Võistleja 1"))
                lValik->ui.liikmed->addItem(rida);
        }
        lValik->ui.leheBox->setCurrentIndex(*viimaneIndex);
        //QMessageBox::information(this, "Teade", "naitaLiikemeteValikkast()3", QMessageBox::Ok);
        lValik->ui.liikmed->setCurrentRow(0);
        if(lValik->exec() == QDialog::Accepted){
                for(int i = 0; i < voistlejad.count(); i++){
                        voistlejad[i]->eesNimi = tr("Võistleja 1");
                        voistlejad[i]->perekNimi.clear();
                        voistlejad[i]->klubi.clear();
                        voistlejad[i]->summa.clear();
                        voistlejad[i]->harjutus.clear();
                        voistlejad[i]->silt->setText("0");
                }
                for(int i = 0; i < lValik->ui.liikmed->count(); i++){
                        QString rida = lValik->ui.liikmed->item(i)->text();
                        QStringList read = rida.split(',', QString::KeepEmptyParts);
                        voistlejad[i]->perekNimi = read[0].trimmed();
                        voistlejad[i]->eesNimi = read[1].trimmed();
                        voistlejad[i]->klubi = read[2].trimmed();
                        voistlejad[i]->harjutus = read[3].trimmed();
                }
                *viimaneIndex = lValik->ui.leheBox->currentIndex();
                bool olemas = false;
                for(int i = 0; i < lValik->jalgitavad.count(); i++){
                        for(int j = 0; j < jalgitavad->count(); j++)
                                if(jalgitavad->at(j) == lValik->jalgitavad[i])
                                        olemas = true;
                        if(!olemas){
                                jalgitavad->insert(-1, lValik->jalgitavad[i]);
                        }
                        olemas = false;
                }
                liida();
        }
}

void Voistkond::liida()
{
        int sum = 0;
        double fsum = 0;
        int arv = 0;
        bool komaga = false;
        bool onnestus = false;
        for(int i = 0; i < voistlejad.count(); i++){
                sum += qRound(voistlejad[i]->summa.toFloat(&onnestus) * 10);
                if(!onnestus){
                    QString summa = voistlejad[i]->summa;
                    summa.replace(',', '.');
                    sum += qRound(summa.toFloat(&onnestus) * 10);
                }
                if(!voistlejad[i]->summa.isEmpty() && voistlejad[i]->summa != "0")
                        arv++;
                if(voistlejad[i]->summa.contains(',') || voistlejad[i]->summa.contains('.'))
                    komaga = true;
        }
        fsum = sum;
        summa->setText(QString("%1").arg(fsum / 10));
        summa->setText(summa->text().replace('.', ','));
        if(!summa->text().contains(',') /*&& !summa->text().contains('.')*/ && komaga)
            summa->setText(summa->text() + ",0");
        sum *= 100;
        arv *= 100;
        if(sum > 0 && arv > 0){
                keskmLask = sum / arv;
                //keskmLask /= 100;
        }else keskmLask = 0;
        for(int i = 0; i < voistlejad.count(); i++)
                for(int i = 0; i < (voistlejad.count() - 1); i++)
                        if(voistlejad[i]->summa.toInt() < voistlejad[i+1]->summa.toInt()){
                                Liige vana;
//                                vana.silt = new QLabel(this);
                                vana.silt = new QLineEdit(this);
                                vana.eesNimi = voistlejad[i]->eesNimi;
                                vana.perekNimi = voistlejad[i]->perekNimi;
                                vana.klubi = voistlejad[i]->klubi;
                                vana.summa = voistlejad[i]->summa;
                                vana.harjutus = voistlejad[i]->harjutus;
                                vana.silt->setText(voistlejad[i]->silt->text());
                                voistlejad[i]->eesNimi = voistlejad[i+1]->eesNimi;
                                voistlejad[i]->perekNimi = voistlejad[i+1]->perekNimi;
                                voistlejad[i]->klubi = voistlejad[i+1]->klubi;
                                voistlejad[i]->summa = voistlejad[i+1]->summa;
                                voistlejad[i]->harjutus = voistlejad[i+1]->harjutus;
                                voistlejad[i]->silt->setText(voistlejad[i+1]->silt->text());
                                voistlejad[i+1]->eesNimi = vana.eesNimi;
                                voistlejad[i+1]->perekNimi = vana.perekNimi;
                                voistlejad[i+1]->klubi = vana.klubi;
                                voistlejad[i+1]->summa = vana.summa;
                                voistlejad[i+1]->harjutus = vana.harjutus;
                                voistlejad[i+1]->silt->setText(vana.silt->text());
                                vana.silt->deleteLater();
                        }
}

Voistkond::~Voistkond()
{
        // TODO Auto-generated destructor stub
}

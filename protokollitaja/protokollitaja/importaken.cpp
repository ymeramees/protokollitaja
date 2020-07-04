#include "importaken.h"

ImportAken::ImportAken(QWidget *parent)
    : QDialog(parent)
{
        ui.setupUi(this);
        connect(ui.failistNupp, SIGNAL(clicked()), this, SLOT(failist()));
        connect(ui.maluNupp, SIGNAL(clicked()), this, SLOT(vahemalust()));
        leht = new Leht(0, 6, 0, 0, &kirjutusabi, "Ekraaninimi", 0, "Muu", 0, 0, this);
        ui.scrollArea->setWidgetResizable(true);
        ui.scrollArea->setWidget(leht);
}

void ImportAken::failist()
{
        QString failiNimi = QFileDialog::getOpenFileName(this, "Impordi...", "", "");
        if(failiNimi.isEmpty()) return;
        if(!failiNimi.endsWith(".txt") && !failiNimi.endsWith(".csv")){
                QMessageBox::critical(this, "Protokollitaja", tr("Tundmatu laiendiga fail. Importimine pole kahjuks "
                                "võimalik."), QMessageBox::Ok);
                return;
        }
        QFile fail(failiNimi);
        if(fail.open(QIODevice::ReadOnly | QIODevice::Text)){
                QTextStream sisse(&fail);
                int vSummadeSamm = 0;
                do{
                        QStringList andmed, andmed2, vahepealne;
                        QString rida = sisse.readLine();
                        andmed << rida;

                        if(rida.contains(';') && ui.checkBoxSemik->isChecked()){
                                andmed2 = andmed[0].split(';');
                                andmed = andmed2;
                        }
                        if(rida.contains('\t') && ui.checkBoxTab->isChecked()){
                                andmed2.clear();
                                for(int i = 0; i < andmed.count(); i++){
                                        vahepealne = andmed[i].split('\t');
                                        andmed2 << vahepealne;
                                }
                                andmed = andmed2;
                        }
                        if(rida.contains(',') && ui.checkBoxKoma->isChecked()){
                                andmed2.clear();
                                for(int i = 0; i < andmed.count(); i++){
                                        vahepealne = andmed[i].split(',');
                                        andmed2 << vahepealne;
                                }
                                andmed = andmed2;
                        }
                        if(rida.contains('.') && ui.checkBoxPunkt->isChecked()){
                                andmed2.clear();
                                for(int i = 0; i < andmed.count(); i++){
                                        vahepealne = andmed[i].split('.');
                                        andmed2 << vahepealne;
                                }
                                andmed = andmed2;
                        }
                        if(rida.contains(' ') && ui.checkBoxTuhik->isChecked()){
                                andmed2.clear();
                                for(int i = 0; i < andmed.count(); i++){
                                        vahepealne = andmed[i].split(QRegExp("\\s+"));
                                        andmed2 << vahepealne;
                                }
                                andmed = andmed2;
                        }
                        if(rida.contains(ui.muuEdit->text()) && !ui.muuEdit->text().isEmpty()){
                                andmed2.clear();
                                for(int i = 0; i < andmed.count(); i++){
                                        vahepealne = andmed[i].split(ui.muuEdit->text());
                                        andmed2 << vahepealne;
                                }
                                andmed = andmed2;
                        }
                        if(andmed.count() < 2) return;
                        int arv = 0, arv2 = 0;
                        if(andmed[0].contains('.')) arv = 1;
                        else{
                                bool Ok = false;
                                andmed[0].toInt(&Ok);
                                if(Ok) arv = 1;
                        }
                        if(andmed[andmed.count() - 1].contains('.')) arv2 = 1;
                        else{
                                bool Ok = false;
                                andmed[andmed.count() - 1].toInt(&Ok);
                                if(!Ok) arv2 = 1;
                        }
                        if(andmed[andmed.count() - 1].contains('.')) arv2 = 1;
                        else{
                                bool Ok = false;
                                andmed[andmed.count() - 1].toInt(&Ok);
                                if(!Ok) arv2 = 1;
                        }
                        if(andmed.count() > (7 + arv + arv2)){
                                int sum = 0;
                                sum = andmed[arv + 4].toInt();
                                sum += andmed[arv + 5].toInt();
                                for(int i = arv + 6; i < andmed.count() - arv2 - 1; i++){
                                        if(sum == andmed[i].toInt() && sum != 0) vSummadeSamm = i - arv - 4;
                                        else sum += andmed[i].toInt();
                                }
                        }
                        int seeriateArv = andmed.count() - (arv + 5) - arv2;
                        if(vSummadeSamm != 0)
                                seeriateArv -= seeriateArv / (vSummadeSamm + 1);
                        if(seeriateArv < 0) seeriateArv = 0;
                        leht->seeriateArv = seeriateArv;
                        leht->vSummadeSamm = vSummadeSamm;
                        leht->uusLaskur(0);
                        leht->laskurid[leht->laskurid.count() - 1]->linnuke->setChecked(true);
                        leht->laskurid[leht->laskurid.count() - 1]->eesNimi->setText(andmed[arv].trimmed().remove('\"'));
                        if(!leht->laskurid[leht->laskurid.count() - 1]->eesNimi->text().isEmpty())
                                leht->laskurid[leht->laskurid.count() - 1]->eesNimi->setText(leht->
                                                laskurid[leht->laskurid.count() - 1]->eesNimi->text().remove('\"').at(0).toUpper() + leht->
                                                laskurid[leht->laskurid.count() - 1]->eesNimi->text().remove(0, 1).toLower().remove('\"'));
                        leht->laskurid[leht->laskurid.count() - 1]->perekNimi->setText(andmed[arv + 1].toUpper()
                                        .trimmed().remove('\"'));
                        leht->laskurid[leht->laskurid.count() - 1]->sunniAasta->setText(andmed[arv + 2].trimmed().remove('\"'));
                        if(leht->laskurid[leht->laskurid.count() - 1]->sunniAasta->text().toInt() < 100 &&
                                        !leht->laskurid[leht->laskurid.count() - 1]->sunniAasta->text().contains("."))
                                leht->laskurid[leht->laskurid.count() - 1]->sunniAasta->setText("19" + leht->
                                                laskurid[leht->laskurid.count() - 1]->sunniAasta->text().trimmed());
                        leht->laskurid[leht->laskurid.count() - 1]->klubi->setText(andmed[arv + 3].trimmed().remove('\"'));
                        //QMessageBox::information(this, "Protokollitaja", "Teises kohas", QMessageBox::Ok);
                        int lisaIndeks = 0;
                        for(int i = 0; i < seeriateArv; i++){
                                if(vSummadeSamm != 0)
                                        if(i % vSummadeSamm == 0 && i != 0) lisaIndeks++;
                                leht->laskurid[leht->laskurid.count() - 1]->seeriad[i]->setText(andmed[arv + 4 + i +
                                                                                                       lisaIndeks].trimmed());
                        }
                        //QMessageBox::information(this, "Protokollitaja", "Kolmandas kohas", QMessageBox::Ok);
                        leht->laskurid[leht->laskurid.count() - 1]->setSumma(andmed[andmed.count() - 1 - arv2].trimmed());
                        if(arv2 > 0)
                                leht->laskurid[leht->laskurid.count() - 1]->markus->setText(andmed[andmed.count() - 1]
                                                                                                   .trimmed().remove('\"'));
                        if(leht->laskurid[leht->laskurid.count() - 1]->markus->text().isEmpty() ||
                                        leht->laskurid[leht->laskurid.count() - 1]->markus->text() == leht->laskurid[leht->
                                        laskurid.count() - 1]->klubi->text())
                                leht->laskurid[leht->laskurid.count() - 1]->markus->setText(tr("Märkused"));
                }while(!sisse.atEnd());
        }
}

void ImportAken::puhtaks()
{
        if(leht->laskurid.count() < 1) return;
        for(int i = 0; i < leht->laskurid.count(); i++)
                leht->laskurid[i]->linnuke->setChecked(true);
        leht->eemaldaLaskur();
}

void ImportAken::vahemalust()
{
        int vSummadeSamm = 0;
        QString rida;
        QClipboard *vahemalu = QApplication::clipboard();
        rida = vahemalu->text();
        if(rida.isEmpty()) return;
        QStringList andmed, andmed2, vahepealne, read;
        read = rida.split('\n');
        //QMessageBox::information(this, "Protokollitaja", read[read.count() - 1], QMessageBox::Ok);
        for(int x = 0; x < read.count(); x++){
                andmed.clear();
                andmed2.clear();
                vahepealne.clear();
                rida = read[x];
                //QMessageBox::information(this, "Protokollitaja", rida, QMessageBox::Ok);
                andmed << rida;
                if(rida.contains(';') && ui.checkBoxSemik->isChecked()){
                        andmed2 = andmed[0].split(';');
                        andmed = andmed2;
                }
                if(rida.contains('\t') && ui.checkBoxTab->isChecked()){
                        andmed2.clear();
                        for(int i = 0; i < andmed.count(); i++){
                                vahepealne = andmed[i].split('\t');
                                andmed2 << vahepealne;
                        }
                        andmed = andmed2;
                }
                if(rida.contains(',') && ui.checkBoxKoma->isChecked()){
                        andmed2.clear();
                        for(int i = 0; i < andmed.count(); i++){
                                vahepealne = andmed[i].split(',');
                                andmed2 << vahepealne;
                        }
                        andmed = andmed2;
                }
                if(rida.contains('.') && ui.checkBoxPunkt->isChecked()){
                        andmed2.clear();
                        for(int i = 0; i < andmed.count(); i++){
                                vahepealne = andmed[i].split('.');
                                andmed2 << vahepealne;
                        }
                        andmed = andmed2;
                }
                if(rida.contains(' ') && ui.checkBoxTuhik->isChecked()){
                        andmed2.clear();
                        for(int i = 0; i < andmed.count(); i++){
                                vahepealne = andmed[i].split(QRegExp("\\s+"));
                                andmed2 << vahepealne;
                        }
                        andmed = andmed2;
                }
                if(rida.contains(ui.muuEdit->text()) && !ui.muuEdit->text().isEmpty()){
                        andmed2.clear();
                        for(int i = 0; i < andmed.count(); i++){
                                vahepealne = andmed[i].split(ui.muuEdit->text());
                                andmed2 << vahepealne;
                        }
                        andmed = andmed2;
                }
                if(andmed.count() < 2) return;
                int arv = 0, arv2 = 0;
                if(andmed[0].contains('.')) arv = 1;
                else{
                        bool Ok = false;
                        andmed[0].toInt(&Ok);
                        if(Ok) arv = 1;
                }
                if(andmed[andmed.count() - 1].contains('.')) arv2 = 1;
                else{
                        bool Ok = false;
                        andmed[andmed.count() - 1].toInt(&Ok);
                        if(!Ok) arv2 = 1;
                }
                if(andmed.count() > (7 + arv + arv2)){
                        int sum = 0;
                        sum = andmed[arv + 4].toInt();
                        sum += andmed[arv + 5].toInt();
                        for(int i = arv + 6; i < andmed.count() - arv2 - 1; i++){
                                if(sum == andmed[i].toInt() && sum != 0) vSummadeSamm = i - arv - 4;
                                else sum += andmed[i].toInt();
                        }
                }
                //for(int i = 0; i < andmed.count(); i++)
                //QMessageBox::information(this, "Protokollitaja", QString("vSummadeSamm = %1").arg(vSummadeSamm), QMessageBox::Ok);
                int seeriateArv = andmed.count() - (arv + 5) - arv2;
                if(vSummadeSamm != 0)
                        seeriateArv -= seeriateArv / (vSummadeSamm + 1);
                if(seeriateArv < 0) seeriateArv = 0;
                //QMessageBox::information(this, "Protokollitaja", QString("seeriateArv = %1").arg(seeriateArv), QMessageBox::Ok);
                leht->seeriateArv = seeriateArv;
                leht->vSummadeSamm = vSummadeSamm;
                leht->uusLaskur(0);
                leht->laskurid[leht->laskurid.count() - 1]->linnuke->setChecked(true);
                leht->laskurid[leht->laskurid.count() - 1]->eesNimi->setText(andmed[arv].trimmed().remove('\"'));
                if(!leht->laskurid[leht->laskurid.count() - 1]->eesNimi->text().isEmpty())
                        leht->laskurid[leht->laskurid.count() - 1]->eesNimi->setText(leht->
                                        laskurid[leht->laskurid.count() - 1]->eesNimi->text().remove('\"').at(0).toUpper() + leht->
                                        laskurid[leht->laskurid.count() - 1]->eesNimi->text().remove(0, 1).toLower().remove('\"'));
                leht->laskurid[leht->laskurid.count() - 1]->perekNimi->setText(andmed[arv + 1].toUpper().trimmed().remove('\"'));
                leht->laskurid[leht->laskurid.count() - 1]->sunniAasta->setText(andmed[arv + 2].trimmed().remove('\"'));
                if(leht->laskurid[leht->laskurid.count() - 1]->sunniAasta->text().toInt() < 100 &&
                                        !leht->laskurid[leht->laskurid.count() - 1]->sunniAasta->text().contains("."))
                        leht->laskurid[leht->laskurid.count() - 1]->sunniAasta->setText("19" + leht->
                                        laskurid[leht->laskurid.count() - 1]->sunniAasta->text().trimmed());
                leht->laskurid[leht->laskurid.count() - 1]->klubi->setText(andmed[arv + 3].trimmed().remove('\"'));
                //QMessageBox::information(this, "Protokollitaja", "Teises kohas", QMessageBox::Ok);
                int lisaIndeks = 0;
                for(int i = 0; i < seeriateArv; i++){
                        if(vSummadeSamm != 0)
                                if(i % vSummadeSamm == 0 && i != 0) lisaIndeks++;
                        leht->laskurid[leht->laskurid.count() - 1]->seeriad[i]->setText(andmed[arv + 4 + i +
                                                                                               lisaIndeks].trimmed());
                }
                //QMessageBox::information(this, "Protokollitaja", "Kolmandas kohas", QMessageBox::Ok);
                leht->laskurid[leht->laskurid.count() - 1]->setSumma(andmed[andmed.count() - 1 - arv2].trimmed());
                if(arv2 > 0)
                        leht->laskurid[leht->laskurid.count() - 1]->markus->setText(andmed[andmed.count() - 1]
                                                                                           .trimmed());
                if(leht->laskurid[leht->laskurid.count() - 1]->markus->text().isEmpty() || leht->laskurid[leht->
                         laskurid.count() - 1]->markus->text() == leht->laskurid[leht->laskurid.count() - 1]->
                         klubi->text())
                        leht->laskurid[leht->laskurid.count() - 1]->markus->setText(tr("Märkused"));
        }
}

ImportAken::~ImportAken()
{

}

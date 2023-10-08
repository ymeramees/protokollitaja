#include "importaken.h"

ImportAken::ImportAken(QWidget *parent)
    : QDialog(parent)
{
        ui.setupUi(this);
        connect(ui.failistNupp, SIGNAL(clicked()), this, SLOT(fromFile()));
        connect(ui.maluNupp, SIGNAL(clicked()), this, SLOT(fromClipboard()));
        ui.scrollArea->setWidgetResizable(true);
}

ImportAken::~ImportAken()
{

}

void ImportAken::clearSheet()
{
    if(leht != nullptr){
        if(leht->laskurid.count() < 1) return;
        for(int i = 0; i < leht->laskurid.count(); i++)
            leht->laskurid[i]->linnuke->setChecked(true);
        leht->eemaldaLaskur();
        leht->deleteLater();
        leht = nullptr;
    }
}

int ImportAken::currentCompetitorId()
{
    return m_currentCompetitorId;
}

void ImportAken::fromFile()
{
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Impordi..."),
                "",
                tr("Protokollitaja files (*.kll);;Text files (*.txt);;Comma separated files (*.csv"));
    if(fileName.isEmpty()) return;
    if(fileName.endsWith(".txt") || fileName.endsWith(".csv")){
        QFile fail(fileName);
        if(fail.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream sisse(&fail);
            if(leht == nullptr){
                leht = new Leht(0, 6, 0, 0, &kirjutusabi, tr("Ekraaninimi"), TargetTypes::Other, QualificationEvents::OtherAirRifle, 0, 0, this);
                ui.scrollArea->setWidget(leht);
            }
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
                                        vahepealne = andmed[i].split(QRegularExpression("\\s+"));
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
                        leht->laskurid[leht->laskurid.count() - 1]->eesNimi->setText(andmed[arv].remove('\"').trimmed());
                        leht->laskurid[leht->laskurid.count() - 1]->perekNimi->setText(andmed[arv + 1].toUpper()
                                        .remove('\"').trimmed());
                        leht->laskurid[leht->laskurid.count() - 1]->sunniAasta->setText(andmed[arv + 2].remove('\"').trimmed());
                        if(leht->laskurid[leht->laskurid.count() - 1]->sunniAasta->text().toInt() < 100
                                && leht->laskurid[leht->laskurid.count() - 1]->sunniAasta->text().toInt() > 0
                                && !leht->laskurid[leht->laskurid.count() - 1]->sunniAasta->text().contains("."))
                            leht->laskurid[leht->laskurid.count() - 1]->sunniAasta->setText("19" + leht->
                                                laskurid[leht->laskurid.count() - 1]->sunniAasta->text().trimmed());
                        leht->laskurid[leht->laskurid.count() - 1]->klubi->setText(andmed[arv + 3].remove('\"').trimmed());
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
                                                                                                   .remove('\"').trimmed());
                        if(leht->laskurid[leht->laskurid.count() - 1]->markus->text() ==
                                leht->laskurid[leht->laskurid.count() - 1]->klubi->text())
                            leht->laskurid[leht->laskurid.count() - 1]->markus->setText("");
                }while(!sisse.atEnd());
        }
    } else if (fileName.endsWith(".kll")) {
        fromKllFile(fileName);
    } else {
        QMessageBox::critical(this, "Protokollitaja", tr("Tundmatu laiendiga fail. Importimine pole kahjuks "
                                                         "võimalik."), QMessageBox::Ok);
    }
}

void ImportAken::fromClipboard()
{
    QMessageBox::warning(this, "Protokollitaja", tr("Kas teil .kll faili ei ole? Sealt importimisel (Failist... nupp) tuleb rohkem infot üle, kui vahemälust importimisel."), QMessageBox::Ok);
    if(leht == nullptr){
        leht = new Leht(0, 6, 0, 0, &kirjutusabi, tr("Ekraaninimi"), TargetTypes::Other, QualificationEvents::OtherAirRifle, 0, 0, this);
        ui.scrollArea->setWidget(leht);
    }

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
                                vahepealne = andmed[i].split(QRegularExpression("\\s+"));
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
                leht->laskurid[leht->laskurid.count() - 1]->eesNimi->setText(andmed[arv].remove('\"').trimmed());
                leht->laskurid[leht->laskurid.count() - 1]->perekNimi->setText(andmed[arv + 1].toUpper().remove('\"').trimmed());
                leht->laskurid[leht->laskurid.count() - 1]->sunniAasta->setText(andmed[arv + 2].remove('\"').trimmed());
                if(leht->laskurid[leht->laskurid.count() - 1]->sunniAasta->text().toInt() < 100
                        && leht->laskurid[leht->laskurid.count() - 1]->sunniAasta->text().toInt() > 0
                        && !leht->laskurid[leht->laskurid.count() - 1]->sunniAasta->text().contains("."))
                    leht->laskurid[leht->laskurid.count() - 1]->sunniAasta->setText("19" + leht->
                                        laskurid[leht->laskurid.count() - 1]->sunniAasta->text().trimmed());
                leht->laskurid[leht->laskurid.count() - 1]->klubi->setText(andmed[arv + 3].remove('\"').trimmed());
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
                if(leht->laskurid[leht->laskurid.count() - 1]->markus->text() ==
                        leht->laskurid[leht->laskurid.count() - 1]->klubi->text())
                    leht->laskurid[leht->laskurid.count() - 1]->markus->setText("");
        }
}

void ImportAken::fromKllFile(QString fileName)
{
    KllFileRW reader(nullptr, &kirjutusabi, nullptr, nullptr, this, this);
    TabWidgetWithSettings contents = reader.readKllFile(fileName, currentCompetitorId());


    if (contents.tabWidget != nullptr) {
        QStringList tabNames;
        for (int i = 0; i < contents.tabWidget->count(); i++) {
            Leht *sheet = qobject_cast<Leht*>(qobject_cast<QScrollArea*>(contents.tabWidget->widget(i))->widget());
            if (sheet != 0 && !sheet->voistk){  // Exclude team events for now
                tabNames << (contents.tabWidget->tabText(i) + " - " + sheet->ekraaniNimi);
            }
        }
        bool ok;
        QString chosenTab = QInputDialog::getItem(this, tr("Vali leht"), tr("Leht millelt importida:"), tabNames, 0, false, &ok);

        if (ok && !chosenTab.isEmpty()){
            int tabIndex = tabNames.indexOf(chosenTab);
            Leht* sheet = qobject_cast<Leht*>(qobject_cast<QScrollArea*>(contents.tabWidget->widget(tabIndex))->widget());

            if (sheet != 0) {
                if(leht == nullptr){
                            leht = new Leht(0, sheet->seeriateArv, sheet->vSummadeSamm, 0, &kirjutusabi, tr("Ekraaninimi"), sheet->m_targetType, sheet->eventType(), 0, 0, this);
                    ui.scrollArea->setWidget(leht);
                }

                for(int i = 0; i < sheet->laskurid.count(); i++){
                    leht->uusLaskur(0);
                    Laskur *competitor = leht->laskurid[leht->laskurid.count() - 1];
                    competitor->set(sheet->laskurid[i]);
                    competitor->linnuke->setCheckState(Qt::Checked);
                }
            }
        }
        contents.tabWidget->deleteLater();
    }
}

void ImportAken::setCurrentCompetitorId(int newId)
{
    m_currentCompetitorId = newId;
}

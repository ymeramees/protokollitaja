#include "algusevalik.h"

AlguseValik::AlguseValik(QWidget *parent)
    : QDialog(parent)
{
        ui.setupUi(this);
        ui.failiNimi->setText("");
        connect(ui.avaNupp, SIGNAL(clicked()), this, SLOT(avamine()));
        connect(ui.voistluseNimi, SIGNAL(textEdited(QString)), this, SLOT(muutus(QString)));
        connect(ui.uusNupp, SIGNAL(clicked()), this, SLOT(uus()));
        connect(ui.edasiNupp, SIGNAL(clicked()), this, SLOT(edasi()));
}

void AlguseValik::avamine()
{
    QString uusNimi;
    if(ui.failiNimi->text().isEmpty())
#if QT_VERSION >= 0x050000
        uusNimi = QFileDialog::getOpenFileName(this, tr("Ava fail"),
                QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), tr("Protokollitaja fail (*.kll)"));
#else
        uusNimi = QFileDialog::getOpenFileName(this, tr("Ava fail"),
                QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation), tr("Protokollitaja fail (*.kll)"));
#endif
    else
        uusNimi = QFileDialog::getOpenFileName(this, tr("Ava fail"), ui.failiNimi->text(), tr("Protokollitaja fail (*.kll)"));
    if(!uusNimi.isEmpty()){
                ui.failiNimi->setText(uusNimi);
                //failiNimi.chop(4);
                QFile fail(ui.failiNimi->text());
                if(fail.open(QIODevice::ReadOnly)){
                        QDataStream sisse(&fail);
                        quint32 kontroll, versioon;
                        sisse >> kontroll >> versioon;
                        if(kontroll != 0x00FA3848){
                                QMessageBox::critical(this, tr("Protokollitaja"), tr("Vigane või vale fail!\n%1").arg(ui.failiNimi->text()), QMessageBox::Ok);
                                return;
                        }
                        if(versioon >= 100 && versioon <= 111){
                                QString rida;
                                //char *rida2;
                                sisse >> rida;
                                //rida = QString::fromUtf8(rida2);
                                ui.voistluseNimi->setText(rida);
                                rida.clear();
                                //*rida2 = 0;
                                sisse >> rida;
                                //rida = QString::fromUtf8(rida2);
                                ui.aegKohtEdit->setText(rida);
                        }else QMessageBox::critical(this, tr("Protokollitaja"), tr("Vale versiooni fail!\n\nVõimalik, et "
                                  "tegu on uuema programmi versiooni failiga.\n\n(AlguseValik::avamine())"),QMessageBox::Ok);
                        fail.close();
                }
                //this->accept();
        }
}

void AlguseValik::edasi()
{
        if(ui.voistluseNimi->text().isEmpty() || ui.failiNimi->text().isEmpty()){
                QMessageBox::critical(this, "Protokollitaja", tr("Mõlemad lahtrid peavad olema täidetud!"), QMessageBox::Ok);
                return;
        }
        QFile fail(ui.failiNimi->text());
        if(!fail.open(QIODevice::ReadOnly)){
                if(QMessageBox::question(this, tr("Küsimus"), "Sellise nimega faili ei ole. Kas soovite selle luua?",
                                QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok){
                        fail.close();
                        if(fail.open(QIODevice::WriteOnly)){
                                QDataStream valja(&fail);
                                valja << (quint32)0x00FA3848;	//Kontrollarv
                                valja << (qint32)111;			//Millise programmi versiooni failiga on tegu
                                valja.setVersion(QDataStream::Qt_4_3);
                                valja << ui.voistluseNimi->text()/*.toLatin1()*/;
                                valja << ui.aegKohtEdit->text()/*.toLatin1()*/;
                                valja << 1 << 1 << 5 << 0 << 0 << 0;  //kirjutusabi << autosave << autosave aeg << sakiAsukoht << sakkideArv << järjestamine;
                        }else QMessageBox::critical(this, "Protokollitaja", tr("Ei õnnestu faili luua. Kontrollige kas "
                                        "teil on sinna kausta kirjutamise õigus."), QMessageBox::Ok);
                }else return;
        }
        this->accept();
}

void AlguseValik::muutus(QString)
{
        ui.failiNimi->setText("");
}

void AlguseValik::uus()
{
        QString uusNimi = QInputDialog::getText(this, tr("Sisestage uue võistluse nimi"), tr("Võistluse nimi:"),
                        QLineEdit::Normal, ui.voistluseNimi->text());
#if QT_VERSION >= 0x050000
        QString failiNimi = QFileDialog::getSaveFileName(this, tr("Salvesta"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                                                         + "\\" + uusNimi, tr("Protokollitaja fail (*.kll)"));
#else
        QString failiNimi = QFileDialog::getSaveFileName(this, tr("Salvesta"), QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)
                                                         + "\\" + uusNimi, tr("Protokollitaja fail (*.kll)"));
#endif
        if(!failiNimi.endsWith(".kll"))
                failiNimi.append(".kll");
        if(!uusNimi.isEmpty() && !failiNimi.isEmpty() && failiNimi != ".kll"){
                ui.voistluseNimi->setText(uusNimi);
                ui.failiNimi->setText(failiNimi);
        }
}

AlguseValik::~AlguseValik()
{

}

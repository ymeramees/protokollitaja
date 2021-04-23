#include "initialdialog.h"

InitialDialog::InitialDialog(QWidget *parent)
    : QDialog(parent)
{
        ui.setupUi(this);
        ui.failiNimi->setText("");
        resetDates();

        connect(ui.avaNupp, SIGNAL(clicked()), this, SLOT(avamine()));
        connect(ui.voistluseNimi, SIGNAL(textEdited(QString)), this, SLOT(muutus(QString)));
        connect(ui.uusNupp, SIGNAL(clicked()), this, SLOT(newCompetition()));
        connect(ui.edasiNupp, SIGNAL(clicked()), this, SLOT(edasi()));
}

void InitialDialog::avamine()
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
                        if(versioon >= 100 && versioon <= 112){
                                QString rida;
                                //char *rida2;
                                sisse >> rida;
                                //rida = QString::fromUtf8(rida2);
                                ui.voistluseNimi->setText(rida);
                                rida.clear();
                                //*rida2 = 0;
                                sisse >> rida;
                                //rida = QString::fromUtf8(rida2);
                                ui.kohtEdit->setText(rida);
                        }else QMessageBox::critical(this, tr("Protokollitaja"), tr("Vale versiooni fail!\n\nVõimalik, et "
                                  "tegu on uuema programmi versiooni failiga.\n\n(AlguseValik::avamine())"),QMessageBox::Ok);
                        fail.close();
                }
        }
}

void InitialDialog::closeEvent(QCloseEvent *event)
{
    if(ui.voistluseNimi->text().isEmpty() || ui.kohtEdit->text().isEmpty())
        event->ignore();
    else
        event->accept();
}

QString InitialDialog::competitionName() const
{
    return ui.voistluseNimi->text();
}

void InitialDialog::edasi()
{
    if(competitionName().isEmpty() || fileName().isEmpty() || place().isEmpty()){
        QMessageBox::critical(this, "Protokollitaja", tr("Kõik lahtrid peavad olema täidetud!"), QMessageBox::Ok);
        return;
    }
    if(ui.endDateEdit->date() < ui.startDateEdit->date()) {
        QMessageBox::critical(this, "Protokollitaja", tr("Lõpukuupäev ei saa olla enne alguskuupäeva!"), QMessageBox::Ok);
        return;
    }
    if(ui.startDateEdit->date().daysTo(ui.endDateEdit->date()) > 7) {
        if(QMessageBox::warning(
                    this,
                    "Protokollitaja",
                    tr("Alguse ja lõpukuupäevade vahe on kahtlaselt pikk! Kas soovite jätkata?"),
                    QMessageBox::Ok | QMessageBox::Cancel
                    ) == QMessageBox::Cancel)
        return;
    }
    CompetitionSettings data;
    data.competitionName = competitionName();
    data.startDate = startDate();
    data.endDate = endDate();
    data.place = place();
    data.fileName = fileName();
    QFile fail(fileName());
    if(!fail.open(QIODevice::ReadOnly)){
        if(QMessageBox::question(
                    this,
                    tr("Küsimus"),
                    "Sellise nimega faili ei ole. Kas soovite selle luua?",
                    QMessageBox::Ok | QMessageBox::Cancel
                    ) == QMessageBox::Ok){
            if (SimpleKllFileRW::writeInitialKll(ui.failiNimi->text(), data, this))
                this->accept();
            return;
        } else
            return;
    }
    this->accept();
}

QDate InitialDialog::endDate() const
{
    return ui.endDateEdit->date();
}

QString InitialDialog::fileName() const
{
    return ui.failiNimi->text();
}

void InitialDialog::muutus(QString)
{
        ui.failiNimi->setText("");
}

QString InitialDialog::place() const
{
    return ui.kohtEdit->text();
}

void InitialDialog::resetDates()
{
    setStartDate(QDate::currentDate());
    setEndDate(QDate::currentDate().addDays(1));
    setStartDate(QDate::currentDate());
}

void InitialDialog::setData(CompetitionSettings data)
{
    ui.voistluseNimi->setText(data.competitionName);
    setStartDate(data.startDate);
    setEndDate(data.endDate);
    ui.kohtEdit->setText(data.place);
    ui.failiNimi->setText(data.fileName);
}

void InitialDialog::setEndDate(const QDate endDate)
{
    ui.endDateEdit->setDate(endDate);
}

void InitialDialog::setStartDate(const QDate startDate)
{
    ui.startDateEdit->setDate(startDate);
}

QDate InitialDialog::startDate() const
{
    return ui.startDateEdit->date();
}

void InitialDialog::newCompetition()
{
    bool ok;
    QString uusNimi = QInputDialog::getText(
                this,
                tr("Sisestage uue võistluse nimi"),
                tr("Võistluse nimi:"),
                QLineEdit::Normal,
                ui.voistluseNimi->text(),
                &ok
                );

    if(ok && !uusNimi.isEmpty()) {
        QString failiNimi = QFileDialog::getSaveFileName(
                    this,
                    tr("Salvesta"),
                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + uusNimi,
                    tr("Protokollitaja fail (*.kll)")
                    );

        if(!failiNimi.isEmpty() && !failiNimi.isNull()) {

            if(!failiNimi.endsWith(".kll"))
                failiNimi.append(".kll");

            ui.voistluseNimi->setText(uusNimi);
            ui.failiNimi->setText(failiNimi);
            resetDates();
        }
    }
}

InitialDialog::~InitialDialog()
{

}

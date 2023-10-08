#include "initialdialog.h"

InitialDialog::InitialDialog(QWidget *parent)
    : QDialog(parent)
{
        ui.setupUi(this);
        ui.fileNameEdit->setText("");
        resetDates();

//        QList<QLocale> allLocales = QLocale::matchingLocales(
//            QLocale::AnyLanguage,
//            QLocale::AnyScript,
//            QLocale::AnyCountry);

        QStringList countryNames = {"Afghanistan", "Albania", "Algeria", "Andorra", "Angola", "Argentina", "Armenia", "Australia", "Austria", "Azerbaijan", "Bahamas", "Bahrain", "Bangladesh", "Barbados", "Belarus", "Belgium", "Belize", "Benin", "Bhutan", "Bolivia", "BiH", "Botswana", "Brazil", "Brunei ", "Bulgaria", "Burkina Faso", "Burundi", "Côte d'Ivoire", "Cabo Verde", "Cambodia", "Cameroon", "Canada", "Chad", "Chile", "China", "Colombia", "Comoros", "Congo", "Costa Rica", "Croatia", "Cuba", "Cyprus", "Czechia", "Denmark", "Djibouti", "Dominica", "Dominican", "Ecuador", "Egypt", "El Salvador", "Equatorial Guinea", "Eritrea", "Estonia", "Eswatini", "Ethiopia", "Fiji", "Finland", "France", "Gabon", "Gambia", "Georgia", "Germany", "Ghana", "Greece", "Grenada", "Guatemala", "Guinea", "Guinea-Bissau", "Guyana", "Haiti", "Holy See", "Honduras", "Hungary", "Iceland", "India", "Indonesia", "Iran", "Iraq", "Ireland", "Israel", "Italy", "Jamaica", "Japan", "Jordan", "Kazakhstan", "Kenya", "Kiribati", "Kuwait", "Kyrgyzstan", "Laos", "Latvia", "Lebanon", "Lesotho", "Liberia", "Libya", "Liechtenstein", "Lithuania", "Luxembourg", "Madagascar", "Malawi", "Malaysia", "Maldives", "Mali", "Malta", "Marshall Islands", "Mauritania", "Mauritius", "Mexico", "Micronesia", "Moldova", "Monaco", "Mongolia", "Montenegro", "Morocco", "Mozambique", "Myanmar", "Namibia", "Nauru", "Nepal", "Netherlands", "New Zealand", "Nicaragua", "Niger", "Nigeria", "North Korea", "North Macedonia", "Norway", "Oman", "Pakistan", "Palau", "Palestine State", "Panama", "Papua New Guinea", "Paraguay", "Peru", "Philippines", "Poland", "Portugal", "Qatar", "Romania", "Rwanda", "Saint Lucia", "Samoa", "San Marino", "Saudi Arabia", "Senegal", "Serbia", "Seychelles", "Sierra Leone", "Singapore", "Slovakia", "Slovenia", "Solomon Islands", "Somalia", "South Africa", "South Korea", "South Sudan", "Spain", "Sri Lanka", "Sudan", "Suriname", "Sweden", "Switzerland", "Syria", "Tajikistan", "Tanzania", "Thailand", "Timor-Leste", "Togo", "Tonga", "Trinidad and Tobago", "Tunisia", "Turkey", "Turkmenistan", "Tuvalu", "Uganda", "Ukraine", "UAE", "United Kingdom", "USA", "Uruguay", "Uzbekistan", "Vanuatu", "Venezuela", "Vietnam", "Yemen", "Zambia", "Zimbabwe"
};
//        for(QLocale locale : allLocales) {
//            countryNames.append(QLocale::territoryToString(locale.territory()));
//        }
        countryNames.removeDuplicates();
        countryNames.sort();

        ui.countryCombo->addItems(countryNames);

        connect(ui.browseButton, SIGNAL(clicked()), this, SLOT(avamine()));
        connect(ui.competitionNameEdit, SIGNAL(textEdited(QString)), this, SLOT(muutus(QString)));
        connect(ui.newButton, SIGNAL(clicked()), this, SLOT(newCompetition()));
        connect(ui.forwardButton, SIGNAL(clicked()), this, SLOT(edasi()));
}

void InitialDialog::avamine()
{
    QString uusNimi;
    if(ui.fileNameEdit->text().isEmpty())
#if QT_VERSION >= 0x050000
        uusNimi = QFileDialog::getOpenFileName(this, tr("Ava fail"),
                QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), tr("Protokollitaja fail (*.kll)"));
#else
        uusNimi = QFileDialog::getOpenFileName(this, tr("Ava fail"),
                QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation), tr("Protokollitaja fail (*.kll)"));
#endif
    else
        uusNimi = QFileDialog::getOpenFileName(this, tr("Ava fail"), ui.fileNameEdit->text(), tr("Protokollitaja fail (*.kll)"));
    if(!uusNimi.isEmpty()){
        ui.fileNameEdit->setText(uusNimi);
        //failiNimi.chop(4);
        QFile fail(ui.fileNameEdit->text());
        if(fail.open(QIODevice::ReadOnly)) {
            QDataStream sisse(&fail);
            CompetitionSettings competitionSettings = SimpleKllFileRW::readCompetitionSettings(&sisse, this);
            ui.competitionNameEdit->setText(competitionSettings.competitionName);
            ui.startDateEdit->setDate(competitionSettings.startDate);
            ui.endDateEdit->setDate(competitionSettings.endDate);
            ui.placeEdit->setText(competitionSettings.place);
            ui.countryCombo->setCurrentText(competitionSettings.country);
        } else QMessageBox::critical(this, tr("Protokollitaja"), tr("Vale versiooni fail!\n\nVõimalik, et "
                                                                 "tegu on uuema programmi versiooni failiga.\n\n(AlguseValik::avamine())"),QMessageBox::Ok);
        fail.close();
    }
}

void InitialDialog::closeEvent(QCloseEvent *event)
{
    if(ui.competitionNameEdit->text().isEmpty() || ui.placeEdit->text().isEmpty())
        event->ignore();
    else
        event->accept();
}

QString InitialDialog::competitionName() const
{
    return ui.competitionNameEdit->text();
}

QString InitialDialog::country() const
{
    return ui.countryCombo->currentText();
}

void InitialDialog::edasi()
{
    if(competitionName().isEmpty() || fileName().isEmpty() || place().isEmpty()){
        QMessageBox::critical(this, "Protokollitaja", tr("Kõik lahtrid peavad olema täidetud!\n\nUue võistluse loomiseks vajutage \"Uus\" nupule"), QMessageBox::Ok);
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
                tr("Sellise nimega faili ei ole. Kas soovite selle luua?"),
                    QMessageBox::Ok | QMessageBox::Cancel
                    ) == QMessageBox::Ok){
            if (SimpleKllFileRW::writeInitialKll(ui.fileNameEdit->text(), data, this))
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
    return ui.fileNameEdit->text();
}

void InitialDialog::muutus(QString)
{
        ui.fileNameEdit->setText("");
}

QString InitialDialog::place() const
{
    return ui.placeEdit->text();
}

void InitialDialog::resetDates()
{
    setStartDate(QDate::currentDate());
    setEndDate(QDate::currentDate().addDays(1));
    setStartDate(QDate::currentDate());
}

void InitialDialog::setCompetitionName(QString newName)
{
    ui.competitionNameEdit->setText(newName);
}

void InitialDialog::setCountry(QString newCountry)
{
    ui.countryCombo->setCurrentText(newCountry);
}

void InitialDialog::setData(CompetitionSettings data)
{
    ui.competitionNameEdit->setText(data.competitionName);
    setStartDate(data.startDate);
    setEndDate(data.endDate);
    ui.placeEdit->setText(data.place);
    ui.fileNameEdit->setText(data.fileName);
}

void InitialDialog::setEndDate(const QDate endDate)
{
    ui.endDateEdit->setDate(endDate);
}

void InitialDialog::setFileName(QString newName)
{
    ui.fileNameEdit->setText(newName);
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
                ui.competitionNameEdit->text(),
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

            ui.competitionNameEdit->setText(uusNimi);
            ui.fileNameEdit->setText(failiNimi);
            resetDates();
        }
    }
}

InitialDialog::~InitialDialog()
{

}

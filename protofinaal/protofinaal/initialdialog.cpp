#include "initialdialog.h"
#include "ui_initialdialog.h"

InitialDialog::InitialDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InitialDialog),
    m_finalsFormats()
{
    ui->setupUi(this);
    connect(ui->browseButton, &QPushButton::clicked, this, &InitialDialog::openFile);
    connect(ui->newButton, &QPushButton::clicked, this, &InitialDialog::newFile);
    connect(ui->forwardButton, &QPushButton::clicked, this, &InitialDialog::forward);

    ui->eventBox->addItems(m_finalsFormats.formatIds());
}

InitialDialog::~InitialDialog()
{
    delete ui;
}

QString InitialDialog::competitionName()
{
    return ui->competitionNameEdit->text();
}

QString InitialDialog::fileName()
{
    return ui->fileNameEdit->text();
}

QJsonObject InitialDialog::eventConf()
{
    return m_finalsFormats.confById(ui->eventBox->currentText());
}

QString InitialDialog::eventName()
{
    return ui->eventNameEdit->text();
}

QString InitialDialog::eventType()
{
    return ui->eventBox->currentText();
}

void InitialDialog::forward()
{
    if(ui->competitionNameEdit->text().isEmpty() || ui->fileNameEdit->text().isEmpty()){
            QMessageBox(
                        QMessageBox::Icon::Critical,
                        "Protofinaal",
                        tr("Mõlemad lahtrid peavad olema täidetud!\n\nUue võistluse tegemiseks vajutage \"Uus\" nupule"),
                        QMessageBox::Ok,
                        this
                        ).exec();
            return;
    }
    this->accept();
}

void InitialDialog::newFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Salvesta uus fail"), ui->fileNameEdit->text(), tr("Protofinaali fail (*.fin)"));
    if(!fileName.isEmpty()){
        ui->fileNameEdit->setText(fileName);
        ui->competitionNameEdit->setReadOnly(false);
        ui->competitionNameEdit->setDisabled(false);
        emit updateMe();
    }
}

void InitialDialog::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Ava fail"), ui->fileNameEdit->text(), tr("Protofinaali fail (*.fin)"));
    if(!fileName.isEmpty()){
        ui->fileNameEdit->setText(fileName);
        ui->competitionNameEdit->setReadOnly(true);
        ui->competitionNameEdit->setDisabled(true);
        emit updateMe();
    }
}

QString InitialDialog::timePlace()
{
    return ui->timePlaceEdit->text();
}

void InitialDialog::setCompetitionName(QString competitionName)
{
    ui->competitionNameEdit->setText(competitionName);
}

void InitialDialog::setFileName(QString fileName)
{
    ui->fileNameEdit->setText(fileName);
}

void InitialDialog::setEventName(QString newName)
{
    ui->eventNameEdit->setText(newName);
}

void InitialDialog::setEventType(QString newType)
{
    ui->eventBox->setCurrentText(newType);
}

void InitialDialog::setTimePlace(QString timePlace)
{
    ui->timePlaceEdit->setText(timePlace);
}

#include "initialdialog.h"
#include "ui_initialdialog.h"

InitialDialog::InitialDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InitialDialog)
{
    ui->setupUi(this);
    connect(ui->browseButton, &QPushButton::clicked, this, &InitialDialog::openFile);
    connect(ui->newButton, &QPushButton::clicked, this, &InitialDialog::newFile);
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

void InitialDialog::newFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Salvesta uus fail"), ui->fileNameEdit->text(), tr("Protofinaali fail (*.fin)"));
    if(!fileName.isEmpty()){
        ui->fileNameEdit->setText(fileName);
        ui->competitionNameEdit->setReadOnly(false);
        emit updateMe();
    }
}

void InitialDialog::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Ava fail"), ui->fileNameEdit->text(), tr("Protofinaali fail (*.fin)"));
    if(!fileName.isEmpty()){
        ui->fileNameEdit->setText(fileName);
        ui->competitionNameEdit->setReadOnly(true);
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

void InitialDialog::setTimePlace(QString timePlace)
{
    ui->timePlaceEdit->setText(timePlace);
}

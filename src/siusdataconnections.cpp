#include "siusdataconnections.h"
#include "ui_siusdataconnections.h"

SiusDataConnections::SiusDataConnections(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SiusDataConnections)
{
    ui->setupUi(this);
}

SiusDataConnections::~SiusDataConnections()
{
    delete ui;
}

void SiusDataConnections::connectToSiusData()
{

}

void SiusDataConnections::lineRead()
{

}

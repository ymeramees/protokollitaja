#include "siusdataconnections.h"
#include "ui_siusdataconnections.h"

extern bool verbose;

SiusDataConnections::SiusDataConnections(
        QFile *siusLog,
        QTextStream *log,
        CommonSettings *settings,
        QWidget *parent
        ) : QDialog(parent),
    ui(new Ui::SiusDataConnections)
{
    ui->setupUi(this);
    this->siusLog = siusLog;
    this->log = log;
    m_settings = settings;

    connect(ui->connectButton, &QPushButton::clicked, this, &SiusDataConnections::connectToSiusData);
}

SiusDataConnections::~SiusDataConnections()
{
    if(verbose)
        QTextStream(stdout) << "SiusDataConnections::~SiusDataConnections()" << Qt::endl;
    for(SiusDataConnection *socket : sockets){
        socket->deleteLater();
    }
    sockets.clear();
    delete ui;
}

void SiusDataConnections::connectToSiusData()
{
    if(verbose)
        QTextStream(stdout) << "connectToSiusData()" << Qt::endl;
    bool existing = false;
    for(SiusDataConnection *socket : sockets)
        if(socket->address() == ui->addressEdit->text() && socket->port() == ui->portEdit->text().toInt()){
            existing = true;
            socket->reconnectToSius();
        }

    if(verbose)
        QTextStream(stdout) << "connectToSiusData(): existing = " << existing << Qt::endl;

    if(!existing){
        SiusDataConnection *socket = new SiusDataConnection(
                    ui->addressEdit->text(),
                    ui->portEdit->text().toInt(),
                    sockets.length(),
                    siusLog,
                    log,
                    m_settings,
                    this
                    );
        if(vBox == nullptr){
            vBox = new QVBoxLayout;
            vBox->addStretch(1);
        }
        vBox->insertWidget(vBox->count() - 1, socket);
        ui->existingConnectionsBox->setLayout(vBox);

        sockets.append(socket);
//        ui->verticalLayout->addItem(socket);
        connect(socket, &SiusDataConnection::shotRead, this, &SiusDataConnections::shotRead);
        connect(socket, &SiusDataConnection::statusInfo, this, &SiusDataConnections::statusInfo);
        connect(socket, &SiusDataConnection::disconnectedFromSius, this, &SiusDataConnections::disconnectedFromSius);
    }
    this->accept();
}

void SiusDataConnections::sendDataToRangeControl(QString data)
{
    bool sent = false;
    for(SiusDataConnection *socket : sockets)
        if (socket->isConnected()) {
            socket->sendData(data);
            sent = true;
        }

    if (!sent)
        emit errorInfo(tr("Aktiivseid ühendusi ei leitud! Ei saanud infot saata!"));
}

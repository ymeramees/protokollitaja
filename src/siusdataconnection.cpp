#include "siusdataconnection.h"

SiusDataConnection::SiusDataConnection(QString address, int port, int socketIndex, QFile *siusLog, QTextStream *log, QWidget *parent) : QWidget(parent)
{
    addressLabel = new QLabel(address, this);
    myIndex = socketIndex;
    this->port = port;
    this->siusLog = siusLog;
    this->log = log;

    disconnectButton = new QPushButton(tr("Sulge ühendus"), this);
    connect(disconnectButton, &QPushButton::clicked, this, disconnectFromSius);

    reconnectButton = new QPushButton(tr("Ühendu uuesti"), this);
    connect(reconnectButton, &QPushButton::clicked, this, reconnectToSius);

    siusDataSocket = new QTcpSocket(this);
    connect(siusDataSocket, &QTcpSocket::connected, addressLabel, &QLabel::setEnabled);
    connect(siusDataSocket, &QTcpSocket::disconnected, this, wasDisconnected);

    siusDataSocket->connectToHost(address, port);
    if(progress == 0){
        progress = new QProgressDialog(tr("SiusData'ga ühendumine..."), "Loobu", 0, 0, this);
        progress->setWindowModality(Qt::WindowModal);
        connect(progress, SIGNAL(canceled()), this, SLOT(stopProgress())); //To enable to cancel connection process
    }
    progress->show();
}

SiusDataConnection::~SiusDataConnection(){
    addressLabel->deleteLater();
    disconnectButton->deleteLater();
    reconnectButton->deleteLater();
    row->deleteLater();
    siusDataSocket->abort();
    siusDataSocket->deleteLater();
}

void SiusDataConnection::disconnectFromSius()
{
    socket->disconnectFromHost();
    emit disconnectedFromSius(myIndex);
}

void SiusDataConnection::readFromSius()
{
    if(siusDataSocket->bytesAvailable() > 5 || siusBuffer.length() > 0){
        QStringList lines;
        progress->setLabelText(tr("SiusDatast andmete vastuvõtt..."));

        while(siusDataSocket->bytesAvailable() > 2){
          siusBuffer.append(siusDataSocket->readAll());  //Start of line 5f, end 0d 0a
          emit statusInfo(tr("Saabus info, buffer.length(): %1").arg(siusBuffer.length()));

        }
//    QFile file("siusData.txt");
//    if (file.open(QFile::ReadOnly)){
//        siusDatast = file.readAll();
//        QString rida;
//    }

//        logi->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append); //Saabunud muudatuste ja laskude logifail
//        QTextStream logiValja(logi);

        while(siusBuffer.contains('_')){

//            progressTimer->start();   //Is this really needed?

            if(siusLog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){ //Log of incoming data
                QTextStream out(siusLog);
                out << siusBuffer.left(siusBuffer.indexOf('_', 1));
                siusLog->close();
            }

            log << QTime::currentTime().toString("#hh:mm:ss") << " #buffer.length(): " << siusBuffer.length() << "    buffer.indexOf('_'): " << siusBuffer.indexOf('_', 1) << "\n";

            QString row = "";
            if(siusBuffer.indexOf('_', 1) == -1){
                log << "#clear()\n";
                row = QString("%1").arg(siusBuffer);
                siusBuffer.clear();
            }else{
                row = siusBuffer.left(siusBuffer.indexOf('_', 1));
                siusBuffer.remove(0, siusBuffer.indexOf('_', 1));
            }

            if(!row.contains('_')){
                log << "#break\n";
                break; //Double check not to continue with pointless row
            }
            log << "#rida: " << row;
            lines.append(row);

            QCoreApplication::processEvents();
        }
        if(siusBuffer.length() > 0){   //If there is still data in the buffer, but for some reason reached here, then start the function again after a while
            log << "#buffer.length(): " << siusBuffer.length() << ", uuele ringile minek" << "\n";
            QTimer::singleShot(170, this, SLOT(loeSiusDatast()));
        }

        emit linesRead(lines);
    }
}

void SiusDataConnection::reconnectToSius()
{
    siusDataSocket->connectToHost(addressLabel->text(), port);
}

void SiusDataConnection::stopProgress()
{
    if(progress !=0){
        if(progress->wasCanceled()){
            siusDataSocket->abort();
        }else progress->accept();
    }
}

void SiusDataConnection::wasDisconnected()
{
    addressLabel->setEnabled(false);
    emit disconnectedFromSius(myIndex);
}

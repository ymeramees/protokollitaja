#include "protolehelugejaserver.h"

extern bool verbose;

ProtolehelugejaServer::ProtolehelugejaServer(QObject *parent)
    : QTcpServer(parent)
{
    connect(this, &QTcpServer::newConnection, this, &ProtolehelugejaServer::newConnection);
}

ProtolehelugejaServer::~ProtolehelugejaServer()
{
    for(int i = 0; i < sockets.length(); i++){
        sockets.at(i)->abort();
        sockets.at(i)->deleteLater();
    }
}

void ProtolehelugejaServer::closeConnection(int socketIndex)
{
    QMessageBox::warning(dynamic_cast<QWidget*>(this->parent()), tr("Info"), tr("Protolehelugeja connection %1 lost!").arg(socketIndex), QMessageBox::Ok);
    sockets.at(socketIndex)->deleteLater();
    sockets.removeAt(socketIndex);

    for(int i = 0; i < sockets.count(); i++){
        sockets.at(i)->setSocketIndex(i);
    }
}

void ProtolehelugejaServer::closeConnections()
{
    if(verbose)
        QTextStream(stdout) << "ProtolehelugejaServer::closeConnections()" << Qt::endl;;
    while(hasPendingConnections()){
        QTcpSocket *socket = nextPendingConnection();
        connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
        socket->close();
    }
    for(ProtolehelugejaConnection *socket : sockets){
        socket->disconnect();
        socket->deleteLater();
    }
    sockets.clear();
}

void ProtolehelugejaServer::incomingShotInfo(int socketIndex)
{
    emit shotInfoRead(sockets.at(socketIndex)->lastReceivedLine(), socketIndex);
}

void ProtolehelugejaServer::newConnection()
{
    if(hasPendingConnections()){
        ProtolehelugejaConnection *socket = new ProtolehelugejaConnection(this->nextPendingConnection());
        connect(socket, &ProtolehelugejaConnection::disconnected, this, &ProtolehelugejaServer::closeConnection);
        sockets.append(socket);
        socket->setSocketIndex(sockets.length()-1);

        if(verbose)
            QTextStream(stdout) << "ProtolehelugejaServer::newConnection(), index = " << socket->getSocketIndex() << Qt::endl;;

        connect(socket, &ProtolehelugejaConnection::shotInfoRead, this, &ProtolehelugejaServer::incomingShotInfo);
        connect(socket, &ProtolehelugejaConnection::renewWithTargetNumber, this, &ProtolehelugejaServer::readTargetNumber);
        connect(socket, &ProtolehelugejaConnection::save, this, &ProtolehelugejaServer::saveRequested);
    }
}

void ProtolehelugejaServer::readTargetNumber(int socketIndex)
{
    emit renewWithTargetNumber(sockets.at(socketIndex)->getAskedTargetNumber(), socketIndex);
}

void ProtolehelugejaServer::saveRequested()
{
    emit save();
}

void ProtolehelugejaServer::send(QString data, int socketIndex)
{
    if(sockets.length() > socketIndex)
        sockets.at(socketIndex)->send(data);
    else
        QMessageBox::warning(dynamic_cast<QWidget*>(this->parent()), tr("Error!"), tr("Protolehelugeja address is incorrect! Cannot send the reply!\n\nIt is possible that some connection has been lost."), QMessageBox::Ok);
}

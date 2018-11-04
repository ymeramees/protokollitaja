#include "protolehelugejaserver.h"

extern bool verbose;

ProtolehelugejaServer::ProtolehelugejaServer(QObject *parent)
    : QTcpServer(parent)
{
//    mapper = new QSignalMapper(this);
    connect(this, &QTcpServer::newConnection, this, newConnection);
//    connect(mapper, &QSignalMapper::mapped, this, readTargetNumber);
//    connect(mapper, &QSignalMapper::mapped, this, readTargetNumber);
}

ProtolehelugejaServer::~ProtolehelugejaServer()
{
    for(int i = 0; i < sockets.length(); i++){
        sockets.at(i)->abort();
        sockets.at(i)->deleteLater();
    }
}

void ProtolehelugejaServer::closeConnection()
{

}

void ProtolehelugejaServer::closeConnections()
{
    if(verbose)
        QTextStream(stdout) << "ProtolehelugejaServer::closeConnections()" << endl;
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
//        connect(socket, &QTcpSocket::readyRead, readIncomingData);
        connect(socket, &ProtolehelugejaConnection::disconnected, this, closeConnection);
        sockets.append(socket);
        socket->setSocketIndex(sockets.length()-1);

        if(verbose)
            QTextStream(stdout) << "ProtolehelugejaServer::newConnection(), index = " << socket->getSocketIndex() << endl;

        connect(socket, &ProtolehelugejaConnection::shotInfoRead, this, incomingShotInfo);
        connect(socket, &ProtolehelugejaConnection::renewWithTargetNumber, this, readTargetNumber);
        connect(socket, &ProtolehelugejaConnection::save, this, saveRequested);

//        mapper->setMapping(sockets.last(), sockets.length()-1);
//        connect(socket, &ProtolehelugejaConnection::renewWithTargetNumber, mapper, &QSignalMapper::map);


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
        QMessageBox::warning(dynamic_cast<QWidget*>(this->parent()), "Viga", "Protolehelugeja aadress vale! Ei saa vastust saata!\n\nVõimalik, et mõni ühendus on vahepeal katkenud.", QMessageBox::Ok);
//    blockSize = 0;
//    QByteArray block;
//    QDataStream valja(&block, QIODevice::WriteOnly);
//    valja.setVersion(QDataStream::Qt_4_8);
//    valja << (quint16)0;
//    valja << data;
//    valja.device()->seek(0);
//    valja << (quint16)(block.size() - sizeof(quint16));
//    sockets.at(socketIndex)->write(block);
//#ifdef PROOV
//        qDebug() << "Valja: " << data;
//#endif
//    block.clear();
}

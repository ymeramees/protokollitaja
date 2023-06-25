#include "dataconnection.h"

#define CR 0x0d
#define LF 0x0a

DataConnection::DataConnection(QTcpSocket *parent) : QObject(parent)
{
    socket = parent;
    connect(socket, &QTcpSocket::disconnected, this, &DataConnection::wasDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &DataConnection::readData);
}

void DataConnection::abort()
{
    socket->abort();
}

void DataConnection::readData()
{
    static quint16 blockSize = 0;
    static quint16 protocolVersion = 0;
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_5);

    if (blockSize == 0){
        if (socket->bytesAvailable() < (int)sizeof(quint16)){
            return; //Packet size info (quint16) not yet received
        }
        in >> blockSize;
    }

    if (socket->bytesAvailable() < blockSize){
        return; //Whole packet not yet received
    }

    QString message;

    in >> protocolVersion;
    in >> message;
    blockSize = 0;

    QTextStream(stdout) << "DataConnection::readData, protocolVersion: " << protocolVersion << ", message = " << message << Qt::endl;

    if (protocolVersion == 1)
        emit startListReceived(message.split('\n'));
    else
        emit error(tr("Tundmatu sissetuleva ühenduse versioon: %1\nKontrollige, kas Protokollitaja ja Range Control'i versioonid ühtivad!")
                       .arg(protocolVersion));
}

void DataConnection::wasDisconnected()
{
    emit disconnected(myIndex);
}

int DataConnection::socketIndex()
{
    return myIndex;
}

//QString DataConnection::lastReceivedLine()
//{
//    return lastRecvdLine;
//}

void DataConnection::send(QString data)
{
//    if(verbose)
    QTextStream(stdout) << "DataConnection::send(): " << data << Qt::endl;
//    QByteArray block;
//    QTextStream out(socket);
//    out.setVersion(QDataStream::Qt_5_12);
//    out << (quint16)0;
//    out << data << CR << LF;
//    out.device()->seek(0);
//    out << (quint16)(block.size() - sizeof(quint16));
    socket->write(data.toLatin1().append(CR).append(LF));
//    block.clear();
}

void DataConnection::setSocketIndex(int socketIndex)
{
    myIndex = socketIndex;
}

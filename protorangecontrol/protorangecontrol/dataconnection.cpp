#include "dataconnection.h"

#define CR 0x0d
#define LF 0x0a

DataConnection::DataConnection(QTcpSocket *parent) : QObject(parent)
{
    socket = parent;
    connect(socket, &QTcpSocket::disconnected, this, &DataConnection::wasDisconnected);
}

void DataConnection::abort()
{
    socket->abort();
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

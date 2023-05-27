#include "connectionserver.h"

ConnectionServer::ConnectionServer(QTextStream *incomingLog, QObject *parent) : QObject(parent)
{
    m_incomingLog = incomingLog;
    connect(&m_dataServer, &QTcpServer::newConnection, this, &ConnectionServer::newDataConnection);
    connect(&m_inbandServer, &QTcpServer::newConnection, this, &ConnectionServer::newInbandConnection);
}

ConnectionServer::~ConnectionServer()
{
    foreach(DataConnection *dataSocket, dataSockets){
        dataSocket->abort();
        dataSocket->deleteLater();
    }
}

void ConnectionServer::closeDataConnection(int socketIndex)
{
    emit error(tr("Data connection %1 disconnected!").arg(socketIndex));
    dataSockets.at(socketIndex)->deleteLater();
    dataSockets.removeAt(socketIndex);

    for(int i = 0; i < dataSockets.count(); i++){
        dataSockets.at(i)->setSocketIndex(i);
    }
}

void ConnectionServer::closeDataConnections()
{
//    if(verbose)
    QTextStream(stdout) << "ConnectionServer::closeConnections()" << Qt::endl;
    while(m_dataServer.hasPendingConnections()){
        QTcpSocket *socket = m_dataServer.nextPendingConnection();
        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
        socket->close();
    }
    for(DataConnection *socket : dataSockets){
        socket->disconnect();
        socket->deleteLater();
    }
    dataSockets.clear();
}

//void ConnectionServer::incomingShotInfo(int socketIndex)
//{
//    emit shotInfoRead(dataSockets.at(socketIndex)->lastReceivedLine(), socketIndex);
//}

//void ConnectionServer::readFromInband(QString msg)
//{

//}

void ConnectionServer::newDataConnection()
{
    if(m_dataServer.hasPendingConnections()){
        DataConnection *socket = new DataConnection(m_dataServer.nextPendingConnection());
        connect(socket, &DataConnection::disconnected, this, &ConnectionServer::closeDataConnection);
        dataSockets.append(socket);
        socket->setSocketIndex(dataSockets.length()-1);

//        if(verbose)
        QTextStream(stdout) << "ConnectionServer::newDataConnection(), index = " << socket->socketIndex() << Qt::endl;
        emit newProtokollitajaConnection(socket);
    }
}

void ConnectionServer::newInbandConnection()
{
    QTextStream(stdout) << "ConnectionServer::newInbandConnection()" << Qt::endl;

    InbandConnection *inbandConnection = new InbandConnection(m_incomingLog, m_inbandServer.nextPendingConnection());
    connect(inbandConnection, &InbandConnection::newShot, [this](int target, SiusShotData shotData){
        emit newShot(target, shotData);
    });
    connect(inbandConnection, &InbandConnection::statusUpdate, [this](int target, QString newStatus){
        emit statusUpdate(target, newStatus);
    });
    connect(inbandConnection, &InbandConnection::newTarget, [this](int target, QString ip){
        emit newTarget(target, ip);
    });
    connect(inbandConnection, &InbandConnection::allShots, [this](int target, QString shotsData){
        emit allShots(target, shotsData);
    });
}

void ConnectionServer::sendAllShotsData(QStringList data, DataConnection *connection)
{
    foreach (QString shotRow, data) {
        connection->send(shotRow);
    }
}

void ConnectionServer::sendShotData(QStringList data)
{
    foreach (DataConnection *dataSocket, dataSockets) {
        foreach (QString shotRow, data) {
            dataSocket->send(shotRow);
        }
    }
}

void ConnectionServer::start(int dataPort, int inbandPort)
{
    if (!m_dataServer.listen(QHostAddress::Any, dataPort)) {
        emit error(tr("Unable to start the server: %1.").arg(m_dataServer.errorString()));
    } else {
        emit info(tr("Data server started at port %1").arg(dataPort));
    }

    if (!m_inbandServer.listen(QHostAddress::Any, inbandPort)) {
        emit error(tr("Unable to start the server: %1.").arg(m_inbandServer.errorString()));
    } else {
        emit info(tr("InBand server started at port %1").arg(inbandPort));
    }
}

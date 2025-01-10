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

    foreach(InbandConnection *connection, m_inbandConnections.values()){
        connection->close();
        connection->deleteLater();
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
        connect(socket, &DataConnection::error, this, [this](const QString &msg) { emit error(msg); });
        connect(socket, &DataConnection::startListReceived, this, [this](const QStringList &startList) { emit startListReceived(startList); });
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
    connect(inbandConnection, &InbandConnection::newShot, inbandConnection, [this](int target, SiusShotData shotData){
        emit newShot(target, shotData);
    });
    connect(inbandConnection, &InbandConnection::statusUpdate, inbandConnection, [this](int target, QString newStatus){
        emit statusUpdate(target, newStatus);
    });
    connect(inbandConnection, &InbandConnection::newTarget, inbandConnection, [this](int target, QString ip, int protocolVersion){
        m_inbandProtocolVersions.insert(ip, protocolVersion);
        emit newTarget(target, ip);
        if (protocolVersion == 0)   // Need to ack here in case of old protocol, as a new connection is needed for that
            sendMessage(target, ip, "ack");
        sendFromQueue(target, ip);
    });
    connect(inbandConnection, &InbandConnection::allShots, inbandConnection, [this](int target, QString shotsData){
        emit allShots(target, shotsData);
    });
    connect(inbandConnection, &InbandConnection::disconnected, inbandConnection, [this, inbandConnection](){
        if (m_inbandConnections.contains(inbandConnection->currentIp())) {
            m_inbandConnections.remove(inbandConnection->currentIp());
        }
        inbandConnection->deleteLater();
    });

    if (inbandConnection->currentIp() != "0.0.0.0") {
        m_inbandConnections.insert(inbandConnection->currentIp(), inbandConnection);
    }
}

void ConnectionServer::sendAllShotsData(QStringList data, DataConnection *connection)
{
    foreach (QString shotRow, data) {
        connection->send(shotRow);
    }
}

void ConnectionServer::sendFromQueue(const int target, const QString ip)
{
    if (m_outgoingQueues.contains(ip)) {
        QQueue msgsToSend = m_outgoingQueues.value(ip);
        if (!msgsToSend.isEmpty()) {
            QString msgToSend = msgsToSend.dequeue();
            m_outgoingQueues.insert(ip, msgsToSend);
            sendMessage(target, ip, msgToSend);
        }
    }
}

void ConnectionServer::sendInbandBroadcast(int target)
{
    Utils::sendInbandBroadcast(QString("%1").arg(target));
    emit info(tr("Kadunud InBandi IP küsimus saadetud"));
}

void ConnectionServer::sendMessage(const int target, const QString ip, QString message)
{
    if (m_inbandConnections.contains(ip) && m_inbandProtocolVersions.contains(ip) && m_inbandProtocolVersions.value(ip) == 1) {
        m_inbandConnections.value(ip)->sendMessage(target, message);
        emit info("Sent: " + message.replace("\n", ";"));
    } else {
        if (m_inbandProtocolVersions.contains(ip) && m_inbandProtocolVersions.value(ip) == 0) { // old connection protocol
            QTcpSocket tcpSocket;
            tcpSocket.connectToHost(ip, 5450);
            tcpSocket.waitForConnected(5000);

            QTextStream out(&tcpSocket);
            out << target << Qt::endl;
            out << message << "\nmessage end" << Qt::endl;
            tcpSocket.close();
            emit info("Sent: " + message.replace("\n", ";"));
        } else {
            QQueue<QString> queue = m_outgoingQueues.value(ip);
            queue.enqueue(message);
            m_outgoingQueues.insert(ip, queue);
            sendInbandBroadcast(target);
        }
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

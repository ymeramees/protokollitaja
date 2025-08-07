#include "inbandconnection.h"

#define CR QChar(0x0d)

InbandConnection::InbandConnection(QTextStream *log, QTcpSocket *parent): QObject(parent)
{
    socket = parent;
    m_log = log;
    connect(socket, &QTcpSocket::readyRead, this, &InbandConnection::readIncomingData);
    connect(socket, &QTcpSocket::disconnected, this, [this]() { emit disconnected(); });
    if(socket->peerAddress() == QHostAddress::Null) {
        QTextStream(stdout) << "InbandConnection::InbandConnection, QHostAddress::Null, state(): " << socket->state() << " bytes: " << socket->bytesAvailable()  << Qt::endl;
    } else {
        m_peerAddress = socket->peerAddress().toString().replace(":", "").replace("f", "");
    }

    QTextStream(stdout) << "InbandConnection::InbandConnection, m_peerAddress: " << m_peerAddress << Qt::endl;
//    QTimer::singleShot(100, this, SLOT(readIncomingData()));
}

void InbandConnection::close()
{
    socket->abort();
}

QString InbandConnection::currentIp()
{
    return m_peerAddress;
}

void InbandConnection::readIncomingData()
{
    const QString MESSAGE_END = "message end";

    m_buffer.append(socket->readAll());

    QTextStream(stdout) << "InbandConnection::readIncomingData, m_buffer = " << m_buffer << Qt::endl;

    if (m_buffer.contains(MESSAGE_END)) {   // Old connection protocol
        m_protocolVersion = 0;
        QString message = m_buffer.left(m_buffer.indexOf(MESSAGE_END) + MESSAGE_END.length());
        QString forLog = message;
        *m_log << QTime::currentTime().toString() << ": " << forLog.replace("\n", ";") << Qt::endl;
        m_buffer.remove(0, message.length());
        QTextStream(stdout) << "InbandConnection::readIncomingData, message = " << message << Qt::endl;

        QStringList msgParts = message.split('\n');
        int target = msgParts.at(0).toInt();
        QTextStream(stdout) << "InbandConnection::readIncomingData, msgParts = " << msgParts.join(",") << Qt::endl;

        if (msgParts.length() >= 3) {
            if (msgParts.at(1) == "shot") {
                QTextStream(stdout) << "Received shot: " << msgParts.join(",") << Qt::endl;
                // _SHOT;14;target;Id;60;6;time;3;1;39;value;0;0;shotNo;X;Y;900;0;0;655.35;2154896560;64;560;0
                Lask shot = Lask::fromInband(msgParts);
                SiusShotData shotData;
                shotData.shot = shot;
                shotData.siusShotNo = msgParts.at(3).toInt();
                emit newShot(target, shotData);
            } else if (msgParts.at(1) == "status") {
                emit statusUpdate(target, m_peerAddress, m_protocolVersion, msgParts.at(2));
            } else if (msgParts.at(1) == "call in") {
                emit newTarget(target, m_peerAddress, m_protocolVersion);
            } else if (msgParts.at(1) == "all shots") {
                emit allShots(target, message);
            }
        } else
            QTextStream(stdout) << "Received too short message: " << msgParts.join(",") << Qt::endl;
        if (socket->state() == QAbstractSocket::UnconnectedState) {
            QTextStream(stdout) << "InbandConnection unconnected, deleting" << Qt::endl;
            emit disconnected();
            deleteLater();  // If it has been disconnected, then no need to keep it around anymore
        }
    } else if (m_buffer.contains(CR)) {
        m_protocolVersion = 1;
        QString message = m_buffer.left(m_buffer.indexOf(CR) + 1);
        QString forLog = message;
        *m_log << QTime::currentTime().toString() << ": " << forLog.replace("\n", ";") << Qt::endl;
        m_buffer.remove(0, message.length());
        QTextStream(stdout) << "InbandConnection::readIncomingData(new), message = " << message << Qt::endl;

        QStringList msgParts = message.split(';');
        int target = msgParts.at(0).toInt();
        QTextStream(stdout) << "InbandConnection::readIncomingData(new), msgParts = " << msgParts.join(",") << Qt::endl;

        if (msgParts.length() >= 3) {
            if (msgParts.at(1) == "shot") {
                QTextStream(stdout) << "Received shot(new): " << msgParts.join(",") << Qt::endl;
                // _SHOT;14;target;Id;60;6;time;3;1;39;value;0;0;shotNo;X;Y;900;0;0;655.35;2154896560;64;560;0
                Lask shot = Lask::fromInband(msgParts);
                SiusShotData shotData;
                shotData.shot = shot;
                shotData.siusShotNo = msgParts.at(3).toInt();
                emit newShot(target, shotData);
                sendAck(target);
            } else if (msgParts.at(1) == "status") {
                emit statusUpdate(target, m_peerAddress, m_protocolVersion, msgParts.at(2));
                sendAck(target);
            } else if (msgParts.at(1) == "InBand_Scoring") {
                sendAck(target);
                emit statusUpdate(target, m_peerAddress, m_protocolVersion, msgParts.at(2));
            } else if (msgParts.at(1) == "all shots") {
                emit allShots(target, message);
                sendAck(target);
            }
        } else
            QTextStream(stdout) << "Received too short message: " << msgParts.join(",") << Qt::endl;
        if (socket->state() == QAbstractSocket::UnconnectedState) {
            QTextStream(stdout) << "InbandConnection unconnected, deleting" << Qt::endl;
            emit disconnected();
            deleteLater();  // If it has been disconnected, then no need to keep it around anymore
        }
    }
}

void InbandConnection::sendAck(const int target)
{
    sendMessage(target, "ack");
}

void InbandConnection::sendMessage(const int target, const QString message)
{
    QTextStream out(socket);
    out << target << ";" << message << ";\r";
}

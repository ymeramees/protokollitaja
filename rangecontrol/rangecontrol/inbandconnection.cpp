#include "inbandconnection.h"

InbandConnection::InbandConnection(QTextStream *log, QTcpSocket *parent): QObject(parent)
{
    socket = parent;
    m_log = log;
    connect(socket, &QTcpSocket::readyRead, this, &InbandConnection::readIncomingData);
//    connect(this, &QTcpSocket::disconnected, this, &InbandConnection::deleteLater);
    if(socket->peerAddress() == QHostAddress::Null) {
        QTextStream(stdout) << "InbandConnection::InbandConnection, QHostAddress::Null, state(): " << socket->state() << " bytes: " << socket->bytesAvailable()  << Qt::endl;
    }
    m_peerAddress = socket->peerAddress().toString().replace(":", "").replace("f", "");

    QTextStream(stdout) << "InbandConnection::InbandConnection, m_peerAddress: " << m_peerAddress << Qt::endl;
//    QTimer::singleShot(100, this, SLOT(readIncomingData()));
}

void InbandConnection::readIncomingData()
{
    const QString MESSAGE_END = "message end";

    m_buffer.append(socket->readAll());

    QTextStream(stdout) << "InbandConnection::readIncomingData, m_buffer = " << m_buffer << Qt::endl;

    if (m_buffer.contains(MESSAGE_END)) {
        QString message = m_buffer.left(m_buffer.indexOf(MESSAGE_END) + MESSAGE_END.length());
        m_buffer.remove(0, message.length());
        QTextStream(stdout) << "InbandConnection::readIncomingData, message = " << message << Qt::endl;

        QStringList msgParts = message.split('\n');
        *m_log << QTime::currentTime().toString() << ": " << msgParts.join(";") << Qt::endl;
        int target = msgParts.at(0).toInt();
        QTextStream(stdout) << "InbandConnection::readIncomingData, msgParts = " << msgParts.join(";") << Qt::endl;

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
                emit statusUpdate(target, msgParts.at(2));
            } else if (msgParts.at(1) == "call in") {
                emit newTarget(target, m_peerAddress);
            } else if (msgParts.at(1) == "all shots") {
                emit allShots(target, message);
            }
        }
        if (socket->state() == QAbstractSocket::UnconnectedState) {
            QTextStream(stdout) << "InbandConnection unconnected, deleting" << Qt::endl;
            deleteLater();  // If it has been disconnected, then no need to keep it around anymore
        }
    }
}

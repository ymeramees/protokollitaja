#ifndef INBANDCONNECTION_H
#define INBANDCONNECTION_H

#include <QHostAddress>
#include <QObject>
#include <QTcpSocket>
#include <QTextStream>
#include <QTimer>

#include "siusshotdata.h"
#include "lask.h"

class InbandConnection : public QObject
{
    Q_OBJECT
public:
    explicit InbandConnection(QTextStream *log, QTcpSocket *parent);
    QString currentIp();
    void sendAck(const int target);
    void sendMessage(const int target, const QString message);
    void close();

signals:
    void allShots(int target, QString shotsData);
    void disconnected();
    void newShot(int target, SiusShotData);
    void newTarget(int target, QString ip, int protocolVersion);
    void statusUpdate(int target, QString newStatus);

private:
    int m_protocolVersion = 0;
    QString m_buffer;
    QTextStream *m_log = nullptr;
    QString m_peerAddress = "0.0.0.0";  // Make sure we know the address, even if the peer has disconnected
    QTcpSocket *socket;

private slots:
    void readIncomingData();
};

#endif // INBANDCONNECTION_H

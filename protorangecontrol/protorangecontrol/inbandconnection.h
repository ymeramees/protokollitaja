#ifndef INBANDCONNECTION_H
#define INBANDCONNECTION_H

#include <QHostAddress>
#include <QObject>
#include <QTcpSocket>
#include <QTimer>

#include "siusshotdata.h"
#include "lask.h"

class InbandConnection : public QObject
{
    Q_OBJECT
public:
    explicit InbandConnection(QTextStream *log, QTcpSocket *parent);

signals:
    void allShots(int target, QString shotsData);
    void newShot(int target, SiusShotData);
    void newTarget(int target, QString ip);
    void statusUpdate(int, QString);

private:
    QString m_buffer;
    QTextStream *m_log = nullptr;
    QString m_peerAddress;  // Make sure we know the address, even if the peer has disconnected
    QTcpSocket *socket;

private slots:
    void readIncomingData();
};

#endif // INBANDCONNECTION_H

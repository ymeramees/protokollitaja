#ifndef RECEIVERSERVER_H
#define RECEIVERSERVER_H

#include <QMessageBox>
#include <QTcpServer>
#include <QQueue>
#include "dataconnection.h"
#include "inbandconnection.h"
#include "utils.h"

class ConnectionServer : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionServer(QTextStream *incomingLog, QObject *parent = nullptr);
    ~ConnectionServer();

public slots:
    void closeDataConnections();
    void sendShotData(QStringList data);
    void sendAllShotsData(QStringList data, DataConnection *connection);
    void sendMessage(const int target, const QString ip, QString message);
    void start(int dataPort, int inbandPort);

private:
    QList<DataConnection*> dataSockets;
    QMap<QString, InbandConnection*> m_inbandConnections;
    QMap<QString, int> m_inbandProtocolVersions;
    QMap<QString, QQueue<QString>> m_outgoingQueues;
    QMessageBox messageBox;
    QTextStream *m_incomingLog = nullptr;
    QStringList m_shotsData;
    QTcpServer m_dataServer;
    QTcpServer m_inbandServer;
    void closeDataConnection(int socketIndex);
    void newDataConnection();
    void newInbandConnection();
    void sendFromQueue(const int target, const QString ip);
    void sendInbandBroadcast(int target);

private slots:
//    void incomingShotInfo(int socketIndex);
//    void readFromInband(QString msg);

signals:
    void allShots(int target, QString shotsData);
    void error(QString errorInfo);
    void info(QString info);
    void startListReceived(QStringList startList);
    void newProtokollitajaConnection(DataConnection *connection);
    void newShot(int target, SiusShotData);
    void statusUpdate(int target, QString ip, QString newStatus);
};

#endif // RECEIVERSERVER_H

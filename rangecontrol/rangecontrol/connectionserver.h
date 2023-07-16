#ifndef RECEIVERSERVER_H
#define RECEIVERSERVER_H

#include <QMessageBox>
#include <QTcpServer>
#include "dataconnection.h"
#include "inbandconnection.h"

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
    void start(int dataPort, int inbandPort);

private:
    QList<DataConnection*> dataSockets;
    QMessageBox messageBox;
    QTextStream *m_incomingLog = nullptr;
    QStringList m_shotsData;
    QTcpServer m_dataServer;
    QTcpServer m_inbandServer;
    void closeDataConnection(int socketIndex);
    void newDataConnection();
    void newInbandConnection();

private slots:
//    void incomingShotInfo(int socketIndex);
//    void readFromInband(QString msg);

signals:
    void allShots(int target, QString shotsData);
    void error(QString errorInfo);
    void info(QString info);
    void shotInfoRead(QString shotInfo, int socketIndex);
    void startListReceived(QStringList startList);
    void newProtokollitajaConnection(DataConnection *connection);
    void newShot(int target, SiusShotData);
    void newTarget(int target, QString ip);
    void statusUpdate(int target, QString newStatus);
};

#endif // RECEIVERSERVER_H

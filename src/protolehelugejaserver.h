#ifndef PROTOLEHELUGEJASERVER_H
#define PROTOLEHELUGEJASERVER_H

#include <QList>
#include <QTcpServer>
#include <QMessageBox>
#include <QSignalMapper>

#include "protolehelugejaconnection.h"

class ProtolehelugejaServer : public QTcpServer
{
    Q_OBJECT

public:
    ProtolehelugejaServer(QObject *parent = 0);
    ~ProtolehelugejaServer();
public slots:
    void closeConnections();
    void send(QString data, int socketIndex);

private:
    QList<ProtolehelugejaConnection*> sockets;
    QMessageBox messageBox;
    void closeConnection();
    void newConnection();

private slots:
    void incomingShotInfo(int socketIndex);
    void readTargetNumber(int);
    void saveRequested();

signals:
    void save();
    void shotInfoRead(QString shotInfo, int socketIndex);
    void renewWithTargetNumber(int targetNumber, int socketIndex);
};

#endif // PROTOLEHELUGEJASERVER_H

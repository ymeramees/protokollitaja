#ifndef DATACONNECTION_H
#define DATACONNECTION_H

#include <QDataStream>
#include <QTcpSocket>

class DataConnection : public QObject
{
    Q_OBJECT
public:
    explicit DataConnection(QTcpSocket *parent = 0);

signals:
    void disconnected(int myIndex);
    void error(QString errorMsg);
    void startListReceived(QStringList startList);

public slots:
    void abort();
    int socketIndex();
//    QString lastReceivedLine();
    void send(QString data);
    void setSocketIndex(int socketIndex);

private:
    int myIndex;
//    QMessageBox messageBox;
    QString m_buffer;
//    QString lastRecvdLine;
    QTcpSocket *socket;

private slots:
    void readData();
    void wasDisconnected();
};

#endif // DATACONNECTION_H

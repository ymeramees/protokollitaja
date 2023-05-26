#ifndef PROTOLEHELUGEJACONNECTION_H
#define PROTOLEHELUGEJACONNECTION_H

#include <QTime>
#include <QTcpSocket>
#include <QMessageBox>
#include <QRandomGenerator>

class ProtolehelugejaConnection : public QObject
{
    Q_OBJECT
public:
    explicit ProtolehelugejaConnection(QTcpSocket *parent = 0);

signals:
    void disconnected(int myIndex);
    void renewWithTargetNumber(int myIndex);
    void save();    //Saving is requested by peer
    void shotInfoRead(int myIndex);

public slots:
    void abort();
    int getAskedTargetNumber();
    int getPasswd();
    int getSocketIndex();
    QString lastReceivedLine();
    bool isAuthorized();
    void send(QString data);
    void setAuthorized(bool);
    void setPasswd(int newPasswd);
    void setSocketIndex(int socketIndex);

private:
    bool authorized;
    int askedTargetNumber;
    int myIndex;
    int passwd;
    quint16 blockSize;
    QMessageBox messageBox;
    QString lastRecvdLine;
    QTcpSocket *socket;

private slots:
    void readData();
    void wasDisconnected();
};

#endif // PROTOLEHELUGEJACONNECTION_H

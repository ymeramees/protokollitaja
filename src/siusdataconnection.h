#ifndef SIUSDATACONNECTION_H
#define SIUSDATACONNECTION_H

#include <QWidget>
#include <QFile>
#include <QLabel>
#include <QTcpSocket>
#include <QPushButton>
#include <QHBoxLayout>
#include <QProgressDialog>
#include <QCoreApplication>

class SiusDataConnection : public QWidget
{
    Q_OBJECT
public:
    explicit SiusDataConnection(QString address, int port, int socketIndex, QFile *siusLog, QTextStream *log, QWidget *parent = nullptr);

signals:
    void disconnectedFromSius(int socketIndex);
    void linesRead(QStringList siusInfoLines);
    void statusInfo(QString statusInfo);

public slots:
    void disconnectFromSius();
    void readFromSius();
    void reconnectToSius();
    void setSocketIndex(int newIndex);
    int socketIndex();
    void stopProgress();
    void wasDisconnected();

private:
    int myIndex;
    int port;
    QFile *siusLog;
    QHBoxLayout *row;
    QLabel *addressLabel;
    QProgressDialog *progress;
    QPushButton *reconnectButton;
    QPushButton *disconnectButton;
    QString siusBuffer;
    QTcpSocket *siusDataSocket;
    QTextStream *log;
};

#endif // SIUSDATACONNECTION_H

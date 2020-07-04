#ifndef SIUSDATACONNECTION_H
#define SIUSDATACONNECTION_H

#include <QWidget>
#include <QFile>
#include <QTime>
#include <QTimer>
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
    ~SiusDataConnection();

signals:
    void disconnectedFromSius(int socketIndex);
    void linesRead(QStringList siusInfoLines, int socketIndex);
    void statusInfo(QString statusInfo);

public slots:
    QString address() const;
    void disconnectFromSius();
    int port() const;
    void reconnectToSius();
    void setSocketIndex(int newIndex);
    int socketIndex() const;
    void stopProgress();

private:
    int m_index;
    int m_port;
    QFile *siusLog;
    QHBoxLayout *row;
    QLabel *addressLabel;
    QProgressDialog *progress;
    QPushButton *reconnectButton;
    QPushButton *disconnectButton;
    QString siusBuffer;
    QTcpSocket *siusDataSocket;
    QTextStream *log;
    QTimer *progressTimer;

private slots:
    void connected();
    void readFromSius();
    void wasDisconnected();
};

#endif // SIUSDATACONNECTION_H

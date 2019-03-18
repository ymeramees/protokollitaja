#ifndef SCORINGMACHINECONNECTION_H
#define SCORINGMACHINECONNECTION_H

#include <QObject>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class ScoringMachineConnection : public QObject
{
    Q_OBJECT
public:
    explicit ScoringMachineConnection(QObject *parent = 0);
    ~ScoringMachineConnection();

private:
    bool m_connected;   // Shows if scoring machine is in connected stage
    int m_scoringMachineType;
    int m_sendingStage;   // For RM-IV, showing in which stage sending currently is:
    // 0 - initial/ACK received (end), 1 -ENQ sent, 2 - STX received, 3 - text sent, 4 - text received, need to reply ACK

    QString m_portName;

    enum ScoringMachineType {RMIII, RMIV};

    QByteArray m_dataToSend;

    //QextSerialPort *m_serialPort = 0;
    QSerialPort m_serialPort;

    QTimer m_readTimer;   // Delay between reads from serial port
    QTimer m_sendTimer; // Delay before sending, so that RMIV has time to react

signals:
    void dataSent(QByteArray data);
    void shotRead(QString shotInfo);
    void connectionStatusChanged(QString status);

public slots:
    void closeConnection();
    int CRC(QByteArray const *s) const;
    void connectToMachine();
    void sendToMachine();
    void sendToMachine(QString);

private slots:
    void readFromMachine();
    void connectToRMIII();
    void connectTORMIV();
};

#endif // SCORINGMACHINECONNECTION_H

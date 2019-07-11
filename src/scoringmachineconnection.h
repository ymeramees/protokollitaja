#ifndef SCORINGMACHINECONNECTION_H
#define SCORINGMACHINECONNECTION_H

#include <QObject>
#include <QTimer>
#include <QLineF>
#include <QtMath>
#include <QTextStream>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "../../protokollitaja/src/lask.h"

class ScoringMachineConnection : public QObject
{
    Q_OBJECT
public:
    enum LogLevel {None, Verbose, VeryVerbose};
    enum ScoringMachineType {RMIII, RMIV};
    enum TargetType {AirRifle, AirPistol, SmallboreRifle};

    explicit ScoringMachineConnection(QObject *parent = nullptr);
    ~ScoringMachineConnection();
    bool calculateIsInnerTen(float x, float y);
    bool connected() const;
    int logLevel() const;
    int notOfShotsPerSeries() const;
    int noOfShotsPerTarget() const;
    QString portName() const;
    void setLogLevel(LogLevel logLevel);
    void setNotOfShotsPerSeries(int notOfShotsPerSeries);
    void setNoOfShotsPerTarget(int noOfShotsPerTarget);
    void setPortName(const QString &portName);
    void setScoringMachineType(int machineType);
    void setTargetType(int targetType);
    int targetType() const;

private:
    bool m_connected = false;   // Shows if scoring machine is in connected stage
    int m_attemptCount = 0;    // Number of attempts to read from RMIII
    const int MAX_ATTEMPTS = 3; // Number of attempts to read form RMIII, before changing to RMIV
    bool m_machineChoiceInProgress = false;
    int m_logLevel = None; // Amount of debug/logging data to be sent out
    int m_notOfShotsPerSeries = 10;
    int m_noOfShotsPerTarget = 1;
    int m_scoringMachineType = -1;
    int m_sendingStage = 2;   // For RM-III always 2, for RM-IV, showing in which stage sending currently is:
    // 0 - initial/ACK received (end), 1 -ENQ sent, 2 - STX received, 3 - text sent, 4 - text received, need to reply ACK
    int shotNo = 0; // Current shot number in series
    int m_targetType = -1;

    QString m_portName;

    QByteArray m_dataToSend;

    QSerialPort m_serialPort;

    QTimer m_readTimer;   // Delay between reads from serial port
    QTimer m_sendTimer; // Delay before sending, so that RMIV has time to react
    QTimer m_settingsTimer; // Delay before sending settings, so that machines have time to react

    friend class ScoringMachineConnectionTest;

signals:
    void dataSent(QString data);
    void shotRead(Lask shot);
    void connectionStatusChanged(QString status);

public slots:
    void closeConnection();
    int CRC(QByteArray const *s) const;
    void connectToMachine();
    Lask extractRMIIIShot(QString shotInfo);
    Lask extractRMIVShot(QString);
    void extraShotInTarget();
    void sendSettings();
    void sendToMachine();
    void sendToMachine(QString);

private slots:
    void connectToRMIII();
    void connectToRMIV();
    void readFromMachine();
    void readFromRMIII();
    void readFromRMIV();

protected:
    QString m_serialBuffer;
};

#endif // SCORINGMACHINECONNECTION_H

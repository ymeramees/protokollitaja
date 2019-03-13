#ifndef SCORINGMACHINECONNECTION_H
#define SCORINGMACHINECONNECTION_H

#include <QObject>

class ScoringMachineConnection : public QObject
{
    Q_OBJECT
public:
    explicit ScoringMachineConnection(QObject *parent = 0);
    ~ScoringMachineConnection();

private:
    bool m_connected;   // Shows if scoring machine is in connected stage
    int m_sendingStage;   // For RM-IV, showing in which stage sending currently is:
    // 0 - initial/ACK received (end), 1 -ENQ sent, 2 - STX received, 3 - text sent, 4 - text received, need to reply ACK

    enum ScoringMachineType {RMIII, RMIV};

signals:
    //void shotRead(QString shotInfo);

public slots:
//    void closeConnection();
//    void connectToMachine();
//    void sendSettingString(QString);
    int testFunction(QString input);

private slots:
//    void readFromMachine();
};

#endif // SCORINGMACHINECONNECTION_H

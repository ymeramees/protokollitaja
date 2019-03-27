#include "scoringmachineconnection.h"

/////////////////////////////////////////////////////////////////////////////
/// ToDo list:
/// Choice of scoring machine type process
/// Review and check extractRMIII function result for air pistol
/////////////////////////////////////////////////////////////////////////////

// Defines for scoring machine connection:
#define EOT 0x04
#define ENQ 0x05 	// Anfrage
#define ACK 0x06
#define STX 0x02	// Start Text
#define NAK 0x15
#define CR 0x0d

const char m_enq = ENQ;
const char m_ack = ACK;

ScoringMachineConnection::ScoringMachineConnection(QObject *parent) : QObject(parent)
{
    m_readTimer.setInterval(100);
    m_readTimer.setSingleShot(false);

    m_sendTimer.setInterval(20);
    m_sendTimer.setSingleShot(true);

    m_serialPort.close();   // Make sure things are clear
}

void ScoringMachineConnection::closeConnection()
{
    m_dataToSend = QString("EXIT").toLatin1();
    m_sendingStage = 2;
    sendToMachine();
    m_serialPort.close();
    m_readTimer.stop();
    m_connected = false;
}

void ScoringMachineConnection::connectToMachine()
{
    switch(m_scoringMachineType) {
    case RMIII:
        connectToRMIII();
        break;
    case RMIV:
        connectToRMIV();
        break;
    }
}

void ScoringMachineConnection::connectToRMIII()
{
    if(m_connected){
        sendToMachine("V");
        return;
    }

    emit connectionStatusChanged(tr("Ühendamine: RMIII, ") + m_portName);
    m_serialPort.setPortName(m_portName);
    m_serialPort.setBaudRate(QSerialPort::Baud2400);
    m_serialPort.setDataBits(QSerialPort::Data8);
    m_serialPort.setParity(QSerialPort::NoParity);
    m_serialPort.setStopBits(QSerialPort::OneStop);
    m_serialPort.setFlowControl(QSerialPort::NoFlowControl);

    m_serialPort.open(QIODevice::ReadWrite);

    m_serialPort.setDataTerminalReady(true);
    m_serialPort.setRequestToSend(true);

    sendToMachine("V");
    m_serialPort.flush();
    m_readTimer.start();

}

void ScoringMachineConnection::connectToRMIV()
{
    m_serialPort.flush();
    m_serialPort.close();

    emit connectionStatusChanged(tr("Ühendamine: RMIV, ") + m_portName);
    m_serialPort.setPortName(m_portName);
    m_serialPort.setBaudRate(QSerialPort::Baud9600);
    m_serialPort.setDataBits(QSerialPort::Data8);
    m_serialPort.setParity(QSerialPort::NoParity);
    m_serialPort.setStopBits(QSerialPort::OneStop);
    m_serialPort.setFlowControl(QSerialPort::NoFlowControl);

    m_serialPort.open(QIODevice::ReadWrite);

    m_sendingStage = 0;
    sendToMachine("SNR=");
    m_readTimer.start();
    m_connected = true;
}

int ScoringMachineConnection::CRC(const QByteArray *s) const
{
    int crc = 0;
    for(int i = 0; i < s->size(); i++)
        crc ^=s->at(i);
    return crc;
}

Lask ScoringMachineConnection::extractRMIIIShot(QString shotInfo)
{
    Lask shot;
    QStringList list = shotInfo.split(';', QString::KeepEmptyParts);
    if(list.size() > 5) {
        int x = 0, y = 0;
        float fx = 0, fy = 0;

        switch(m_targetType) {
        case AirRifle:
            fx = list.at(3).toFloat() * 100;
            x = qRound(fx);
            fx = x * 25;
            fx /= 10;
            fx = qRound(fx);

            fy = list.at(4).toFloat() * 100;
            y = qRound(fy);
            fy = y *25;
            fy /= 10;
            fy = qRound(fy);
            break;
        case AirPistol:
            //Calculation is same as for smallbore rifle
        case SmallboreRifle:
            fx = list.at(3).toFloat() * 100;
            x = qRound(fx);
            fx = x * 160;
            fx /= 10;
            fx = qRound(fx);

            fy = list.at(4).toFloat() * 100;
            y = qRound(fy);
            fy = y *160;
            fy /= 10;
            fy = qRound(fy);
            break;
        }

        fx /= 100;
        fy /= 100;

        shot.setLask(list.at(1));
        shot.setX(fx);
        shot.setY(fy);
    }
    return shot;
}

Lask ScoringMachineConnection::extractRMIVShot(QString)
{

}

void ScoringMachineConnection::readFromMachine()
{
    switch(m_scoringMachineType) {
    case RMIII:
        readFromRMIII();
        break;
    case RMIV:
        readFromRMIV();
        break;
    }
}

void ScoringMachineConnection::readFromRMIII()
{
    static bool firstTime = true;   // This is needed to send setting string to RMIII twice, as it might not react to first attempt.

    if(m_serialPort.bytesAvailable() > 0) {
        static QString buffer;
        QString currentText;
        buffer.append(m_serialPort.readAll());
        if(buffer.contains(CR)) {
            currentText = buffer.left(buffer.indexOf(CR) + 1);
            buffer.remove(0, buffer.indexOf(CR) + 1);
        }else
            return;

        m_connected = true; // If text was received, then connection is established
        m_machineChoiceInProgress = false;

        if(!currentText.contains("START") && !currentText.contains("SCHEIBE") && !currentText.contains("Keine")
                && currentText.contains(';')) {

        }
    }
}

void ScoringMachineConnection::readFromRMIV()
{

}

void ScoringMachineConnection::sendToMachine()
{
    // 0 - initial/ACK received (end), 1 -ENQ sent, 2 - STX received, 3 - text sent, 4 - text received, need to reply ACK
    switch (m_sendingStage) {
    case 0:
        m_serialPort.write(&m_enq);
        m_serialPort.flush();
        m_sendingStage = 1; // ENQ sent
        break;
    case 2:
        if(m_scoringMachineType == RMIV)
            m_dataToSend.append(CRC(&m_dataToSend));
        m_dataToSend.append(CR);
        m_serialPort.write(m_dataToSend);
        emit dataSent(m_dataToSend);
        m_serialPort.flush();
        if(m_scoringMachineType == RMIV)
            m_sendingStage = 3; // Text sent
        break;
    case 4:
        m_serialPort.write(&m_ack);
        emit dataSent(&m_ack);
        m_serialPort.flush();
        m_sendingStage = 0; // ACK sent
        break;
    default:
        break;
    }
}

void ScoringMachineConnection::sendToMachine(QString data)
{
    if(!data.isEmpty()) // If this is empty, then probably it is second stage of sending
        m_dataToSend = data.toLatin1();
    m_sendTimer.start();
}

void ScoringMachineConnection::setScoringMachineType(int machineType)
{
    m_scoringMachineType = machineType;
}

void ScoringMachineConnection::setTargetType(int targetType)
{
    m_targetType = targetType;
}

ScoringMachineConnection::~ScoringMachineConnection()
{

}

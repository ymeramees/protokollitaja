#include "scoringmachineconnection.h"

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

    //m_serialPort = new QextSerialPort();
    //m_serialPort = new QSerialPort();
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
        connectTORMIV();
        break;
    }
}

void ScoringMachineConnection::connectToRMIII()
{
    if(m_connected){
        sendToMachine("V");
        return;
    }

    emit connectionStatusChanged(tr("Ühendamine: ") + m_portName);
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

void ScoringMachineConnection::connectTORMIV()
{
    m_serialPort.flush();
    m_serialPort.close();
}

int ScoringMachineConnection::CRC(const QByteArray *s) const
{
    int crc = 0;
    for(int i = 0; i < s->size(); i++)
        crc ^=s->at(i);
    return crc;
}

void ScoringMachineConnection::readFromMachine()
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

ScoringMachineConnection::~ScoringMachineConnection()
{

}

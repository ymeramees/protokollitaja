#include "scoringmachineconnection.h"

/////////////////////////////////////////////////////////////////////////////
/// ToDo list:
/// Connecting to machines
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
    connect(&m_readTimer, &QTimer::timeout, this, &ScoringMachineConnection::readFromMachine);

    m_sendTimer.setInterval(20);
    m_sendTimer.setSingleShot(true);
    connect(&m_sendTimer, &QTimer::timeout, this, qOverload<>(&ScoringMachineConnection::sendToMachine));

    m_settingsTimer.setInterval(1000);
    m_settingsTimer.setSingleShot(true);
    connect(&m_settingsTimer, &QTimer::timeout, this, &ScoringMachineConnection::sendSettings);

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

Lask ScoringMachineConnection::extractRMIVShot(QString shotInfo)
{
    Lask shot;
    QStringList list = shotInfo.split(';');    //SCH=1;7.2;2973.0;164.7;G#CR
    if(list.count() >= 4){
        bool onnestus = false;
        QLineF coordinates = QLineF::fromPolar(list.at(2).left(list.at(2).indexOf('.')).toInt(), list.at(3).toDouble(&onnestus) - 90);   // 0 in QLineF is at 3 o'clock, but in Disag it is at 12 o'clock
        if(!onnestus)   // If with point cannot be converted to double, needs to be replaced with dot
            coordinates = QLineF::fromPolar(list.at(2).toInt(), list.value(3).replace(".", ",").toDouble(&onnestus) - 90);

        if(onnestus){
            shot.setLask(list.at(1));
            qreal fx = 0, fy = 0;
            fx = coordinates.p2().x();
            fy = coordinates.p2().y();
            fx = qRound(fx);
            fy = qRound(fy);
            shot.setX(float(fx / 100));
            shot.setY(float(fy / 100));
        }
    }
    return shot;
}

int ScoringMachineConnection::noOfShotsPerTarget() const
{
    return m_noOfShotsPerTarget;
}

QString ScoringMachineConnection::portName() const
{
    return m_portName;
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
    //static bool firstTime = true;   // This is needed to send settings to RMIII twice, as it might not react to first attempt.

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
            Lask shot = extractRMIIIShot(currentText);
            if(!shot.isEmpty())
                emit shotRead(shot);
        }
    }
}

void ScoringMachineConnection::readFromRMIV()
{
    if(m_serialPort.bytesAvailable() > 0) {
        static QString buffer;
        QString currentText;
        buffer.append(m_serialPort.readAll());
        if(buffer.contains(CR)) {
            currentText = buffer.left(buffer.indexOf(CR) + 1);
            currentText.replace(STX, "");
            if(currentText.contains("SCH=")){
                Lask shot = extractRMIVShot(currentText);
                if(!shot.isEmpty())
                    emit shotRead(shot);
                m_sendingStage = 4;  // To reply with confirmation
                sendToMachine("");  // Sends ACK
//                m_settingsTimer.setInterval(600);   // Here delay before setting can be shorter
                m_settingsTimer.start();    // Machine needs to be set, in ordet it to take next target in
            }
            buffer.remove(0, buffer.indexOf(CR) + 1);
        }else if(buffer.contains(STX)){
            currentText = buffer.left(buffer.indexOf(STX) + 1);
            currentText.replace(STX, "STX");
            if(m_sendingStage == 1){
//#ifdef PROOV
//        qDebug() << "saabus: STX\n";
//#endif
                m_sendingStage = 2;  // STX received
//#ifdef PROOV
//        qDebug() << "saatmiseEtapp = 2";
//#endif
                sendToMachine("");  // If STX was received, then probably the machine is ready to receive text
            }
            buffer.remove(0, buffer.indexOf(STX) + 1);
        }else if(buffer.contains(NAK)){
            currentText = buffer.left(buffer.indexOf(NAK) + 1);
            currentText.replace(NAK, "NAK");
            if(m_sendingStage == 3){
//#ifdef PROOV
//        qDebug() << "saabus: NAK\n";
//#endif
                m_sendingStage = 0;  // NAK received, so something is wrong -> initial stage
//#ifdef PROOV
//        qDebug() << "saatmiseEtapp = 0";
//#endif
            }
            buffer.remove(0, buffer.indexOf(NAK) + 1);
        }else if(buffer.contains(ACK)){
            currentText = buffer.left(buffer.indexOf(ACK) + 1);
            currentText.replace(ACK, "ACK");
            if(m_sendingStage == 3){
//#ifdef PROOV
//        qDebug() << "saabus: ACK";
//#endif
                m_sendingStage = 0;  // ACK received -> initial stage
//#ifdef PROOV
//        qDebug() << "saatmiseEtapp = 0";
//#endif
            }
            buffer.remove(0, buffer.indexOf(ACK) + 1);
        }else
            return;
    }
}

void ScoringMachineConnection::sendSettings()
{
    m_settingsTimer.setInterval(1000);  // To restore initial value

    QString settingString;
    switch(m_scoringMachineType) {
    case RMIII:
        settingString = "111111111";
        switch(m_targetType){   // Set target type
        case AirRifle:
            settingString.replace(0, 1, "1");
            break;
        case AirPistol:
            settingString.replace(0, 1, "2");
            break;
        case SmallboreRifle:
            settingString.replace(0, 1, "6");
            break;
        }
        settingString.replace(8, 1, QString("%1").arg(m_noOfShotsPerTarget));
        settingString.replace(3, 1, "2");
        sendToMachine(settingString);
        break;
    case RMIV:
        m_sendingStage = 0;
        // Examples: "SCH=KK50;TEA=KT;RIA=ZR;SSC=2;SZI=10;SGE=10;"; //"SCH=LP;TEA=KT;TEG=1500;SSC=2;SGE=10;";//"SCH=LGES;TEA=ZT;SSC=1;SZI=10;SGE=10;KSD;";
        QString settingString;
        switch(m_targetType){
        case AirRifle: settingString = "SCH=LGES;";
            break;
        case AirPistol: settingString = "SCH=LP;";
            break;
        case SmallboreRifle: settingString = "SCH=KK50;";
            break;
        default : settingString = "SCH=LGES;";
        }
        settingString.append("TEA=KT;RIA=ZR;"); //Teiler with full rings and shot value with tenths
        settingString.append(QString("SSC=%1;").arg(m_noOfShotsPerTarget));  //Laskude arv lehes
        settingString.append(QString("SGE=%1;SZI=%1;").arg(10));    // No of shots in series, currently always 10
//    #ifdef PROOV
//    //    s.append("KSD;");
//        qDebug() << "Seadistamine: " << s;
//    #endif
    //    if(!ui->trukkimiseBox->isChecked())   //Kas trükkida lehele lasud ja tulemused või ei
    //        s.append("KSD;");
        sendToMachine(settingString);
        break;
    }
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
            m_dataToSend.append(char(CRC(&m_dataToSend)));
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

void ScoringMachineConnection::setNoOfShotsPerTarget(int noOfShotsPerTarget)
{
    m_noOfShotsPerTarget = noOfShotsPerTarget;
}

void ScoringMachineConnection::setPortName(const QString &portName)
{
    m_portName = portName;
}

void ScoringMachineConnection::setScoringMachineType(int machineType)
{
    m_scoringMachineType = machineType;
}

void ScoringMachineConnection::setTargetType(int targetType)
{
    m_targetType = targetType;
}

int ScoringMachineConnection::targetType() const
{
    return m_targetType;
}

ScoringMachineConnection::~ScoringMachineConnection()
{

}

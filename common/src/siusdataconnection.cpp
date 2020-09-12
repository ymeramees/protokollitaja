#include "siusdataconnection.h"

#define CR 0x0d
#define LF 0x0a

extern bool verbose;

SiusDataConnection::SiusDataConnection(
        QString address,
        int port,
        int socketIndex,
        QFile *siusLog,
        QTextStream *log,
        CommonSettings *settings,
        QWidget *parent
        ) : QWidget(parent)
{
    if(verbose)
        QTextStream(stdout) << "SiusDataConnection()" << endl;
    addressLabel = new QLabel(address);
    m_index = socketIndex;
    m_port = port;
    this->siusLog = siusLog;
    this->log = log;
    m_settings = settings;

    if(verbose)
        QTextStream(stdout) << "connectToSiusData()2" << endl;
    disconnectButton = new QPushButton(tr("Sulge ühendus"));
    connect(disconnectButton, &QPushButton::clicked, this, &SiusDataConnection::disconnectFromSius);

    reconnectButton = new QPushButton(tr("Ühendu uuesti"));
    connect(reconnectButton, &QPushButton::clicked, this, &SiusDataConnection::reconnectToSius);

    row = new QHBoxLayout;
    row->addWidget(addressLabel);
    row->addWidget(reconnectButton);
    row->addWidget(disconnectButton);
    setLayout(row);

    siusDataSocket = new QTcpSocket(this);
    connect(siusDataSocket, &QTcpSocket::connected, this, &SiusDataConnection::connected);
    connect(siusDataSocket, &QTcpSocket::readyRead, this, &SiusDataConnection::readFromSius);
    connect(siusDataSocket, &QTcpSocket::disconnected, this, &SiusDataConnection::wasDisconnected);

    siusDataSocket->connectToHost(address, port);

    progress = new QProgressDialog(tr("SiusData'ga ühendumine..."), "Loobu", 0, 0);
    progress->setWindowModality(Qt::WindowModal);
    connect(progress, SIGNAL(canceled()), this, SLOT(stopProgress())); //To enable to cancel connection process
    progress->show();

    progressTimer = new QTimer(this);
    progressTimer->setSingleShot(true);
    progressTimer->setInterval(3000);
    connect(progressTimer, &QTimer::timeout, this, &SiusDataConnection::stopProgress);
    progressTimer->start();
}

SiusDataConnection::~SiusDataConnection()
{
    if(verbose)
        QTextStream(stdout) << "SiusDataConnection::~SiusDataConnection(): " << m_index << endl;
    addressLabel->deleteLater();
    addressLabel = nullptr;
    disconnectButton->deleteLater();
    disconnectButton = nullptr;
    reconnectButton->deleteLater();
    reconnectButton = nullptr;
    row->deleteLater();
    siusDataSocket->abort();
    siusDataSocket->deleteLater();
}

QString SiusDataConnection::address() const
{
    return addressLabel->text();
}

void SiusDataConnection::connected()
{
    if(verbose)
        QTextStream(stdout) << "SiusDataConnection::connected(): " << m_index << endl;
    addressLabel->setEnabled(true);
    disconnectButton->setEnabled(true);
}

void SiusDataConnection::disconnectFromSius()
{
    siusDataSocket->disconnectFromHost();
    emit disconnectedFromSius(m_index);
}

std::optional<SiusShotData> SiusDataConnection::extractShotData(
        QString totalRow,
        QString shotRow,
        int socketIndex,
        QTextStream *log,
        CommonSettings *settings
        )
{
    QStringList totalRowParts = totalRow.split(';');
    QStringList previousRowParts = shotRow.split(';');
    if (totalRowParts.length() < 13){
        return std::nullopt;  // Probably the event's last total row, which can be ignored
    } else if (previousRowParts.length() < 16){
        *log << QTime::currentTime().toString("hh:mm:ss") << " #ERROR: Too few fields in shot row: " << shotRow;
        return std::nullopt;
    }

    bool ok;
    SiusShotData shotData;

    int previousId = previousRowParts[3].toInt(&ok);
    if (!ok){
        *log << QTime::currentTime().toString("hh:mm:ss") << " #ERROR: Unable to extract id from shot row: " << shotRow;
        return std::nullopt;
    }

    int currentId = totalRowParts[3].toInt(&ok);
    if (ok)
        shotData.id = currentId;
    else {
        *log << QTime::currentTime().toString("hh:mm:ss") << " #ERROR: Unable to extract id from row: " << totalRow;
        return std::nullopt;
    }

    if (currentId != previousId){
        *log << QTime::currentTime().toString("hh:mm:ss") << " #ERROR: Competitor Id's do not match: " << currentId << " != " << previousId;
        return std::nullopt;
    }

    int shotType = previousRowParts[9].toInt(&ok);
    if (!ok){
        *log << QTime::currentTime().toString("hh:mm:ss") << " #ERROR: Unable to extract shot type (competition/sighters) from row: " << shotRow;
        return std::nullopt;
    }

    int shotNo = previousRowParts[13].toInt(&ok);
    if (ok)
        shotData.siusShotNo = shotNo;
    else {
        *log << QTime::currentTime().toString("hh:mm:ss") << " #ERROR: Unable to extract shot number from row: " << shotRow;
        return std::nullopt;
    }

    Lask shot(shotRow);

    if (settings->competitionShotTypes().contains(shotType))
        shot.setCompetitionShot(true);
    else if (settings->sighterShotTypes().contains(shotType))
        shot.setCompetitionShot(false);
    else {
        *log << QTime::currentTime().toString("hh:mm:ss") << " #ERROR: Unknown shot type in shot row: " << shotRow;
        if( totalRowParts[6] == "0")
            shot.setCompetitionShot(false);
        else
            shot.setCompetitionShot(true);
        // TODO Send unknown new shot type to api server
    }

    shotData.shot = shot;
    shotData.socketIndex = socketIndex;

    return  std::optional<SiusShotData>{shotData};
}

int SiusDataConnection::port() const
{
    return m_port;
}

void SiusDataConnection::readFromSius()
{
    if(siusDataSocket->bytesAvailable() > 0 || siusBuffer.length() > 0){
//        QStringList lines;
        progress->setLabelText(tr("SiusDatast andmete vastuvõtt..."));

        while(siusDataSocket->bytesAvailable() > 0){
          siusBuffer.append(siusDataSocket->readAll());  //Start of line 5f, end 0d 0a
          emit statusInfo(tr("Saabus info, buffer.length(): %1").arg(siusBuffer.length()));

        }

        while(siusBuffer.contains('_')){

            progressTimer->start();   //To postpone closing of the progress dialog

            if(siusLog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){ //Log of incoming data
                QTextStream out(siusLog);
                out << siusBuffer.left(siusBuffer.indexOf('_', 1));
                siusLog->close();
            }

            *log << QTime::currentTime().toString("hh:mm:ss") << " # buffer.length(): " << siusBuffer.length() << "    buffer.indexOf('_'): " << siusBuffer.indexOf('_', 1) << "\n";

            QString row = "";
            if(siusBuffer.indexOf('_', 1) == -1){
                if(siusBuffer.contains(CR) || siusBuffer.contains(LF)){ // Make sure complete last row has arrived
                    *log << QTime::currentTime().toString("hh:mm:ss") << " #clear()\n";
                    row = QString("%1").arg(siusBuffer);
                    siusBuffer.clear();
                    emit statusInfo(tr("Viimane rida, buffer.length(): %1").arg(siusBuffer.length()));
                } else
                    break;
            }else{
                row = siusBuffer.left(siusBuffer.indexOf('_', 1));
                siusBuffer.remove(0, siusBuffer.indexOf('_', 1));
            }

            if(verbose)
                QTextStream(stdout) << "SiusDataConnection::readFromSius() " << m_index << ": " << row << endl;

            *log << QTime::currentTime().toString("hh:mm:ss") << " #rida: " << row;

            static QString previousRow;

            if(row.startsWith("_TOTL") && !previousRow.isEmpty()){
//                *log << QTime::currentTime().toString("hh:mm:ss") << " #break\n";
//                break; //Double check not to continue with pointless row
//            }
//                *log << QTime::currentTime().toString("hh:mm:ss") << " #rida: " << row;
//            lines.append(row);
                std::optional<SiusShotData> shotData = extractShotData(row, previousRow, m_index, log, m_settings);
                if (shotData.has_value())
                    emit shotRead(shotData.value());

                previousRow.clear();

            } else if (row.startsWith("_SHOT")){
                previousRow = row;
            }

            QCoreApplication::processEvents();
        }
        if(siusBuffer.length() > 0){   //If there is still data in the buffer, but for some reason reached here, then start the function again after a while
            *log << QTime::currentTime().toString("hh:mm:ss") << " #buffer.length(): " << siusBuffer.length() << ", uuele ringile minek" << "\n";
            QTimer::singleShot(170, this, SLOT(readFromSius()));
        }

//        if(lines.size() > 0)
//            emit linesRead(lines, m_index);
    }
}

void SiusDataConnection::reconnectToSius()
{
    siusDataSocket->abort();
    progress->show();
    progressTimer->start();
    siusDataSocket->connectToHost(addressLabel->text(), m_port);
}

void SiusDataConnection::setSocketIndex(int newIndex)
{
    m_index = newIndex;
}

int SiusDataConnection::socketIndex() const
{
    return m_index;
}

void SiusDataConnection::stopProgress()
{
    if(progress != nullptr){
        if(progress->wasCanceled()){
            siusDataSocket->abort();
        }else progress->accept();
    }
}

void SiusDataConnection::wasDisconnected()
{
    if(verbose)
        QTextStream(stdout) << "SiusDataConnection::wasDisconnected(): " << m_index << endl;
    if(addressLabel != nullptr && disconnectButton != nullptr)
        if(addressLabel->isEnabled()){
            addressLabel->setEnabled(false);
            disconnectButton->setEnabled(false);
            emit disconnectedFromSius(m_index);
        }
}

#include "siusdataconnection.h"

extern bool verbose;

SiusDataConnection::SiusDataConnection(QString address, int port, int socketIndex, QFile *siusLog, QTextStream *log, QWidget *parent) : QWidget(parent)
{
    if(verbose)
        QTextStream(stdout) << "SiusDataConnection()" << endl;
    addressLabel = new QLabel(address);
    m_index = socketIndex;
    m_port = port;
    this->siusLog = siusLog;
    this->log = log;

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

int SiusDataConnection::port() const
{
    return m_port;
}

void SiusDataConnection::readFromSius()
{
    if(siusDataSocket->bytesAvailable() > 5 || siusBuffer.length() > 0){
        QStringList lines;
        progress->setLabelText(tr("SiusDatast andmete vastuvõtt..."));

        while(siusDataSocket->bytesAvailable() > 2){
          siusBuffer.append(siusDataSocket->readAll());  //Start of line 5f, end 0d 0a
          emit statusInfo(tr("Saabus info, buffer.length(): %1").arg(siusBuffer.length()));

        }
//    QFile file("siusData.txt");
//    if (file.open(QFile::ReadOnly)){
//        siusDatast = file.readAll();
//        QString rida;
//    }

//        logi->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append); //Saabunud muudatuste ja laskude logifail
//        QTextStream logiValja(logi);

        while(siusBuffer.contains('_')){

            progressTimer->start();   //To postpone closing of the progress dialog

            if(siusLog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){ //Log of incoming data
                QTextStream out(siusLog);
                out << siusBuffer.left(siusBuffer.indexOf('_', 1));
                siusLog->close();
            }

            *log << QTime::currentTime().toString("#hh:mm:ss") << " #buffer.length(): " << siusBuffer.length() << "    buffer.indexOf('_'): " << siusBuffer.indexOf('_', 1) << "\n";

            QString row = "";
            if(siusBuffer.indexOf('_', 1) == -1){
                *log << "#clear()\n";
                row = QString("%1").arg(siusBuffer);
                siusBuffer.clear();
            }else{
                row = siusBuffer.left(siusBuffer.indexOf('_', 1));
                siusBuffer.remove(0, siusBuffer.indexOf('_', 1));
            }

            if(verbose)
                QTextStream(stdout) << "SiusDataConnection::readFromSius() " << m_index << ": " << row << endl;

            if(!row.contains('_')){
                *log << "#break\n";
                break; //Double check not to continue with pointless row
            }
            *log << "#rida: " << row;
            lines.append(row);

            QCoreApplication::processEvents();
        }
        if(siusBuffer.length() > 0){   //If there is still data in the buffer, but for some reason reached here, then start the function again after a while
            *log << "#buffer.length(): " << siusBuffer.length() << ", uuele ringile minek" << "\n";
            QTimer::singleShot(170, this, SLOT(loeSiusDatast()));
        }

        emit linesRead(lines, m_index);
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

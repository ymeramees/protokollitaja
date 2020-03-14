#include "protolehelugejaconnection.h"

extern bool verbose;

ProtolehelugejaConnection::ProtolehelugejaConnection(QTcpSocket *parent) : QObject(parent)
{
    askedTargetNumber = 0;
    authorized = false;
    blockSize = 0;
    passwd = 0;
    socket = parent;
    connect(socket, &QTcpSocket::readyRead, this, &ProtolehelugejaConnection::readData);
    connect(socket, &QTcpSocket::disconnected, this, &ProtolehelugejaConnection::wasDisconnected);

    qsrand(QTime::currentTime().msec());
    while(passwd < 1000)
        passwd = qrand() % 9999;    //Password is needed to make sure it is not too easy to connect and start inserting results
    messageBox.setText(tr("Serverisse on loodud uus ühendus.\n\nParool: %1").arg(passwd));
    messageBox.show();
}

void ProtolehelugejaConnection::abort()
{
    socket->abort();
}

void ProtolehelugejaConnection::wasDisconnected()
{
    emit disconnected(myIndex);
}

int ProtolehelugejaConnection::getAskedTargetNumber()
{
    return askedTargetNumber;
}

int ProtolehelugejaConnection::getPasswd()
{
    return passwd;
}

int ProtolehelugejaConnection::getSocketIndex()
{
    return myIndex;
}

bool ProtolehelugejaConnection::isAuthorized()
{
    return authorized;
}

QString ProtolehelugejaConnection::lastReceivedLine()
{
    return lastRecvdLine;
}

void ProtolehelugejaConnection::readData()
{
    if(verbose)
        QTextStream(stdout) << "ProtolehelugejaConnection::readData()" << endl;
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_8);

    if (blockSize == 0){
        if (socket->bytesAvailable() < (int)sizeof(quint16)){
            return; //Packet size info (quint16) not yet received
        }
        in >> blockSize;
    }

    if (socket->bytesAvailable() < blockSize){
        return; //Whole packet not yet received
    }

    QString lineIn;
    int receivedPasswd = 0;
    if(authorized){  //If the connection is authorized, some information is being sent
        lineIn.clear();
        in >> lineIn;
        blockSize = 0;
        messageBox.hide();
        if(verbose)
            QTextStream(stdout) << "ProtolehelugejaConnection::readData() lineIn: " << lineIn << endl;
    }else{  //If the connection is not authorized, password must be received and checked
        in >> receivedPasswd;
        if(verbose)
            QTextStream(stdout) << "ProtolehelugejaConnection::readData() receivedPasswd: " << receivedPasswd << endl;
        messageBox.hide();

        blockSize = 0;
        if(passwd != 0 && receivedPasswd == passwd){
            authorized = true;
            passwd = 0;
            send("OK");
        }
        else{ QMessageBox::information(dynamic_cast<QWidget*>(this->parent()), "Teade", tr("Keegi proovis ühenduda vale parooliga! Ühendust ei loodud!"), QMessageBox::Ok);
            socket->disconnect();
        }
        return;
    }

    if(lineIn == "Tere"){    //Confirmation of establishing the connection
        messageBox.setText(tr("Ühendus loodud"));
        messageBox.show();
        return;
    }else if(lineIn.startsWith("Versioon:")){    //Version of the connection protocol between Protokollitaja and Protolehelugeja
        lineIn.remove(0, 9);
        if(lineIn.toInt() > 4){
            send(tr("Viga:Protokollitaja ja Protolehelugeja versioonid ei ühti!\nProtokollitaja on uuem, seega on vaja uuendada Protolehelugejat või mõlemaid."));
            authorized = false;
            socket->disconnect();
        }
        else if(lineIn.toInt() < 4){
            send(tr("Viga:Protokollitaja ja Protolehelugeja versioonid ei ühti!\nProtolehelugeja on uuem, seega on vaja uuendada Protokollitajat"));
            authorized = false;
            socket->disconnect();
        }else send("Versioon OK");

        return;
    }else if(lineIn.startsWith("Siffer:")){  //Competitor with these target numbers requested
        lineIn.remove(0, 7);
        askedTargetNumber = lineIn.toInt();
        emit renewWithTargetNumber(myIndex);
    }else if(lineIn.startsWith("Laskur:")){  //Results received
        //OLD: "Laskur:siffer - siffer;Eesnimi;Perekonnanimi;seeriate arv;laskude arv;seeriad;selle seeria lasud; x; y; summa;aktiivne seeria;harjutus;lasku lehes;kümnendikega lugemine (true/false)
        //NEW: "Laskur:siffer - siffer;Eesnimi;Perekonnanimi;seeriate arv;laskude arv;loetud seeria nr (0-5); loetud seeria; loetud seeria lasud; x; y
        lastRecvdLine = lineIn;
        emit shotInfoRead(myIndex);

    }else if(lineIn.startsWith(tr("Käsk:Salvestada"))){  //Saving is requested
        emit save();
    }
}

void ProtolehelugejaConnection::send(QString data)
{
    if(verbose)
        QTextStream(stdout) << "ProtolehelugejaConnection::send(): " << data << endl;
    blockSize = 0;
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << (quint16)0;
    out << data;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    socket->write(block);
    block.clear();
}

void ProtolehelugejaConnection::setAuthorized(bool authorized)
{
    this->authorized = authorized;
}

void ProtolehelugejaConnection::setPasswd(int newPasswd)
{
    passwd = newPasswd;
}

void ProtolehelugejaConnection::setSocketIndex(int socketIndex)
{
    myIndex = socketIndex;
}

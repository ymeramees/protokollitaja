#include "protolehelugejaconnection.h"

extern bool verbose;

ProtolehelugejaConnection::ProtolehelugejaConnection(QObject *parent) : QObject(parent)
{
    askedTargetNumber = 0;
    blockSize = 0;
    passwd = 0;
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &ProtolehelugejaConnection::readData);

    qsrand(QTime::currentTime().msec());
    while(passwd < 1000)
        passwd = qrand() % 9999;    //Parool on vajalik, et ei saaks keegi suvaline ühenduda serveriga ja tulemusi sisestama hakata
//    socket->setPasswd(passwd);
    messageBox.setText(tr("Serverisse on loodud uus ühendus.\n\nParool: %1").arg(passwd));
    messageBox.show();
}

void ProtolehelugejaConnection::abort()
{
    socket->abort();
}

void ProtolehelugejaConnection::wasDisconnected()
{
    emit disconnected();
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
            return; //Ei ole paketi suurust tähistav quint16 veel kohale jõudnud
        }
        in >> blockSize;
    }

    if (socket->bytesAvailable() < blockSize){
        return; //Ei ole kogu pakett veel kohale jõudnud
    }

    QString lineIn;
    int receivedPasswd = 0;
    if(authorized){  //Kui on autoriseeritud ühendus, saadetakse ilmselt muud infot
        lineIn.clear();
        in >> lineIn;
        blockSize = 0;
        messageBox.hide();
        if(verbose)
            QTextStream(stdout) << "ProtolehelugejaConnection::readData() lineIn: " << lineIn << endl;
//#ifdef PROOV
//        qDebug() << "Sisse: " << lineIn;
//#endif
    }else{  //Kui on autoriseerimata ühendus, peab saabuma parool ning seda tuleb kontrollida
//        in >> sisse;    //Parooli ees on "Parool:"
        in >> receivedPasswd;
        if(verbose)
            QTextStream(stdout) << "ProtolehelugejaConnection::readData() receivedPasswd: " << receivedPasswd << endl;
        messageBox.hide();

        blockSize = 0;
        if(passwd != 0 && receivedPasswd == passwd){
            authorized = true;
//            this->setAuthorized(true);
//            parool = 0;
            send("OK"/*, socketIndex*/);
        }
        else QMessageBox::information(dynamic_cast<QWidget*>(this->parent()), "Teade", tr("Keegi proovis ühenduda vale parooliga! Ühendust ei loodud!"), QMessageBox::Ok);
        return;
    }

    if(lineIn == "Tere"){    //Ühenduse loomise kinnitus
        messageBox.setText(tr("Ühendus loodud"));
        messageBox.show();
        return;
    }else if(lineIn.startsWith("Versioon:")){    //Protolehelugeja ja Protokollitaja omavahelise ühenduse versioon
        lineIn.remove(0, 9);
        if(lineIn.toInt() > 2){
            send(tr("Viga:Protokollitaja ja Protolehelugeja versioonid ei ühti!\nProtokollitaja on uuem, seega on vaja uuendada Protolehelugejat või mõlemaid.")/*, socketIndex*/);
            authorized = false;
//            this->setAuthorized(false);
        }
        else if(lineIn.toInt() < 2){
            send(tr("Viga:Protokollitaja ja Protolehelugeja versioonid ei ühti!\nProtolehelugeja on uuem, seega on vaja uuendada Protokollitajat")/*, socketIndex*/);
            authorized = false;
//            this->setAuthorized(false);
        }else send("Versioon OK"/*, socketIndex*/);

        return;
    }else if(lineIn.startsWith("Siffer:")){  //Küsitakse laskurit, kes on selle sifriga
        lineIn.remove(0, 7);
        askedTargetNumber = lineIn.toInt();
        emit renewWithTargetNumber(myIndex);
    }else if(lineIn.startsWith("Laskur:")){  //Saabusid loetud tulemused
        //"Laskur:siffer - siffer;Eesnimi;Perekonnanimi;seeriate arv;laskude arv;seeriad;selle seeria lasud; x; y; summa;aktiivne seeria;harjutus;lasku lehes;kümnendikega lugemine (true/false)
        lastRecvdLine = lineIn;
        emit shotInfoRead(myIndex);

    }else if(lineIn.startsWith(tr("Käsk:Salvestada"))){  //Palutakse salvestada
        emit save();
    }/*else if(sisse.startsWith(tr("Käsk:Vabastada"))){  //Suletakse ühendus ja palutakse vorguLaskur vabastada
        if(vorguLaskur != 0)
            vorguLaskur->setEnabled(true);
    }*/
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
//#ifdef PROOV
//        qDebug() << "Valja: " << data;
//#endif
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

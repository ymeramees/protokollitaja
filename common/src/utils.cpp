#include "utils.h"

QStringList Utils::getLocalIps()
{
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    QList<QHostAddress> addresses;
    QStringList cleanedAddresses;

    foreach(QNetworkInterface interface, interfaces) {
        if (interface.type() == QNetworkInterface::Ethernet || interface.type() == QNetworkInterface::Wifi)
            addresses.append(interface.allAddresses());
    }

    foreach(QHostAddress addr, addresses) {
        if (addr.isGlobal() && addr.protocol() == QAbstractSocket::IPv4Protocol)
            cleanedAddresses.append(addr.toString());
    }

    cleanedAddresses.removeDuplicates();
    return cleanedAddresses;
}

void Utils::sendInbandBroadcast(QString target)
{
    QStringList addresses = Utils::getLocalIps();
    QByteArray datagram = "InBand uu?;" + addresses.first().toLocal8Bit() + ";" + target.toLocal8Bit();
    QUdpSocket udpSocket;
    udpSocket.writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, 45744);
    udpSocket.waitForBytesWritten();
    udpSocket.disconnectFromHost();
}

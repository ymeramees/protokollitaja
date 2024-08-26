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

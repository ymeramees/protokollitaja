#include "header.h"

Header::Header(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *hBox = new QHBoxLayout;
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

//    hBox->addWidget(new QLabel("Rada"));
//    hBox->addWidget(new QLabel("ID"));
//    hBox->addWidget(new QLabel("Eesnimi"));
//    hBox->addWidget(new QLabel("Perekonnanimi"));
//    hBox->addWidget(new QLabel("Klubi"));
//    hBox->addWidget(new QLabel("Harjutus"));
//    hBox->addWidget(new QLabel("Komadega"));
//    hBox->addWidget(new QLabel("Klass"));
//    hBox->addWidget(new QLabel("Laskude arv"));
//    hBox->addWidget(new QLabel("Viimane lask"));
//    hBox->addWidget(new QLabel("Summa"));
//    hBox->addWidget(new QLabel("Käsunupp"));
    QLabel *ipLabel = new QLabel(tr("IP aadress(id): %1").arg(cleanedAddresses.join(", ")));
    ipLabel->setAlignment(Qt::AlignRight);
    hBox->addWidget(ipLabel);
//    hBox->addWidget(new QLabel("Staatus"));

    setLayout(hBox);
}

#include "header.h"

Header::Header(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *hBox = new QHBoxLayout;
    QStringList cleanedAddresses = Utils::getLocalIps();

//    hBox->addWidget(new QLabel("Lane"));
//    hBox->addWidget(new QLabel("ID"));
//    hBox->addWidget(new QLabel("First Name"));
//    hBox->addWidget(new QLabel("Last Name"));
//    hBox->addWidget(new QLabel("Club"));
//    hBox->addWidget(new QLabel("Event"));
//    hBox->addWidget(new QLabel("With decimals"));
//    hBox->addWidget(new QLabel("Class"));
//    hBox->addWidget(new QLabel("Number of shots"));
//    hBox->addWidget(new QLabel("Last shot"));
//    hBox->addWidget(new QLabel("Total"));
//    hBox->addWidget(new QLabel("Command button"));
    QLabel *ipLabel = new QLabel(tr("IP address(es): %1").arg(cleanedAddresses.join(", ")));
    ipLabel->setAlignment(Qt::AlignRight);
    hBox->addWidget(ipLabel);
//    hBox->addWidget(new QLabel("Status"));

    setLayout(hBox);
}

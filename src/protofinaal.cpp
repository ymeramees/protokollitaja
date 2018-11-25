#include "protofinaal.h"

Protofinaal::Protofinaal(QWidget *parent)
    : QMainWindow(parent)
{
//    setMinimumHeight(250);
    initialize();
}

Protofinaal::~Protofinaal()
{

}

void Protofinaal::createLayout(QJsonObject &jsonObj)
{
    QVBoxLayout *vBox = new QVBoxLayout;

    int teams = jsonObj["Teams"].toInt();

    for(int i = 0; i < teams; i++){
        vBox->addWidget(new Team(jsonObj, i+1));
    }

    QWidget *widget = new QWidget();
    widget->setLayout(vBox);
//    widget->setStyleSheet("border:1px solid rgb(0, 255, 0); ");
    setCentralWidget(widget);
}

void Protofinaal::initialize()
{
    QJsonDocument configJson;
    QFile configFile("60ohupuss.json");
    if(configFile.open(QIODevice::ReadOnly)){
        configJson = QJsonDocument::fromJson(configFile.readAll());
    }else
        QMessageBox::critical(this, tr("Viga!"), tr("Harjutuse faili ei leitud!"));

    QJsonObject jsonObj = configJson.object();
    if(!(jsonObj.contains("Event") && jsonObj["Event"].isString()) ||
            !(jsonObj.contains("Teams") && jsonObj["Teams"].isDouble()) ||
            !(jsonObj.contains("Members_in_team") && jsonObj["Members_in_team"].isDouble()) ||
            !(jsonObj.contains("Shots") && jsonObj["Shots"].isArray()))
        QMessageBox::critical(this, tr("Viga!"), tr("Harjutuse fail vigane!"));

    if(verbose)
        QTextStream(stdout) << "Event: " << configJson["Event"].toString() << endl;
    setWindowTitle(configJson["Event"].toString());

    createLayout(jsonObj);
}

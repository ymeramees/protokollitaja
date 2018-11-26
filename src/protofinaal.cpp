#include "protofinaal.h"

Protofinaal::Protofinaal(QWidget *parent)
    : QMainWindow(parent)
{
//    setMinimumHeight(250);
    createMenus();
    initialize();
}

Protofinaal::~Protofinaal()
{

}

void Protofinaal::clear()
{
    foreach (Team *team, teams) {
        vBox->removeWidget(team);
        team->deleteLater();
    }
    teams.clear();
}

void Protofinaal::createLayout(QJsonObject &jsonObj)
{
    vBox = new QVBoxLayout;

    int teamsNo = jsonObj["Teams"].toInt();

    for(int i = 0; i < teamsNo; i++){
        Team *team = new Team(jsonObj, i+1);
        teams.append(team);
        vBox->addWidget(team);
    }

    QWidget *widget = new QWidget();
    widget->setLayout(vBox);
//    widget->setStyleSheet("border:1px solid rgb(0, 255, 0); ");
    setCentralWidget(widget);
}

void Protofinaal::createMenus()
{
    QMenu *fileMenu = this->menuBar()->addMenu(tr("&File"));

    QAction *openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &Protofinaal::open);

    QAction *saveAct = new QAction(tr("&Save..."), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save to an existing file"));
    connect(saveAct, &QAction::triggered, this, &Protofinaal::save);

    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
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

//    if(verbose)
//        QTextStream(stdout) << "Event: " << configJson["Event"].toString() << endl;
    setWindowTitle(jsonObj["Event"].toString());

    createLayout(jsonObj);
}

void Protofinaal::open()
{
    clear();
    readFinalsFile("save.json");
}

void Protofinaal::readFinalsFile(QString fileName)
{
    QFile file(fileName);
    QJsonDocument fileJson;

    if(file.open(QIODevice::ReadOnly)){
        fileJson = QJsonDocument::fromJson(file.readAll());
        QJsonObject jsonObj = fileJson.object();

        if(jsonObj["fileVersion"].toInt() > 300)
            QMessageBox::warning(this, tr("Viga!"), tr("Faili versioon on uuem, kui see versioon programmist. Faili avamisel võib tekkida vigu!"), QMessageBox::Ok);

        QJsonArray teamsArray = jsonObj["Teams"].toArray();
        for(int i = 0; i < teamsArray.size(); i++) {
            QJsonObject teamJson = teamsArray.at(i).toObject();
            Team *team = new Team(teamJson, i+1, this);
            teams.append(team);
            vBox->addWidget(team);
        }
        centralWidget()->setLayout(vBox);
    }else
        QMessageBox::critical(this, tr("Viga!"), tr("Faili avamine ei ole võimalik!"), QMessageBox::Ok);
}

void Protofinaal::save()
{
    writeFile("save.json");
}

void Protofinaal::toJson(QJsonObject &json) const
{
    QJsonArray teamsArray;
    foreach(Team *team, teams){
        QJsonObject teamObj;
        team->toJson(teamObj);
        teamsArray.append(teamObj);
    }
    json["Teams"] = teamsArray;
}

void Protofinaal::writeFile(QString fileName)
{
    QFile file(fileName);

    if(file.open(QIODevice::WriteOnly)){
        QJsonObject finalsObj;
        toJson(finalsObj);
        finalsObj["fileVersion"] = 300;
        QJsonDocument jsonDoc(finalsObj);
        file.write(jsonDoc.toJson());
    }else
        QMessageBox::critical(this, tr("Viga!"), tr("Faili kirjutamine ei ole võimalik!\nKontrollige, kas teil on sinna kausta kirjutamise õigused."), QMessageBox::Ok);
}

/////////////////////////////////////////////////////////////////////////////
///ToDo:
/// In progress: Additional level TeamsTable, needs modifications to be fully implemented.
/// ProtofinaalTest (TeamsTableTest), to test that shots' data from SiusData is added to correct competitor
/// Sorting based on results
/// Add shoot-off possibility
/// Print function
/// Show targets on spectator window
/// Save as function
/// Some export function
/// Import SiusData startlist
/// Connection to target scoring machine
/// Add competition stage number, so that there can be sighting shots between competition series (3 positions final)
/// Add ignore shot(s) possibility - OK
/// Add possibility to add missed shot(s)
///
/////////////////////////////////////////////////////////////////////////////

#include "protofinaal.h"

Protofinaal::Protofinaal(QWidget *parent)
    : m_settings("Protofinaal", "Protofinaali conf"), QMainWindow(parent)
{
    createMenus();
    setStatusBar(statusBar());
    teamsTable = new TeamsTable(this);
//    QWidget *widget = new QWidget();
//    widget->setLayout(vBox);
//    widget->setStyleSheet("border:1px solid rgb(0, 255, 0); ");
    setCentralWidget(teamsTable);

    this->setGeometry(9, 36, 800, 600);
    readSettings();

    QTimer::singleShot(100, this, SLOT(initialize()));

    if(verbose)
        QTextStream(stdout) << "currentFile = " << currentFile << endl;

    logFile = new QFile(QFileInfo(currentFile).dir().absolutePath() + QString("/Protofinaal logi %1.log").arg(QDate::currentDate().toString(Qt::ISODate)));

    if(logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){ //Log file
        logOut.setDevice(logFile);
    }else{
        QMessageBox::critical(this, tr("Viga"), tr("Logi faili kirjutamine ei õnnestunud! Kontrollige, et teil oleks kirjutamisõigus sinna kausta, kus asub võistluste fail."), QMessageBox::Ok);
    }
}

Protofinaal::~Protofinaal()
{

}

void Protofinaal::clear()
{
//    foreach (Team *team, teams) {
//        vBox->removeWidget(team);
//        team->deleteLater();
//    }
//    teams.clear();
    if(teamsTable != nullptr)
        teamsTable->clear();
    competitionName.clear();
    timePlace.clear();
}

void Protofinaal::closeEvent(QCloseEvent *event)
{
    if(verbose)
        QTextStream(stdout) << "Protofinaal::closeEvent()" << endl;
    event->accept();
    qApp->quit();
}

//void Protofinaal::createLayout(QJsonObject &jsonObj)
//{
//    int teamsNo = jsonObj["Teams"].toInt();

//    for(int i = 0; i < teamsNo; i++){
//        Team *team = new Team(jsonObj, i+1);
//        connect(team, &Team::teamUpdated, this, &Protofinaal::updateSpectatorWindow);
//        connect(team, &Team::statusInfo, this, &Protofinaal::statusBarInfoChanged);
//        teams.append(team);
//        vBox->addWidget(team);
//    }
//}

void Protofinaal::createMenus()
{
    QMenu *fileMenu = this->menuBar()->addMenu(tr("&Fail"));
    QMenu *editMenu = this->menuBar()->addMenu(tr("&Tulemused"));

    QAction *openAct = new QAction(tr("&Ava..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Ava fail"));
    connect(openAct, &QAction::triggered, this, &Protofinaal::open);

    QAction *saveAct = new QAction(tr("&Salvesta"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Salvesta fail"));
    connect(saveAct, &QAction::triggered, this, &Protofinaal::save);

    QAction *exitAct = new QAction(tr("&Välju"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Välju programmist"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    QAction *showSpectatorWindowAct = new QAction(tr("&Tulemuste aken"), this);
    showSpectatorWindowAct->setStatusTip(tr("Ava tulemuste aken"));
    connect(showSpectatorWindowAct, &QAction::triggered, this, &Protofinaal::showSpecatorWindowOnSecondScreen);

    QAction *connectToSiusDataAct = new QAction(tr("&SiusData"), this);
    connectToSiusDataAct->setStatusTip(tr("Ühendu SiusData'ga"));
    connect(connectToSiusDataAct, &QAction::triggered, this, &Protofinaal::connectToSiusData);

    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu->addAction(showSpectatorWindowAct);
    editMenu->addAction(connectToSiusDataAct);
}

void Protofinaal::connectToSiusData()
{
    siusLog = new QFile(QFileInfo(currentFile).dir().absolutePath() + QString(tr("/Protofinaal sisse logi %1.log")).arg(QDate::currentDate().toString(Qt::ISODate)));

    if(siusDataConnections == nullptr){
        siusDataConnections = new SiusDataConnections(siusLog, &logOut, &m_settings, this);
        connect(siusDataConnections, &SiusDataConnections::statusInfo, this, &Protofinaal::statusBarInfoChanged);
        if(teamsTable != nullptr)
            connect(siusDataConnections, &SiusDataConnections::shotRead, teamsTable, &TeamsTable::readSiusInfo);
        connect(siusDataConnections, &SiusDataConnections::disconnectedFromSius, this, &Protofinaal::connectionToSiusLost);
    }

    if(siusLog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){ //Saabunud võrguliikluse logi
        QTextStream valja(siusLog);
        valja << "///////////////////////////////" << competitionName << ", " << QDateTime::currentDateTime().toString() <<  "///////////////////////////////\n";
        siusLog->close();
    }

    siusDataConnections->exec();
}

void Protofinaal::connectionToSiusLost(int connectionIndex)
{
    // TODO To be implemented
}

void Protofinaal::initialize()
{
    if(initialDialog == nullptr){
        initialDialog = new InitialDialog(this);
        connect(initialDialog, &InitialDialog::updateMe, this, &Protofinaal::updateInitialDialog);
    }

    QJsonObject initialJson = readFinalsFile(currentFile, false);
    initialDialog->setFileName(currentFile);
    initialDialog->setCompetitionName(initialJson["competitionName"].toString());
    initialDialog->setTimePlace(initialJson["timePlace"].toString());

    if(initialDialog->exec() == QDialog::Accepted){
        writeSettings();
        currentFile = initialDialog->fileName();
        QFile testOpenFile(initialDialog->fileName());
        if(testOpenFile.open(QIODevice::ReadOnly)){ //Check if file exists
            if(verbose)
                QTextStream(stdout) << "File exists: " << initialDialog->fileName() << endl;
            testOpenFile.close();
            loadFile(currentFile);
        }else{  //File does not exist
            if(verbose)
                QTextStream(stdout) << "Create new file: " << initialDialog->fileName() << endl;

            competitionName = initialDialog->competitionName();
            timePlace = initialDialog->timePlace();

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
            eventName = jsonObj["Event"].toString();
            teamsTable->createLayout(jsonObj);
        }

        logOut << "///////////////////////////////" << competitionName << ", " << QDateTime::currentDateTime().toString() <<  "///////////////////////////////\n";
        statusBarInfoChanged(tr("Avatud fail: ") + currentFile);
        showSpecatorWindowOnSecondScreen();

    }else if(initialDialog->result() == QDialog::Rejected)
        QCoreApplication::quit();

    setWindowTitle("Protofinaal - " + competitionName + " - " + eventName);
}

void Protofinaal::loadFile(QString fileName)
{
    if(verbose)
        QTextStream(stdout) << "Protofinaal::loadFile()" << endl;
    clear();
    QJsonObject jsonObj = readFinalsFile(fileName);


//    centralWidget()->setLayout(vBox);
}

void Protofinaal::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Ava fail"), currentFile, tr("Protofinaali fail (*.fin)"));
    if(!fileName.isEmpty()){
        currentFile = fileName;
        loadFile(fileName);
        writeSettings();
        updateSpectatorWindow();
    }
}

QJsonObject Protofinaal::readFinalsFile(QString fileName, bool showErrors)
{
    QFile file(fileName);
    QJsonDocument fileJson;
    QJsonObject jsonObj;

    if(file.open(QIODevice::ReadOnly)){
        fileJson = QJsonDocument::fromJson(file.readAll());
        jsonObj = fileJson.object();

        if(jsonObj["fileVersion"].toInt() > 300)
            QMessageBox::warning(this, tr("Viga!"), tr("Faili versioon on uuem, kui see versioon programmist. Faili avamisel võib tekkida vigu!"), QMessageBox::Ok);

        competitionName = jsonObj["competitionName"].toString();
        eventName = jsonObj["eventName"].toString();
        timePlace = jsonObj["timePlace"].toString();
    }else if(showErrors)
        QMessageBox::critical(this, tr("Viga!"), tr("Faili avamine ei ole võimalik!\n\nAsukoht: ") + file.fileName(), QMessageBox::Ok);
    return jsonObj;
}

void Protofinaal::readSettings()
{
    QFile file("conf.json");
    QJsonDocument fileJson;

    if(file.open(QIODevice::ReadOnly)){
        fileJson = QJsonDocument::fromJson(file.readAll());
        QJsonObject jsonObj = fileJson.object();

        currentFile = jsonObj["lastFile"].toString();

        if(jsonObj.contains("windowXLocation") && jsonObj["windowXLocation"].isDouble() && jsonObj.contains("windowYLocation") && jsonObj["windowYLocation"].isDouble())
            //For some reason window frame is not included, so 9 and 36 need to be added. At least on Win7
            this->setGeometry(jsonObj["windowXLocation"].toInt() + 9, jsonObj["windowYLocation"].toInt() + 36, 800, 600);

    }else
        QMessageBox::critical(
                    this,
                    tr("Viga!"),
                    tr("Seadete faili avamine ei ole võimalik!\nKasutatakse vaikimisi seadeid.\n\nAsukoht: ").append(file.fileName()),
                    QMessageBox::Ok
                    );
}

//void Protofinaal::readSiusInfo(SiusShotData shotData)
//{
//    if(verbose)
//        QTextStream(stdout) << "readSiusInfo()" << endl;

//    // FIXME To be implemented
////    for(QString row : lines){
//        //Search for competitor whose line was received:
////        QStringList rowParts = row.split(';');
//    Competitor *thisCompetitor = nullptr;
//    foreach (Team *team, teams) {
//        if(team->getCompetitorWithID(shotData.id) != nullptr){
//            thisCompetitor = team->getCompetitorWithID(shotData.id);

//            bool success = thisCompetitor->readSiusShot(shotData);
//            if (success)
//                statusBarInfoChanged(QString("Lisatud: %1%2%3 = %4").arg(thisCompetitor->name()).arg(tr(" lask ")).arg(shotData.siusShotNo).arg(shotData.shot.getSLask()));
//            else statusBarInfoChanged(QString("Viga, ei õnnestunud lisada: %1%2%3 = %4").arg(thisCompetitor->name()).arg(tr(" lask ")).arg(shotData.siusShotNo).arg(shotData.shot.getSLask()));
//        }
//    }
////            if(row.startsWith("_SHOT") && !competitionStarted){   //Shot data, sighting shots
////                    thisCompetitor->setPreviousSiusRow(row);

////                    statusBarInfoChanged(rowParts[3] + tr(": Proovilask"));

////                    Lask lask(row);
////                    if(verbose)
////                        QTextStream(stdout) << "rowParts[3]: " << rowParts[3] << ", rida: " << row;
////                }else if(row.startsWith("_TOTL") && !competitionStarted){   //Result row
////                    if(rowParts.count() > 12){  //If row is not long enough, then probably it is total result, which is not interesting here
////                        if(rowParts[12] != "0"){   //If it is sighting shot, then this is 0, but if competition shot, then it is series result
////                            if(thisCompetitor->previousSiusRow().startsWith("_SHOT")){  //As previous shot was already competition shot, then it needs to be added to someone
////                                competitionStarted = true;
////                                statusBarInfoChanged(rowParts[3] + tr(": algavad võistluslasud"));

////                                QStringList previousRowParts = thisCompetitor->previousSiusRow().split(';');
////                                Lask lask(thisCompetitor->previousSiusRow());

////                                thisCompetitor->setShot(0, thisCompetitor->previousSiusRow());
////                                statusBarInfoChanged(thisCompetitor->name() + tr(" lask 1 = ") + lask.getSLask());
////                                if(verbose)
////                                    QTextStream(stdout) << "rowParts[12]: " << rowParts[12] << "voistlus = true" << "previousRowParts[10]: " << previousRowParts[10] << "\n\n";
////                            }
////                        }
////                    }
////                }else if(row.startsWith("_SHOT") && competitionStarted){   //Võistluslasu rida

////                    int shotNo = rowParts[13].toInt() - 1 /*- shootOffShots*/;  //Siusist tulnud lasu Nr, koodi lühendamise eesmärgil
////                    Lask lask(row);

////                    thisCompetitor->setShot(shotNo, row);
////                    statusBarInfoChanged(QString("%1%2%3 = %4").arg(thisCompetitor->name()).arg(tr(" lask ")).arg(shotNo).arg(lask.getSLask()));
////                }
////            }
////        }
//    if(thisCompetitor == nullptr)
//            statusBarInfoChanged(tr("Sellise ID'ga võistlejat ei leitud: ") + shotData.id);
////    }
//    sumAllTeams();
//}

void Protofinaal::save()
{
    writeFinalsFile(currentFile);
}

void Protofinaal::showSpecatorWindowOnSecondScreen()
{
    if(qApp->desktop()->numScreens() >= 2 && qApp->desktop()->isVirtualDesktop()){
        spectatorWindow.move(qApp->desktop()->screenGeometry(this).width() + 100, 100);
        if(qApp->desktop()->screenNumber(this) != qApp->desktop()->screenNumber(&spectatorWindow) && qApp->desktop()->screenNumber(&spectatorWindow) != -1)
            spectatorWindow.showFullScreen();
        else{
            spectatorWindow.move(-1000, 100);
            if(qApp->desktop()->screenNumber(this) != qApp->desktop()->screenNumber(&spectatorWindow))
                spectatorWindow.showFullScreen();
        }
    }else{
        QMessageBox::critical(this, tr("Viga"), tr("Teist ekraani ei leitud. Programmi korralikuks"
    " funktsioneerimiseks on vajalik kahe ekraani olemasolu."), QMessageBox::Ok);
        spectatorWindow.show();
    }

    if(spectatorWindow.isFullScreen())
        QMessageBox::information(this, tr("Teade"), tr("Tulemuse aken näidatud teisel ekraanil"), QMessageBox::Ok);

    spectatorWindow.setHeading(competitionName, timePlace, eventName, tr("Koht"), tr("Nimi"), tr("Seeria/lask"), tr("Vahe"));
    updateSpectatorWindow();
}

void Protofinaal::statusBarInfoChanged(QString newStatusInfo)
{
    statusBar()->showMessage(newStatusInfo, 5000);
    logOut << QTime::currentTime().toString() << " " << newStatusInfo << endl;
}

//void Protofinaal::sumAllTeams()
//{
//    foreach (Team *team, teams) {
//        team->sumAll();
//    }
//}

QJsonObject Protofinaal::toJson() const
{
//    QJsonArray teamsArray;
//    foreach(Team *team, teams){
//        QJsonObject teamObj;
//        team->toJson(teamObj);
//        teamsArray.append(team->toJson());
//    }
//    QJsonObject json;
//    json["Teams"] = teamsArray;
    QJsonObject finalsObj = teamsTable->toJson();
    finalsObj["fileVersion"] = 300;
    finalsObj["competitionName"] = competitionName;
    finalsObj["eventName"] = eventName;
    finalsObj["timePlace"] = timePlace;
    return finalsObj;
}

void Protofinaal::updateInitialDialog()
{
    currentFile = initialDialog->fileName();
    QJsonObject initialJson = readFinalsFile(currentFile, false);
    initialDialog->setFileName(currentFile);
    initialDialog->setCompetitionName(initialJson["competitionName"].toString());
    initialDialog->setTimePlace(initialJson["timePlace"].toString());
}

void Protofinaal::updateSpectatorWindow()
{
    // FIXME To be implemented
//    if(verbose)
//        QTextStream(stdout) << "Protofinaal::updateSpectatorWindow()" << endl;
//    spectatorWindow.clearResults();
//    if(verbose)
//        QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), teams.size() = " << teams.size() << endl;
//    foreach (Team *team, teams){
//        QVector<Competitor*> teamCompetitors = team->teamCompetitors();
//        if(verbose)
//            QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), teamCompetitors.size() = " << teamCompetitors.size() << endl;
//        if(teamCompetitors.size() == 0)
//            return;
////        if(verbose){
////            QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), team->index() = " << team->index() << endl;
////            QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), team->teamName() = " << team->teamName() << endl;
////            QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), teamCompetitors.at(0)->name() = " << teamCompetitors.at(0)->name() << endl;
////            QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), teamCompetitors.at(0)->lastResult() = " << teamCompetitors.at(0)->lastResult() << endl;
////            QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), team->lastSum() = " << team->lastSum() << endl;
////        }
//        spectatorWindow.addRow(team->index(), team->teamName(), teamCompetitors.at(0)->name(), teamCompetitors.at(0)->lastResult(), team->lastSum(), "0");
//        if(teamCompetitors.size() > 1){
//            for(int i = 1; i < teamCompetitors.size(); i++){
//                if(verbose)
//                    QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), teamCompetitors.at(i)->lastResult() = " << teamCompetitors.at(i)->lastResult() << endl;
//                spectatorWindow.addRow("", "", teamCompetitors.at(i)->name(), teamCompetitors.at(i)->lastResult(), "", "");
//            }
//        }

////        QJsonObject teamJson;
////        team->toJson(teamJson);
////        QJsonArray competitorsArray = teamJson["Members_in_team"].toArray();
////        for(QJsonValue jsonValue : competitorsArray){
////            QJsonObject competitorObj = jsonValue.toObject();
////            spectatorWindow.addRow("1. ", teamJson["teamName"].toString(), competitorObj["nameEdit"].toString(), competitorObj["Sum"].toString(),  competitorObj["Sum"].toString(), "0");
////        }
//    }
}

void Protofinaal::writeFinalsFile(QString fileName)
{
    QFile file(fileName);

    if(file.open(QIODevice::WriteOnly)){
        QJsonDocument jsonDoc(toJson());
        file.write(jsonDoc.toJson());
        statusBar()->showMessage(tr("Fail salvestatud"), 5000);
    }else
        QMessageBox::critical(this, tr("Viga!"), tr("Faili kirjutamine ei ole võimalik!\nKontrollige, kas teil on sinna kausta kirjutamise õigused."), QMessageBox::Ok);
}

void Protofinaal::writeSettings()
{
    QFile file("conf.json");

    if(file.open(QIODevice::WriteOnly)){
        QJsonObject jsonObj;

        jsonObj["lastFile"] = currentFile;
        jsonObj["windowXLocation"] = this->x();
        jsonObj["windowYLocation"] = this->y();
        QJsonDocument jsonDoc(jsonObj);
        file.write(jsonDoc.toJson());
    }else
        QMessageBox::critical(this, tr("Viga!"), tr("Seadete faili kirjutamine ei ole võimalik!\nKontrollige, kas teil on sinna kausta kirjutamise õigused."), QMessageBox::Ok);
}

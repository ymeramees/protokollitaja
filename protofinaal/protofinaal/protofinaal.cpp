/////////////////////////////////////////////////////////////////////////////
///ToDo:
/// In progress:
/// Enable switching from one final stage to another
/// Would be good to create a new file immediately when going forward from initialdialog
/// In Windows when exiting from initialdialog, the program won't exit?
/// Crashes when big negative offset is used (same as number of shots) - OK
/// Enable adjusting Sius shot number offset - OK
/// Sorting based on results - OK
/// Show last shot number in spectators window - OK
/// Add possibility to have two teamTables - OK
/// Choice of event types - OK
/// Medal match - OK
/// Import SiusData startlist - OK
/// Add possibility to add missed shot(s)
/// Add ignore shot(s) possibility - Needs to be implemented again
/// Add shoot-off possibility
/// Print function
/// Save as function
/// Show targets on spectator window
/// Some export function
/// Add competition stage number, so that there can be sighting shots between competition series (3 positions final)
/// Connection to target scoring machine
/// OLD - Additional level TeamsTable, needs modifications to be fully implemented.
/// OLD - ProtofinaalTest (TeamsTableTest), to test that shots' data from SiusData is added to correct competitor
///
/////////////////////////////////////////////////////////////////////////////

#include "protofinaal.h"

Protofinaal::Protofinaal(QWidget *parent)
    : m_settings("Protofinaal", "Protofinaali conf"), QMainWindow(parent)
{
    createMenus();
    setStatusBar(statusBar());
//    QWidget *widget = new QWidget();
//    widget->setLayout(vBox);
//    widget->setStyleSheet("border:1px solid rgb(0, 255, 0); ");
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    setCentralWidget(scrollArea);
    scrollArea->setLayout(&vBox);

    this->setGeometry(9, 36, 1200, 600);
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
    foreach (TeamsTable2022 *teamsTable, m_teamsTables) {
        teamsTable->clear();
        teamsTable->deleteLater();
    }
    m_teamsTables.clear();
    competitionName.clear();
    timePlace.clear();
    eventName.clear();
    eventType.clear();
    m_spectatorWindow.clearResults();
}

void Protofinaal::closeEvent(QCloseEvent *event)
{
    if(verbose)
        QTextStream(stdout) << "Protofinaal::closeEvent()" << endl;
    if (m_modifiedAfterSave) {
        int reply = QMessageBox::question(this, "Protofinaal", tr("Kas soovid muudatused salvestada ja programmist väljuda?"),	QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (reply == QMessageBox::Save) {
            save();
            event->accept();
        } else if(reply == QMessageBox::Cancel) {
            event->ignore();
        } else {
            event->accept();
        }
    } else {
        event->accept();
    }

    if(event->isAccepted())
        qApp->quit();
}

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

    QAction *importSiusStartListAct = new QAction(tr("&Impordi Sius startlist..."), this);
    importSiusStartListAct->setShortcuts(QKeySequence::Open);
    importSiusStartListAct->setStatusTip(tr("Impordi Sius startlist"));
    connect(importSiusStartListAct, &QAction::triggered, this, &Protofinaal::importSiusStartList);

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
    fileMenu->addAction(importSiusStartListAct);
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
        foreach (TeamsTable2022 *teamsTable, m_teamsTables) {
            connect(siusDataConnections, &SiusDataConnections::shotRead, teamsTable, &TeamsTable2022::readSiusInfo);
        }
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

void Protofinaal::importSiusStartList()
{
    QString fileName = m_initialDialog->fileName().left(QDir::fromNativeSeparators(m_initialDialog->fileName()).lastIndexOf("/"));
    QString filePath = QFileDialog::getOpenFileName(this, tr("Ava startlist"), fileName, tr("Comma separated file (*.csv)"));
    if (filePath.isEmpty())
        return;

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QStringList rows;
        while(!in.atEnd()){  // Read the whole file in
            QString newRow = in.readLine();
            int newTarget = newRow.split(';').at(10).toInt();
            bool wasAdded = false;
            if (rows.size() > 0) {
                for (int i = 0; i < rows.size(); i++) {
                    int currentTarget = rows.at(i).split(';').at(10).toInt();
                    if (newTarget < currentTarget) {
                        rows.insert(i, newRow);
                        wasAdded = true;
                        break ;
                    }
                }
            }
            if (!wasAdded)
                rows.append(newRow);
        }

        foreach(TeamsTable2022 *teamsTable, m_teamsTables) {
            QStringList forCurrentTable;
            for(int i = 0; i < teamsTable->teamsCount(); i++)
                if (!rows.isEmpty())
                    forCurrentTable.append(rows.takeFirst());
            teamsTable->setCompetitiorsData(forCurrentTable);
        }

    } else
        QMessageBox::critical(this, "Viga", "Ei õnnestunud faili avada!", QMessageBox::Ok);
}

void Protofinaal::initialize()
{
    if(verbose)
        QTextStream(stdout) << "initialize" << endl;

    if(m_initialDialog == nullptr){
        m_initialDialog = new InitialDialog(this);
        connect(m_initialDialog, &InitialDialog::updateMe, this, &Protofinaal::updateInitialDialog);
    }

    QJsonObject initialJson = readFinalsFile(currentFile, false);
    m_initialDialog->setFileName(currentFile);
    m_initialDialog->setCompetitionName(initialJson["competitionName"].toString());
    m_initialDialog->setTimePlace(initialJson["timePlace"].toString());
    m_initialDialog->setEventName(initialJson["eventName"].toString());
    m_initialDialog->setEventType(initialJson["eventType"].toString());

    if(m_initialDialog->exec() == QDialog::Accepted){
        writeSettings();
        currentFile = m_initialDialog->fileName();
        QFile testOpenFile(m_initialDialog->fileName());
        if(testOpenFile.open(QIODevice::ReadOnly)){ //Check if file exists    // FIXME Re-enable
            if(verbose)
                QTextStream(stdout) << "File exists: " << m_initialDialog->fileName() << endl;
            testOpenFile.close();
            loadFile(currentFile);
        }else{  //File does not exist
            if(verbose)
                QTextStream(stdout) << "Create new file: " << m_initialDialog->fileName() << endl;

            competitionName = m_initialDialog->competitionName();
            timePlace = m_initialDialog->timePlace();
            eventName = m_initialDialog->eventName();
            eventType = m_initialDialog->eventType();

            QJsonDocument configJson;
            QFile configFile(QString("%1.json").arg(eventType));
            if(configFile.open(QIODevice::ReadOnly)){
                configJson = QJsonDocument::fromJson(configFile.readAll());
            }else
                QMessageBox::critical(this, tr("Viga!"), tr("Harjutuse faili ei leitud!"));

            QJsonObject jsonObj = configJson.object();
            if(!(jsonObj.contains("event") && jsonObj["event"].isString()) ||
                    !(jsonObj.contains("relaysTogether") && jsonObj["relaysTogether"].isDouble()) ||
                    !(jsonObj.contains("teams") && jsonObj["teams"].isDouble()) ||
                    !(jsonObj.contains("membersInTeam") && jsonObj["membersInTeam"].isDouble()) ||
                    !(jsonObj.contains("shots") && jsonObj["shots"].isArray()) ||
                    !(jsonObj.contains("scoringWithPoints") && jsonObj["scoringWithPoints"].isBool()))
                QMessageBox::critical(this, tr("Viga!"), tr("Harjutuse fail vigane!"));

            m_scoringWithPoints = false;
            if (jsonObj.contains("scoringWithPoints") && jsonObj["scoringWithPoints"].isBool()) {
                m_scoringWithPoints = jsonObj["scoringWithPoints"].toBool();
            }

            int numberOfRelaysTogether = jsonObj["relaysTogether"].toDouble();
            for (int relay = 1; relay <= numberOfRelaysTogether; relay++) {
                TeamsTable2022 *m_teamsTable = new TeamsTable2022();
                vBox.addWidget(m_teamsTable);
                if (numberOfRelaysTogether > 1)
                    m_teamsTable->setTableName(QString("Grupp %1").arg(relay));

                connect(m_teamsTable, &TeamsTable2022::updateSpectatorWindow, this, &Protofinaal::updateSpectatorWindow);
                connect(m_teamsTable, &TeamsTable2022::modified, [this]() { m_modifiedAfterSave = true; });
                m_teamsTables.append(m_teamsTable);
                m_teamsTable->createLayout(jsonObj, m_scoringWithPoints);
            }
        }

        logOut << "///////////////////////////////" << competitionName << ", " << QDateTime::currentDateTime().toString() <<  "///////////////////////////////\n";
        statusBarInfoChanged(tr("Avatud fail: ") + currentFile);
        showSpecatorWindowOnSecondScreen();

    }else if(m_initialDialog->result() == QDialog::Rejected)
        QCoreApplication::quit();

    setWindowTitle("Protofinaal - test 3 - " + currentFile + " - " + competitionName + " - " + eventName);
}

void Protofinaal::loadFile(QString fileName)
{
    if(verbose)
        QTextStream(stdout) << "Protofinaal::loadFile()" << endl;
    clear();
    QJsonObject jsonObj = readFinalsFile(fileName);

    if(!(jsonObj.contains("relays") && jsonObj["relays"].isArray()) ||
            !(jsonObj.contains("competitionName") && jsonObj["competitionName"].isString()) ||
            !(jsonObj.contains("eventName") && jsonObj["eventName"].isString()) ||
            !(jsonObj.contains("eventType") && jsonObj["eventType"].isString()) ||
            !(jsonObj.contains("timePlace") && jsonObj["timePlace"].isString()) ||
            !(jsonObj.contains("fileVersion") && jsonObj["fileVersion"].isDouble()))
        QMessageBox::critical(this, tr("Viga!"), tr("Finaali fail vigane!"));

    m_scoringWithPoints = false;
    if (jsonObj.contains("scoringWithPoints"))
        m_scoringWithPoints = jsonObj["scoringWithPoints"].toBool();

    if (verbose)
        QTextStream(stdout) << "Protofinaal::loadFile(), scoringWithPoints: " << m_scoringWithPoints << endl;

    QJsonArray relaysArray = jsonObj["relays"].toArray();
    int noOfRelays = relaysArray.size();
    int relayNo = 1;
    foreach (QJsonValue relayJson, relaysArray) {
        TeamsTable2022 *m_teamsTable = new TeamsTable2022();
        vBox.addWidget(m_teamsTable);
        if (noOfRelays > 1)
            m_teamsTable->setTableName(QString("Grupp %1").arg(relayNo++));

        connect(m_teamsTable, &TeamsTable2022::updateSpectatorWindow, this, &Protofinaal::updateSpectatorWindow);
        connect(m_teamsTable, &TeamsTable2022::modified, [this]() { m_modifiedAfterSave = true; });
        m_teamsTables.append(m_teamsTable);
        m_teamsTable->createLayout(relayJson.toObject(), m_scoringWithPoints);
    }

    m_modifiedAfterSave = false;

//    centralWidget()->setLayout(vBox);
}

void Protofinaal::open()
{
    if (m_modifiedAfterSave) {
        int reply = QMessageBox::question(this, "Protofinaal", tr("Kas soovid muudatused salvestada?"),	QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (reply == QMessageBox::Save) {
            save();
        } else if(reply == QMessageBox::Cancel) {
            return;
        }
    }

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

        m_scoringWithPoints = false;
        if (jsonObj.contains("scoringWithPoints"))
            m_scoringWithPoints = jsonObj["scoringWithPoints"].toBool();
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
            this->setGeometry(jsonObj["windowXLocation"].toInt() + 9, jsonObj["windowYLocation"].toInt() + 36, 1200, 600);

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
        m_spectatorWindow.move(qApp->desktop()->screenGeometry(this).width() + 100, 100);
        if(qApp->desktop()->screenNumber(this) != qApp->desktop()->screenNumber(&m_spectatorWindow) && qApp->desktop()->screenNumber(&m_spectatorWindow) != -1)
            m_spectatorWindow.showFullScreen();
        else{
            m_spectatorWindow.move(-1000, 100);
            if(qApp->desktop()->screenNumber(this) != qApp->desktop()->screenNumber(&m_spectatorWindow))
                m_spectatorWindow.showFullScreen();
        }
    }else{
        QMessageBox::critical(this, tr("Viga"), tr("Teist ekraani ei leitud. Programmi korralikuks"
    " funktsioneerimiseks on vajalik kahe ekraani olemasolu."), QMessageBox::Ok);
        m_spectatorWindow.show();
    }

    if(m_spectatorWindow.isFullScreen())
        QMessageBox::information(this, tr("Teade"), tr("Tulemuse aken näidatud teisel ekraanil"), QMessageBox::Ok);

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
//    foreach(Team *team, teams){
//        QJsonObject teamObj;
//        team->toJson(teamObj);
//        teamsArray.append(team->toJson());
//    }
//    QJsonObject json;
//    json["teams"] = teamsArray;
    QJsonArray relaysArray;
    foreach (TeamsTable2022 *teamsTable, m_teamsTables) {
        relaysArray.append(teamsTable->toJson());
    }

    QJsonObject finalsObj;// = m_teamsTable->toJson();
    finalsObj["relays"] = relaysArray;
    finalsObj["fileVersion"] = 300;
    finalsObj["competitionName"] = competitionName;
    finalsObj["eventName"] = eventName;
    finalsObj["eventType"] = eventType;
    finalsObj["timePlace"] = timePlace;
    finalsObj["scoringWithPoints"] = m_scoringWithPoints;
    return finalsObj;
}

void Protofinaal::updateInitialDialog()
{
    currentFile = m_initialDialog->fileName();
    QJsonObject initialJson = readFinalsFile(currentFile, false);
    m_initialDialog->setFileName(currentFile);
    QString newCompetitionName = initialJson["competitionName"].toString();
    if (!newCompetitionName.isEmpty())
        m_initialDialog->setCompetitionName(newCompetitionName);
    QString newTimeAndPlace = initialJson["timePlace"].toString();
    if (!newTimeAndPlace.isEmpty())
        m_initialDialog->setTimePlace(newTimeAndPlace);
    QString newEventName = initialJson["eventName"].toString();
    if (!newEventName.isEmpty())
        m_initialDialog->setEventName(newEventName);
}

void Protofinaal::updateSpectatorWindow()
{
    if(verbose)
        QTextStream(stdout) << "Protofinaal::updateSpectatorWindow()" << endl;

    m_spectatorWindow.clearResults();
    int shotNo = m_teamsTables.first()->lastValidShotIndex() + 1;
    QString shotHeader = tr("Lask");
    if (shotNo > 0)
        shotHeader.prepend(QString("%1. ").arg(shotNo));
    if (m_scoringWithPoints)
        m_spectatorWindow.setHeading(competitionName, timePlace, eventName, tr("Koht"), tr("Nimi"), shotHeader, tr("Punktid"), tr("Kokku"));
    else
        m_spectatorWindow.setHeading(competitionName, timePlace, eventName, tr("Koht"), tr("Nimi"), shotHeader, tr("Seeria"), tr("Kokku"));

    for (int i = m_teamsTables.size() - 1; i >= 0; i-- ) {
        m_spectatorWindow.addRow("", "", "", "", "", "");   // To add some spacing between different groups
        m_spectatorWindow.addRow("", "", "", "", "", "");
        m_spectatorWindow.addRow("", "", "", "", "", "");
//    foreach(TeamsTable2022 *m_teamsTable, m_teamsTables) {
        auto results = m_teamsTables.at(i)->getSortedResults();

        int index = results.size();
        foreach(auto currentResult, results) {
            m_spectatorWindow.addRow(
                        QString("%1.").arg(index--),
                        m_teamsTables.at(i)->tableName(),
                        currentResult.name,
                        currentResult.shotValue,
                        currentResult.seriesOrPoints,
                        currentResult.totalScore
                        );
        }
    }
//    for(int i = 0; i < results.size(); i++) {
//        auto currentResult = results.value(i + 1);
//        m_spectatorWindow.addRow(
//                    QString("%1.").arg(i + 1),
//                    "Grupp 1",
//                    currentResult.name,
//                    currentResult.shotValue,
//                    currentResult.points,
//                    currentResult.totalPoints
//                    );
//    }
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
////        QJsonArray competitorsArray = teamJson["membersInTeam"].toArray();
////        for(QJsonValue jsonValue : competitorsArray){
////            QJsonObject competitorObj = jsonValue.toObject();
////            spectatorWindow.addRow("1. ", teamJson["teamName"].toString(), competitorObj["nameEdit"].toString(), competitorObj["Sum"].toString(),  competitorObj["Sum"].toString(), "0");
////        }
//    }
}

void Protofinaal::writeFinalsFile(QString fileName)
{
    QFile file(fileName);

    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument jsonDoc(toJson());
        file.write(jsonDoc.toJson());
        statusBar()->showMessage(tr("Fail salvestatud"), 5000);
        m_modifiedAfterSave = false;
    } else
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

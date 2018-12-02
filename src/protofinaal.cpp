#include "protofinaal.h"

Protofinaal::Protofinaal(QWidget *parent)
    : QMainWindow(parent)
{
    createMenus();
    setStatusBar(statusBar());
    vBox = new QVBoxLayout;
    QWidget *widget = new QWidget();
    widget->setLayout(vBox);
//    widget->setStyleSheet("border:1px solid rgb(0, 255, 0); ");
    setCentralWidget(widget);

    this->setGeometry(9, 36, 800, 600);
    readSettings();

    QTimer::singleShot(100, this, SLOT(initialize()));

    if(verbose)
        QTextStream(stdout) << "currentFile = " << currentFile << endl;
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

void Protofinaal::createLayout(QJsonObject &jsonObj)
{
    int teamsNo = jsonObj["Teams"].toInt();

    for(int i = 0; i < teamsNo; i++){
        Team *team = new Team(jsonObj, i+1);
        connect(team, &Team::teamUpdated, this, &Protofinaal::updateSpectatorWindow);
        teams.append(team);
        vBox->addWidget(team);
    }
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

    QAction *exitAct = new QAction(tr("&Välju"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Välju programmist"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    QAction *showSpectatorWindowAct = new QAction(tr("&Tulemuste aken"), this);
    showSpectatorWindowAct->setStatusTip(tr("Ava tulemuste aken"));
    connect(showSpectatorWindowAct, &QAction::triggered, this, &Protofinaal::showSpecatorWindowOnSecondScreen);

    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu->addAction(showSpectatorWindowAct);
}

void Protofinaal::initialize()
{
    if(initialDialog == nullptr){
        initialDialog = new InitialDialog(this);
        connect(initialDialog, &InitialDialog::updateMe, this, &Protofinaal::updateInitialDialog);
    }

    QJsonObject initialJson = readFinalsFile(currentFile);
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
            createLayout(jsonObj);
        }
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
    QJsonArray teamsArray = jsonObj["Teams"].toArray();

    for(int i = 0; i < teamsArray.size(); i++) {
        QJsonObject teamJson = teamsArray.at(i).toObject();
        Team *team = new Team(teamJson, i+1, this);
        connect(team, &Team::teamUpdated, this, &Protofinaal::updateSpectatorWindow);
        teams.append(team);
        vBox->addWidget(team);
    }
    centralWidget()->setLayout(vBox);
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
        QMessageBox::critical(this, tr("Viga!"), tr("Faili avamine ei ole võimalik!"), QMessageBox::Ok);
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
            this->setGeometry(jsonObj["windowXLocation"].toInt() + 9, jsonObj["windowYLocation"].toInt() + 36, 800, 600);  //For some reason window frame is not included, so 9 and 36 need to be added. At least on Win7

    }else
        QMessageBox::critical(this, tr("Viga!"), tr("Seadete faili avamine ei ole võimalik!"), QMessageBox::Ok);
}

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

    spectatorWindow.setHeading(competitionName, timePlace, eventName, tr("Koht"), tr("Nimi"), tr("Seeria"), tr("Vahe"));
    updateSpectatorWindow();
//    spectatorWindow.addRow("1.", "", "P. PAKIRAAM", "51,3", "51,3", "0");

//    QString resultsTemplate = "<!DOCTYPE html>\n<html>\n<body>\n<h1>%1</h1>\n<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\">\n<thead>\n</thead>\n<tbody>\n</tbody>\n%2\n</body>\n</html>";
//    QString resultRow =
//    spectatorWindow.setResults("<!DOCTYPE html>\n<html>\n<head>\n<title>Hello, Devslopes</title>\n</head>\n<body>\n<h1>Welcome!</h1>\n<h2>Want to learn code?</h2>\n<h3 >Then you've vome to the right place!</h3>\n<h4>So get started now!</h4>\n<p>Muu tekst tuleb siia. Pikem <b>jutt</b>, mida <i>näidata</i> lehel. Ma ei suuda nii pikalt kirjutada, kui vaja oleks.</p>\n<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\">\n<tbody><tr>\n<td style=\"border: none; padding: 0; padding-top: 5\">\n<p align=\"center\"><font face=\"Arial, sans-serif\" style=\"font-size: 30px\">1</font></p></td><td style=\"border: none; padding: 0; padding-left: 10\">\n<p><font face=\"Arial, sans-serif\" style=\"font-size: 30px\">Anton</font></p>\n</td>\n<td style=\"border: none; padding: 0\">\n<p><font face=\"Arial, sans-serif\" style=\"font-size: 30px\">FARFOROVSKI</font></p>\n</td>\n<td style=\"border: none; padding: 0\">\n<p align=\"center\"><font face=\"Arial, sans-serif\" style=\"font-size: 10pt\">1994</font></p>\n</td>\n<br>\n<br>\n<hr>\n<p>Lisaks, liitu meie <em>TASUTA</em> emaili nimekirjaga.</p>\n</body>\n</html>");
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
    if(verbose)
        QTextStream(stdout) << "Protofinaal::updateSpectatorWindow()" << endl;
    spectatorWindow.clearResults();
    if(verbose)
        QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), teams.size() = " << teams.size() << endl;
    foreach (Team *team, teams){
        QVector<Competitor*> teamCompetitors = team->teamCompetitors();
        if(verbose)
            QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), teamCompetitors.size() = " << teamCompetitors.size() << endl;
        if(teamCompetitors.size() == 0)
            return;
//        if(verbose){
//            QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), team->index() = " << team->index() << endl;
//            QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), team->teamName() = " << team->teamName() << endl;
//            QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), teamCompetitors.at(0)->name() = " << teamCompetitors.at(0)->name() << endl;
//            QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), teamCompetitors.at(0)->lastResult() = " << teamCompetitors.at(0)->lastResult() << endl;
//            QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), team->lastSum() = " << team->lastSum() << endl;
//        }
        spectatorWindow.addRow(team->index(), team->teamName(), teamCompetitors.at(0)->name(), teamCompetitors.at(0)->lastResult(), team->lastSum(), "0");
        if(teamCompetitors.size() > 1){
            for(int i = 1; i < teamCompetitors.size(); i++){
                if(verbose)
                    QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), teamCompetitors.at(i)->lastResult() = " << teamCompetitors.at(i)->lastResult() << endl;
                spectatorWindow.addRow("", "", teamCompetitors.at(i)->name(), teamCompetitors.at(i)->lastResult(), "", "");
            }
        }

//        QJsonObject teamJson;
//        team->toJson(teamJson);
//        QJsonArray competitorsArray = teamJson["Members_in_team"].toArray();
//        for(QJsonValue jsonValue : competitorsArray){
//            QJsonObject competitorObj = jsonValue.toObject();
//            spectatorWindow.addRow("1. ", teamJson["teamName"].toString(), competitorObj["nameEdit"].toString(), competitorObj["Sum"].toString(),  competitorObj["Sum"].toString(), "0");
//        }
    }
}

void Protofinaal::writeFinalsFile(QString fileName)
{
    QFile file(fileName);

    if(file.open(QIODevice::WriteOnly)){
        QJsonObject finalsObj;
        toJson(finalsObj);
        finalsObj["fileVersion"] = 300;
        finalsObj["competitionName"] = competitionName;
        finalsObj["eventName"] = eventName;
        finalsObj["timePlace"] = timePlace;
        QJsonDocument jsonDoc(finalsObj);
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

/////////////////////////////////////////////////////////////////////////////
/// ToDo:
/// In progress:
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

QString organization = "Ümeramees";
QString programName = VER_INTERNALNAME_STR;
QString programVersion = VER_PRODUCTVERSION_STR;
extern bool verbose;

Protofinaal::Protofinaal(QString fileName, QWidget* parent)
    : QMainWindow(parent)
    , m_settings("Protofinaal", "Protofinaali conf")
{
    if (m_settings.language().isEmpty()) {
        changeLanguage(true);
    }

    setupTranslator();

    createMenus();
    setStatusBar(statusBar());
    //    QWidget *widget = new QWidget();
    //    widget->setLayout(vBox);
    //    widget->setStyleSheet("border:1px solid rgb(0, 255, 0); ");
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    setCentralWidget(scrollArea);

    QWidget* testWidget = new QWidget;
    testWidget->setLayout(&m_vBox);
    scrollArea->setWidget(testWidget);

    this->setGeometry(9, 36, 1500, 600);
    readSettings();
    if (!fileName.isEmpty())
        m_currentFile = fileName;

    QTimer::singleShot(100, this, SLOT(initialize()));

    if (verbose)
        QTextStream(stdout) << "currentFile = " << m_currentFile << Qt::endl;

    m_logFile = new QFile(QFileInfo(m_currentFile).dir().absolutePath() + QString("/Protofinaal log %1.log").arg(QDate::currentDate().toString(Qt::ISODate)));

    if (m_logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) { // Log file
        m_logOut.setDevice(m_logFile);
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to write log file! Make sure you have write permission to the folder where the competition file is located."), QMessageBox::Ok);
    }
}

Protofinaal::~Protofinaal()
{
}

void Protofinaal::changeLanguage(bool atStartup)
{
    QDir dir(":/languages/");
    QStringList fileNames = dir.entryList(QStringList("*.qm"));
    QStringList languages;
    for (QString language : fileNames) {
        languages.append(language.remove(".qm"));
    }
    bool ok = false;
    QString newLanguage = QInputDialog::getItem(this, "Choose a language", "Language:", languages, languages.indexOf(m_settings.language()), false, &ok);

    if (ok) {
        m_settings.setLanguage(newLanguage);
        m_settings.writeSettings();

        setupTranslator();

        if (!atStartup)
            QMessageBox::information(this, tr("Info"), tr("Language change will be applied after restart"));
    }
}

void Protofinaal::clear()
{
    foreach (TeamsTable* teamsTable, m_teamsTables) {
        teamsTable->clear();
        teamsTable->deleteLater();
    }
    m_teamsTables.clear();
    m_competitionName.clear();
    m_timePlace.clear();
    m_eventName.clear();
    m_eventType.clear();
    m_spectatorWindow.clearResults();
    m_spectatorWindow.clearTargets();
}

void Protofinaal::closeEvent(QCloseEvent* event)
{
    if (verbose)
        QTextStream(stdout) << "Protofinaal::closeEvent()" << Qt::endl;
    if (m_modifiedAfterSave) {
        int reply = QMessageBox::question(this, "Protofinaal", tr("Do you want to save changes and exit?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (reply == QMessageBox::Save) {
            save();
            event->accept();
        } else if (reply == QMessageBox::Cancel) {
            event->ignore();
        } else {
            event->accept();
        }
    } else {
        event->accept();
    }

    if (event->isAccepted()) {
        writeSettings();
        qApp->quit();
    }
}

void Protofinaal::createMenus()
{
    QMenu* fileMenu = this->menuBar()->addMenu(tr("&File"));
    QMenu* editMenu = this->menuBar()->addMenu(tr("&Results"));
    QMenu* languageMenu = this->menuBar()->addMenu(tr("Language"));

    QAction* openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open file"));
    connect(openAct, &QAction::triggered, this, &Protofinaal::open);

    QAction* saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save file"));
    connect(saveAct, &QAction::triggered, this, &Protofinaal::save);

    QAction *eksportXLSAct = new QAction(tr("Export xls..."), this);
    eksportXLSAct->setStatusTip(tr("Export results to .xls file"));
    connect(eksportXLSAct, &QAction::triggered, this, &Protofinaal::eksportXLS);

    QAction* importSiusStartListAct = new QAction(tr("&Import Sius startlist..."), this);
    importSiusStartListAct->setShortcuts(QKeySequence::Open);
    importSiusStartListAct->setStatusTip(tr("Import Sius startlist"));
    connect(importSiusStartListAct, &QAction::triggered, this, &Protofinaal::importSiusStartList);

    QAction* exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit from the program"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    QAction* showSpectatorWindowAct = new QAction(tr("&Spectator View"), this);
    showSpectatorWindowAct->setStatusTip(tr("Open the spectator view"));
    connect(showSpectatorWindowAct, &QAction::triggered, this, &Protofinaal::showSpecatorWindowOnSecondScreen);

    QAction* connectToSiusDataAct = new QAction(tr("&SiusData"), this);
    connectToSiusDataAct->setStatusTip(tr("Connect to SiusData"));
    connect(connectToSiusDataAct, &QAction::triggered, this, &Protofinaal::connectToSiusData);

#ifdef QT_DEBUG
    QAction *deleteAllShotsAct = new QAction(tr("Delete Shots"), this);
    deleteAllShotsAct->setStatusTip(tr("Deletes all shots from selected competitors"));
    connect(deleteAllShotsAct, SIGNAL(triggered()), this, SLOT(deleteAllShots()));
#endif

    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(importSiusStartListAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu->addAction(eksportXLSAct);
    editMenu->addSeparator();
    editMenu->addAction(showSpectatorWindowAct);
    editMenu->addAction(connectToSiusDataAct);

    QAction* changeLanguageAct = new QAction(tr("Language"), this);
    changeLanguageAct->setStatusTip(tr("Change application language"));
    connect(changeLanguageAct, &QAction::triggered, [this]() { changeLanguage(false); });

    languageMenu->addAction(changeLanguageAct);

#ifdef QT_DEBUG
    QMenu *testMenu = this->menuBar()->addMenu(tr("T&esting"));
    testMenu->addAction(deleteAllShotsAct);
#endif
}

void Protofinaal::connectToSiusData()
{
    m_siusLog = new QFile(QFileInfo(m_currentFile).dir().absolutePath() + QString(tr("/Protofinaal incoming log %1.log")).arg(QDate::currentDate().toString(Qt::ISODate)));

    if (m_siusDataConnections == nullptr) {
        m_siusDataConnections = new SiusDataConnections(m_siusLog, &m_logOut, &m_settings, this);
        connect(m_siusDataConnections, &SiusDataConnections::statusInfo, this, &Protofinaal::statusBarInfoChanged);
        foreach (TeamsTable* teamsTable, m_teamsTables) {
            connect(m_siusDataConnections, &SiusDataConnections::shotRead, teamsTable, &TeamsTable::readSiusInfo);
        }
        connect(m_siusDataConnections, &SiusDataConnections::disconnectedFromSius, this, &Protofinaal::connectionToSiusLost);
    }

    if (m_siusLog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) { // Saabunud võrguliikluse logi
        QTextStream valja(m_siusLog);
        valja << "///////////////////////////////" << m_competitionName << ", " << QDateTime::currentDateTime().toString() << "///////////////////////////////\n";
        m_siusLog->close();
    }

    m_siusDataConnections->exec();
}

void Protofinaal::connectionToSiusLost(int connectionIndex)
{
    Q_UNUSED(connectionIndex);
    // TODO To be implemented
}

void Protofinaal::deleteAllShots()
{
    foreach (TeamsTable *teamsTable, m_teamsTables) {
        teamsTable->deleteAllShots();
    }
    updateSpectatorWindow();
    m_modifiedAfterSave = true;
}

void Protofinaal::eksportXLS()
{
    QString fileLocation = m_currentFile.left(m_currentFile.length() - 4);
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export"), fileLocation + ".xls",
                    tr("Excel workbook file (*.xls)"));
    if (fileName.isEmpty()) {
        return;
    }
    if (!fileName.endsWith(".xls")) {
        fileName.append(".xls");
    }

    XlsExportData exportData = buildXlsExportData(m_teamsTables);

    XLSExportService exporter;
    QString errorMessage;
    if (exporter.exportResults(fileName, m_competitionName, m_timePlace, m_eventName, exportData.maxShots, exportData.blocks, &errorMessage)) {
        statusBarInfoChanged(tr("File exported: ") + fileName);
    } else {
        QMessageBox::critical(this, tr("Error!"), errorMessage.isEmpty() ? tr("Failed to write file!") : errorMessage, QMessageBox::Ok);
    }
}

void Protofinaal::importSiusStartList()
{
    QString fileName = m_initialDialog->fileName().left(QDir::fromNativeSeparators(m_initialDialog->fileName()).lastIndexOf("/"));
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open startlist"), fileName, tr("Comma separated file (*.csv)"));
    if (filePath.isEmpty())
        return;

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QStringList rows;
        while (!in.atEnd()) { // Read the whole file in
            QString newRow = in.readLine();
            int newTarget = newRow.split(';').at(10).toInt();
            bool wasAdded = false;
            if (rows.size() > 0) {
                for (int i = 0; i < rows.size(); i++) {
                    int currentTarget = rows.at(i).split(';').at(10).toInt();
                    if (newTarget < currentTarget) {
                        rows.insert(i, newRow);
                        wasAdded = true;
                        break;
                    }
                }
            }
            if (!wasAdded)
                rows.append(newRow);
        }

        foreach (TeamsTable* teamsTable, m_teamsTables) {
            QStringList forCurrentTable;
            for (int i = 0; i < teamsTable->teamsCount(); i++)
                if (!rows.isEmpty())
                    forCurrentTable.append(rows.takeFirst());
            teamsTable->setCompetitiorsData(forCurrentTable);
        }
    } else
        QMessageBox::critical(this, tr("Error"), tr("Failed to open the file!"), QMessageBox::Ok);
}

void Protofinaal::initialize()
{
    if (verbose)
        QTextStream(stdout) << "initialize" << Qt::endl;

    if (m_initialDialog == nullptr) {
        m_initialDialog = new InitialDialog(this);
        connect(m_initialDialog, &InitialDialog::updateMe, this, &Protofinaal::updateInitialDialog);
    }

    QJsonObject initialJson = readFinalsFile(m_currentFile, false);
    m_initialDialog->setFileName(m_currentFile);
    m_initialDialog->setCompetitionName(initialJson["competitionName"].toString());
    m_initialDialog->setTimePlace(initialJson["timePlace"].toString());
    m_initialDialog->setEventName(initialJson["eventName"].toString());
    m_initialDialog->setEventType(initialJson["eventType"].toString());

    if (m_initialDialog->exec() == QDialog::Accepted) {
        writeSettings();
        m_currentFile = m_initialDialog->fileName();
        QFile testOpenFile(m_initialDialog->fileName());
        if (testOpenFile.open(QIODevice::ReadOnly)) { // Check if file exists
            if (verbose)
                QTextStream(stdout) << "File exists: " << m_initialDialog->fileName() << Qt::endl;
            testOpenFile.close();
            loadFile(m_currentFile);
        } else { // File does not exist
            if (verbose)
                QTextStream(stdout) << "Create new file: " << m_initialDialog->fileName() << Qt::endl;

            m_competitionName = m_initialDialog->competitionName();
            m_timePlace = m_initialDialog->timePlace();
            m_eventName = m_initialDialog->eventName();
            m_eventType = m_initialDialog->eventType();

            createLayoutFromConf(m_initialDialog->eventConf());
        }

        m_logOut << "///////////////////////////////" << m_competitionName << ", " << QDateTime::currentDateTime().toString() << "///////////////////////////////\n";
        statusBarInfoChanged(tr("Opened file: ") + m_currentFile);
        showSpecatorWindowOnSecondScreen();
    } else if (m_initialDialog->result() == QDialog::Rejected)
        QCoreApplication::quit();

    setWindowTitle(programName + " - " + programVersion + " - " + m_currentFile + " - " + m_competitionName + " - " + m_eventName);
}

void Protofinaal::loadFile(QString fileName)
{
    if (verbose)
        QTextStream(stdout) << "Protofinaal::loadFile()" << Qt::endl;
    clear();
    QJsonObject jsonObj = readFinalsFile(fileName);

    if (!(jsonObj.contains("competitionName") && jsonObj["competitionName"].isString()) || !(jsonObj.contains("eventName") && jsonObj["eventName"].isString()) || !(jsonObj.contains("eventType") && jsonObj["eventType"].isString()) || !(jsonObj.contains("timePlace") && jsonObj["timePlace"].isString()) || !(jsonObj.contains("fileVersion") && jsonObj["fileVersion"].isDouble()))
        QMessageBox::critical(this, tr("Error!"), tr("Finals file is broken!"));

    m_eventType = jsonObj["eventType"].toString();

    if (jsonObj.contains("startList") && jsonObj["startList"].isArray() && jsonObj.contains("eventConf") && jsonObj["eventConf"].isObject()) {
        createLayoutFromConf(jsonObj["eventConf"].toObject());
        QJsonArray startListArray = jsonObj["startList"].toArray();

        foreach (TeamsTable* teamsTable, m_teamsTables) {
            for (int i = 0; i < teamsTable->teamsCount(); i++)
                if (!startListArray.isEmpty()) {
                    QJsonArray data = startListArray.takeAt(0).toArray();
                    teamsTable->setCompetitiorsData(i, data);
                }
        }

        initializeSpectatorTargets();
    } else if (jsonObj.contains("relays") && jsonObj["relays"].isArray()) {

        m_scoringWithPoints = false;
        if (jsonObj.contains("scoringWithPoints"))
            m_scoringWithPoints = jsonObj["scoringWithPoints"].toBool();

        if (verbose)
            QTextStream(stdout) << "Protofinaal::loadFile(), scoringWithPoints: " << m_scoringWithPoints << Qt::endl;

        QJsonArray relaysArray = jsonObj["relays"].toArray();
        int noOfRelays = relaysArray.size();
        int relayNo = 1;
        for (const QJsonValue& relayJson : std::as_const(relaysArray)) {
            TeamsTable* m_teamsTable = new TeamsTable();
            m_vBox.addWidget(m_teamsTable);
            if (noOfRelays > 1)
                m_teamsTable->setTableName(QString("Group %1").arg(relayNo++));

            connect(m_teamsTable, &TeamsTable::updateSpectatorWindow, this, &Protofinaal::updateSpectatorWindow);
            connect(m_teamsTable, &TeamsTable::modified, [this]() { m_modifiedAfterSave = true; });
            connect(m_teamsTable, &TeamsTable::shotReadForTarget, this, &Protofinaal::updateTargetWithShot);
            m_teamsTables.append(m_teamsTable);
            m_teamsTable->createLayout(relayJson.toObject(), m_scoringWithPoints);
        }

        m_modifiedAfterSave = false;

        initializeSpectatorTargets();
    } else
        QMessageBox::critical(this, tr("Error!"), tr("Finals file is broken!"));
}

void Protofinaal::open()
{
    if (m_modifiedAfterSave) {
        int reply = QMessageBox::question(this, "Protofinaal", tr("Do you want to save changes?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (reply == QMessageBox::Save) {
            save();
        } else if (reply == QMessageBox::Cancel) {
            return;
        }
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), m_currentFile, tr("Protofinaal file (*.fin)"));
    if (!fileName.isEmpty()) {
        m_currentFile = fileName;
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

    if (file.open(QIODevice::ReadOnly)) {
        fileJson = QJsonDocument::fromJson(file.readAll());
        jsonObj = fileJson.object();

        if (jsonObj["fileVersion"].toInt() > 301)
            QMessageBox::warning(this, tr("Error!"), tr("File version is newer than this version of the program. Errors may occur when opening the file!"), QMessageBox::Ok);

        m_competitionName = jsonObj["competitionName"].toString();
        m_eventName = jsonObj["eventName"].toString();
        m_timePlace = jsonObj["timePlace"].toString();

        m_scoringWithPoints = false;
        if (jsonObj.contains("scoringWithPoints"))
            m_scoringWithPoints = jsonObj["scoringWithPoints"].toBool();
    } else if (showErrors)
        QMessageBox::critical(this, tr("Error!"), tr("Cannot open the file!\n\nLocation: ") + file.fileName(), QMessageBox::Ok);
    return jsonObj;
}

void Protofinaal::readSettings()
{
    m_currentFile = m_settings.lastOpenFileName();
    this->setGeometry(m_settings.windowXLocation() + 9, m_settings.windowYLocation() + 36, 1200, 600);
}

// void Protofinaal::readSiusInfo(SiusShotData shotData)
//{
//     if(verbose)
//         QTextStream(stdout) << "readSiusInfo()" << Qt::endl;

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
////            if(row.startsWith("_SHOT") && !m_competitionStarted){   //Shot data, sighting shots
////                    thisCompetitor->setPreviousSiusRow(row);

////                    statusBarInfoChanged(rowParts[3] + tr(": Proovilask"));

////                    Lask lask(row);
////                    if(verbose)
////                        QTextStream(stdout) << "rowParts[3]: " << rowParts[3] << ", rida: " << row;
////                }else if(row.startsWith("_TOTL") && !m_competitionStarted){   //Result row
////                    if(rowParts.count() > 12){  //If row is not long enough, then probably it is total result, which is not interesting here
////                        if(rowParts[12] != "0"){   //If it is sighting shot, then this is 0, but if competition shot, then it is series result
////                            if(thisCompetitor->previousSiusRow().startsWith("_SHOT")){  //As previous shot was already competition shot, then it needs to be added to someone
////                                m_competitionStarted = true;
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
////                }else if(row.startsWith("_SHOT") && m_competitionStarted){   //Võistluslasu rida

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
    writeFinalsFile(m_currentFile);
}

void Protofinaal::createLayoutFromConf(QJsonObject conf)
{
    if (!(conf.contains("event") && conf["event"].isString()) || !(conf.contains("relaysTogether") && conf["relaysTogether"].isDouble()) || !(conf.contains("teams") && conf["teams"].isDouble()) || !(conf.contains("membersInTeam") && conf["membersInTeam"].isDouble()) || !(conf.contains("shots") && conf["shots"].isArray()) || !(conf.contains("scoringWithPoints") && conf["scoringWithPoints"].isBool()))
        QMessageBox::critical(this, tr("Error!"), tr("Event file is broken!"));

    m_scoringWithPoints = false;
    if (conf.contains("scoringWithPoints") && conf["scoringWithPoints"].isBool()) {
        m_scoringWithPoints = conf["scoringWithPoints"].toBool();
    }

    int numberOfRelaysTogether = conf["relaysTogether"].toDouble();
    for (int relay = 1; relay <= numberOfRelaysTogether; relay++) {
        TeamsTable* m_teamsTable = new TeamsTable();
        m_vBox.addWidget(m_teamsTable);
        if (numberOfRelaysTogether > 1)
            m_teamsTable->setTableName(QString("Group %1").arg(relay));

        connect(m_teamsTable, &TeamsTable::updateSpectatorWindow, this, &Protofinaal::updateSpectatorWindow);
        connect(m_teamsTable, &TeamsTable::modified, [this]() { m_modifiedAfterSave = true; });
        connect(m_teamsTable, &TeamsTable::shotReadForTarget, this, &Protofinaal::updateTargetWithShot);
        m_teamsTables.append(m_teamsTable);
        m_teamsTable->createLayout(conf, m_scoringWithPoints);
    }
}

void Protofinaal::setupTranslator()
{
    qApp->removeTranslator(&m_translator);

    bool loadResult = m_translator.load(m_settings.language(), ":/languages");
    if (!loadResult) {
        QTextStream(stdout) << "Translation file for " << m_settings.language() << "loading failed!, working dir: " << QDir::currentPath() << Qt::endl;
    }
    qApp->installTranslator(&m_translator);
}

void Protofinaal::showSpecatorWindowOnSecondScreen() // FIXME To be reimplemented
{
    //    if(QGuiApplication::screens().size() >= 2 /*&& qApp->desktop()->isVirtualDesktop()*/){
    //        m_spectatorWindow.move(QGuiApplication::primaryScreen()->screenGeometry(this).width() + 100, 100);
    //        if(qApp->desktop()->screenNumber(this) != qApp->desktop()->screenNumber(&m_spectatorWindow) && qApp->desktop()->screenNumber(&m_spectatorWindow) != -1)
    //            m_spectatorWindow.showFullScreen();
    //        else{
    //            m_spectatorWindow.move(-1000, 100);
    //            if(qApp->desktop()->screenNumber(this) != qApp->desktop()->screenNumber(&m_spectatorWindow))
    //                m_spectatorWindow.showFullScreen();
    //        }
    //    }else{

    m_spectatorWindow.show();
    //    }

    QList<QScreen*> screens = QGuiApplication::screens();

    if (screens.size() >= 2) {
        QScreen* currentScreen = QGuiApplication::screenAt(this->geometry().center());
        int currentIndex = screens.indexOf(currentScreen);
        QScreen* otherScreen = screens.at((currentIndex + 1) % screens.size());
        m_spectatorWindow.setScreen(otherScreen);
        m_spectatorWindow.move(otherScreen->geometry().center() - m_spectatorWindow.rect().center());
        m_spectatorWindow.showFullScreen();
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Second screen not found. For the program to work properly,"
                                                    " two screens are required."),
            QMessageBox::Ok);
        m_spectatorWindow.show();
    }

    updateSpectatorWindow();

    if (m_spectatorWindow.isFullScreen())
        QMessageBox::information(this, tr("Info"), tr("Spectator view shown on the second screen"), QMessageBox::Ok);
}

void Protofinaal::statusBarInfoChanged(QString newStatusInfo)
{
    statusBar()->showMessage(newStatusInfo, 5000);
    m_logOut << QTime::currentTime().toString() << " " << newStatusInfo << Qt::endl;
}

// void Protofinaal::sumAllTeams()
//{
//     foreach (Team *team, teams) {
//         team->sumAll();
//     }
// }

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
    foreach (TeamsTable* teamsTable, m_teamsTables) {
        relaysArray.append(teamsTable->toJson());
    }

    QJsonObject finalsObj; // = m_teamsTable->toJson();
    finalsObj["relays"] = relaysArray;
    finalsObj["fileVersion"] = 301;
    finalsObj["competitionName"] = m_competitionName;
    finalsObj["eventName"] = m_eventName;
    finalsObj["eventType"] = m_eventType;
    finalsObj["timePlace"] = m_timePlace;
    finalsObj["scoringWithPoints"] = m_scoringWithPoints;
    return finalsObj;
}

void Protofinaal::updateInitialDialog()
{
    m_currentFile = m_initialDialog->fileName();
    QJsonObject initialJson = readFinalsFile(m_currentFile, false);
    m_initialDialog->setFileName(m_currentFile);
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
    if (verbose)
        QTextStream(stdout) << "Protofinaal::updateSpectatorWindow()" << Qt::endl;

    m_spectatorWindow.clearResults();

    // Update shots on existing targets
    for (int i = 0; i < m_teamsTables.size(); i++) {
        foreach (Team* team, m_teamsTables.at(i)->getTeams()) {
            foreach (Competitor* competitor, team->teamCompetitors()) {
                if (competitor->isActive()) {
                    QString targetNo = QString::number(competitor->id());

                    // Reset target before redrawing all shots
                    if (m_spectatorWindow.hasTarget(targetNo)) {
                        m_spectatorWindow.resetTarget(targetNo);
                    }

                    // // Draw existing shots on the target
                    // int lastShot = competitor->lastValidShotIndex();
                    // for (int shotIdx = 0; shotIdx <= lastShot; shotIdx++)
                    // {
                    //     auto shot = competitor->shotAt(shotIdx);
                    //     if (shot.has_value())
                    //     {
                    //         m_spectatorWindow.updateTarget(targetNo, shot.value());
                    //     }
                    // }
                }
            }
        }
    }

    int shotNo = m_teamsTables.first()->lastValidShotIndex() + 1;
    QString shotHeader = tr("Shot");
    if (shotNo > 0)
        shotHeader.prepend(QString("%1. ").arg(shotNo));
    if (m_scoringWithPoints)
        m_spectatorWindow.setHeading(m_competitionName, m_timePlace, m_eventName, tr("Place"), tr("Name"), shotHeader, tr("Points"), tr("Diff"));
    else
        m_spectatorWindow.setHeading(m_competitionName, m_timePlace, m_eventName, tr("Place"), tr("Name"), shotHeader, tr("Series"), tr("Diff"));

    for (int i = m_teamsTables.size() - 1; i >= 0; i--) {
        m_spectatorWindow.addRow("", "", "", "", "", "", ""); // To add some spacing between different groups
        m_spectatorWindow.addRow("", "", "", "", "", "", "");
        m_spectatorWindow.addRow("", "", "", "", "", "", "");
        //    foreach(TeamsTable *m_teamsTable, m_teamsTables) {
        auto results = m_teamsTables.at(i)->getSortedResults().values();

        int index = results.size();
        foreach (auto currentResult, results) {
            QString diff = "";
            if (index - 2 >= 0) {
                QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), total10Score = " << results.at(index - 2).total10Score << ", total10Score = " << currentResult.total10Score << Qt::endl;
                diff = QString("%1").arg(results.at(index - 2).total10Score - currentResult.total10Score);
                if (diff.length() > 1)
                    diff.insert(diff.length() - 2, ",");
                else
                    diff.prepend("0,");
                QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), diff = " << diff << Qt::endl;
            }
            m_spectatorWindow.addRow(
                QString("%1.").arg(index--),
                m_teamsTables.at(i)->tableName(), // TODO Do we need this?
                currentResult.name,
                currentResult.shotValue,
                currentResult.seriesOrPoints,
                currentResult.totalScore,
                diff);
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
    //        QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), teams.size() = " << teams.size() << Qt::endl;
    //    foreach (Team *team, teams){
    //        QVector<Competitor*> teamCompetitors = team->teamCompetitors();
    //        if(verbose)
    //            QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), teamCompetitors.size() = " << teamCompetitors.size() << Qt::endl;
    //        if(teamCompetitors.size() == 0)
    //            return;
    ////        if(verbose){
    ////            QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), team->index() = " << team->index() << Qt::endl;
    ////            QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), team->teamName() = " << team->teamName() << Qt::endl;
    ////            QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), teamCompetitors.at(0)->name() = " << teamCompetitors.at(0)->name() << Qt::endl;
    ////            QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), teamCompetitors.at(0)->lastResult() = " << teamCompetitors.at(0)->lastResult() << Qt::endl;
    ////            QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), team->lastSum() = " << team->lastSum() << Qt::endl;
    ////        }
    //        spectatorWindow.addRow(team->index(), team->teamName(), teamCompetitors.at(0)->name(), teamCompetitors.at(0)->lastResult(), team->lastSum(), "0");
    //        if(teamCompetitors.size() > 1){
    //            for(int i = 1; i < teamCompetitors.size(); i++){
    //                if(verbose)
    //                    QTextStream(stdout) << "Protofinaal::updateSpectatorWindow(), teamCompetitors.at(i)->lastResult() = " << teamCompetitors.at(i)->lastResult() << Qt::endl;
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

void Protofinaal::updateTargetWithShot(char targetLetter, SiusShotData shotData)
{
    QString targetNo = QString(QChar(targetLetter));
    m_spectatorWindow.updateTarget(targetNo, shotData.shot);
}

void Protofinaal::writeFinalsFile(QString fileName)
{
    QFile file(fileName);

    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument jsonDoc(toJson());
        file.write(jsonDoc.toJson());
        statusBar()->showMessage(tr("File saved"), 5000);
        m_modifiedAfterSave = false;
    } else
        QMessageBox::critical(this, tr("Error!"), tr("Cannot write to file!\nMake sure you have write permission to that folder."), QMessageBox::Ok);
}

void Protofinaal::writeSettings()
{
    m_settings.setLastOpenFileName(m_currentFile);
    m_settings.setWindowXLocation(this->x());
    m_settings.setWindowYLocation(this->y());

    m_settings.writeSettings();
}

void Protofinaal::initializeSpectatorTargets()
{
    if (verbose)
        QTextStream(stdout) << "Protofinaal::initializeSpectatorTargets()" << Qt::endl;

    m_spectatorWindow.clearTargets();

    QVector<QPair<QString, QPair<QString, int>>> targetData;
    int gunType = getGunTypeFromEventType();

    char targetLetter = 65;
    for (int i = 0; i < m_teamsTables.size(); i++) {
        foreach (Team* team, m_teamsTables.at(i)->getTeams()) {
            foreach (Competitor* competitor, team->teamCompetitors()) {
                if (competitor->isActive()) {
                    competitor->setTargetLetter(targetLetter);
                    QString targetNo = QString(QChar(targetLetter++));
                    QString name = competitor->name();
                    m_spectatorWindow.addTarget(name, targetNo, gunType);
                }
            }
        }
    }
}

int Protofinaal::getGunTypeFromEventType() const
{
    // Convert event type string to gun type
    // 0 = Air Rifle (Õhupüss)
    // 1 = Air Pistol (Õhupüstol)
    // 2 = Sport Rifle (Sportpüss)

    if (m_eventType.contains("püss", Qt::CaseInsensitive) || m_eventType.contains("Rifle", Qt::CaseInsensitive) || m_eventType.contains("AR40") || m_eventType.contains("AR60")) {
        return 0; // Air Rifle
    } else if (m_eventType.contains("püstol", Qt::CaseInsensitive) || m_eventType.contains("Pistol", Qt::CaseInsensitive) || m_eventType.contains("AP40") || m_eventType.contains("AP60")) {
        return 1; // Air Pistol
    } else if (m_eventType.contains("Sport", Qt::CaseInsensitive) || m_eventType.contains("SB_", Qt::CaseInsensitive) || m_eventType.contains("PRONE") || m_eventType.contains("Lamades")) {
        return 2; // Sport Rifle
    }

    // Default to Air Rifle
    return 0;
}

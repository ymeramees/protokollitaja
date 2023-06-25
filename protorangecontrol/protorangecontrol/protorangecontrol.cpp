#include "protorangecontrol.h"

/////////////////////////////////////////////////////////////////////////////
/// ToDo list:
/// Pooleli:
/// 1. Võistluslaskude numeratsioon algab ikkagi 0'ist, ei ole vaja üle nummerdada (vaja testida - ei tööta)
/// 2. Kui mõni lask vahelt puudu on, siis võiks automaatselt kõik lasud uuesti küsida
/// 3. Komakohtade seadistamine tööriistade menüüst
/// 4. Mis saab, kui programm kokku jookseb? (Protokollitajasse ei tulnud loetud lasud üle (teha erand Inbandist tulnud laskude üle lugemiseks))
/// 5. Näidata jooksvat aega
/// 6. Näidata veateadet, kui on eriti kauge 0?
///
/////////////////////////////////////////////////////////////////////////////

QString organization = "Ümeramees";
QString programName = VER_INTERNALNAME_STR;
QString programVersion = VER_PRODUCTVERSION_STR;
extern bool verbose;

ProtoRangeControl::ProtoRangeControl(QString language, QWidget *parent)
    : QMainWindow(parent), m_server(&m_incomingLog), m_language(language)
{
    readSettings();

    if (m_language.isEmpty()) {
        changeLanguage();
//        QDir dir(":/languages/");
//        QStringList fileNames = dir.entryList(QStringList("*.qm"));
//        QStringList languages;
//        for (QString language : fileNames) {
//            languages.append(language.remove(".qm"));
//        }
//        m_language = QInputDialog::getItem(this, "Choose a language", "Language:", languages, 0, false);
    }

    QTranslator translator;
    bool loadResult = translator.load(m_language, ":/languages");
    if(!loadResult) {
        QTextStream(stdout) << "Translation file loading failed!, working dir: " << QDir::currentPath() << Qt::endl;
    }
    qApp->installTranslator(&translator);

    createMenus();
    setStatusBar(statusBar());

    vBox = new QVBoxLayout;
//    vBox->setSpacing(1);
    QWidget *widget = new QWidget();
    widget->setLayout(vBox);
    QScrollArea *area = new QScrollArea;
    area->setWidgetResizable(true);
    area->setWidget(widget);
//    widget->setStyleSheet("border:1px solid rgb(0, 255, 0); ");
    setCentralWidget(area);

    this->setGeometry(9, 36, 1000, 600);
    setWindowTitle(programName + " - " + programVersion);

    initialize();
}

ProtoRangeControl::~ProtoRangeControl()
{
}

Lane* ProtoRangeControl::addLane(int targetNo, QString ip)
{
    QTextStream(stdout) << "ProtoRangeControl::addLane, targetNo = " << targetNo << Qt::endl;
    Lane *lane = new Lane(targetNo, m_disciplines, ip);
    connect(lane, &Lane::commandIssued, this, &ProtoRangeControl::sendInit);
    m_lanes.insert(lane->target().toInt(), lane);
    foreach (Lane* lane, m_lanes) { // Remove all lanes
        vBox->removeWidget(lane);
    }
    foreach (Lane* lane, m_lanes) { // Add all lanes in sorted order
        vBox->addWidget(lane);
    }
    return lane;
}

void ProtoRangeControl::addShot(SiusShotData shotData)
{
    m_shots.append(shotData);
}

void ProtoRangeControl::allShotsDataReceived(int target, QString shotsData)
{
    bool accepted = false;
    QStringList msgParts = shotsData.split('\n');
    QStringList shotsList;
    for (int i = 2; i < (msgParts.count() - 1); i += 3) {  // First 2 parts and last are not related to shots
        shotsList.append(QString("%1;%2;%3").arg(msgParts.at(i), msgParts.at(i + 1), msgParts.at(i + 2)));
    }

//    int firstCompetitionShotNo = QInputDialog::getInt(
//                this,
//                "Mitmendast lasust on võistluslasud?",
//                QString("Rada: %1\n%2\n%3").arg(msgParts.first(), shotsList.join('\n'), "Esimese võistluslasu nr: "),
//                1,
//                1,
//                120,
//                1,
//                &accepted
//                );
//    if (accepted) {
        auto laneOpt = findLane(QString("%1").arg(target));
        if (laneOpt) {
//            laneOpt.value()->setCurrentShotIndex(0);
            int id = laneOpt.value()->id().toInt();    // TODO make it safer
            int shotNo = 1;
//            for (int i = --firstCompetitionShotNo; i < shotsList.size(); i++) {
            foreach (QString row, shotsList) {
                // lane, "shot", value, index, x (in mm?), y (in mm?), "message end"
//                QStringList inbandList = shotsList.at(i).split(';');
                QStringList inbandList = row.split(';');
                inbandList.prepend("shot");
                inbandList.prepend("target");
                inbandList.append("message end");
                inbandList.insert(3, QString("%1").arg(shotNo));
                Lask shot = Lask::fromInband(inbandList);

                Lask::TargetType targetType;
                if (laneOpt.value()->discipline().compare("airrifle", Qt::CaseInsensitive) == 0) {
                    targetType = Lask::Ohupuss;
                } else if (laneOpt.value()->discipline().compare("airpistol", Qt::CaseInsensitive) == 0) {
                    targetType = Lask::Ohupustol;
                } else
                    targetType = Lask::Muu;

                if (targetType != Lask::Muu)
                    shot.setInnerTen(Lask::calcIfInnerTen(targetType, shot.X(), shot.Y()));
                shot.setCompetitionShot(true);

                SiusShotData shotData;
                shotData.id = id;
                shotData.shot = shot;
                shotData.siusShotNo = shotNo++;
//                shotData.siusShotNo = laneOpt.value()->increaseAndGetCurrentShotIndex();
                addShot(shotData);  // Save only competition shots

                laneOpt.value()->setLastShotLabel(QString("(%1.): %2").arg(shotData.siusShotNo).arg(shotData.shot.getSLask()));
                publishShot(shotData);

                QJsonObject shotLogJson;
                shotLogJson["time"] = QTime::currentTime().toString();
                shotLogJson["id"] = shotData.id;
                shotLogJson["shotNo"] = shotData.siusShotNo;
                shotLogJson["shot"] = shotData.shot.toJson();
                *m_shotsLog << QJsonDocument(shotLogJson).toJson(QJsonDocument::Compact) << Qt::endl;
            }
        }
//    }
}

void ProtoRangeControl::changeLanguage()
{
    QDir dir(":/languages/");
    QStringList fileNames = dir.entryList(QStringList("*.qm"));
    QStringList languages;
    for (QString language : fileNames) {
        languages.append(language.remove(".qm"));
    }
    bool ok = false;
    QString newLanguage = QInputDialog::getItem(this, "Choose a language", "Language:", languages, languages.indexOf(m_language), false, &ok);

    if (ok) {
        m_language = newLanguage;

        qApp->removeTranslator(&m_translator);

        bool loadResult = m_translator.load(m_language, ":/languages");
        if(!loadResult) {
            QTextStream(stdout) << "Translation file loading failed!, working dir: " << QDir::currentPath() << Qt::endl;
        }
        qApp->installTranslator(&m_translator);

        saveSettings();
        QMessageBox::information(this, tr("Teade"), tr("Keele vahetus rakendub programmi uuesti käivitamisel"));
    }
}

void ProtoRangeControl::continueSendingMessage()
{
    int noSelected = 0;
    foreach (Lane* lane, m_lanes) {
        if (lane->selected())
            noSelected++;
    }

    static QVector<Lane*> sentLanes;
    foreach (Lane* lane, m_lanes) {
        if (lane->selected() && !sentLanes.contains(lane)) {
            bool canSend = true;
            if (m_message == "reset" && lane->inCompetition()) {
                if (QMessageBox::warning(
                            this,
                            tr("Võistlus käimas!"),
                            tr("Rajal %1 on võistlus veel käimas, oled kindel, et tahad seda nullida?").arg(lane->target()),
                            QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
                    canSend = false;
            }
            if (canSend) {
                if (m_message == "reset")
                    lane->setCurrentShotIndex(0);
                sendMessage(lane, m_message);
            }
            sentLanes.append(lane);
            break;
        }
    }
    if (sentLanes.size() < noSelected) {
        QTimer::singleShot(50, this, SLOT(continueSendingMessage()));
    } else {
        sentLanes.clear();
        m_message.clear();
    }
}

QStringList ProtoRangeControl::convertToSiusRows(SiusShotData shotData)
{
    QStringList rowWithTotal;
    int shotType = 0;   // Smallbore rifle competition shot
    if (!shotData.shot.isCompetitionShot()) {
        if (shotData.shot.isInnerTen())
            shotType = 544;  // Smallbore rifle sighting shot inner ten
        else
            shotType = 32;  // Smallbore rifle sighting shot
    } else if (shotData.shot.isInnerTen()) {
        shotType = 512; // Smallbore rifle competition shot inner ten
    }
    rowWithTotal.append(
                QString("_SHOT;0;0;%1;60;74;%2;3;31;%3;%4;0;0;%5;%6;%7;900;0;0;655.35;2958939934;61;449;0")
                .arg(shotData.id)
                .arg(shotData.shot.shotTime().toString())
                .arg(shotType)
                .arg(shotData.shot.get10Lask())
                .arg(shotData.siusShotNo)
                .arg(shotData.shot.stringMX(), shotData.shot.stringMY())
                );
    int shotInTotal = shotData.shot.get10Lask();
    QString decimalShotInTotal = shotData.shot.getSLask();
    if (!shotData.shot.isCompetitionShot()) {   // In case of sighting shots, total must be 0
        shotInTotal = 0;
        decimalShotInTotal = "0";
    }
    rowWithTotal.append(
                QString("_TOTL;0;0;%1;103;T;%2;0;Q;0;0;S;%2;0;P;%2;0;G;%2;0;G;%2;0;G;%2;0;G;%2;0;G;%2;0;G;%3;0;")
                .arg(shotData.id)
                .arg(shotInTotal)
                .arg(decimalShotInTotal)
                );

    return rowWithTotal;
}

void ProtoRangeControl::clearLanes()
{
    foreach (Lane *lane, m_lanes) {
        if (lane->selected())
            lane->clear();
    }
}

void ProtoRangeControl::createMenus()
{
    QMenu *fileMenu = this->menuBar()->addMenu(tr("&Fail"));
    QMenu *toolsMenu = this->menuBar()->addMenu(tr("&Tööriistad"));
    QMenu *controlMenu = this->menuBar()->addMenu(tr("&Käsklused"));
    QMenu *languageMenu = this->menuBar()->addMenu(tr("Keel"));

    QAction *addLaneAct = new QAction(tr("&Lisa rada"), this);
    addLaneAct->setStatusTip(tr("Lisab ühe raja rea"));
    connect(addLaneAct, &QAction::triggered, [this]() {
        addLane(0, "");
    });

    QAction *importStartListAct = new QAction(tr("Import startlist..."), this);
    importStartListAct->setStatusTip(tr("Impordi startlisti fail"));
    connect(importStartListAct, &QAction::triggered, this, &ProtoRangeControl::importStartList);

//    QAction *openAct = new QAction(tr("&Ava..."), this);
//    openAct->setShortcuts(QKeySequence::Open);
//    openAct->setStatusTip(tr("Ava fail"));
//    connect(openAct, &QAction::triggered, this, &Protofinaal::open);

    QAction *exitAct = new QAction(tr("&Välju"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Välju programmist"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

//    fileMenu->addAction(openAct);
    fileMenu->addAction(addLaneAct);
    fileMenu->addAction(importStartListAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    QAction *setCheckedAllAct = new QAction(tr("Märgi kõik"), this);
    setCheckedAllAct->setStatusTip(tr("Lisab linnukesed kõigile radadele"));
    connect(setCheckedAllAct, &QAction::triggered, this, &ProtoRangeControl::setCheckedAll);

    QAction *unsetCheckedAllAct = new QAction(tr("Eemalda märgistused"), this);
    unsetCheckedAllAct->setStatusTip(tr("Eemaldab linnukesed kõigilt radadelt"));
    connect(unsetCheckedAllAct, &QAction::triggered, this, &ProtoRangeControl::unsetCheckedAll);

    QAction *setCheckedTargetTypeAct = new QAction(tr("Märklehe tüüp"), this);
    setCheckedTargetTypeAct->setStatusTip(tr("Muudab kõigil märgitud radadel märklehe tüüpi"));
    connect(setCheckedTargetTypeAct, &QAction::triggered, this, &ProtoRangeControl::setTargetTypes);

    QAction *setCheckedShotsAct = new QAction(tr("Laskude arv"), this);
    setCheckedShotsAct->setStatusTip(tr("Muudab kõigil märgitud radadel võistluslaskude arvu"));
    connect(setCheckedShotsAct, &QAction::triggered, this, &ProtoRangeControl::setNumberOfShots);

    QAction *clearLanesAct = new QAction(tr("Tühjenda rajad"), this);
    clearLanesAct->setStatusTip(tr("Eemaldab kõigilt märgitud radadelt laskurid"));
    connect(clearLanesAct, &QAction::triggered, this, &ProtoRangeControl::clearLanes);

    QAction *saveSettingsAct = new QAction(tr("Salvesta seaded"), this);
    saveSettingsAct->setStatusTip(tr("Salvestab praegused seaded ja rajad"));
    connect(saveSettingsAct, &QAction::triggered, this, &ProtoRangeControl::saveSettings);

    toolsMenu->addAction(setCheckedAllAct);
    toolsMenu->addAction(unsetCheckedAllAct);
    toolsMenu->addSeparator();
    toolsMenu->addAction(setCheckedTargetTypeAct);
    toolsMenu->addAction(setCheckedShotsAct);
    toolsMenu->addSeparator();
    toolsMenu->addAction(clearLanesAct);
    toolsMenu->addAction(saveSettingsAct);

    QAction *initSelectedSightersAct = new QAction(tr("Saada nimed (init)"), this);
    initSelectedSightersAct->setStatusTip(tr("Saadab võistlejate andmed märgitud radadele"));
    connect(initSelectedSightersAct, &QAction::triggered, this, &ProtoRangeControl::sendInitToAllSelected);

    QAction *startSelectedSightersAct = new QAction(tr("Alusta proovidega"), this);
    startSelectedSightersAct->setStatusTip(tr("Annab käsu märgitud radadele alustada proovilaskudega"));
    connect(startSelectedSightersAct, &QAction::triggered, [this](){
        sendMessageToAllSelected("start sighters");
    });

    QAction *stopSelectedSightersAct = new QAction(tr("Lõpeta proovid"), this);
    stopSelectedSightersAct->setStatusTip(tr("Annab käsu märgitud radadele lõpetada proovilasud"));
    connect(stopSelectedSightersAct, &QAction::triggered, [this](){
        sendMessageToAllSelected("stop sighters");
    });

    QAction *startSelectedMatchAct = new QAction(tr("Alusta võistlust"), this);
    startSelectedMatchAct->setStatusTip(tr("Annab käsu märgitud radadele alustada võistlust"));
    connect(startSelectedMatchAct, &QAction::triggered, [this](){
        sendMessageToAllSelected("start match");
    });

    QAction *continueSelectedAct = new QAction(tr("Jätka"), this);
    continueSelectedAct->setStatusTip(tr("Annab käsu märgitud radadele jätkata"));
    connect(continueSelectedAct, &QAction::triggered, [this](){
        sendMessageToAllSelected("continue");
    });

    QAction *stopSelectedMatchAct = new QAction(tr("Lõpeta võistlus"), this);
    stopSelectedMatchAct->setStatusTip(tr("Annab käsu märgitud radadele lõpetada võistlus"));
    connect(stopSelectedMatchAct, &QAction::triggered, [this](){
        sendMessageToAllSelected("stop match");
    });

    QAction *blinkSelectedAct = new QAction(tr("Vilguta"), this);
    blinkSelectedAct->setStatusTip(tr("Annab käsu märgitud radadele vilgutada taustavalgusega"));
    connect(blinkSelectedAct, &QAction::triggered, [this](){
        sendMessageToAllSelected("blink");
    });

    QAction *askAllShotsAct = new QAction(tr("Küsi kõik lasud"), this);
    askAllShotsAct->setStatusTip(tr("Küsib märgitud radadelt kõik lasud"));
    connect(askAllShotsAct, &QAction::triggered, [this](){
        sendMessageToAllSelected("send all shots");
    });

    QAction *resetSelectedAct = new QAction(tr("Reset"), this);
    resetSelectedAct->setStatusTip(tr("Reset'ib märgitud rajad"));
    connect(resetSelectedAct, &QAction::triggered, [this](){
        sendMessageToAllSelected("reset");
    });

    QAction *sendShotsAct = new QAction(tr("Saada lasud"), this);
    resetSelectedAct->setStatusTip(tr("Saadab märgitud laskurite olemasolevad lasud Protokollitajasse"));
    connect(sendShotsAct, &QAction::triggered, [this](){
        publishSelectedCompetitorShots();
    });

    controlMenu->addAction(initSelectedSightersAct);
    controlMenu->addAction(startSelectedSightersAct);
    controlMenu->addAction(stopSelectedSightersAct);
    controlMenu->addAction(startSelectedMatchAct);
    controlMenu->addAction(continueSelectedAct);
    controlMenu->addAction(stopSelectedMatchAct);
    controlMenu->addSeparator();
    controlMenu->addAction(blinkSelectedAct);
    controlMenu->addAction(askAllShotsAct);
    controlMenu->addAction(resetSelectedAct);
    controlMenu->addSeparator();
    controlMenu->addAction(sendShotsAct);

    QAction *changeLanguageAct = new QAction(tr("Programmi keel"), this);
    resetSelectedAct->setStatusTip(tr("Programmi keele valik"));
    connect(changeLanguageAct, &QAction::triggered, [this](){
        changeLanguage();
    });

    languageMenu->addAction(changeLanguageAct);
}

std::optional<Lane*> ProtoRangeControl::findLane(int laneNo)
{
    return findLane(QString("%1").arg(laneNo));
}

std::optional<Lane*> ProtoRangeControl::findLane(QString laneNo)
{
    foreach(Lane *lane, m_lanes) {
        if (lane->target().compare(laneNo) == 0) {
            return lane;
        }
    }
    return {};
}

void ProtoRangeControl::importStartList()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Ava startlist"), "", tr("Comma separated file (*.csv)"));
    if(fileName.isEmpty())
        return;

    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&file);
        QStringList rows;
        while (!in.atEnd()) {
            QString row = in.readLine();
            if (row.startsWith("Erä", Qt::CaseInsensitive)) {
                QMessageBox::critical(this, tr("Viga"), tr("Kasutage Siusi startlisti formaati, kuna Inbandi omas ei ole ID'sid!"), QMessageBox::Ok);
                return;
            } else {
                QTextStream(stdout) << "ProtoRangeControl::importStartList: Competitor row: " << row << Qt::endl;
                QStringList rowParts = row.split(";");
                if (rowParts.length() >= 15){
                    int targetNo = rowParts.at(10).toInt();
                    if (targetNo == 0)
                        targetNo = QInputDialog::getInt(this, tr("Sisesta raja number"), QString("%1 %2: ").arg(rowParts.at(3), rowParts.at(2)));
                    auto laneOpt = findLane(targetNo);
                    if (laneOpt) {
                        laneOpt.value()->setSiusCompetitorRow(row);
                    } else {   // That means that existing lane was not found
                        QTextStream(stdout) << "ProtoRangeControl::importStartList: Lane with target " << rowParts.at(10) << " not found, ignoring!" << Qt::endl;
//                        addLane(row, "");
                    }
                }
            }
        }
    } else
        QMessageBox::critical(this, "Viga", "Ei õnnestunud faili avada!", QMessageBox::Ok);
}

void ProtoRangeControl::initialize()
{
    QTextStream(stdout) << "ProtoRangeControl::initialize" << Qt::endl;

    vBox->addWidget(new Header);

    setupLanes();

    QFile *logFile = new QFile(QString("ProtoRangeControl sisse %1.log").arg(QDate::currentDate().toString(Qt::ISODate)));
    if (logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        m_incomingLog.setDevice(logFile);
        m_incomingLog << "///////////////////////////////" << programVersion << ", " << QDateTime::currentDateTime().toString() <<  "///////////////////////////////\n";
    }

    if (m_shotsLog == nullptr) {
        QFile *logFile = new QFile(QString("ProtoRangeControl lasud %1.log").arg(QDate::currentDate().toString(Qt::ISODate)));
        if (logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
            m_shotsLog = new QTextStream(logFile);
            *m_shotsLog << "///////////////////////////////" << programVersion << ", " << QDateTime::currentDateTime().toString() <<  "///////////////////////////////\n";
        }
    }

    if (tcpSocket == nullptr)
        tcpSocket = new QTcpSocket(this);

    connect(&m_server, &ConnectionServer::error, this, [this](QString error) {
        showMessage(error);
    });
    connect(&m_server, &ConnectionServer::info, this, [this](QString info) {
        showMessage(info);
    });
    connect(&m_server, &ConnectionServer::newShot, this, &ProtoRangeControl::newShot);
    connect(&m_server, &ConnectionServer::newTarget, this, &ProtoRangeControl::newTargetIp);
    connect(&m_server, &ConnectionServer::statusUpdate, this, &ProtoRangeControl::updateStatus);
    connect(&m_server, &ConnectionServer::allShots, this, &ProtoRangeControl::allShotsDataReceived);
    connect(&m_server, &ConnectionServer::newProtokollitajaConnection, this, &ProtoRangeControl::publishAllShots);
    connect(&m_server, &ConnectionServer::startListReceived, this, &ProtoRangeControl::loadStartList);
    m_server.start(m_serverPort, m_inbandPort);
}

void ProtoRangeControl::loadStartList(QStringList startList)
{
    // targetNo;id;firstName;lastName;club;discipline;decimals;numberOfShots
    for (QString row : startList) {
        QTextStream(stdout) << "ProtoRangeControl::loadStartList: Competitor row: " << row << Qt::endl;
        QStringList rowParts = row.split(";");
        if (row.size() > 0) {
            if (rowParts.size() < 8) {
                QMessageBox::critical(this, tr("Viga"), tr("Vigane stardinimekirja rida!\n%1").arg(row), QMessageBox::Ok);
            } else {
                QString targetNo = rowParts.at(0);
                QTextStream(stdout) << "ProtoRangeControl::loadStartList: targetNo = " << targetNo << Qt::endl;
                if (targetNo == "0" || targetNo.isEmpty())
                    targetNo = QString("%1").arg(QInputDialog::getInt(this, tr("Sisesta raja number"), QString("%1 %2: ").arg(rowParts.at(2), rowParts.at(3))));
                auto laneOpt = findLane(targetNo);
                if (laneOpt) {
                    QTextStream(stdout) << "ProtoRangeControl::loadStartList: laneOpt defined" << Qt::endl;
                    if (!laneOpt.value()->inCompetition())
                        laneOpt.value()->setStartListCompetitorRow(row);
                    else
                        QMessageBox::critical(this, tr("Viga"), tr("Rajal %1 on võistlus käimas, uut laskurit ei imporditud!").arg(targetNo), QMessageBox::Ok);
                } else {   // That means that existing lane was not found
                    QTextStream(stdout) << "ProtoRangeControl::loadStartList: Lane with target " << rowParts.at(0) << " not found, ignoring!" << Qt::endl;
                }
            }
        }
    }
}

void ProtoRangeControl::publishAllShots(DataConnection *connection)
{
    QTextStream(stdout) << "ProtoRangeControl::publishAllShots(), m_shots.size() = " << m_shots.size() << Qt::endl;
    QStringList shotRows;
    QVector<int> sentIds;
    foreach(SiusShotData shotData, m_shots) {
        if (!sentIds.contains(shotData.id)) {
            shotRows.append(QString("_GRPH;0;0;%1;60;73;11:15:50.75;8;0;6;1;0;0;61;2958939953").arg(shotData.id)); // Prepend _GRPH to competitor's shots, so that Protokollitaja considers these to be competition shots
            sentIds.append(shotData.id);
        }
        shotRows.append(convertToSiusRows(shotData));
        QTextStream(stdout) << "ProtoRangeControl::publishAllShots(): shotRows: " << shotRows.join("\n") << Qt::endl;
    }
    m_server.sendAllShotsData(shotRows, connection);
}

void ProtoRangeControl::publishSelectedCompetitorShots()
{
    QVector<int> idsToSend;
    foreach (Lane* lane, m_lanes) {
        if (lane->selected())
            idsToSend.append(lane->id().toInt());    // TODO make it safer
    }

    QStringList shotRows;
    QVector<int> sentIds;
    foreach(SiusShotData shotData, m_shots) {
        if (idsToSend.contains(shotData.id)) {
            if (!sentIds.contains(shotData.id)) {
                shotRows.append(QString("_GRPH;0;0;%1;60;73;11:15:50.75;8;0;6;1;0;0;61;2958939953").arg(shotData.id)); // Prepend _GRPH to competitor's shots, so that Protokollitaja considers these to be competition shots
                sentIds.append(shotData.id);
            }
            shotRows.append(convertToSiusRows(shotData));
            QTextStream(stdout) << "ProtoRangeControl::publishSelectedCompetitorShots(): shotRows: " << shotRows.join("\n") << Qt::endl;
        }
    }
    m_server.sendShotData(shotRows);
}

void ProtoRangeControl::publishShot(SiusShotData shotData)
{
    QStringList rowsToSend = convertToSiusRows(shotData);
    if (shotData.siusShotNo == 1) {
        // Prepend _GRPH to competitor's shots, so that Protokollitaja considers these to be competition shots
        rowsToSend.prepend(QString("_GRPH;0;0;%1;60;73;11:15:50.75;8;0;6;1;0;0;61;2958939953").arg(shotData.id));
    }
    QTextStream(stdout) << "ProtoRangeControl::publishShot: " << rowsToSend.join("\\n") << Qt::endl;
    m_server.sendShotData(rowsToSend);
}

void ProtoRangeControl::newShot(int target, SiusShotData shotData)
{
    showMessage(QString("Lasu info: %1;%2,%3,%4,%5")
                .arg(target)
                .arg(shotData.id)
                .arg(shotData.shot.getSLask())
                .arg(shotData.shot.X())
                .arg(shotData.shot.Y())
                );
    auto laneOpt = findLane(QString("%1").arg(target));
    if (laneOpt) {
        shotData.id = laneOpt.value()->id().toInt();    // TODO make it safer

        Lask::TargetType targetType;
        QTextStream(stdout) << "Discipline: " << laneOpt.value()->discipline() << Qt::endl;
        if (laneOpt.value()->discipline().compare("airrifle", Qt::CaseInsensitive) == 0) {
            targetType = Lask::Ohupuss;
        } else if (laneOpt.value()->discipline().compare("airpistol", Qt::CaseInsensitive) == 0) {
            targetType = Lask::Ohupustol;
        } else
            targetType = Lask::Muu;

        if (targetType != Lask::Muu)
            shotData.shot.setInnerTen(Lask::calcIfInnerTen(targetType, shotData.shot.X(), shotData.shot.Y()));
        if (laneOpt.value()->inCompetition()) {
            shotData.shot.setCompetitionShot(true);
//            shotData.siusShotNo = laneOpt.value()->increaseAndGetCurrentShotIndex();    // not needed actually
            addShot(shotData);  // Save only competition shots
        } else {
            shotData.shot.setCompetitionShot(false);
        }
        laneOpt.value()->setLastShotLabel(QString("(%1.): %2").arg(shotData.siusShotNo).arg(shotData.shot.getSLask()));
        publishShot(shotData);
//        QJsonDocument shotLogJson;
        QJsonObject shotLogJson;
        shotLogJson["time"] = QTime::currentTime().toString();
        shotLogJson["id"] = shotData.id;
        shotLogJson["shotNo"] = shotData.siusShotNo;
        shotLogJson["shot"] = shotData.shot.toJson();
//        QString shotJson = QJsonDocument(shotData.shot.toJson()).toJson(QJsonDocument::Compact);
//        *m_shotsLog << QString("%1;%2; %4").arg(shotData.id).arg(shotData.siusShotNo).arg(shotJson) << Qt::endl;
        *m_shotsLog << QJsonDocument(shotLogJson).toJson(QJsonDocument::Compact) << Qt::endl;
    }
}

void ProtoRangeControl::newTargetIp(int target, QString ip)
{
    showMessage(tr("Märgi IP: ") + ip);
    Lane *currentLane = nullptr;
    bool exists = false;
    foreach(Lane *lane, m_lanes) {
        if (lane->target().toInt() == target) {
            lane->setIp(ip);
            exists = true;
            currentLane = lane;
            break;
        }
    }

    if (!exists) {
        currentLane = addLane(target, ip);
        saveSettings();
    }

    if (currentLane != nullptr)
        sendAck(currentLane);
}

void ProtoRangeControl::readSettings()
{
    QFile file("range_control_conf.json");
    QJsonDocument fileJson;

    if(file.open(QIODevice::ReadOnly)){
        fileJson = QJsonDocument::fromJson(file.readAll());
        QJsonObject jsonObj = fileJson.object();

        if (jsonObj["disciplines"].isArray()){
            m_disciplines.clear();
            QJsonArray array = jsonObj["disciplines"].toArray();
            foreach(QJsonValue item, array){
                m_disciplines.append(item.toString());
            }
        }

        if(jsonObj.contains("windowXLocation") && jsonObj["windowXLocation"].isDouble() && jsonObj.contains("windowYLocation") && jsonObj["windowYLocation"].isDouble())
            //For some reason window frame is not included, so 9 and 36 need to be added. At least on Win7
            this->setGeometry(jsonObj["windowXLocation"].toInt() + 9, jsonObj["windowYLocation"].toInt() + 36, 800, 600);

        if(jsonObj.contains("port") && jsonObj["port"].isDouble())
            m_serverPort = jsonObj["port"].toInt();

        if(jsonObj.contains("inbandPort") && jsonObj["inbandPort"].isDouble())
            m_inbandPort = jsonObj["inbandPort"].toInt();

        if(jsonObj.contains("language") && jsonObj["language"].isString())
            m_language = jsonObj["language"].toString();
    }/*else
        QMessageBox::critical(
                    this,
                    tr("Viga!"),
                    tr("Seadete faili avamine ei ole võimalik!\nKasutatakse vaikimisi seadeid.\n\nAsukoht: ").append(file.fileName()),
                    QMessageBox::Ok
                    );*/
}

void ProtoRangeControl::saveSettings()
{
    QFile file("range_control_conf.json");
    QJsonDocument fileJson;

    if(file.open(QIODevice::WriteOnly)){

        QJsonObject settingsJson;

        if (m_disciplines.size() > 0){
            settingsJson["disciplines"] = QJsonArray::fromStringList(m_disciplines);
        }

        settingsJson["windowXLocation"] = this->x();
        settingsJson["windowYLocation"] = this->y();

        if(m_serverPort != 0)
            settingsJson["port"] = m_serverPort;

        if(m_inbandPort != 0)
            settingsJson["inbandPort"] = m_inbandPort;

        if(!m_language.isEmpty())
            settingsJson["language"] = m_language;

        QJsonArray lanesJson;
        foreach(Lane *lane, m_lanes) {
            if (!lane->ip().isEmpty()) {
                QJsonObject laneJson;
                laneJson["target"] = lane->target();
                laneJson["ip"] = lane->ip();
                lanesJson.append(laneJson);
            }
        }

        if (lanesJson.size() > 0)
            settingsJson["lanes"] = lanesJson;
        fileJson.setObject(settingsJson);

        file.write(fileJson.toJson());
    } else
        QTextStream(stdout) << "ProtoRangeControl::saveSettings: Unable to save settings!" << Qt::endl;
}

void ProtoRangeControl::sendAck(Lane *lane)
{
    QString message = "ack";
    sendMessage(lane, message);
}

void ProtoRangeControl::sendInit(Lane *lane)
{
    if (lane->inCompetition()) {
        if (QMessageBox::warning(
                    this,
                    tr("Võistlus käimas!"),
                    tr("Rajal %1 on võistlus veel käimas, oled kindel, et tahad seda uuesti alustada?").arg(lane->target()),
                    QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
            return;
    }
    QString message = "init\n";
    message.append(lane->firstName() + " " + lane->lastName() + "\n");
    message.append(lane->club() + "\n");
    message.append(lane->discipline() + "\n");
    message.append(" \n");
    message.append(lane->decimals() + "\n");
    message.append(lane->noOfShots());

    lane->setCurrentShotIndex(0);

    showMessage(QString("Saadan: %1, %2").arg(lane->target(), message));
    sendMessage(lane, message);
}

void ProtoRangeControl::sendInitToAllSelected()
{
    int noSelected = 0;
    foreach (Lane* lane, m_lanes) {
        if (lane->selected())
            noSelected++;
    }

    static QVector<Lane*> sentLanes;
    foreach (Lane* lane, m_lanes) {
        if (lane->selected() && !sentLanes.contains(lane)) {
            sendInit(lane);
            sentLanes.append(lane);
            break;
        }
    }

//    QTextStream(stdout) << "ProtoRangeControl::sendInitToAllSelected: sentLanes.size() = " << sentLanes.size() << " noSelected = " << noSelected << Qt::endl;
    if (sentLanes.size() < noSelected) {
//        QTextStream(stdout) << "ProtoRangeControl::sendInitToAllSelected: starting timer" << Qt::endl;
        QTimer::singleShot(50, this, SLOT(sendInitToAllSelected()));
    } else {
        sentLanes.clear();
    }
}

void ProtoRangeControl::sendMessage(Lane *lane, QString message)
{
    QTextStream(stdout) << "ProtoRangeControl::sendMessage(" << message << ")" << Qt::endl;
    if (tcpSocket != nullptr && !lane->ip().isEmpty()) {
        tcpSocket->connectToHost(lane->ip(), 5450);
        tcpSocket->waitForConnected(5000);

        QTextStream out(tcpSocket);
        out << lane->target() << Qt::endl;
        out << message << "\nmessage end" << Qt::endl;
        tcpSocket->close();
        showMessage("Sent: " + message.replace("\n", ";"));
    }
}

void ProtoRangeControl::sendMessageToAllSelected(QString message)
{
    m_message = message;
    continueSendingMessage();
}

void ProtoRangeControl::setCheckedAll()
{
    foreach (Lane *lane, m_lanes) {
        lane->setSelected(true);
    }
}

void ProtoRangeControl::setNumberOfShots()
{
    bool wasAccepted = false;
    int newNumber =  QInputDialog::getInt(this, tr("Sisesta laskude arv"), tr("Laskude arv: "), 60, 0, 120, 1, &wasAccepted);
    if (wasAccepted)
        foreach (Lane *lane, m_lanes) {
            if (lane->selected())
                lane->setShotsNumber(newNumber);
        }
}

void ProtoRangeControl::setTargetTypes()
{
    bool wasAccepted = false;
    QString targetType = QInputDialog::getItem(this, tr("Vali märklehe tüüp"), tr("Märkleht: "), m_disciplines, 0, false, &wasAccepted);
    if (wasAccepted)
        foreach (Lane *lane, m_lanes) {
            if (lane->selected())
                lane->setDiscipline(targetType);
        }
}

void ProtoRangeControl::setupLanes()
{
    QFile file("range_control_conf.json");
    QJsonDocument fileJson;

    if(file.open(QIODevice::ReadOnly)){
        fileJson = QJsonDocument::fromJson(file.readAll());
        QJsonObject jsonObj = fileJson.object();

        if(jsonObj["lanes"].isArray()) {
            foreach(const QJsonValue & value, jsonObj["lanes"].toArray()) {
                QJsonObject laneObj = value.toObject();
                QString target = laneObj["target"].toString();
                QString ip = laneObj["ip"].toString();
                addLane(target.toInt(), ip);
            }
        }
    }
}

void ProtoRangeControl::showMessage(QString msg)
{
    QTextStream(stdout) << "ProtoRangeControl::showMessage(" << msg << ")" << Qt::endl;
    statusBar()->showMessage(msg, 5000);
}

void ProtoRangeControl::unsetCheckedAll()
{
    foreach (Lane *lane, m_lanes) {
        lane->setSelected(false);
    }
}

void ProtoRangeControl::updateStatus(int target, QString newStatus)
{
    showMessage(QString("Uuendus: %1, ").arg(target) + newStatus);
    foreach(Lane *lane, m_lanes) {
        if (lane->target().toInt() == target) {
            lane->setStatus(newStatus);
            break;
        }
    }
}

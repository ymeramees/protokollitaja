#include "kllfilerw.h"

KllFileRW::KllFileRW(
        Andmebaas *competitorDatabase,
        bool *autoComplete,
        int *ranking,
        LiikmeteValikKast *competitorsPickingBox,
        QWidget *parentWindow,
        QObject *parent
        ) : QObject(parent)
{
    m_autoComplete = autoComplete;
    m_competitorDatabase = competitorDatabase;
    m_competitorsPickingBox = competitorsPickingBox;
    m_parentWindow = parentWindow;
    m_sorting = ranking;
}

int KllFileRW::addCompetitor(QTabWidget *tabWidget, int previousId)
{
    if(tabWidget->count() > 0){
        int lastId = checkDuplicateIds(tabWidget, previousId);
        Leht* thisSheet = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
        thisSheet->uusLaskur(++lastId);
        if(thisSheet->voistk)
            thisSheet->voistkonnad[thisSheet->voistkonnad.count() - 1]->nimi->setFocus();
        else thisSheet->laskurid[thisSheet->laskurid.count() -1]->eesNimi->setFocus();

        return lastId;
    } else
        return previousId;
}

int KllFileRW::checkDuplicateIds(QTabWidget *tabWidget, int lastId) // Needed for reading older files
{
    for(int i = 0; i < tabWidget->count(); i++){    // First find the biggest existing ID
        Leht* sheet = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());

        if(!sheet->voistk)   // Teams have no IDs
            for(int j = 0; j < sheet->laskurid.count(); j++){
                if(sheet->laskurid[j]->id > lastId)
                    lastId = sheet->laskurid[j]->id;
            }
    }

    for(int i = 0; i < tabWidget->count(); i++){    // Check all tabs
        Leht* sheet = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());

        if(!sheet->voistk)
            for(int j = 0; j < sheet->laskurid.count(); j++){    // Check all competitors
                for(int x = 0; x < tabWidget->count(); x++){    // Comparing with all other competitors
                    Leht* sheet2 = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(x))->widget());
                    if(!sheet2->voistk)
                        for(int y = 0; y < sheet2->laskurid.count(); y++){
                            if(!(i == x && j == y) && sheet->laskurid[j]->id == sheet2->laskurid[y]->id)
                                sheet2->laskurid[y]->id = ++lastId;   // Second competitor gets new ID
                        }
                }
            }
    }
    return lastId;
}

TabWidgetWithSettings KllFileRW::readKllFile(QString fileName, int startingId)
{
#ifdef QT_DEBUG
    qDebug() << "readKllFile()";
#endif
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    TabWidgetWithSettings kllData;
    QFile fail(fileName);
    if(fail.open(QIODevice::ReadOnly)){
        QDataStream in(&fail);

        CompetitionSettings competitionSettings = SimpleKllFileRW::readCompetitionSettings(&in, nullptr);

        if (!competitionSettings.competitionName.isEmpty()) {  // This should fail, when reading settings failed
            kllData.competitionName = competitionSettings.competitionName;
            kllData.startDate = competitionSettings.startDate;
            kllData.endDate = competitionSettings.endDate;
            kllData.place = competitionSettings.place;
            kllData.country = competitionSettings.country;
        } else {
            QApplication::restoreOverrideCursor();
            return kllData;
        }

        if (competitionSettings.fileVersion == 112) {
            in >> kllData.webCompetitionId;
            if (kllData.webCompetitionId.contains(QRegularExpression(QStringLiteral("[^\\x{0000}-\\x{007F}]")))) {
                if (QMessageBox::warning(
                        m_parentWindow,
                        tr("Viga!"),
                        tr("Võistluse veebi ID (%1) sisaldab kummalisi tähemärke ja on ilmselt vigane!"
                           "\n\nKas soovite selle kustutada? See ei mõjuta muud, kui ainult veebi laadimist.")
                            .arg(kllData.webCompetitionId),
                        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
                    kllData.webCompetitionId = "";
            }
        }
        else if (competitionSettings.fileVersion >= 113)
            kllData.webCompetitionId = competitionSettings.jsonData["webCompetitionId"].toString("");

        if (kllData.webCompetitionId == "Empty")
            kllData.webCompetitionId = "";

        int uploadInterval = 30;

        if (competitionSettings.fileVersion < 113) {
            in >> kllData.autoComplete >> kllData.autosaveEnabled >> kllData.autosaveInterval >> kllData.tabLocation >> kllData.tabCount;

            if(competitionSettings.fileVersion >= 108)
                in >> kllData.ranking;
            else
                kllData.ranking = 0;
            if(competitionSettings.fileVersion >=111)
                in >> uploadInterval;

            if(uploadInterval < 30)
                uploadInterval = 30;

            kllData.uploadInterval = uploadInterval;
        } else {
            kllData.autoComplete = competitionSettings.jsonData["autoComplete"].toInt();
            kllData.autosaveEnabled = competitionSettings.jsonData["autosaveEnabled"].toInt();
            kllData.autosaveInterval = competitionSettings.jsonData["autosaveInterval"].toInt();
            kllData.tabLocation = competitionSettings.jsonData["tabsLocation"].toInt();
            kllData.tabCount = competitionSettings.jsonData["tabCount"].toInt();
            kllData.ranking = competitionSettings.jsonData["ranking"].toInt();
            kllData.uploadInterval = competitionSettings.jsonData["uploadInterval"].toInt();
        }

#ifdef QT_DEBUG
        qDebug() << "readKllFile(): << fileVersion << competitionName << koht << m_country << kirjutusA << autosave << aeg << sakiAsukoht << sakkideArv";
        qDebug() << "readKllFile(): << " << competitionSettings.fileVersion << " << " << kllData.competitionName << " << " << kllData.place << " << " << kllData.country << " << " << kllData.autoComplete << " << " << kllData.autosaveEnabled << " << " << kllData.autosaveInterval << " << " << kllData.tabLocation << " << " << kllData.tabCount;
#endif

        kllData.lastCompetitorId = startingId;  // For importing into already existing competition
        if(competitionSettings.fileVersion == 100){
            kllData.tabWidget = new QTabWidget();
            kllData.tabWidget->setTabPosition(QTabWidget::North);
            for(int i = 0; i < kllData.tabCount; i++){
                int s = 0, vs = 0, a = 0, noOfCompetitors = 0, min = 0, max = 0, tabIndex = 0;
                bool team = false;
                QString tabName, screenName;
                in >> tabName >> s >> a >> screenName >> min >> max >> team >> tabIndex >> noOfCompetitors;
                QScrollArea *area = new QScrollArea(kllData.tabWidget);
                area->setWidgetResizable(true);
//                leheIndeks = lIndeks;
                Leht* sheet = new Leht(
                            m_competitorDatabase,
                            s,
                            vs,
                            a,
                            m_autoComplete,
                            screenName,
                            0,
                            "Muu",
                            false,
                            m_sorting,
                            kllData.tabWidget,
                            team,
                            m_competitorsPickingBox,
                            tabIndex
                            );
                area->setWidget(sheet);
                kllData.tabWidget->addTab(area, tabName);
                kllData.tabWidget->setCurrentIndex(0);  // Otherwise in Windooz only last tab is visible and others are hidden
                for(int j = 0; j < noOfCompetitors; j++){
                    // As so old kll file does not contain competitor ID, it can be created
                    kllData.lastCompetitorId = addCompetitor(kllData.tabWidget, kllData.lastCompetitorId);
                }

                sheet->alustamine = true;
                sheet->setMinTimeMs(min);
                sheet->setMaxTimeMs(max);
                if(sheet->voistk){
                    QString teamName, name, lastName, club, labelTxt, sum, notes;
                    for(int j = 0; j < noOfCompetitors; j++){
                        in >> teamName;
                        sheet->voistkonnad[j]->nimi->setText(teamName);
                        for(int k = 0; k < s; k++){
                            in >> name >> lastName >> club >> sum >> labelTxt;
                            sheet->voistkonnad[j]->voistlejad[k]->eesNimi = name;
                            sheet->voistkonnad[j]->voistlejad[k]->perekNimi = lastName;
                            sheet->voistkonnad[j]->voistlejad[k]->klubi = club;
                            sheet->voistkonnad[j]->voistlejad[k]->summa = sum;
                            sheet->voistkonnad[j]->voistlejad[k]->silt->setText(labelTxt);
                        }
                        in >> sum >> notes;
                        sheet->voistkonnad[j]->summa->setText(sum);
                        sheet->voistkonnad[j]->markus->setText(notes);
//                        leht->voistkonnad[j]->muutus3();
                    }
                }else{
                    QString name, lastName, birthY, club, currentSeries, sum, finals, innerTens, notes;
                    for(int j = 0; j < noOfCompetitors; j++){
                        in >> name >> lastName >> birthY >> club;
                        sheet->laskurid[j]->eesNimi->setText(name);
                        sheet->laskurid[j]->perekNimi->setText(lastName);
                        sheet->laskurid[j]->sunniAasta->setText(birthY);
                        sheet->laskurid[j]->klubi->setText(club);
                        for(int k = 0; k < s; k++){
                            in >> currentSeries;
                            sheet->laskurid[j]->seeriad[k]->setText(currentSeries);
                        }
                        in >> sum >> finals >> innerTens >> notes;
                        sheet->laskurid[j]->setSumma(sum);
                        sheet->laskurid[j]->finaal->setText(finals);
                        sheet->laskurid[j]->kumned->setText(innerTens);
                        sheet->laskurid[j]->markus->setText(notes);
                        sheet->laskurid[j]->liida();
                    }
                }
                sheet->alustamine = false;
            }
        } else if(competitionSettings.fileVersion >= 101 && competitionSettings.fileVersion <= 112) {
            kllData.tabWidget = new QTabWidget();
            kllData.tabWidget->setTabPosition(QTabWidget::North);

            for(int i = 0; i < kllData.tabCount; i++){
                int s = 0, noOfShots = 10, vs = 0, a = 0, noOfCompetitors = 0, min = 0, max = 0, tabIndex = 0, weaponType = 0;
                bool team = false;
                bool withDecimals = false;
                bool toBeShown = true;
                QString tabName, screenName, eventType = "Muu";
                in >> tabName >> s;
                if(competitionSettings.fileVersion >= 107)
                    in >> noOfShots;
                in >> vs >> a >> screenName;
                if(competitionSettings.fileVersion != 101)
                    in >> weaponType;
                if(competitionSettings.fileVersion >= 103)
                    in >> eventType;
                if(eventType.isEmpty())
                    eventType = "Muu";
                if(competitionSettings.fileVersion >= 105)
                    in >> withDecimals;
                if(competitionSettings.fileVersion >= 108)
                    in >> toBeShown;
                in >> min >> max >> team >> tabIndex >> noOfCompetitors;

                QScrollArea *area = new QScrollArea(kllData.tabWidget);
                area->setWidgetResizable(true);
//                leheIndeks = lIndeks;
                Leht* sheet = new Leht(
                            m_competitorDatabase,
                            s,
                            vs,
                            a,
                            m_autoComplete,
                            screenName,
                            weaponType,
                            eventType,
                            withDecimals,
                            m_sorting,
                            kllData.tabWidget,
                            team,
                            m_competitorsPickingBox,
                            tabIndex,
                            noOfShots
                            );
                area->setWidget(sheet);
                kllData.tabWidget->addTab(area, tabName);
                kllData.tabWidget->setCurrentIndex(0);  // Otherwise in Windooz only last tab is visible and others are hidden
                for(int j = 0; j < noOfCompetitors; j++){
                    sheet->uusLaskur(0);   // First add competitor with ID 0, so it can be later determined if correct ID was read from the file
                }

                sheet->alustamine = true;
                sheet->naidata = toBeShown;    // Will this sheet be shown on spectators window
                sheet->setMinTimeMs(min); // Min time to show
                sheet->setMaxTimeMs(max); // Max time to show
                if(sheet->voistk){
                    if(competitionSettings.fileVersion >= 106){
                        int following = 0, followee = 0;
                        in >> following;
                        for(int i = 0; i < following; i++){
                            in >> followee;
                            sheet->jalgitavad.append(followee);
                        }
                    }
                    QString teamName, name, lastName, club, eventType, label, sum, notes;
                    for(int j = 0; j < noOfCompetitors; j++){
                        in >> teamName;
                        sheet->voistkonnad[j]->nimi->setText(teamName);
                        for(int k = 0; k < s; k++){
                            in >> name >> lastName >> club;
                            if(competitionSettings.fileVersion >= 106){
                                in >> eventType;
                            }
                            in >> sum >> label;
                            sheet->voistkonnad[j]->voistlejad[k]->eesNimi = name;
                            sheet->voistkonnad[j]->voistlejad[k]->perekNimi = lastName;
                            sheet->voistkonnad[j]->voistlejad[k]->klubi = club;
                            if(competitionSettings.fileVersion >= 106){
                                sheet->voistkonnad[j]->voistlejad[k]->harjutus = eventType;
                            }
                            sheet->voistkonnad[j]->voistlejad[k]->summa = sum;
                            sheet->voistkonnad[j]->voistlejad[k]->silt->setText(label);
                        }
                        in >> sum >> notes;
                        sheet->voistkonnad[j]->summa->setText(sum);
                        sheet->voistkonnad[j]->markus->setText(notes);
//                        leht->voistkonnad[j]->muutus3();
                    }
                }else{
                    int shootOffShot = -1, shot, id, noOfShootOffShots = 10;   // In older versions there are 10 shoot off shots, in newer versions 24, will be changed below
                    QString targetNo, targetNumberingStart, targetNumberingEnd, name, lastName, birthY, club, currentSeries, sum, finals, innerTens, notes;
                    for(int j = 0; j < noOfCompetitors; j++){
                        if(competitionSettings.fileVersion >= 104)
                            in >> targetNo >> targetNumberingStart >> targetNumberingEnd;
                        if(competitionSettings.fileVersion >= 107)
                            in >> id;
                        in >> name >> lastName >> birthY >> club;
                        sheet->laskurid[j]->rajaNr->setText(targetNo);
                        sheet->laskurid[j]->sifriAlgus->setText(targetNumberingStart);
                        sheet->laskurid[j]->sifriLopp->setText(targetNumberingEnd);
                        sheet->laskurid[j]->id = id;
                        sheet->laskurid[j]->eesNimi->setText(name);
                        sheet->laskurid[j]->perekNimi->setText(lastName);
                        sheet->laskurid[j]->sunniAasta->setText(birthY);
                        sheet->laskurid[j]->klubi->setText(club);

                        for(int k = 0; k < s; k++){
                            in >> currentSeries;
                            sheet->laskurid[j]->seeriad[k]->setText(currentSeries);
                            if(competitionSettings.fileVersion >= 107 && competitionSettings.fileVersion <= 108){
                                int x, y;
                                for(int l = 0; l < sheet->laskurid[j]->lasud[k].count(); l++){
                                    in >> shot >> x >> y;
                                    sheet->laskurid[j]->lasud[k][l]->set10Lask(shot);
                                    sheet->laskurid[j]->lasud[k][l]->setMmX(x);
                                    sheet->laskurid[j]->lasud[k][l]->setMmY(y);
                                }
                            }else if(competitionSettings.fileVersion >= 109){
                                QString x, y;
                                QTime shotTime;
                                bool isInnerTen = false;
                                for(int l = 0; l < sheet->laskurid[j]->lasud[k].count(); l++){
                                    in >> shot >> x >> y;
                                    if(competitionSettings.fileVersion >= 111){
                                        in >> shotTime >> isInnerTen;
                                        sheet->laskurid[j]->lasud[k][l]->setShotTime(shotTime);
                                        sheet->laskurid[j]->lasud[k][l]->setInnerTen(isInnerTen);
                                    }
                                    sheet->laskurid[j]->lasud[k][l]->set10Lask(shot);
                                    sheet->laskurid[j]->lasud[k][l]->setMmX(x);
                                    sheet->laskurid[j]->lasud[k][l]->setMmY(y);
                                }
                            }
                            if(competitionSettings.fileVersion >= 110){
                                noOfShootOffShots = 24;    // Newer Finaal file has 24 shoot off shots
                            }
                        }
                        in >> sum >> finals;
                        sheet->laskurid[j]->lisaLasud.clear();
                        for(int k = 0; k < noOfShootOffShots; k++){
                            in >> shootOffShot;
                            sheet->laskurid[j]->lisaLasud << shootOffShot;
                        }
                        if(sheet->laskurid[j]->lisaLasud.count() < 24)
                            for(int k = sheet->laskurid[j]->lisaLasud.count(); k < 24; k++)
                                sheet->laskurid[j]->lisaLasud << -1; // If there are not enough shoot off shots, add empty shots
                        in >> innerTens >> notes;
                        sheet->laskurid[j]->setSumma(sum);
                        sheet->laskurid[j]->finaal->setText(finals);
                        sheet->laskurid[j]->kumned->setText(innerTens);
                        sheet->laskurid[j]->markus->setText(notes);
                        sheet->laskurid[j]->liida();
                    }
                }
                sheet->alustamine = false;
            }
            kllData.lastCompetitorId = checkDuplicateIds(kllData.tabWidget, 0);
        } else if (competitionSettings.fileVersion <= 113) {
            kllData.tabWidget = new QTabWidget();
            kllData.tabWidget->setTabPosition(QTabWidget::North);

            QJsonArray tabsArray = competitionSettings.jsonData["tabs"].toArray();
            for (QJsonValue tabJson : tabsArray) {
                QJsonObject tabObject = tabJson.toObject();
                int seriesCount = 0, noOfShots = 10, seriesInSubTotal = 0, autocomplete = 0, competitorCount = 0, tabIndex = 0, weaponType = 0;
                bool teamEvent = false;
                bool withDecimals = false;
                bool toBeShown = true;
                QString tabName, displayName, eventType = "Muu";

                tabName = tabObject["name"].toString();
                seriesCount = tabObject["seriesCount"].toInt();
                noOfShots = tabObject["shotsCount"].toInt();
                seriesInSubTotal = tabObject["shotsCount"].toInt();
                autocomplete = tabObject["autocomplete"].toInt();
                displayName = tabObject["displayName"].toString();
                weaponType = tabObject["weaponType"].toInt();
                eventType = tabObject["event"].toString();

                if(eventType.isEmpty())
                    eventType = "Muu";

                withDecimals = tabObject["decimals"].toBool();
                toBeShown = tabObject["toBeShown"].toBool();
                teamEvent = tabObject["teamEvent"].toBool();
                tabIndex = tabObject["tabIndex"].toInt();
                competitorCount = tabObject["competitorCount"].toInt();

#ifdef QT_DEBUG
                qDebug() << "readKllFile(): << leheIndeks << tabName << seriesCount << shotsCount << seriesInSubTotal << autocomplete << << displayName << weaponType << event << competitorCount";
                qDebug() << "readKllFile(): << " << tabIndex << " << " << tabName << " << " << seriesCount << " << " << noOfShots << " << " << seriesInSubTotal << " << " << autocomplete << " << " << displayName << " << " << weaponType << " << " << eventType << " << " << competitorCount;
#endif

                QScrollArea *area = new QScrollArea(kllData.tabWidget);
                area->setWidgetResizable(true);
                Leht* sheet = new Leht(
                            m_competitorDatabase,
                            seriesCount,
                            seriesInSubTotal,
                            autocomplete,
                            m_autoComplete,
                            displayName,
                            weaponType,
                            eventType,
                            withDecimals,
                            m_sorting,
                            kllData.tabWidget,
                            teamEvent,
                            m_competitorsPickingBox,
                            tabIndex,
                            noOfShots
                            );
                area->setWidget(sheet);
                kllData.tabWidget->addTab(area, tabName);
                kllData.tabWidget->setCurrentIndex(0);  // Otherwise in Windooz only last tab is visible and others are hidden

                sheet->alustamine = true;
                sheet->naidata = toBeShown;    // Will this sheet be shown on spectators window
                sheet->setMinTimeMs(tabObject["minTime"].toInt());
                sheet->setMaxTimeMs(tabObject["maxTime"].toInt());
                if (sheet->voistk) {
                    QJsonArray followeesArray = tabObject["followees"].toArray();
                    for (QJsonValue followeeIndex : followeesArray) {
                        sheet->jalgitavad.append(followeeIndex.toInt());
                    }

                    QJsonArray teamsArray = tabObject["teams"].toArray();
                    for (QJsonValue teamJson : teamsArray){
                        sheet->uusLaskur(teamJson.toObject());
                    }
                } else {
                    QJsonArray competitorsArray = tabObject["competitors"].toArray();
                    for (QJsonValue competitorJson : competitorsArray){
                        sheet->uusLaskur(competitorJson.toObject());
                    }
                }
                sheet->alustamine = false;
            }
            kllData.lastCompetitorId = checkDuplicateIds(kllData.tabWidget, 0);
        } else QMessageBox::critical(
                    m_parentWindow,
                    tr("Protokollitaja"),
                    tr("Vale versiooni fail!\n\nVõimalik, et tegu on uuema programmi versiooni failiga.\n\n(Protokollitaja::loefail())"),
                    QMessageBox::Ok
                    );
        fail.close();
    }else QMessageBox::critical(m_parentWindow, tr("Protokollitaja"), tr("Ei leia faili!"), QMessageBox::Ok);
    QApplication::restoreOverrideCursor();
#ifdef QT_DEBUG
    qDebug() << "readKllFile() lõpp";
#endif
    return kllData;
}

bool KllFileRW::writeKllFile(QTabWidget *toWrite)
{
    // TODO To be implemented
}

#include "teamstable2022.h"

TeamsTable2022::TeamsTable2022(QWidget *parent) : QWidget(parent)
{
//    m_parent = parent;
    vBox = new QVBoxLayout;
    setLayout(vBox);
//    setStyleSheet("border:1px solid rgb(0, 255, 0); ");
}

void TeamsTable2022::clear()
{
    foreach (Team2022 *team, m_teams) {
        vBox->removeWidget(team);
        team->deleteLater();
    }
    m_teams.clear();
}

void TeamsTable2022::createLayout(QJsonObject jsonObj)
{
    int teamsNo = 4;    // Defaults to 4

    if(jsonObj["teams"].isArray()){
        teamsNo = jsonObj["teams"].toArray().size();
    } else {
        teamsNo = jsonObj["teams"].toInt();
    }

    for(int i = 0; i < teamsNo; i++){
        Team2022 *team = nullptr;
        if(jsonObj["teams"].isArray()){
            QJsonObject teamJson = jsonObj["teams"].toArray().at(i).toObject();
            team = new Team2022(teamJson, i+1);
        } else {
            team = new Team2022(jsonObj, i+1);
        }
//        if(m_parent != nullptr){
            connect(team, &Team2022::teamUpdated, this, &TeamsTable2022::sumAllTeams);
            connect(team, &Team2022::statusInfo, this, &TeamsTable2022::statusInfoChanged);
            connect(team, &Team2022::modified, this, &TeamsTable2022::modified);
//        }
        m_teams.append(team);
        vBox->addWidget(team);
    }
    sumAllTeams();
}

QMap<int, TeamsTable2022::Result> TeamsTable2022::getCurrentResults() const
{
    QMap<int, Result> results;
    int currentShotNo = lastValidShotIndex();

    foreach(Team2022 *team, m_teams) {
        QString result = QString("%1").arg(team->result10At(currentShotNo));
        if (result != "0" && result != "-999")
            result.insert(result.length() - 1,',');  // Comma needs to be added
        else if (result != "-999")
            result = "0,0";
        else
            result = "";

//        if (result != "0,0" && result != "-999")
        results[team->index()] = Result {
                team->teamName(),
                result,
                team->points10At(currentShotNo),
                team->pointsTotal()};
    }

    return results;
}

QMap<int, TeamsTable2022::Result> TeamsTable2022::getSortedResults() const
{
    QMap<int, Result> results;
    int currentShotNo = lastValidShotIndex();

    foreach(Team2022 *team, m_teams) {
        QString result = QString("%1").arg(team->result10At(currentShotNo));
        if (result != "0" && result != "-999")
            result.insert(result.length() - 1,',');  // Comma needs to be added
        else if (result != "-999")
            result = "0,0";
        else
            result = "";

//        if (result != "0,0" && result != "-999")
        results.insertMulti(team->pointsTotal().replace(',', '.').toDouble() * 10, Result {
                team->teamName(),
                result,
                team->points10At(currentShotNo),
                team->pointsTotal()});
    }

    return results;
}

int TeamsTable2022::lastValidShotIndex() const
{
    int largestIndex = -1;
    foreach(Team2022 *team, m_teams) {
        if (team->lastValidShotIndex() > largestIndex)
            largestIndex = team->lastValidShotIndex();
    }

    return largestIndex;
}

void TeamsTable2022::readSiusInfo(SiusShotData shotData)
{
    if(verbose)
        QTextStream(stdout) << "readSiusInfo()" << endl;

//    for(QString row : lines){
        //Search for competitor whose line was received:
//        QStringList rowParts = row.split(';');
    Competitor2022 *thisCompetitor = nullptr;
    foreach (Team2022 *team, m_teams) {
        if(team->getCompetitorWithID(shotData.id) != nullptr){
            thisCompetitor = team->getCompetitorWithID(shotData.id);

            bool success = thisCompetitor->readSiusShot(shotData);
            if (success) {
                emit modified();
                emit statusInfoChanged(QString("Lisatud: %1%2%3 = %4").arg(thisCompetitor->name()).arg(tr(" lask ")).arg(shotData.siusShotNo).arg(shotData.shot.getSLask()));
            } else
                emit statusInfoChanged(QString("Viga, ei õnnestunud lisada: %1%2%3 = %4").arg(thisCompetitor->name()).arg(tr(" lask ")).arg(shotData.siusShotNo).arg(shotData.shot.getSLask()));
        }
    }
//            if(row.startsWith("_SHOT") && !competitionStarted){   //Shot data, sighting shots
//                    thisCompetitor->setPreviousSiusRow(row);

//                    statusBarInfoChanged(rowParts[3] + tr(": Proovilask"));

//                    Lask lask(row);
//                    if(verbose)
//                        QTextStream(stdout) << "rowParts[3]: " << rowParts[3] << ", rida: " << row;
//                }else if(row.startsWith("_TOTL") && !competitionStarted){   //Result row
//                    if(rowParts.count() > 12){  //If row is not long enough, then probably it is total result, which is not interesting here
//                        if(rowParts[12] != "0"){   //If it is sighting shot, then this is 0, but if competition shot, then it is series result
//                            if(thisCompetitor->previousSiusRow().startsWith("_SHOT")){  //As previous shot was already competition shot, then it needs to be added to someone
//                                competitionStarted = true;
//                                statusBarInfoChanged(rowParts[3] + tr(": algavad võistluslasud"));

//                                QStringList previousRowParts = thisCompetitor->previousSiusRow().split(';');
//                                Lask lask(thisCompetitor->previousSiusRow());

//                                thisCompetitor->setShot(0, thisCompetitor->previousSiusRow());
//                                statusBarInfoChanged(thisCompetitor->name() + tr(" lask 1 = ") + lask.getSLask());
//                                if(verbose)
//                                    QTextStream(stdout) << "rowParts[12]: " << rowParts[12] << "voistlus = true" << "previousRowParts[10]: " << previousRowParts[10] << "\n\n";
//                            }
//                        }
//                    }
//                }else if(row.startsWith("_SHOT") && competitionStarted){   //Võistluslasu rida

//                    int shotNo = rowParts[13].toInt() - 1 /*- shootOffShots*/;  //Siusist tulnud lasu Nr, koodi lühendamise eesmärgil
//                    Lask lask(row);

//                    thisCompetitor->setShot(shotNo, row);
//                    statusBarInfoChanged(QString("%1%2%3 = %4").arg(thisCompetitor->name()).arg(tr(" lask ")).arg(shotNo).arg(lask.getSLask()));
//                }
//            }
//        }
    if(thisCompetitor == nullptr)
            statusInfoChanged(tr("Sellise ID'ga võistlejat ei leitud: ") + shotData.id);
//    }
    sumAllTeams();
}

void TeamsTable2022::setTableName(QString newName)
{
    m_tableName = newName;
}

void TeamsTable2022::sumAllTeams()
{
    if(verbose)
        QTextStream(stdout) << "TeamsTable2022::sumAllTeams()" << endl;

    int currentShotIndex = lastValidShotIndex();

    for (int shotNo = 0; shotNo <= currentShotIndex; shotNo++ ) {
        std::vector<int> results;
        bool shotMissing = false;
//        QHash<Team2022*, int> teamsResults;
        foreach (Team2022 *team, m_teams) {
            if (team->result10At(shotNo) >= 0)
                results.push_back(team->result10At(shotNo));
            else if (team->isActive())
                shotMissing = true;
//            teamsResults.insert(team, team->result10At(i));
        }

        if(verbose && shotNo == currentShotIndex)
            QTextStream(stdout) << "TeamsTable2022::sumAllTeams(): currentShotIndex = " << currentShotIndex << " shotMissing = " << shotMissing << " results.size() = " << results.size() << endl;

        if (!shotMissing) { // Add points only if all competitors have finished or 0 is added if anyone is unable to shoot
            std::sort(results.begin(), results.end());

            //        std::set<int> uniqueResults(results.begin(), results.end());

            QMap<int, int> counts;

            foreach(int result, results) {
                int count = std::count(results.begin(), results.end(), result);
                counts[result] = count;
            }

            QMap<int, int> pointsAllocation;

            if (m_teams.size() == 2) {    // Medal match
                if (m_teams.first()->result10At(shotNo) > 0 && m_teams.first()->result10At(shotNo) == m_teams.last()->result10At(shotNo)) {
                    m_teams.first()->setPoints(shotNo, 10); // Draw
                    m_teams.last()->setPoints(shotNo, 10);
                } else if (m_teams.first()->result10At(shotNo) > m_teams.last()->result10At(shotNo)) {
                    m_teams.first()->setPoints(shotNo, 20); // First had a better result
                    m_teams.last()->setPoints(shotNo, 0);
                } else {
                    m_teams.first()->setPoints(shotNo, 0);
                    m_teams.last()->setPoints(shotNo, 20); // Second must have had a better result
                }
                if (m_teams.first()->result10At(shotNo) == 0)
                    m_teams.first()->setPoints(shotNo, 0);
                if (m_teams.last()->result10At(shotNo) == 0)
                    m_teams.last()->setPoints(shotNo, 0);
            } else {
                int missingResults = m_teams.size() - results.size();    // Maximum points that are given is 4
                int startingIndex = m_teams.size() - 1 - missingResults;
                for (int j = startingIndex; j >= 0; j--) {
                    if(verbose)
                        QTextStream(stdout) << "TeamsTable2022::sumAllTeams4(): j = " << j << " results.size() = " << (int)results.size() << endl;

                    int currentResultCount = counts.value(results[j]);
                    if (currentResultCount == 1)
                        pointsAllocation[results[j]] = (j + 1 + missingResults) * 10;
                    else {
                        double dCurrentResultCount = currentResultCount;
                        double maxPoints = j + 1 + missingResults;
                        int pointsToAllocate = (dCurrentResultCount / 2 * (2 * maxPoints + (dCurrentResultCount - 1) * -1)) * 10;
                        if (!pointsAllocation.contains(results[j])) {
                            //                        int points = pointsToAllocate / currentResultCount;
                            //                        pointsAllocation[results[j]] = ((currentResultCount * j + currentResultCount) * 10) / currentResultCount;
                            pointsAllocation[results[j]] = pointsToAllocate / currentResultCount;
                        }
                    }
                }

                foreach (Team2022 *team, m_teams) {
                    if (team->result10At(shotNo) > 0)
                        team->setPoints(shotNo, pointsAllocation[team->result10At(shotNo)]);
                    if (team->result10At(shotNo) == 0)
                        team->setPoints(shotNo, 0); // In case of late or missed shot, points won't be given
                    //            teamsResults.insert(team, team->result10At(i));
                }
            }
        }
    }
    emit updateSpectatorWindow();
}

QString TeamsTable2022::tableName() const
{
    return m_tableName;
}

QJsonObject TeamsTable2022::toJson() const
{
    QJsonArray teamsArray;
    foreach(Team2022 *team, m_teams){
//        QJsonObject teamObj;
//        team->toJson(teamObj);
        teamsArray.append(team->toJson());
    }
    QJsonObject json;
    json["teams"] = teamsArray;
    return json;
}

#include "teamstable.h"

TeamsTable::TeamsTable(QWidget *parent) : QWidget(parent)
{
//    m_parent = parent;
    vBox = new QVBoxLayout;
    setLayout(vBox);
    setStyleSheet("border:1px solid rgb(0, 255, 0); ");
}

void TeamsTable::clear()
{
    foreach (Team *team, teams) {
        vBox->removeWidget(team);
        team->deleteLater();
    }
    teams.clear();
}

void TeamsTable::createLayout(QJsonObject &jsonObj)
{
    int teamsNo = 0;

    if(jsonObj["Teams"].isArray()){
        teamsNo = jsonObj["Teams"].toArray().size();
    } else {
        jsonObj["Teams"].toInt();
    }

    for(int i = 0; i < teamsNo; i++){
        Team *team = nullptr;
        if(jsonObj["Teams"].isArray()){
            QJsonObject teamJson = jsonObj["Teams"].toArray().at(i).toObject();
            team = new Team(teamJson, i+1);
        } else {
            team = new Team(jsonObj, i+1);
        }
//        if(m_parent != nullptr){
            connect(team, &Team::teamUpdated, this, &TeamsTable::updateSpectatorWindow);
            connect(team, &Team::statusInfo, this, &TeamsTable::statusInfoChanged);
//        }
        teams.append(team);
        vBox->addWidget(team);
    }
}

void TeamsTable::readSiusInfo(SiusShotData shotData)
{
    if(verbose)
        QTextStream(stdout) << "readSiusInfo()" << endl;

    // FIXME To be implemented
//    for(QString row : lines){
        //Search for competitor whose line was received:
//        QStringList rowParts = row.split(';');
    Competitor *thisCompetitor = nullptr;
    foreach (Team *team, teams) {
        if(team->getCompetitorWithID(shotData.id) != nullptr){
            thisCompetitor = team->getCompetitorWithID(shotData.id);

            bool success = thisCompetitor->readSiusShot(shotData);
            if (success)
                statusInfoChanged(QString("Lisatud: %1%2%3 = %4").arg(thisCompetitor->name()).arg(tr(" lask ")).arg(shotData.siusShotNo).arg(shotData.shot.getSLask()));
            else statusInfoChanged(QString("Viga, ei õnnestunud lisada: %1%2%3 = %4").arg(thisCompetitor->name()).arg(tr(" lask ")).arg(shotData.siusShotNo).arg(shotData.shot.getSLask()));
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

void TeamsTable::sumAllTeams()
{
    foreach (Team *team, teams) {
        team->sumAll();
    }
}

QJsonObject TeamsTable::toJson() const
{
    QJsonArray teamsArray;
    foreach(Team *team, teams){
//        QJsonObject teamObj;
//        team->toJson(teamObj);
        teamsArray.append(team->toJson());
    }
    QJsonObject json;
    json["Teams"] = teamsArray;
    return json;
}

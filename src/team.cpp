#include "team.h"

Team::Team(QJsonObject &configJson, int index, QWidget *parent) : QWidget(parent)
{
    int competitorsInTeam = 0;

    QGridLayout *layout = new QGridLayout;

    teamName.setToolTip(tr("Võistkonna nimi"));
    teamName.setText("Nimi");
    indexLabel.setText(QString("%1.").arg(index));
    layout->addWidget(&indexLabel);
    layout->addWidget(&teamName, 0, 1);

    if(configJson["Members_in_team"].isDouble()){  //Event config file
        if(verbose)
            QTextStream(stdout) << "Team::Team(uus)" << endl;
        competitorsInTeam = configJson["Members_in_team"].toInt();

        Competitor *competitor = new Competitor(configJson["Shots"].toArray());
        connect(competitor, &Competitor::newShot, this, &Team::sum);

        teamCompetitors.append(competitor);
        layout->addWidget(competitor, 0, 2);

        for(int i = 1; i < competitorsInTeam; i++){
            Competitor *competitor = new Competitor(configJson["Shots"].toArray());
            connect(competitor, &Competitor::newShot, this, &Team::sum);

            teamCompetitors.append(competitor);
            layout->addWidget(competitor, i, 2);
        }

        sumLabel.setText("0,0");

    }else if(configJson["Members_in_team"].isArray()){  //Loaded finals file
        if(verbose)
            QTextStream(stdout) << "Team::Team(failist)" << endl;
        QJsonArray competitorsArray = configJson["Members_in_team"].toArray();
        competitorsInTeam = competitorsArray.size();

        for (int i = 0; i < competitorsArray.size(); i++) {
            Competitor *competitor = new Competitor(competitorsArray.at(i).toObject());
            connect(competitor, &Competitor::newShot, this, &Team::sum);

            teamCompetitors.append(competitor);
            layout->addWidget(competitor, i, 2);
        }
        teamName.setText(configJson["teamName"].toString());
    }else
        QMessageBox::critical(this, tr("Viga!"), tr("Vigane fail!\nMembers_in_team != isDouble && != isArray"), QMessageBox::Ok);

    layout->addWidget(&sumLabel, 0, layout->columnCount());

    if(competitorsInTeam < 2){  //Individual finals
        teamName.hide();
        sumLabel.hide();
    }

//    if(verbose)
//        QTextStream(stdout) << "layout->rowCount(): " << layout->rowCount() << endl;

    layout->setContentsMargins(0, 2, 0, 2);
    setLayout(layout);
}

Team::~Team()
{
    if(verbose)
        QTextStream(stdout) << "Team::~Team()" << endl;
    foreach (Competitor *competitor, teamCompetitors) {
        competitor->deleteLater();
    }
    teamCompetitors.clear();
}

void Team::sum()
{
    if(teamCompetitors.size() > 1){
        int teamSum = 0;
        for(int i = 0; i < teamCompetitors.size(); i++){
            teamSum += teamCompetitors.at(i)->current10Sum();
            QTextStream(stdout) << "current10Sum: " << teamCompetitors.at(i)->current10Sum() << endl;
        }

        double dTeamSum = teamSum;
        dTeamSum /= 10;
        sumLabel.setText(QString("%1").arg(dTeamSum).replace('.', ','));
    }
}

void Team::toJson(QJsonObject &json) const
{
    QJsonArray competitorsArray;
    foreach (Competitor *competitor, teamCompetitors){
        QJsonObject competitorObj;
        competitor->toJson(competitorObj);
        competitorsArray.append(competitorObj);
    }
    json["Members_in_team"] = competitorsArray;
    json["teamName"] = teamName.text();
}

#include "team.h"

Team::Team(QJsonObject configJson, int index, QWidget *parent) : QWidget(parent)
{
    int competitorsInTeam = configJson["Members_in_team"].toInt();
//    int shots = configJson["Shots"].toArray()[0].toInt();

    QGridLayout *layout = new QGridLayout;
//    QVBoxLayout *vBox = new QVBoxLayout;

    indexLabel = new QLabel(QString("%1.").arg(index));
    layout->addWidget(indexLabel);

    if(competitorsInTeam > 1)   //Team finals
        layout->addWidget(new QLineEdit(QString("Nimi")), 0, 1);

    Competitor *competitor = new Competitor(configJson["Shots"].toArray());
    connect(competitor, &Competitor::newShot, this, &Team::sum);

    teamCompetitors.append(competitor);
    layout->addWidget(competitor, 0, 2);
//    vBox->addLayout(layout);

    for(int i = 1; i < competitorsInTeam; i++){
        Competitor *competitor = new Competitor(configJson["Shots"].toArray());
        connect(competitor, &Competitor::newShot, this, &Team::sum);

        teamCompetitors.append(competitor);
        layout->addWidget(competitor, i, 2);
    }

    if(competitorsInTeam > 1){  //Team finals
        sumLabel = new QLabel("0,0");
        layout->addWidget(sumLabel, 0, layout->columnCount());
    }
//    if(verbose)
//        QTextStream(stdout) << "layout->rowCount(): " << layout->rowCount() << endl;

    layout->setContentsMargins(0, 2, 0, 2);
    setLayout(layout);
}

Team::~Team()
{

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
        sumLabel->setText(QString("%1").arg(dTeamSum).replace('.', ','));
    }
}

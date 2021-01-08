#include "team.h"

Team::Team(QJsonObject &configJson, int index, QWidget *parent) : QWidget(parent)
{
    int competitorsInTeam = 0;

    QGridLayout *layout = new QGridLayout;

    m_teamName.setToolTip(tr("Võistkonna nimi"));
    m_teamName.setText("");
    m_indexLabel.setText(QString("%1.").arg(index));
    layout->addWidget(&m_indexLabel);
    layout->addWidget(&m_teamName, 0, 1);

    if(configJson["Members_in_team"].isDouble()){  //Event config file
        if(verbose)
            QTextStream(stdout) << "Team::Team(uus)" << endl;
        competitorsInTeam = configJson["Members_in_team"].toInt();

        Competitor *competitor = new Competitor(index * 10 + 1, configJson["Shots"].toArray());
        connect(competitor, &Competitor::newShot, this, &Team::sum);
        connect(competitor, &Competitor::statusInfo, this, &Team::statusInfo);

        m_teamCompetitors.append(competitor);
        layout->addWidget(competitor, 0, 2);

        for(int i = 1; i < competitorsInTeam; i++){
            Competitor *competitor = new Competitor(index * 10 + 1 + i, configJson["Shots"].toArray());
            connect(competitor, &Competitor::newShot, this, &Team::sum);
            connect(competitor, &Competitor::statusInfo, this, &Team::statusInfo);

            m_teamCompetitors.append(competitor);
            layout->addWidget(competitor, i, 2);
        }

        m_sumLabel.setText("0,0");

    }else if(configJson["Members_in_team"].isArray()){  //Loaded finals file
        if(verbose)
            QTextStream(stdout) << "Team::Team(failist laadimine)" << endl;
        QJsonArray competitorsArray = configJson["Members_in_team"].toArray();
        competitorsInTeam = competitorsArray.size();

        for (int i = 0; i < competitorsArray.size(); i++) {
            Competitor *competitor = new Competitor(competitorsArray.at(i).toObject());
            connect(competitor, &Competitor::newShot, this, &Team::sum);
            connect(competitor, &Competitor::statusInfo, this, &Team::statusInfo);

            m_teamCompetitors.append(competitor);
            layout->addWidget(competitor, i, 2);
        }
        m_teamName.setText(configJson["teamName"].toString());
    }else
        QMessageBox::critical(this, tr("Viga!"), tr("Vigane fail!\nMembers_in_team != isDouble && != isArray"), QMessageBox::Ok);

    layout->addWidget(&m_sumLabel, 0, layout->columnCount());

    if(competitorsInTeam < 2){  //Individual finals
        m_teamName.hide();
        m_sumLabel.hide();
    }

//    if(verbose)
//        QTextStream(stdout) << "layout->rowCount(): " << layout->rowCount() << endl;

    layout->setContentsMargins(0, 2, 0, 2);
    setLayout(layout);
    sum();
}

Team::~Team()
{
    if(verbose)
        QTextStream(stdout) << "Team::~Team()" << endl;
    foreach (Competitor *competitor, m_teamCompetitors) {
        competitor->deleteLater();
    }
    m_teamCompetitors.clear();
}

Competitor* Team::getCompetitorWithID(int id)
{
    foreach (Competitor *competitor, m_teamCompetitors){
        if(competitor->id() == id)
            return competitor;
    }
    return nullptr;
}

QString Team::index()
{
    return m_indexLabel.text();
}

QString Team::lastSum()
{
    return m_sumLabel.text();
}

void Team::sum()
{
    if(verbose)
        QTextStream(stdout) << m_indexLabel.text() << " Team::sum()" << endl;
    if(m_teamCompetitors.size() > 1){
        int teamSum = 0;
        for(int i = 0; i < m_teamCompetitors.size(); i++){
            teamSum += m_teamCompetitors.at(i)->current10Sum();
            if(verbose)
                QTextStream(stdout) << "current10Sum: " << m_teamCompetitors.at(i)->current10Sum() << endl;
        }

        double dTeamSum = teamSum;
        dTeamSum /= 10;
        QTextStream(stdout) << "dTeamSum = " << dTeamSum << endl;
        m_sumLabel.setText(QString("%1").arg(dTeamSum));
        QTextStream(stdout) << "Team::sum()2" << endl;
        m_sumLabel.setText(m_sumLabel.text().replace('.', ','));
        QTextStream(stdout) << "Team::sum()3" << endl;
    }else
        m_sumLabel.setText(m_teamCompetitors.at(0)->lastSum());
    emit teamUpdated();
}

void Team::sumAll()
{
    foreach (Competitor *competitor, m_teamCompetitors){
        competitor->sum();
    }
}

QVector<Competitor *> Team::teamCompetitors()
{
    return m_teamCompetitors;
}

QString Team::teamName()
{
    return m_teamName.text();
}

QJsonObject Team::toJson() const
{
    QJsonArray competitorsArray;
    foreach (Competitor *competitor, m_teamCompetitors){
//        QJsonObject competitorObj;
//        competitor->toJson(competitorObj);
        competitorsArray.append(competitor->toJson());
    }
    QJsonObject json;
    json["Members_in_team"] = competitorsArray;
    json["teamName"] = m_teamName.text();
    return json;
}

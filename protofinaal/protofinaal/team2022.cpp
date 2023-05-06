#include "team2022.h"

Team2022::Team2022(QJsonObject &configJson, int index, const bool scoringWithPoints, QWidget *parent) : QWidget(parent)
{
    int competitorsInTeam = 1;  // Defaults to 1

    QGridLayout *layout = new QGridLayout;

    m_teamName.setToolTip(tr("Võistkonna nimi"));
    m_teamName.setPlaceholderText("Võistkond");
    m_teamName.setText("");
    m_indexLabel.setText(QString("%1.").arg(index));
    layout->addWidget(&m_indexLabel);
    layout->addWidget(&m_teamName, 0, 1);

    if(configJson["membersInTeam"].isDouble()){  //Event config file
        if(verbose)
            QTextStream(stdout) << "Team::Team(uus)" << endl;
        competitorsInTeam = configJson["membersInTeam"].toInt();

        Competitor2022 *competitor = new Competitor2022(index * 10 + 1, configJson["shots"].toArray(), scoringWithPoints);
        connect(competitor, &Competitor2022::newShot, this, &Team2022::calculatePointsTotal);
        connect(competitor, &Competitor2022::newShot, this, &Team2022::modified);
        connect(competitor, &Competitor2022::modified, this, &Team2022::modified);
        connect(competitor, &Competitor2022::statusInfo, this, &Team2022::statusInfo);

        m_teamCompetitors.append(competitor);
        layout->addWidget(competitor, 0, 2);

        for(int i = 1; i < competitorsInTeam; i++){
            Competitor2022 *competitor = new Competitor2022(index * 10 + 1 + i, configJson["shots"].toArray(), scoringWithPoints);
            connect(competitor, &Competitor2022::newShot, this, &Team2022::calculatePointsTotal);
            connect(competitor, &Competitor2022::newShot, this, &Team2022::modified);
            connect(competitor, &Competitor2022::modified, this, &Team2022::modified);
            connect(competitor, &Competitor2022::statusInfo, this, &Team2022::statusInfo);

            m_teamCompetitors.append(competitor);
            layout->addWidget(competitor, i, 2);
        }

        m_sumLabel.setText("0,0");

    }else if(configJson["membersInTeam"].isArray()){  //Loaded finals file
        if(verbose)
            QTextStream(stdout) << "Team::Team(failist laadimine)" << endl;
        QJsonArray competitorsArray = configJson["membersInTeam"].toArray();
        competitorsInTeam = competitorsArray.size();

        for (int i = 0; i < competitorsArray.size(); i++) {
            Competitor2022 *competitor = new Competitor2022(competitorsArray.at(i).toObject(), scoringWithPoints);
            connect(competitor, &Competitor2022::newShot, this, &Team2022::calculatePointsTotal);
            connect(competitor, &Competitor2022::newShot, this, &Team2022::modified);
            connect(competitor, &Competitor2022::modified, this, &Team2022::modified);
            connect(competitor, &Competitor2022::statusInfo, this, &Team2022::statusInfo);

            m_teamCompetitors.append(competitor);
            layout->addWidget(competitor, i, 2);
        }
        m_teamName.setText(configJson["teamName"].toString());
    }else
        QMessageBox::critical(this, tr("Viga!"), tr("Vigane fail!\nMembersInTeam != isDouble && != isArray"), QMessageBox::Ok);

    layout->addWidget(&m_sumLabel, 0, layout->columnCount());

    if(competitorsInTeam < 2){  //Individual finals
        m_teamName.hide();
        m_sumLabel.hide();
    }

//    if(verbose)
//        QTextStream(stdout) << "layout->rowCount(): " << layout->rowCount() << endl;

    layout->setContentsMargins(0, 2, 0, 2);
    setLayout(layout);
    calculatePointsTotal();
}

Team2022::~Team2022()
{
    if(verbose)
        QTextStream(stdout) << "Team::~Team()" << endl;
    foreach (Competitor2022 *competitor, m_teamCompetitors) {
        competitor->deleteLater();
    }
    m_teamCompetitors.clear();
}

Competitor2022* Team2022::getCompetitorWithID(int id)
{
    foreach (Competitor2022 *competitor, m_teamCompetitors){
        if(competitor->id() == id)
            return competitor;
    }
    return nullptr;
}

int Team2022::index()
{
    return m_indexLabel.text().remove(".").toInt();
}

bool Team2022::isActive() const
{
    return m_teamCompetitors.at(0)->isActive();
}

QString Team2022::teamTotal()
{
    if (m_teamCompetitors.size() > 0)
        return m_teamCompetitors.at(0)->total();
    else
        return "";
}

int Team2022::lastValidShotIndex() const
{
    int largestIndex = -1;
    foreach(Competitor2022 *competitor, m_teamCompetitors) {
        int lastIndex = competitor->lastValidShotIndex();
        if (lastIndex > largestIndex)
            largestIndex = lastIndex;
    }

    return largestIndex;
}

int Team2022::result10At(int index)
{
    int teamSum = -999;
    foreach(Competitor2022 *competitor, m_teamCompetitors) {
        auto shotOpt = competitor->shotAt(index);
        if (shotOpt.has_value()) {
            if (teamSum == -999) {
                teamSum = shotOpt.value().get10Lask();
            } else {
                teamSum += shotOpt.value().get10Lask();
            }
        }
    }
    return teamSum;
}

void Team2022::setFirstCompetitiorData(int id, QString displayName, QString result)
{
    if (m_teamCompetitors.size() > 0) {
        m_teamCompetitors.first()->setId(id);
        m_teamCompetitors.first()->setDisplayName(displayName);
        m_teamCompetitors.first()->setQualificationResult(result);
    }
}

bool Team2022::setPoints(int shotNo, int points)
{
    if (m_teamCompetitors.size() > 0) {
        m_teamCompetitors.first()->setPoints(shotNo, points);   // Points are added and shown only on the first competitor
        return true;
    } else
        return false;
}

QString Team2022::resultAt(int index)
{
    if (m_teamCompetitors.size() > 0)
        return m_teamCompetitors.first()->resultAt(index);
    else return 0;
}

void Team2022::calculatePointsTotal()
{
    if(verbose)
        QTextStream(stdout) << m_indexLabel.text() << " Team::pointsTotal()" << endl;
    if(m_teamCompetitors.size() > 0){
//        int teamSum = 0;
//        for(int i = 0; i < m_teamCompetitors.size(); i++){
//            teamSum += m_teamCompetitors.at(i)->current10Sum();
//            if(verbose)
//                QTextStream(stdout) << "current10Sum: " << m_teamCompetitors.at(i)->current10Sum() << endl;
//        }

//        double dTeamSum = teamSum;
//        dTeamSum /= 10;
//        QTextStream(stdout) << "dTeamSum = " << dTeamSum << endl;
//        m_sumLabel.setText(QString("%1").arg(dTeamSum));
//        QTextStream(stdout) << "Team::sum()2" << endl;
//        m_sumLabel.setText(m_sumLabel.text().replace('.', ','));
//        QTextStream(stdout) << "Team::sum()3" << endl;
//    }else
        m_teamCompetitors.at(0)->sum();
        m_sumLabel.setText(m_teamCompetitors.at(0)->total());
    }
    emit teamUpdated();
}

void Team2022::sumAll()
{
    foreach (Competitor2022 *competitor, m_teamCompetitors){
        competitor->sum();
    }
}

QVector<Competitor2022 *> Team2022::teamCompetitors()
{
    return m_teamCompetitors;
}

QString Team2022::teamName()
{
    QString name = m_teamCompetitors.first()->name();
    if (m_teamCompetitors.size() == 2)
        name.append("/" + m_teamCompetitors.at(1)->name());
    else if (m_teamCompetitors.size() > 2)
        name = m_teamName.text();
    return name;
}

QJsonObject Team2022::toJson() const
{
    QJsonArray competitorsArray;
    foreach (Competitor2022 *competitor, m_teamCompetitors){
//        QJsonObject competitorObj;
//        competitor->toJson(competitorObj);
        competitorsArray.append(competitor->toJson());
    }
    QJsonObject json;
    json["membersInTeam"] = competitorsArray;
    json["teamName"] = m_teamName.text();
    return json;
}

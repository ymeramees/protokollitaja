#include "team.h"

Team::Team(QJsonObject &configJson, int index, const bool scoringWithPoints, QWidget *parent) : QWidget(parent)
{
    int competitorsInTeam = 1;  // Defaults to 1

    QGridLayout *layout = new QGridLayout;

    m_teamName.setToolTip(tr("Team name"));
    m_teamName.setPlaceholderText(tr("Team"));
    m_teamName.setText("");
    m_indexLabel.setText(QString("%1.").arg(index));
    layout->addWidget(&m_indexLabel);
    layout->addWidget(&m_teamName, 0, 1);

    if(configJson["membersInTeam"].isDouble()){  //Event config file
        if(verbose)
            QTextStream(stdout) << "Team::Team(uus)" << Qt::endl;
        competitorsInTeam = configJson["membersInTeam"].toInt();

        Competitor *competitor = new Competitor(index * 10 + 1, configJson["shots"].toArray(), scoringWithPoints);
        connect(competitor, &Competitor::newShot, this, &Team::calculatePointsTotal);
        connect(competitor, &Competitor::newShot, this, &Team::modified);
        connect(competitor, &Competitor::modified, this, &Team::modified);
        connect(competitor, &Competitor::statusInfo, this, &Team::statusInfo);

        m_teamCompetitors.append(competitor);
        layout->addWidget(competitor, 0, 2);

        for(int i = 1; i < competitorsInTeam; i++){
            Competitor *competitor = new Competitor(index * 10 + 1 + i, configJson["shots"].toArray(), scoringWithPoints);
            connect(competitor, &Competitor::newShot, this, &Team::calculatePointsTotal);
            connect(competitor, &Competitor::newShot, this, &Team::modified);
            connect(competitor, &Competitor::modified, this, &Team::modified);
            connect(competitor, &Competitor::statusInfo, this, &Team::statusInfo);

            m_teamCompetitors.append(competitor);
            layout->addWidget(competitor, i, 2);
        }

        m_sumLabel.setText("0,0");

    }else if(configJson["membersInTeam"].isArray()){  //Loaded finals file
        if(verbose)
            QTextStream(stdout) << "Team::Team(failist laadimine)" << Qt::endl;
        QJsonArray competitorsArray = configJson["membersInTeam"].toArray();
        competitorsInTeam = competitorsArray.size();

        for (int i = 0; i < competitorsArray.size(); i++) {
            Competitor *competitor = new Competitor(competitorsArray.at(i).toObject(), scoringWithPoints);
            connect(competitor, &Competitor::newShot, this, &Team::calculatePointsTotal);
            connect(competitor, &Competitor::newShot, this, &Team::modified);
            connect(competitor, &Competitor::modified, this, &Team::modified);
            connect(competitor, &Competitor::statusInfo, this, &Team::statusInfo);

            m_teamCompetitors.append(competitor);
            layout->addWidget(competitor, i, 2);
        }
        m_teamName.setText(configJson["teamName"].toString());
    }else
        QMessageBox::critical(this, tr("Error!"), tr("Broken file!\nMembersInTeam != isDouble && != isArray"), QMessageBox::Ok);

    layout->addWidget(&m_sumLabel, 0, layout->columnCount());

    if(competitorsInTeam < 2){  //Individual finals
        m_teamName.hide();
        m_sumLabel.hide();
    }

//    if(verbose)
//        QTextStream(stdout) << "layout->rowCount(): " << layout->rowCount() << Qt::endl;

    layout->setContentsMargins(0, 2, 0, 2);
    setLayout(layout);
    calculatePointsTotal();
}

Team::~Team()
{
    if(verbose)
        QTextStream(stdout) << "Team::~Team()" << Qt::endl;
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

int Team::index()
{
    return m_indexLabel.text().remove(".").toInt();
}

bool Team::isActive() const
{
    return m_teamCompetitors.at(0)->isActive();
}

int Team::team10Total() const
{
    if (m_teamCompetitors.size() > 0) {
        // sumAll();
        int teamSum = 0;
        foreach(Competitor *competitor, m_teamCompetitors) {
            teamSum += competitor->current10Sum();
        }
        return teamSum;
    } else
        return 0;
}

QString Team::teamTotal() const
{
    if (m_teamCompetitors.size() > 0) {
        // TODO make a better solution for this
        // sumAll();
        int teamSum = 0;
        foreach(Competitor *competitor, m_teamCompetitors) {
            teamSum += competitor->total().replace(',', '.').toDouble() * 10;
        }
        double dTeamSum = teamSum;
        return QString("%1").arg(dTeamSum / 10).replace('.', ',');
    } else
        return "";
}

int Team::lastValidShotIndex() const
{
    int largestIndex = -1;
    foreach(Competitor *competitor, m_teamCompetitors) {
        int lastIndex = competitor->lastValidShotIndex();
        if (lastIndex > largestIndex)
            largestIndex = lastIndex;
    }

    return largestIndex;
}

int Team::result10At(int index) const
{
    int teamSum = -999;
    foreach(Competitor *competitor, m_teamCompetitors) {
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

void Team::setCompetitorsData(QJsonArray data)
{
    QListIterator<Competitor*> iterator(m_teamCompetitors);
    iterator.toFront();
    for (const QJsonValue &competitorJson: std::as_const(data)) {
        if (iterator.hasNext())
            iterator.next()->setData(competitorJson.toObject());
    }
}

void Team::setFirstCompetitiorData(int id, QString displayName, QString result)
{
    if (m_teamCompetitors.size() > 0) {
        m_teamCompetitors.first()->setId(id);
        m_teamCompetitors.first()->setDisplayName(displayName);
        m_teamCompetitors.first()->setQualificationResult(result);
    }
}

bool Team::setPoints(int shotNo, int points)
{
    if (m_teamCompetitors.size() > 0) {
        // TODO to be implemented properly for teams
        m_teamCompetitors.first()->setPoints(shotNo, points);   // Points are added and shown only on the first competitor
        return true;
    } else
        return false;
}

QString Team::resultAt(int index) const
{
    if (m_teamCompetitors.size() == 1)
        return m_teamCompetitors.first()->resultAt(index);
    else if (m_teamCompetitors.size() > 0) {
        // sumAll();
        // TODO make a better solution for this
        int teamSum = 0;
        foreach(Competitor *competitor, m_teamCompetitors) {
            teamSum += competitor->resultAt(index).replace(',', '.').toDouble() * 10;
        }
        double dTeamSum = teamSum;
        return QString("%1").arg(dTeamSum / 10).replace('.', ',');
    } else return 0;
}

void Team::calculatePointsTotal()
{
    if(verbose)
        QTextStream(stdout) << m_indexLabel.text() << " Team::pointsTotal()" << Qt::endl;
    sumAll();
    if(m_teamCompetitors.size() > 1){
//        int teamSum = 0;
//        for(int i = 0; i < m_teamCompetitors.size(); i++){
//            teamSum += m_teamCompetitors.at(i)->current10Sum();
//            if(verbose)
//                QTextStream(stdout) << "current10Sum: " << m_teamCompetitors.at(i)->current10Sum() << Qt::endl;
//        }

//        double dTeamSum = teamSum;
//        dTeamSum /= 10;
//        QTextStream(stdout) << "dTeamSum = " << dTeamSum << Qt::endl;
//        m_sumLabel.setText(QString("%1").arg(dTeamSum));
//        QTextStream(stdout) << "Team::sum()2" << Qt::endl;
//        m_sumLabel.setText(m_sumLabel.text().replace('.', ','));
//        QTextStream(stdout) << "Team::sum()3" << Qt::endl;
//    }else
        // TODO make a better solution for this
        int teamSum = 0;
        foreach(Competitor *competitor, m_teamCompetitors) {
            teamSum += competitor->total().replace(',', '.').toDouble() * 10;
        }
        double dTeamSum = teamSum;
        m_sumLabel.setText(QString("%1").arg(dTeamSum / 10).replace('.', ','));
    } else if (m_teamCompetitors.size() == 1) {
        m_sumLabel.setText(m_teamCompetitors.at(0)->total());
    }
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

QString Team::teamName() const
{
    QString name = m_teamCompetitors.first()->name();
    if (m_teamCompetitors.size() == 2)
        name.append("/" + m_teamCompetitors.at(1)->name());
    else if (m_teamCompetitors.size() > 2)
        name = m_teamName.text();
    return name;
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
    json["membersInTeam"] = competitorsArray;
    json["teamName"] = m_teamName.text();
    return json;
}

XlsTeamBlock Team::toXlsData(int maxShots, int lastShotIdx, const QString &rank) const
{
    XlsTeamBlock block;
    
    // Individual competitor rows
    foreach (Competitor *competitor, m_teamCompetitors) {
        block.competitorRows.append(competitor->toXlsData(maxShots, lastShotIdx));
    }

    // Team total row - sum all competitors' series
    XlsShotRow teamRow;
    teamRow.rank = rank;
    teamRow.name = teamName();

    // Determine number of series from first competitor
    if (!block.competitorRows.isEmpty()) {
        int seriesCount = block.competitorRows.first().series.size();
        
        for (int seriesIdx = 0; seriesIdx < seriesCount; seriesIdx++) {
            XlsSeries teamSeries;
            double teamSeriesSum = 0.0;
            
            // Determine series size from first competitor
            int seriesSize = 0;
            if (seriesIdx < block.competitorRows.first().series.size()) {
                seriesSize = block.competitorRows.first().series[seriesIdx].shots.size();
            }
            
            // Sum shots at each position in this series
            for (int shotInSeries = 0; shotInSeries < seriesSize; shotInSeries++) {
                int teamShot10 = 0;
                bool hasValidShot = false;
                
                foreach (const XlsShotRow &compRow, block.competitorRows) {
                    if (seriesIdx < compRow.series.size() && 
                        shotInSeries < compRow.series[seriesIdx].shots.size()) {
                        int shot = compRow.series[seriesIdx].shots[shotInSeries];
                        if (shot >= 0) {
                            teamShot10 += shot;
                            hasValidShot = true;
                        }
                    }
                }
                
                teamSeries.shots.append(hasValidShot ? teamShot10 : -1);
                if (hasValidShot) {
                    teamSeriesSum += teamShot10 / 10.0;
                }
            }
            
            teamSeries.total = QString::number(teamSeriesSum, 'f', 1);
            teamRow.series.append(teamSeries);
        }
    }
    
    // Calculate team total as sum of competitor totals
    double teamTotalValue = 0.0;
    foreach (const XlsShotRow &compRow, block.competitorRows) {
        if (!compRow.total.isEmpty()) {
            teamTotalValue += compRow.total.toDouble();
        }
    }
    teamRow.total = QString::number(teamTotalValue, 'f', 1);
    block.teamTotalRow = teamRow;

    return block;
}

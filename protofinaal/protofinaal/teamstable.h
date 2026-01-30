#ifndef TEAMSTABLE_H
#define TEAMSTABLE_H

#include <QVBoxLayout>
#include <set>
#include "team.h"
#include "xlsexportservice.h"

//class Protofinaal;

class TeamsTable : public QWidget
{
    Q_OBJECT
public:
    struct Result {
        QString name;   ///< First/second competitor name or team name.
        QString shotValue;   ///< Shot value.
        QString seriesOrPoints; ///< Series result or points for this shot.
        QString totalScore;   ///< Result/Points in total.
        int total10Score;    ///< Total score multiplied by 10
    };
    explicit TeamsTable(QWidget *parent = nullptr);
    void clear();
    void createLayout(QJsonObject jsonObj, const bool scoringWithPoints);
    void deleteAllShots();
    QMap<int, Result> getCurrentResults() const;
    QMultiMap<int, Result> getSortedResults() const;
    QVector<Team*> getTeams() const;
    int lastValidShotIndex() const;
    void setCompetitiorsData(QStringList rows);
    void setCompetitiorsData(int index, QJsonArray data);
    void setTableName(QString newName);
    QString tableName() const;
    int teamsCount() const;
    int totalCompetitorsCount() const;
    QJsonObject toJson() const;
    QVector<XlsTeamBlock> toXlsData(int maxShots) const;

signals:
    void modified();
    void statusInfoChanged(QString newStatusInfo);
    void updateSpectatorWindow();

public slots:
    void readSiusInfo(SiusShotData shotData);
    void sumAllTeams();

private:
//    Protofinaal *m_parent = nullptr;
    QString m_tableName;
    QVector<Team*> m_teams;
    QVBoxLayout *vBox = nullptr;
    void sortTeams();

};

#endif // TEAMSTABLE_H

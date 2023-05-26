#ifndef TEAMSTABLE2022_H
#define TEAMSTABLE2022_H

#include <QVBoxLayout>
#include <set>
#include "team2022.h"

//class Protofinaal;

class TeamsTable2022 : public QWidget
{
    Q_OBJECT
public:
    struct Result {
        QString name;   ///< First competitor name.
        QString shotValue;   ///< Shot value.
        QString seriesOrPoints; ///< Series result or points for this shot.
        QString totalScore;   ///< Result/Points in total.
    };
    explicit TeamsTable2022(QWidget *parent = nullptr);
    void clear();
    void createLayout(QJsonObject jsonObj, const bool scoringWithPoints);
    QMap<int, Result> getCurrentResults() const;
    QMultiMap<int, Result> getSortedResults() const;
    int lastValidShotIndex() const;
    void setCompetitiorsData(QStringList rows);
    void setTableName(QString newName);
    QString tableName() const;
    int teamsCount();
    QJsonObject toJson() const;

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
    QVector<Team2022*> m_teams;
    QVBoxLayout *vBox = nullptr;
    void sortTeams();

};

#endif // TEAMSTABLE2022_H

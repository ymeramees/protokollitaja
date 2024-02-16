#ifndef TEAMSTABLE_H
#define TEAMSTABLE_H

#include <QVBoxLayout>
#include <set>
#include "team.h"

//class Protofinaal;

class TeamsTable : public QWidget
{
    Q_OBJECT
public:
    struct Result {
        QString name;   ///< First competitor name.
        QString shotValue;   ///< Shot value.
        QString seriesOrPoints; ///< Series result or points for this shot.
        QString totalScore;   ///< Result/Points in total.
        int total10Score;    ///< Total score multiplied by 10
    };
    explicit TeamsTable(QWidget *parent = nullptr);
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
    QVector<Team*> m_teams;
    QVBoxLayout *vBox = nullptr;
    void sortTeams();

};

#endif // TEAMSTABLE_H

#ifndef TEAMSTABLE_H
#define TEAMSTABLE_H

#include <QVBoxLayout>
#include "team.h"

//class Protofinaal;

class TeamsTable : public QWidget
{
    Q_OBJECT
public:
    explicit TeamsTable(QWidget *parent = nullptr);
    void clear();
    void createLayout(QJsonObject &jsonObj);
    QJsonObject toJson() const;

signals:
    void statusInfoChanged(QString newStatusInfo);
    void updateSpectatorWindow();

public slots:
    void readSiusInfo(SiusShotData shotData);
    void sumAllTeams();

private:
//    Protofinaal *m_parent = nullptr;
    QVector<Team*> teams;
    QVBoxLayout *vBox = nullptr;
    void sortTeams();

};

#endif // TEAMSTABLE_H

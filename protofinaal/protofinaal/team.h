#ifndef TEAM_H
#define TEAM_H

#include <QLabel>
#include <QWidget>
#include <QString>
#include <QLineEdit>
#include <QJsonArray>
#include <QJsonObject>
#include <QGridLayout>
#include <QVBoxLayout>

#include "competitor.h"

class Team : public QWidget
{
    Q_OBJECT
public:
    explicit Team(QJsonObject &configJson, int index, const bool scoringWithPoints, QWidget *parent = nullptr);
    ~Team();
    int index();
    bool isActive() const;
    int team10Total();
    QString teamTotal();
    int lastValidShotIndex() const;
    QString teamName();
    QVector<Competitor*> teamCompetitors();
    QJsonObject toJson() const;
    QString resultAt(int index);
    int result10At(int index);
    void setFirstCompetitiorData(int id, QString displayName, QString result);
    bool setPoints(int shotNo, int points);

signals:
    void modified();
    void statusInfo(QString statusInfo);
    void teamUpdated();

public slots:
    Competitor* getCompetitorWithID(int id);
    void calculatePointsTotal();
    void sumAll();

private:
    QLabel m_indexLabel;
    QLabel m_sumLabel;
    QLineEdit m_teamName;
    QVector<Competitor*> m_teamCompetitors;
};

#endif // TEAM_H

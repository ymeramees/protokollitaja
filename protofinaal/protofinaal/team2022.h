#ifndef TEAM2022_H
#define TEAM2022_H

#include <QLabel>
#include <QWidget>
#include <QString>
#include <QLineEdit>
#include <QJsonArray>
#include <QJsonObject>
#include <QGridLayout>
#include <QVBoxLayout>

#include "competitor2022.h"

class Team2022 : public QWidget
{
    Q_OBJECT
public:
    explicit Team2022(QJsonObject &configJson, int index, QWidget *parent = nullptr);
    ~Team2022();
    int index();
    bool isActive() const;
    QString pointsTotal();
    int lastValidShotIndex() const;
    QString teamName();
    QVector<Competitor2022*> teamCompetitors();
    QJsonObject toJson() const;
    QString points10At(int index);
    int result10At(int index);
    void setFirstCompetitiorData(int id, QString displayName, QString result);
    bool setPoints(int shotNo, int points);

signals:
    void modified();
    void statusInfo(QString statusInfo);
    void teamUpdated();

public slots:
    Competitor2022* getCompetitorWithID(int id);
    void calculatePointsTotal();
    void sumAll();

private:
    QLabel m_indexLabel;
    QLabel m_sumLabel;
    QLineEdit m_teamName;
    QVector<Competitor2022*> m_teamCompetitors;
};

#endif // TEAM2022_H

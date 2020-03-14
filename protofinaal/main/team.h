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
    explicit Team(QJsonObject &configJson, int index, QWidget *parent = nullptr);
    ~Team();
    QString index();
    QString lastSum();
    QString teamName();
    QVector<Competitor*> teamCompetitors();
    void toJson(QJsonObject &json) const;

signals:
    void statusInfo(QString statusInfo);
    void teamUpdated();

public slots:
    Competitor* getCompetitorWithID(int id);
    void sum();
    void sumAll();

private:
    QLabel m_indexLabel;
    QLabel m_sumLabel;
    QLineEdit m_teamName;
    QVector<Competitor*> m_teamCompetitors;
};

#endif // TEAM_H

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
    explicit Team(QJsonObject configJson, int index, QWidget *parent = nullptr);
    ~Team();

signals:

public slots:
    void sum();

private:
    QLabel *indexLabel = nullptr;
    QLabel *sumLabel = nullptr;
    QVector<Competitor*> teamCompetitors;
};

#endif // TEAM_H

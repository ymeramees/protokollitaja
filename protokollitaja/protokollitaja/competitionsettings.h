#ifndef COMPETITIONSETTINGS_H
#define COMPETITIONSETTINGS_H

#include <QJsonObject>
#include <QMetaType>
#include <QString>
#include <QDate>

struct CompetitionSettings {
    QString competitionName;
    QString fileName;
    int fileVersion;
    QDate startDate;
    QDate endDate;
    QString place;
    QString country;
    QJsonObject jsonData;
};

Q_DECLARE_METATYPE(CompetitionSettings)

#endif // COMPETITIONSETTINGS_H

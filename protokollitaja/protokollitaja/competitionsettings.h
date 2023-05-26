#ifndef COMPETITIONSETTINGS_H
#define COMPETITIONSETTINGS_H

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
};

Q_DECLARE_METATYPE(CompetitionSettings)

#endif // COMPETITIONSETTINGS_H

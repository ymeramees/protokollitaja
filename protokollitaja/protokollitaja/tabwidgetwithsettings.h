#ifndef TABWIDGETWITHSETTINGS_H
#define TABWIDGETWITHSETTINGS_H

#include <QString>
#include <QTabWidget>

struct TabWidgetWithSettings {
    QTabWidget *tabWidget = nullptr;
    QString competitionName;
    QDate startDate;
    QDate endDate;
    QString place;
    QString country;
    QString webCompetitionId;
    int autoComplete = 1;
    int autosave = 1;
    int lastCompetitorId = 0;
    int sorting = 1;
    int tabLocation = 2;
    int time = 5;
    int uploadTime = 30;
};

#endif // TABWIDGETWITHSETTINGS_H

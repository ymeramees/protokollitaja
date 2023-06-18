#ifndef TABWIDGETWITHSETTINGS_H
#define TABWIDGETWITHSETTINGS_H

#include <QDate>
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
    int autosaveEnabled = 1;
    int lastCompetitorId = 0;
    int ranking = 1;
    int tabLocation = 2;
    int tabCount = 0;
    int autosaveInterval = 5;
    int uploadInterval = 30;
};

#endif // TABWIDGETWITHSETTINGS_H

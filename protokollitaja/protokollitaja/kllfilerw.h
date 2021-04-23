#ifndef KLLFILERW_H
#define KLLFILERW_H

#include <QFile>
#include <QDebug>
#include <QObject>
#include <QWidget>
#include <QTabWidget>
#include <QScrollArea>
#include <QMessageBox>
#include <QApplication>

#include "leht.h"
#include "competitionsettings.h"
#include "tabwidgetwithsettings.h"

class KllFileRW : public QObject
{
    Q_OBJECT
public:
    explicit KllFileRW(
            Andmebaas *competitorDatabase,
            bool *autoComplete,
            int *sorting,
            LiikmeteValikKast *competitorsPickingBox,
            QWidget *parentWindow,
            QObject *parent = nullptr
            );
    TabWidgetWithSettings readKllFile(QString fileName, int startingId);
    bool writeKllFile(QTabWidget *toWrite);

signals:

private:
    int addCompetitor(QTabWidget *tabWidget, int lastId);
    int checkDuplicateIds(QTabWidget *tabWidget, int lastId);
    bool *m_autoComplete = nullptr;
    Andmebaas *m_competitorDatabase = nullptr;
    LiikmeteValikKast *m_competitorsPickingBox = nullptr;
    QWidget *m_parentWindow = nullptr;
    int *m_sorting = nullptr;
};

#endif // KLLFILERW_H

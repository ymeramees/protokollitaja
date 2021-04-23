#ifndef SIMPLEKLLFILERW_H
#define SIMPLEKLLFILERW_H

#include <QFile>
#include <QObject>
#include <QDataStream>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>

#include "competitionsettings.h"

// TODO To be integrated into KllFileRW after refactoring Tabwidget data model

class SimpleKllFileRW : public QObject
{
    Q_OBJECT
public:
    explicit SimpleKllFileRW(QObject *parent = nullptr);
    static CompetitionSettings readCompetitionSettings(QDataStream *inStream, QWidget *parent);
    static CompetitionSettings readCompetitionSettingsFromKll(QString fileName, QWidget *parent);
    static bool writeInitialKll(QString fileName, CompetitionSettings data, QWidget *parent);

signals:

};

#endif // SIMPLEKLLFILERW_H

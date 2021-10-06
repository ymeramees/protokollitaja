#ifndef STARTLISTWRITER_H
#define STARTLISTWRITER_H

#include <QFile>
#include <QVector>
#include <QFileDialog>
#include <QStringList>
#include <QTextStream>
#include <QMessageBox>
#include "commonsettings.h"

class StartListWriter : public QObject
{
    Q_OBJECT
public:
    enum StartListType {SIUS = 0, INBAND = 1};
    struct StartListCompetitor {
        QString target;
        QString ID;
        QString firstName;
        QString name;
        QString club;
        QString result;
        int discipline;
        QString competitorClass;
    };

    explicit StartListWriter(
            QVector<StartListCompetitor> competitorsList,
            QString filePath,
            StartListType type,
            QObject *parent = nullptr,
            int relay = 0
            );

signals:

public slots:

private:
    int m_relay;
    QString m_filePath;
    QVector<StartListCompetitor> m_competitorsList;

    QString replaceLetterWithNumber(QString in);
private slots:
    int writeInbandData(QTextStream*);
    int writeSiusData(QTextStream*);
    void writeStartListFile(StartListType);
};

#endif // STARTLISTWRITER_H

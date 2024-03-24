#ifndef FINALSFORMATS_H
#define FINALSFORMATS_H

#include <QFile>
#include <QObject>
#include <QString>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QJsonDocument>

class FinalsFormats : public QObject
{
    Q_OBJECT
public:

    explicit FinalsFormats(QString fileName);
    QStringList formatIds();
    QJsonObject confById(QString id);

private:
    QJsonArray m_finalsFormats;
};

#endif // FINALSFORMATS_H

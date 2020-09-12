#ifndef SETTINGS_H
#define SETTINGS_H

//#include <QObject>
#include <QVector>
#include <QSettings>

class CommonSettings// : public QObject
{
//    Q_OBJECT
public:
    explicit CommonSettings(QString organization, QString programName/*QObject *parent = nullptr*/);
    QVector<int> competitionShotTypes() const;
    QVector<int> sighterShotTypes() const;
    QString fileName() const;
    QString competitionShotTypesString() const;
    QString sighterShotTypesString() const;
    void setCompetitionShotTypes(const QString newShotTypes);
    void setSighterShotTypes(const QString newShotTypes);
    void readSettings();
    void writeSettings();

//signals:

private:
    QString m_folderName;
    QString m_fileName;
    QVector<int> m_competitionShotTypes;
    QVector<int> m_sighterShotTypes;
    QVector<int> toIntVector(const QString intsList);
    QString toString(const QVector<int> intVector) const;

};

#endif // SETTINGS_H

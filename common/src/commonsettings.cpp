#include "commonsettings.h"

CommonSettings::CommonSettings(QString organization, QString programName/*QObject *parent*/)// : QObject(parent)
{
    m_folderName = organization;
    m_fileName = programName;
    m_sighterShotTypes = {32,35,36,37,39,544,547,551,1060};
    m_competitionShotTypes = {0,3,4,5,7,512,515,519,1028,1029,1036,2304};

    readSettings();
}

QVector<int> CommonSettings::competitionShotTypes() const
{
    return m_competitionShotTypes;
}

QVector<int> CommonSettings::sighterShotTypes() const
{
    return m_sighterShotTypes;
}

QString CommonSettings::fileName() const
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, m_folderName, m_fileName);
    return settings.fileName();
}

QString CommonSettings::competitionShotTypesString() const
{
    return toString(m_competitionShotTypes);
}

QString CommonSettings::sighterShotTypesString() const
{
    return toString(m_sighterShotTypes);
}

void CommonSettings::setCompetitionShotTypes(const QString newShotTypes)
{
    m_competitionShotTypes = toIntVector(newShotTypes);
}

void CommonSettings::setSighterShotTypes(const QString newShotTypes)
{
    m_sighterShotTypes = toIntVector(newShotTypes);
}

QVector<int> CommonSettings::toIntVector(const QString intsList)
{
    QStringList stringList = intsList.split(",");
    QVector<int> intsVector;
    for (QString type : stringList)
        intsVector.append(type.toInt());
    return intsVector;
}

QString CommonSettings::toString(const QVector<int> intVector) const
{
    QStringList stringList;
    for(int type : intVector) {
        stringList.append(QString("%1").arg(type));
    }

    return stringList.join(",");
}

void CommonSettings::readSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, m_folderName, m_fileName);
    QStringList allKeys = settings.allKeys();
    QString fileName = settings.fileName();
    if(allKeys.length() > 0){
        settings.beginGroup("siusConnection");
        if(allKeys.filter(QRegExp("siusConnection.competitionShotTypes")).length() > 0)
            setCompetitionShotTypes(settings.value("competitionShotTypes").toString());
        if(allKeys.filter(QRegExp("siusConnection.sighterShotTypes")).length() > 0)
            setSighterShotTypes(settings.value("sighterShotTypes").toString());
        settings.endGroup();
    }
}

void CommonSettings::writeSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, m_folderName, m_fileName);
    settings.beginGroup("siusConnection");
    settings.setValue("competitionShotTypes", competitionShotTypesString());
    settings.setValue("sighterShotTypes", sighterShotTypesString());
    settings.endGroup();
}

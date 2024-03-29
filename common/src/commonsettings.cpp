#include "commonsettings.h"

CommonSettings::CommonSettings(QString organization, QString programName)
{
    m_settingsFolderName = organization;
    m_settingsFileName = programName;
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
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, m_settingsFolderName, m_settingsFileName);
    return settings.fileName();
}

QString CommonSettings::competitionShotTypesString() const
{
    return toString(m_competitionShotTypes);
}

QString CommonSettings::language() const
{
    return m_language;
}

QString CommonSettings::lastOpenFileName() const
{
    return m_lastOpenFileName;
}

int CommonSettings::shotNoIndexInSius() const
{
    return m_shotNoIndexInSius;
}

int CommonSettings::shotValueIndexInSius() const
{
    return m_shotValueIndexInSius;
}

QString CommonSettings::sighterShotTypesString() const
{
    return toString(m_sighterShotTypes);
}

void CommonSettings::setCompetitionShotTypes(const QString newShotTypes)
{
    m_competitionShotTypes = toIntVector(newShotTypes);
}

void CommonSettings::setLanguage(const QString newLanguage)
{
    m_language = newLanguage;
}

void CommonSettings::setLastOpenFileName(const QString fileName)
{
    m_lastOpenFileName = fileName;
}

void CommonSettings::setSighterShotTypes(const QString newShotTypes)
{
    m_sighterShotTypes = toIntVector(newShotTypes);
}

void CommonSettings::setWindowXLocation(const int X)
{
    m_windowXLocation = X;
}

void CommonSettings::setWindowYLocation(const int Y)
{
    m_windowYLocation = Y;
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

int CommonSettings::windowXLocation() const
{
    return m_windowXLocation;
}

int CommonSettings::windowYLocation() const
{
    return m_windowYLocation;
}

void CommonSettings::readSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, m_settingsFolderName, m_settingsFileName);
    QStringList allKeys = settings.allKeys();
    if(allKeys.length() > 0){
        if(allKeys.filter("language", Qt::CaseInsensitive).length() > 0)
            setLanguage(settings.value("language").toString());
        if(allKeys.filter("lastOpenFileName", Qt::CaseInsensitive).length() > 0)
            setLastOpenFileName(settings.value("lastOpenFileName").toString());
        if(allKeys.filter("windowXLocation", Qt::CaseInsensitive).length() > 0)
            setWindowXLocation(settings.value("windowXLocation").toInt());
        if(allKeys.filter("windowYLocation", Qt::CaseInsensitive).length() > 0)
            setWindowYLocation(settings.value("windowYLocation").toInt());

        settings.beginGroup("siusConnection");
        if(allKeys.filter("siusConnection/competitionShotTypes", Qt::CaseInsensitive).length() > 0)
            setCompetitionShotTypes(settings.value("competitionShotTypes").toString());
        if(allKeys.filter("siusConnection/sighterShotTypes", Qt::CaseInsensitive).length() > 0)
            setSighterShotTypes(settings.value("sighterShotTypes").toString());
        if(allKeys.filter("siusConnection/shotNoIndexInSius", Qt::CaseInsensitive).length() > 0)
            m_shotNoIndexInSius = settings.value("shotNoIndexInSius").toInt();
        if(allKeys.filter("siusConnection/shotValueIndexInSius", Qt::CaseInsensitive).length() > 0)
            m_shotValueIndexInSius = settings.value("shotValueIndexInSius").toInt();
        settings.endGroup();
    }
}

void CommonSettings::writeSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, m_settingsFolderName, m_settingsFileName);
    settings.setValue("language", language());
    settings.setValue("lastOpenFileName", lastOpenFileName());
    settings.setValue("windowXLocation", windowXLocation());
    settings.setValue("windowYLocation", windowYLocation());

    settings.beginGroup("siusConnection");
    settings.setValue("competitionShotTypes", competitionShotTypesString());
    settings.setValue("sighterShotTypes", sighterShotTypesString());
    settings.setValue("shotNoIndexInSius", shotNoIndexInSius());
    settings.setValue("shotValueIndexInSius", shotValueIndexInSius());
    settings.endGroup();
}

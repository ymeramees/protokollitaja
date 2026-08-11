#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QVector>
#include <QSettings>

class CommonSettings
{
public:
    enum EventTypes {AR40, AR60, AP40, AP60, PRONE30, PRONE60, SB_3_POSITIONS_3x10, SB_3_POSITIONS_3x20, SB_3_POSITIONS_3x40, FREE_PISTOL60, SB_PISTOL_30_30, SB_PISTOL_20_20_20, CISM_PISTOL, RAPID_FIRE_PISTOL, RUNNING_TARGET_20_20, RUNNING_TARGET_30_30, OTHER_AR, OTHER_AP, OTHER_SB_RIFLE, OTHER_SB_PISTOL};
    const QStringList eventNames = {"10m Air Rifle 40 Shots", "10m Air Rifle 60 Shots", "10m Air Pistol 40 Shots", "10m Air Pistol 60 Shots", "50m 30 Shots Prone", "50m 60 Shots Prone", "50m Rifle 3x10 3 Positions", "50m Rifle 3x20 3 Positions", "50m Rifle 3x40 3 Positions", "50m Free Pistol 60 Shots", "25m Pistol", "25m Standard Pistol", "CISM püstol", "25m Rapid Fire Pistol", "50m 20+20 Running Target Mixed", "50m 30+30 Running Target", "Muu õhupüss", "Muu õhupüstol", "Other Smallbore rifle", "Muu spordipüstol"};
    explicit CommonSettings(QString organization, QString programName/*QObject *parent = nullptr*/);
    QVector<int> competitionShotTypes() const;
    QVector<int> sighterShotTypes() const;
    QString fileName() const;
    QString competitionShotTypesString() const;
    QString language() const;
    QString lastOpenFileName() const;
    QString sighterShotTypesString() const;
    int shotNoIndexInSius() const;
    int shotValueIndexInSius() const;
    int windowXLocation() const;
    int windowYLocation() const;
    void setCompetitionShotTypes(const QString newShotTypes);
    void setLanguage(const QString newLanguage);
    void setLastOpenFileName(const QString fileName);
    void setSighterShotTypes(const QString newShotTypes);
    void setWindowXLocation(const int X);
    void setWindowYLocation(const int Y);
    void readSettings();
    void writeSettings();

//signals:

private:
    QString m_lastOpenFileName;
    QString m_settingsFolderName;
    QString m_settingsFileName;
    QString m_language;
    int m_shotNoIndexInSius = 13;    // Not sure if these are needed anymore
    int m_shotValueIndexInSius = 10;
    int m_windowXLocation;
    int m_windowYLocation;
    QVector<int> m_competitionShotTypes;
    QVector<int> m_sighterShotTypes;
    QVector<int> toIntVector(const QString intsList);
    QString toString(const QVector<int> intVector) const;

};

#endif // SETTINGS_H

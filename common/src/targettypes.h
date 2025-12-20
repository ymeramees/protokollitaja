#ifndef TARGETTYPES_H
#define TARGETTYPES_H

#include <QObject>
#include <QString>
#include <QMap>

class TargetTypes : public QObject
{
    Q_OBJECT

public:
    enum TargetType {
        Other,
        AirRifle,
        AirPistol,
        SmallboreRifle,
        FreePistol,
        RapidPistol,
        Rifle300m,
        RunningTarget50m,
        RunningTarget10m,
        OtherRifle,
        OtherPistol
    };

    struct Target {
        QString targetId;
        QString name;
        bool isRifleDB;
    };

    static Target targetData(const TargetType targetType);
    static TargetType fromOld(const int targetType);
    static TargetType fromString(const QString targetId);
    static TargetType fromTargetName(const QString targetName);
    static QStringList targetNames();

    // static const TargetType All[] = {
    //     TargetType::Other,
    //     TargetType::AirRifle,
    //     TargetType::AirPistol,
    //     TargetType::SmallboreRifle,
    //     TargetType::FreePistol,
    //     TargetType::RapidPistol,
    //     TargetType::Rifle300m,
    //     TargetType::RunningTarget50m,
    //     TargetType::RunningTarget10m,
    //     TargetType::OtherRifle,
    //     TargetType::OtherPistol
    // };
private:
    static QMap<TargetType, Target>& targetDefinitions();
    static QMap<int, QString>& oldTargetTypeToId();
};

#endif /* TARGETTYPES_H */

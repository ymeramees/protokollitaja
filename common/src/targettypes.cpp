#include "targettypes.h"

QMap<TargetTypes::TargetType, TargetTypes::Target>& TargetTypes::targetDefinitions()
{
    static QMap<TargetTypes::TargetType, TargetTypes::Target> *types = new QMap<TargetType, Target>({
        {Other, {"Other", tr("Muu"), false}},
        {AirRifle, {"AirRifle", tr("Õhupüss"), true}},
        {AirPistol, {"AirPistol", tr("Õhupüstol"), false}},
        {SmallboreRifle, {"SmallboreRifle", tr("Sportpüss"), true}},
        {FreePistol, {"FreePistol", tr("Spordipüstol"), false}},
        {RapidPistol, {"RapidPistol", tr("Siluett"), false}},
        {Rifle300m, {"Rifle300m", tr("300m püss"), true}},
        {RunningTarget50m, {"RunningTarget50m", tr("50m liikuv märk"), true}},
        {RunningTarget10m, {"RunningTarget10m", tr("10m liikuv märk"), true}},
        {OtherRifle, {"OtherRifle", tr("Muu püss"), true}},
        {OtherPistol, {"OtherPistol", tr("Muu püstol"), false}}
    });
    return *types;
}

QMap<int, QString>& TargetTypes::oldTargetTypeToId()
{
    static QMap<int, QString> *mapping = new QMap<int, QString>({
        {0, "Other"},
        {1, "AirRifle"},
        {2, "AirPistol"},
        {3, "SmallboreRifle"},
        {4, "FreePistol"},
        {5, "RapidPistol"},
        {6, "Rifle300m"},
        {7, "RunningTarget50m"},
        {8, "RunningTarget10m"},
        {9, "OtherRifle"},
        {10, "OtherPistol"}
    });
    return *mapping;
}

TargetTypes::Target TargetTypes::targetData(const TargetTypes::TargetType targetType)
{
    return targetDefinitions()[targetType];
}

TargetTypes::TargetType TargetTypes::fromOld(const int targetType)
{
    if (oldTargetTypeToId().contains(targetType)) {
        return fromString(oldTargetTypeToId()[targetType]);
    } else
        return TargetTypes::Other;
}

TargetTypes::TargetType TargetTypes::fromString(const QString targetId)
{
    if (!targetId.isEmpty()) {
        for (auto i = targetDefinitions().cbegin(), end = targetDefinitions().cend(); i != end; ++i)
            if (i.value().targetId == targetId)
                return i.key();
    }
    return TargetTypes::Other;
}

TargetTypes::TargetType TargetTypes::fromTargetName(const QString targetName)
{
    for (auto i = targetDefinitions().cbegin(), end = targetDefinitions().cend(); i != end; ++i)
        if (i.value().name == targetName)
            return i.key();
    return TargetTypes::Other;
}

QStringList TargetTypes::targetNames()
{
    QStringList names;
    for (Target targetDef : targetDefinitions()) {
        names << targetDef.name;
    }
    return names;
}

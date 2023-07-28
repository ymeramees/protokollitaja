#include "targettypes.h"

QMap<TargetTypes::TargetType, TargetTypes::Target>& TargetTypes::targetDefinitions()
{
    static QMap<TargetTypes::TargetType, TargetTypes::Target> *types = new QMap<TargetType, Target>({
        {Other, {tr("Muu"), false}},
        {AirRifle, {tr("Õhupüss"), true}},
        {AirPistol, {tr("Õhupüstol"), false}},
        {SmallboreRifle, {tr("Sportpüss"), true}},
        {FreePistol, {tr("Spordipüstol"), false}},
        {RapidPistol, {tr("Siluett"), false}},
        {Rifle300m, {tr("300m püss"), true}},
        {RunningTarget50m, {tr("50m liikuv märk"), true}},
        {RunningTarget10m, {tr("10m liikuv märk"), true}},
        {OtherRifle, {tr("Muu püss"), true}},
        {OtherPistol, {tr("Muu püstol"), false}}
    });
    return *types;
}

TargetTypes::Target TargetTypes::targetData(const TargetTypes::TargetType targetType)
{
    return targetDefinitions()[targetType];
}

QStringList TargetTypes::targetNames()
{
    QStringList names;
    for (Target targetDef : targetDefinitions()) {
        names << targetDef.name;
    }
    return names;
}

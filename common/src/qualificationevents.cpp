#include "qualificationevents.h"

QMap<QualificationEvents::EventType, QualificationEvents::QualificationEvent>& QualificationEvents::eventDefinitions()
{
    static QMap<EventType, QualificationEvent> *definitions = new QMap<EventType, QualificationEvent>({
        {AirRifle60, { tr("60l Õhupüss"), TargetTypes::TargetType::AirRifle, 6, 0}},
        {AirRifle40, { tr("40l Õhupüss"), TargetTypes::TargetType::AirRifle, 4, 0}},
        {AirRifle20, { tr("20l Õhupüss"), TargetTypes::TargetType::AirRifle, 2, 0}},
        {AirPistol60, { tr("60l Õhupüstol"), TargetTypes::TargetType::AirPistol, 6, 0}},
        {AirPistol40, { tr("40l Õhupüstol"), TargetTypes::TargetType::AirPistol, 4, 0}},
        {AirPistol20, { tr("20l Õhupüstol"), TargetTypes::TargetType::AirPistol, 2, 0}},
        {Rifle3x40_50m, { tr("3x40l Standard"), TargetTypes::TargetType::SmallboreRifle, 12, 4}},
        {Rifle3x20_50m, { tr("3x20l Standard"), TargetTypes::TargetType::SmallboreRifle, 6, 2}},
        {Rifle3x10_50m, { tr("3x10l Standard"), TargetTypes::TargetType::SmallboreRifle, 3, 1}},
        {RifleProne60_50m, { tr("60l Lamades"), TargetTypes::TargetType::SmallboreRifle, 6, 0}},
        {RifleProne30_50m, { tr("30l Lamades"), TargetTypes::TargetType::SmallboreRifle, 3, 0}},
        {RapidFirePistol, { tr("Olümpiakiirlaskmine"), TargetTypes::TargetType::RapidPistol, 6, 3}},
        {Pistol_25m, { tr("30+30l Spordipüstol"), TargetTypes::TargetType::FreePistol, 6, 3}},
        {Pistol30_25m, { tr("30l Ringmärk"), TargetTypes::TargetType::FreePistol, 3, 0}},
        {Pistol30Rapid_25m, { tr("30l Siluett"), TargetTypes::TargetType::RapidPistol, 3, 0}},
        {StandardPistol, { tr("20+20+20l Spordipüstol"), TargetTypes::TargetType::RapidPistol, 6, 2}},
        {FreePistol60_50m, { tr("60l Vabapüstol"), TargetTypes::TargetType::FreePistol, 6, 0}},
        {FreePistol30_50m, { tr("30l Vabapüstol"), TargetTypes::TargetType::FreePistol, 3, 0}},
        {PistolCISM_25m, { tr("CISM püstol"), TargetTypes::TargetType::RapidPistol, 6, 2}},
        {Rifle3x40_300m, { tr("300m 3x40l Standard"), TargetTypes::TargetType::Rifle300m, 12, 4}},
        {Rifle3x20_300m, { tr("300m 3x20l Standard"), TargetTypes::TargetType::Rifle300m, 6, 2}},
        {Rifle3x10_300m, { tr("300m 3x10l Standard"), TargetTypes::TargetType::Rifle300m, 3, 1}},
        {RifleProne60_300m, { tr("300m 60l Lamades"), TargetTypes::TargetType::Rifle300m, 6, 0}},
        {RifleProne30_300m, { tr("300m 30l Lamades"), TargetTypes::TargetType::Rifle300m, 3, 0}},
        {RunningTarget_50m, { tr("30+30l Metssiga"), TargetTypes::TargetType::RunningTarget50m, 6, 3}},
        {RunningTargetMixed_50m, { tr("20+20l Metssiga"), TargetTypes::TargetType::RunningTarget50m, 4, 2}},
        {RunningTarget_10m, { tr("30+30l Liikuv Märk"), TargetTypes::TargetType::RunningTarget10m, 6, 3}},
        {RunningTargetMixed_10m, { tr("20+20l Liikuv Märk"), TargetTypes::TargetType::RunningTarget10m, 4, 2}},
        {OtherAirRifle, { tr("Muu Õhupüss"), TargetTypes::TargetType::AirRifle, 6, 0}},
        {OtherAirPistol, { tr("Muu Õhupüstol"), TargetTypes::TargetType::AirPistol, 6, 0}},
        {OtherSmallboreRifle, { tr("Muu sportpüss"), TargetTypes::TargetType::SmallboreRifle, 6, 0}},
        {Other300mRifle, { tr("Muu 300m"), TargetTypes::TargetType::Rifle300m, 6, 0}},
        {Other25mPistol, { tr("Muu püstol"), TargetTypes::TargetType::FreePistol, 6, 0}},
        {Other, { tr("Muu"), TargetTypes::TargetType::Other, 6, 0}}
    });
    return *definitions;
}

QualificationEvents::QualificationEvent QualificationEvents::eventData(const EventType eventType)
{
    return eventDefinitions()[eventType];
}

QualificationEvents::QualificationEvent QualificationEvents::eventData(const int eventTypeIndex)
{
    return eventDefinitions().values().at(eventTypeIndex);
}

QStringList QualificationEvents::eventNames()
{
    QStringList names;
    for (QualificationEvent event : eventDefinitions()) {
        names << event.name;
    }
    return names;
}

QualificationEvents::EventType QualificationEvents::fromOldString(const QString eventTypeString)
{
    if(eventTypeString.isEmpty()) {
        return QualificationEvents::OtherAirRifle;
    } else if(eventTypeString == "40l Õhupüss") {
        return QualificationEvents::AirRifle40;
    } else if(eventTypeString == "60l Õhupüss") {
        return QualificationEvents::AirRifle60;
    } else if(eventTypeString == "40l Õhupüstol") {
        return QualificationEvents::AirPistol40;
    } else if(eventTypeString == "60l Õhupüstol") {
        return QualificationEvents::AirPistol60;
    } else if(eventTypeString == "30l Lamades") {
        return QualificationEvents::RifleProne30_50m;
    } else if(eventTypeString == "60l Lamades") {
        return QualificationEvents::RifleProne60_50m;
    } else if(eventTypeString == "3x10l Standard") {
        return QualificationEvents::Rifle3x10_50m;
    } else if(eventTypeString == "3x20l Standard") {
        return QualificationEvents::Rifle3x20_50m;
    } else if(eventTypeString == "3x40l Standard") {
        return QualificationEvents::Rifle3x40_50m;
    } else if(eventTypeString == "60l Vabapüstol") {
        return QualificationEvents::FreePistol60_50m;
    } else if(eventTypeString == "30+30l Spordipüstol") {
        return QualificationEvents::Pistol_25m;
    } else if(eventTypeString == "20+20+20l Spordipüstol") {
        return QualificationEvents::StandardPistol;
    } else if(eventTypeString == "CISM püstol") {
        return QualificationEvents::PistolCISM_25m;
    } else if(eventTypeString == "Olümpiakiirlaskmine") {
        return QualificationEvents::RapidFirePistol;
    } else if(eventTypeString == "20+20l Metssiga") {
        return QualificationEvents::RunningTargetMixed_50m;
    } else if(eventTypeString == "30+30l Metssiga") {
        return QualificationEvents::RunningTarget_50m;
    } else if(eventTypeString == "Muu õhupüss") {
        return QualificationEvents::OtherAirRifle;
    } else if(eventTypeString == "Muu õhupüstol") {
        return QualificationEvents::OtherAirPistol;
    } else if(eventTypeString == "Muu sportpüss") {
        return QualificationEvents::OtherSmallboreRifle;
    } else if(eventTypeString == "Muu spordipüstol") {
        return QualificationEvents::Other25mPistol;
    }
    return QualificationEvents::Other;
}

QualificationEvents::EventType QualificationEvents::fromString(const QString eventTypeString)
{
    for (auto i = eventDefinitions().cbegin(), end = eventDefinitions().cend(); i != end; ++i)
        if (i.value().name == eventTypeString)
            return i.key();
    return QualificationEvents::Other;
}

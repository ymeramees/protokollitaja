#include "qualificationevents.h"

QMap<QualificationEvents::EventType, QualificationEvents::QualificationEvent>& QualificationEvents::eventDefinitions()
{
    static QMap<EventType, QualificationEvent> *definitions = new QMap<EventType, QualificationEvent>({
        {AirRifle60, { tr("60l Õhupüss"), TargetTypes::TargetType::AirRifle, 6, 0, true}},
        {AirRifle40, { tr("40l Õhupüss"), TargetTypes::TargetType::AirRifle, 4, 0, true}},
        {AirRifle20, { tr("20l Õhupüss"), TargetTypes::TargetType::AirRifle, 2, 0, true}},
        {AirPistol60, { tr("60l Õhupüstol"), TargetTypes::TargetType::AirPistol, 6, 0, false}},
        {AirPistol40, { tr("40l Õhupüstol"), TargetTypes::TargetType::AirPistol, 4, 0, false}},
        {AirPistol20, { tr("20l Õhupüstol"), TargetTypes::TargetType::AirPistol, 2, 0, false}},
        {Rifle3x40_50m, { tr("3x40l Standard"), TargetTypes::TargetType::SmallboreRifle, 12, 4, false}},
        {Rifle3x20_50m, { tr("3x20l Standard"), TargetTypes::TargetType::SmallboreRifle, 6, 2, false}},
        {Rifle3x10_50m, { tr("3x10l Standard"), TargetTypes::TargetType::SmallboreRifle, 3, 1, false}},
        {RifleProne60_50m, { tr("60l Lamades"), TargetTypes::TargetType::SmallboreRifle, 6, 0, true}},
        {RifleProne30_50m, { tr("30l Lamades"), TargetTypes::TargetType::SmallboreRifle, 3, 0, true}},
        {RapidFirePistol, { tr("Olümpiakiirlaskmine"), TargetTypes::TargetType::RapidPistol, 6, 3, false}},
        {Pistol_25m, { tr("30+30l Spordipüstol"), TargetTypes::TargetType::FreePistol, 6, 3, false}},
        {Pistol30_25m, { tr("30l Ringmärk"), TargetTypes::TargetType::FreePistol, 3, 0, false}},
        {Pistol30Rapid_25m, { tr("30l Siluett"), TargetTypes::TargetType::RapidPistol, 3, 0, false}},
        {StandardPistol, { tr("20+20+20l Spordipüstol"), TargetTypes::TargetType::RapidPistol, 6, 2, false}},
        {FreePistol60_50m, { tr("60l Vabapüstol"), TargetTypes::TargetType::FreePistol, 6, 0, false}},
        {FreePistol30_50m, { tr("30l Vabapüstol"), TargetTypes::TargetType::FreePistol, 3, 0, false}},
        {PistolCISM_25m, { tr("CISM püstol"), TargetTypes::TargetType::RapidPistol, 6, 2, false}},
        {Rifle3x40_300m, { tr("300m 3x40l Standard"), TargetTypes::TargetType::Rifle300m, 12, 4, false}},
        {Rifle3x20_300m, { tr("300m 3x20l Standard"), TargetTypes::TargetType::Rifle300m, 6, 2, false}},
        {Rifle3x10_300m, { tr("300m 3x10l Standard"), TargetTypes::TargetType::Rifle300m, 3, 1, false}},
        {RifleProne60_300m, { tr("300m 60l Lamades"), TargetTypes::TargetType::Rifle300m, 6, 0, false}},
        {RifleProne30_300m, { tr("300m 30l Lamades"), TargetTypes::TargetType::Rifle300m, 3, 0, false}},
        {RunningTarget_50m, { tr("30+30l Metssiga"), TargetTypes::TargetType::RunningTarget50m, 6, 3, false}},
        {RunningTargetMixed_50m, { tr("20+20l Metssiga"), TargetTypes::TargetType::RunningTarget50m, 4, 2, false}},
        {RunningTarget_10m, { tr("30+30l Liikuv Märk"), TargetTypes::TargetType::RunningTarget10m, 6, 3, false}},
        {RunningTargetMixed_10m, { tr("20+20l Liikuv Märk"), TargetTypes::TargetType::RunningTarget10m, 4, 2, false}},
        {OtherAirRifle, { tr("Muu Õhupüss"), TargetTypes::TargetType::AirRifle, 6, 0, false}},
        {OtherAirPistol, { tr("Muu Õhupüstol"), TargetTypes::TargetType::AirPistol, 6, 0, false}},
        {OtherSmallboreRifle, { tr("Muu sportpüss"), TargetTypes::TargetType::SmallboreRifle, 6, 0, false}},
        {Other300mRifle, { tr("Muu 300m"), TargetTypes::TargetType::Rifle300m, 6, 0, false}},
        {Other25mPistol, { tr("Muu püstol"), TargetTypes::TargetType::FreePistol, 6, 0, false}},
        {Other, { tr("Muu"), TargetTypes::TargetType::Other, 6, 0, false}},
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

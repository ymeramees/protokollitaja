#include "qualificationevents.h"

QMap<QualificationEvents::EventType, QualificationEvents::QualificationEvent>& QualificationEvents::eventDefinitions()
{
    static QMap<EventType, QualificationEvent> *definitions = new QMap<EventType, QualificationEvent>({
        {AirRifle60, { "AirRifle60", tr("60l Õhupüss"), TargetTypes::TargetType::AirRifle, 6, 0, true, {"airrifle"}}},
        {AirRifle40, { "AirRifle40", tr("40l Õhupüss"), TargetTypes::TargetType::AirRifle, 4, 0, true, {"airrifle"}}},
        {AirRifle20, { "AirRifle20", tr("20l Õhupüss"), TargetTypes::TargetType::AirRifle, 2, 0, true, {"airrifle"}}},
        {AirPistol60, { "AirPistol60", tr("60l Õhupüstol"), TargetTypes::TargetType::AirPistol, 6, 0, false, {"airpistol"}}},
        {AirPistol40, { "AirPistol40", tr("40l Õhupüstol"), TargetTypes::TargetType::AirPistol, 4, 0, false, {"airpistol"}}},
        {AirPistol20, { "AirPistol20", tr("20l Õhupüstol"), TargetTypes::TargetType::AirPistol, 2, 0, false, {"airpistol"}}},
        {Rifle3x40_50m, { "Rifle3x40_50m", tr("3x40l Standard"), TargetTypes::TargetType::SmallboreRifle, 12, 4, false, {"50m_3positions"}}},
        {Rifle3x20_50m, { "Rifle3x20_50m", tr("3x20l Standard"), TargetTypes::TargetType::SmallboreRifle, 6, 2, false, {"50m_3positions"}}},
        {Rifle3x10_50m, { "Rifle3x10_50m", tr("3x10l Standard"), TargetTypes::TargetType::SmallboreRifle, 3, 1, false, {"50m_3positions"}}},
        {RifleProne60_50m, { "RifleProne60_50m", tr("60l Lamades"), TargetTypes::TargetType::SmallboreRifle, 6, 0, true, {"50m_rifle"}}},
        {RifleProne30_50m, { "RifleProne30_50m", tr("30l Lamades"), TargetTypes::TargetType::SmallboreRifle, 3, 0, true, {"50m_rifle"}}},
        {RapidFirePistol, { "RapidFirePistol", tr("Olümpiakiirlaskmine"), TargetTypes::TargetType::RapidPistol, 6, 3, false, {"RapidPistol"}}},
        {Pistol_25m, { "Pistol_25m", tr("30+30l Spordipüstol"), TargetTypes::TargetType::FreePistol, 6, 3, false, {"Pistol25m", "rapid_SP"}}},
        {Pistol_25m_CFP, { "Pistol_25m_CFP", tr("30+30l Täiskaliiber"), TargetTypes::TargetType::FreePistol, 6, 3, false, {"CFPistol", "rapid_CFP"}}},
        {Pistol30_25m, { "Pistol30_25m", tr("30l SP Ringmärk"), TargetTypes::TargetType::FreePistol, 3, 0, false, {"Pistol25m"}}},
        {Pistol30_25m, { "Pistol30_25m", tr("30l TK Ringmärk"), TargetTypes::TargetType::FreePistol, 3, 0, false, {"CFPistol"}}},
        {Pistol30Rapid_25m, { "Pistol30Rapid_25m", tr("30l SP Siluett"), TargetTypes::TargetType::RapidPistol, 3, 0, false, {"rapid_SP"}}},
        {Pistol30Rapid_25m, { "Pistol30Rapid_25m", tr("30l TK Siluett"), TargetTypes::TargetType::RapidPistol, 3, 0, false, {"rapid_CFP"}}},
        {StandardPistol, { "StandardPistol", tr("20+20+20l Standardpüstol"), TargetTypes::TargetType::FreePistol, 6, 2, false, {"Pistol25mStandard"}}},
        {FreePistol60_50m, { "FreePistol60_50m", tr("60l Vabapüstol"), TargetTypes::TargetType::FreePistol, 6, 0, false, {"50m_pistol"}}},
        {FreePistol30_50m, { "FreePistol30_50m", tr("30l Vabapüstol"), TargetTypes::TargetType::FreePistol, 3, 0, false, {"50m_pistol"}}},
        {PistolCISM_25m, { "PistolCISM_25m", tr("CISM TK püstol"), TargetTypes::TargetType::RapidPistol, 6, 2, false, {"CFPistolMilitaryRapid"}}},
        {Rifle3x40_300m, { "Rifle3x40_300m", tr("300m 3x40l Standard"), TargetTypes::TargetType::Rifle300m, 12, 4, false, {"300m_3positions"}}},
        {Rifle3x20_300m, { "Rifle3x20_300m", tr("300m 3x20l Standard"), TargetTypes::TargetType::Rifle300m, 6, 2, false, {"300m_3positions"}}},
        {Rifle3x10_300m, { "Rifle3x10_300m", tr("300m 3x10l Standard"), TargetTypes::TargetType::Rifle300m, 3, 1, false, {"300m_3positions"}}},
        {RifleProne60_300m, { "RifleProne60_300m", tr("300m 60l Lamades"), TargetTypes::TargetType::Rifle300m, 6, 0, false, {"300m_rifle"}}},
        {RifleProne30_300m, { "RifleProne30_300m", tr("300m 30l Lamades"), TargetTypes::TargetType::Rifle300m, 3, 0, false, {"300m_rifle"}}},
        {RunningTarget_50m, { "RunningTarget_50m", tr("30+30l Metssiga"), TargetTypes::TargetType::RunningTarget50m, 6, 3, false, {"RunningTarget50m"}}},
        {RunningTargetMixed_50m, { "RunningTargetMixed_50m", tr("20+20l Metssiga"), TargetTypes::TargetType::RunningTarget50m, 4, 2, false, {"RunningTargetMixed50m"}}},
        {RunningTarget_10m, { "RunningTarget_10m", tr("30+30l Liikuv Märk"), TargetTypes::TargetType::RunningTarget10m, 6, 3, false, {"RunningTarget10m"}}},
        {RunningTargetMixed_10m, { "RunningTargetMixed_10m", tr("20+20l Liikuv Märk"), TargetTypes::TargetType::RunningTarget10m, 4, 2, false, {"RunningTargetMixed10m"}}},
        {OtherAirRifle, { "OtherAirRifle", tr("Muu Õhupüss"), TargetTypes::TargetType::AirRifle, 6, 0, false, {"airrifle"}}},
        {OtherAirPistol, { "OtherAirPistol", tr("Muu Õhupüstol"), TargetTypes::TargetType::AirPistol, 6, 0, false, {"airpistol"}}},
        {OtherSmallboreRifle, { "OtherSmallboreRifle", tr("Muu sportpüss"), TargetTypes::TargetType::SmallboreRifle, 6, 0, false, {"50m_rifle"}}},
        {Other300mRifle, { "Other300mRifle", tr("Muu 300m"), TargetTypes::TargetType::Rifle300m, 6, 0, false, {"300m_rifle"}}},
        {Other25mPistol, { "Other25mPistol", tr("Muu püstol"), TargetTypes::TargetType::FreePistol, 6, 0, false, {"Pistol25m"}}},
        {Other, { "Other", tr("Muu"), TargetTypes::TargetType::Other, 6, 0, false, {"other"}}},
    });
    return *definitions;
}

QMap<int, QString>& QualificationEvents::oldEventTypeToId()
{
    static QMap<int, QString> *mapping = new QMap<int, QString>({
        { 0, "AirRifle60"},
        { 1, "AirRifle40"},
        { 2, "AirRifle20"},
        { 3, "AirPistol60"},
        { 4, "AirPistol40"},
        { 5, "AirPistol20"},
        { 6, "Rifle3x40_50m"},
        { 7, "Rifle3x20_50m"},
        { 8, "Rifle3x10_50m"},
        { 9, "RifleProne60_50m"},
        { 10, "RifleProne30_50m"},
        { 11, "RapidFirePistol"},
        { 12, "Pistol_25m"},
        { 13, "Pistol30_25m"},
        { 14, "Pistol30Rapid_25m"},
        { 15, "StandardPistol"},
        { 16, "FreePistol60_50m"},
        { 17, "FreePistol30_50m"},
        { 18, "PistolCISM_25m"},
        { 19, "Rifle3x40_300m"},
        { 20, "Rifle3x20_300m"},
        { 21, "Rifle3x10_300m"},
        { 22, "RifleProne60_300m"},
        { 23, "RifleProne30_300m"},
        { 24, "RunningTarget_50m"},
        { 25, "RunningTargetMixed_50m"},
        { 26, "RunningTarget_10m"},
        { 27, "RunningTargetMixed_10m"},
        { 28, "OtherAirRifle"},
        { 29, "OtherAirPistol"},
        { 30, "OtherSmallboreRifle"},
        { 31, "Other300mRifle"},
        { 32, "Other25mPistol"},
        { 33, "Other"}
    });
    return *mapping;
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
    } else if(eventTypeString == "20+20+20l Spordipüstol" || eventTypeString == "20+20+20l Standardpüstol") {
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

QualificationEvents::EventType QualificationEvents::fromEventName(const QString eventName)
{
    for (auto i = eventDefinitions().cbegin(), end = eventDefinitions().cend(); i != end; ++i)
        if (i.value().name == eventName)
            return i.key();
    return QualificationEvents::Other;
}

QualificationEvents::EventType QualificationEvents::fromString(const QString eventTypeString)
{
    if (!eventTypeString.isEmpty()) {
        for (auto i = eventDefinitions().cbegin(), end = eventDefinitions().cend(); i != end; ++i)
            if (i.value().eventTypeId == eventTypeString)
                return i.key();
    }
    return QualificationEvents::Other;
}

QualificationEvents::EventType QualificationEvents::fromOld(const int oldEventType)
{
    if (oldEventTypeToId().contains(oldEventType)) {
        return fromString(oldEventTypeToId()[oldEventType]);
    } else
        return QualificationEvents::Other;
}

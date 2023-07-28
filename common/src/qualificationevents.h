#ifndef QUALIFICATIONEVENTS_H
#define QUALIFICATIONEVENTS_H

#include <QObject>
#include <QString>
#include <QMap>

#include "targettypes.h"

class QualificationEvents : public QObject
{
    Q_OBJECT

public:
    enum EventType {
        AirRifle60,
        AirRifle40,
        AirRifle20,
        AirPistol60,
        AirPistol40,
        AirPistol20,
        Rifle3x40_50m,
        Rifle3x20_50m,
        Rifle3x10_50m,
        RifleProne60_50m,
        RifleProne30_50m,
        RapidFirePistol,
        Pistol_25m,
        Pistol30_25m,
        Pistol30Rapid_25m,
        StandardPistol,
        FreePistol60_50m,
        FreePistol30_50m,
        PistolCISM_25m,
        Rifle3x40_300m,
        Rifle3x20_300m,
        Rifle3x10_300m,
        RifleProne60_300m,
        RifleProne30_300m,
        RunningTarget_50m,
        RunningTargetMixed_50m,
        RunningTarget_10m,
        RunningTargetMixed_10m,
        OtherAirRifle,
        OtherAirPistol,
        OtherSmallboreRifle,
        Other300mRifle,
        Other25mPistol,
        Other
    };

    struct QualificationEvent {
        QString name;
        TargetTypes::TargetType targetType;
        int numberOfSeries;
        int seriesInSubtotal;
    };

    static QualificationEvent eventData(const EventType eventType);
    static QualificationEvent eventData(const int eventTypeIndex);
    static QStringList eventNames();
    static EventType fromOldString(const QString eventTypeString);
    static EventType fromString(const QString eventTypeString);

private:
    static QMap<EventType, QualificationEvent>& eventDefinitions();
};

#endif /* QUALIFICATIONEVENTS_H */

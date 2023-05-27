#ifndef LASK_H
#define LASK_H

#include <QtGlobal>

#include <QJsonObject>
#include <QString>
#include <QPoint>
#include <QtMath>
#include <QTime>

class Lask
{

public:
    enum TargetType {Muu = 0, Ohupuss = 1, Ohupustol = 2, Sportpuss = 3, Spordipustol = 4, Puss = 5, Pustol = 6};
    enum OriginType {Manual = 0, ScoringMachine = 1, Sius = 2, Inband = 3};
    Lask();
    Lask(int shot10Times, int x = -999, int y = -999, bool innerTen = false, QTime shotTime = QTime(), bool competitionShot = true, OriginType shotOrigin = Manual);
    Lask(QString siusRow);
    Lask(QJsonObject);
    void set(const Lask *l);
    bool isEmpty();

public:
    static bool calcIfInnerTen(TargetType targetType, long x, long y);
    static Lask fromInband(QStringList inbandRowParts);

    void clear();
    bool equals(const Lask other) const;
    int getILask() const;
//    float getFLask() const;   // No point of having this fn, as result is often wrong
    QString getSLask() const;
    int get10Lask() const;
    bool isCompetitionShot() const;
    bool isInnerTen() const;
    int X() const;
    int Y() const;
//    QPoint XY();
    void setCompetitionShot(const bool isCompetitionShot);
    void setInnerTen(const bool isInnerTen);
    void setLask(int);
    void setLask(float);
    bool setLask(QString);
    void set10Lask(int);    // Insert shot value as multiplied by 10
    bool set10Lask(QString);    // Insert shot value as multiplied by 10
    void setShotTime(QTime newTime);
    void setMX(QString s);
    void setMmX(float);
    void setMmX(QString);
    void setNanoX(QString);
    void setMY(QString s);
    void setMmY(float);
    void setMmY(QString);
    void setNanoY(QString);
    void setNanoXY(QPoint);
    void setSiusShot(QString siusRow);
    void setShotOrigin(const OriginType newOrigin);
    OriginType shotOrigin() const;
    QString stringX() const;
    QString stringY() const;
    QString stringMX() const;
    QString stringMY() const;
    QTime shotTime() const;
    QJsonObject toJson() const;

    bool operator ==(const Lask other) const;

private:
    int m_lask;   // Value as multiplied with 10 (10,5 -> 105)
    bool m_competitionShot = true;
    bool m_innerTen;
    OriginType m_shotOrigin = Manual;
    int m_x;    // Values in nanometers, to keep it as int, but have 3 decimal point accuracy with millimeters
    int m_y;    // Same here
    QTime m_shotTime;
    float stringToFloat(QString s);
};

#endif // LASK_H

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
    Lask();
    Lask(int shot10Times, int x = -999, int y = -999, bool innerTen = false, QTime shotTime = QTime());
    Lask(QString siusRow);
    Lask(QJsonObject);
    void set(const Lask *l);
    bool isEmpty();

public:
    enum {Muu = 0, Ohupuss = 1, Ohupustol = 2, Sportpuss = 3, Spordipustol = 4, Puss = 5, Pustol = 6};
    static bool calcIfInnerTen(int targetType, int x, int y);

    void clear();
    bool equals(const Lask other) const;
    int getILask() const;
//    float getFLask() const;   // No point of having this fn, as result is often wrong
    QString getSLask() const;
    int get10Lask() const;
    bool isInnerTen() const;
    int X() const;
    int Y() const;
//    QPoint XY();
    void setInnerTen(bool isInnerTen);
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
    QString stringX() const;
    QString stringY() const;
    QTime shotTime() const;
    QJsonObject toJson() const;

    bool operator ==(const Lask other) const;

private:
    int lask;   // Value as multiplied with 10 (10,5 -> 105)
    bool m_innerTen;
    int m_x;    // Values in nanometers, to keep it as int, but have 3 decimal point accuracy with millimeters
    int m_y;    // Same here
    QTime m_shotTime;
    float stringToFloat(QString s);
};

#endif // LASK_H

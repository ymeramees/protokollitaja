#ifndef LASK_H
#define LASK_H

#include <QtGlobal>
//#if QT_VERSION >= 0x050000
//#include <QtWidgets>
//#else
//#include <QtGui/QWidget>
//#endif

#include <QJsonObject>
#include <QString>
#include <QPoint>
#include <QTime>

class Lask /*: public QWidget*/
{
//    Q_OBJECT

public:
    Lask(/*QWidget *parent = 0*/);
    Lask(int shot10Times, QString x = "-999", QString y = "-999", bool innerTen = false, QTime shotTime = QTime());
    void set(const Lask *l);
    bool isEmpty();
//    void operator=(const Lask & l);

public /*slots*/:
    void clear();
    bool equals(const Lask other) const;
    int getILask() const;
    float getFLask() const;
    QString getSLask() const;
    int get10Lask() const;
    bool isInnerTen() const;
    float X() const;
    float Y() const;
//    QPoint XY();
    void setInnerTen(bool isInnerTen);
    void setLask(int);
    void setLask(float);
    bool setLask(QString);
    void set10Lask(int);    //Lasu väärtuse sisestamine 10kordsena
    bool set10Lask(QString);    //Lasu väärtuse sisestamine 10kordsena
    void setShotTime(QTime newTime);
    void setMX(QString s);
    void setX(float);
    void setX(QString);
    void setMY(QString s);
    void setY(float);
    void setY(QString);
    void setXY(QPoint);
    QString stringX() const;
    QString stringY() const;
    QTime shotTime() const;
    QJsonObject toJson();

    bool operator ==(const Lask other) const;

private:
    int lask;   //Hoiab lasu väärtust kümnekordsena (10,5 -> 105)
//    int ilask;
//    float flask; <- seda ei ole ilmselt vaja
    bool m_innerTen;
    QString m_x;    //Enne oli int ja siis float aga parem vist on kui on QString, väärtused mm'des
    QString m_y;    //Sama siin, väärtused mm'des
    QTime m_shotTime;
};

#endif // LASK_H

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

class Lask /*: public QWidget*/
{
//    Q_OBJECT

public:
    Lask(/*QWidget *parent = 0*/);
    void set(const Lask *l);
    bool isEmpty();
//    void operator=(const Lask & l);

public /*slots*/:
    void clear();
    int getILask();
    float getFLask();
    QString getSLask();
    int get10Lask();
    float X();
    float Y();
//    QPoint XY();
    void setLask(int);
    void setLask(float);
    bool setLask(QString);
    void set10Lask(int);    //Lasu väärtuse sisestamine 10kordsena
    bool set10Lask(QString);    //Lasu väärtuse sisestamine 10kordsena
    void setX(float);
    void setX(QString);
    void setY(float);
    void setY(QString);
    void setXY(QPoint);
    QString stringX();
    QString stringY();
    QJsonObject toJson();

private:
    int lask;   //Hoiab lasu väärtust kümnekordsena (10,5 -> 105)
//    int ilask;
//    float flask; <- seda ei ole ilmselt vaja
    QString x;    //Enne oli int ja siis float aga parem vist on kui on QString, väärtused mm'des
    QString y;    //Sama siin, väärtused mm'des
};

#endif // LASK_H

﻿#include "lask.h"

Lask::Lask()
{
    clear();
}

Lask::Lask(int shot10Times, int x, int y, bool innerTen, QTime shotTime)
{
    lask = shot10Times;
    m_x = x;
    m_y = y;
    m_innerTen = innerTen;
    m_shotTime = shotTime;
}

Lask::Lask(QString siusRow)
{
    setSiusShot(siusRow);
}

Lask::Lask(QJsonObject shotJson)
{
    clear();

    setLask(shotJson["shotValue"].toString());
    m_x = shotJson["shotX"].toInt();
    m_y = shotJson["shotY"].toInt();
    m_shotTime = QTime::fromString(shotJson["shotTime"].toString());
    m_innerTen = shotJson["innerTen"].toBool();
}

void Lask::clear()
{
    m_innerTen = false;
    lask = -999;
    m_shotTime = QTime();
    m_x = -999;
    m_y = -999;
}

bool Lask::equals(const Lask other) const
{
    if(lask == other.get10Lask()
            && m_innerTen == other.isInnerTen()
            && stringX().compare(other.stringX()) == 0
            && stringY().compare(other.stringY()) == 0
            && m_shotTime.toString().compare(other.shotTime().toString()) == 0)
        return true;
    else
        return false;
}

int Lask::getILask() const
{
    if(lask == -999)
        return lask;
    return lask / 10;   // As shot value is kept multiplied with 10, then it needs to be divided, decimals will be cut off
}

QString Lask::getSLask() const
{
    if(lask == -999)
        return "";  // If there is no shot value, empty string is returned
    QString l = QString("%1").arg(lask);
    l.insert(l.length() - 1,',');  // Comma needs to be added
    return l;
}

int Lask::get10Lask() const
{
    return lask;
}

bool Lask::calcIfInnerTen(int targetType, int x, int y)
{
    //Calculate shot center distance for determing inner tens:
    if(x != -999 && y != -999){
        long centerDistance = qRound(qSqrt(x*x + y*y));

        switch(targetType){
        case Ohupuss : {
            if(centerDistance <= 2000)
                return true;
            break;
        }
        case Ohupustol : {
            if(centerDistance <= 4750)
                return true;
            break;
        }
        case Sportpuss : {
            if(centerDistance <= 5300)
                return true;
            break;
        }
        }
    }
    return false;
}

bool Lask::isInnerTen() const
{
    return m_innerTen;
}

bool Lask::isEmpty()
{
    if(lask == -999 && m_x == -999 && m_y == -999)
        return true;
    else return false;
}

int Lask::X() const
{
    return m_x;   // Returns nanometers
}

int Lask::Y() const
{
    return m_y;   // Returns nanometers
}

void Lask::setInnerTen(bool isInnerTen)
{
    m_innerTen = isInnerTen;
}

//QPoint Lask::XY()
//{
//    return QPoint(x, y);
//}

void Lask::setLask(int l)
{
    lask = l * 10;  // Shot value is kept multiplied with 10
}

void Lask::setLask(float l)
{
    lask = qRound(l * 10);  // Shot value is kept multiplied with 10
}

bool Lask::setLask(QString l)
{
    if(l.isEmpty()){
        lask = -999;
        return true;
    }

    bool onnestus = false;
    float flask = 0;
    flask = l.toFloat(&onnestus);
    if(!onnestus){
        if(l.contains(',')){
            l.replace(',', '.');
        }else if(l.contains('.')){
            l.replace('.', ',');
        }
        flask = l.toFloat(&onnestus);
    }
    lask = qRound(flask * 10);
    return onnestus;
}

void Lask::set10Lask(int l) // Shot value is kept multiplied with 10
{
    lask = l;
}

bool Lask::set10Lask(QString l)
{
    bool onnestus = false;
    int ilask = 0;
    ilask = l.toInt(&onnestus);
    if(!onnestus){
        if(l.contains(',')){
            l.replace(',', '.');
        }else if(l.contains('.')){
            l.replace('.', ',');
        }
        ilask = l.toInt(&onnestus);
    }
    lask = ilask;
    return onnestus;
}

void Lask::setShotTime(QTime newTime)
{
    m_shotTime = newTime;
}

void Lask::setMX(QString s)
{
    m_x = qRound(stringToFloat(s) * 1000000);   // meters to nanometers
}

void Lask::setMmX(float k)
{
    m_x = qRound(k * 1000);
}

void Lask::setMmX(QString s)
{
    m_x = qRound(stringToFloat(s) * 1000);   // millimeters to nanometers;
}

void Lask::setNanoX(QString s)
{
    m_x = qRound(stringToFloat(s));   // nanometers;
}

void Lask::setMY(QString s)
{
    m_y = qRound(stringToFloat(s) * 1000000);   // meters to nanometers
}

void Lask::setMmY(float k)
{
    m_y = qRound(k * 1000);
}

void Lask::setMmY(QString s)
{
    m_y = qRound(stringToFloat(s) * 1000);   // millimeters to nanometers;
}

void Lask::setNanoY(QString s)
{
    m_y = qRound(stringToFloat(s));   // nanometers;
}

void Lask::setNanoXY(QPoint p)
{
    m_x = p.x(); // Nanometers
    m_y = p.y(); // Nanometers
}

void Lask::setSiusShot(QString siusRow)
{
    clear();

    QStringList rowParts = siusRow.split(';');
    if (rowParts.length() < 16)
        return; // Ignore faulty rows to avoid crashes // TODO implement a nicer solution

    if(rowParts[11].toInt() == 0){  //If [11] is 0, then results are being read with decimals and shot value is in  [10]. If not 0, then results are with full rings.
        set10Lask(rowParts[10]);
    }else
        set10Lask(rowParts[11]);
    setMX(rowParts[14]);
    setMY(rowParts[15]);
    setShotTime(QTime::fromString(rowParts[6]));
    if(rowParts[9].toInt() >= 512)
        setInnerTen(true);
}

void Lask::set(const Lask *l)
{
    this->m_innerTen = l->isInnerTen();
    this->lask = l->lask;
    this->m_shotTime = l->shotTime();
    this->m_x = l->m_x;
    this->m_y = l->m_y;
}

float Lask::stringToFloat(QString s)
{
    bool success = false;
    float fx = 0;
    fx = s.toFloat(&success);
    if(!success){
        if(s.contains(',')){
            s.replace(',', '.');
        }else if(s.contains('.')){
            s.replace('.', ',');
        }
        fx = s.toFloat(&success);
    }
    return fx;
}

QString Lask::stringX() const
{
    double fx = m_x;
    return QString("%1").arg(fx / 1000);
}

QString Lask::stringY() const
{
    double fy = m_y;
    return QString("%1").arg(fy / 1000);
}

QTime Lask::shotTime() const
{
    return m_shotTime;
}

QJsonObject Lask::toJson() const
{
    QJsonObject shotJson;
    shotJson["shotValue"] = getSLask();
    shotJson["shotX"] = X();
    shotJson["shotY"] = Y();
    shotJson["shotTime"] = shotTime().toString();   // FIXME milliseconds part is lost
    shotJson["innerTen"] = isInnerTen();
    return shotJson;
}

bool Lask::operator ==(const Lask other) const
{
    if(equals(other))
        return true;
    else
        return false;
}

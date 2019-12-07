#include "lask.h"

Lask::Lask(/*QWidget *parent*/)/* : QWidget(parent)*/
{
    clear();
//    hide();
}

Lask::Lask(int shot10Times, QString x, QString y, bool innerTen, QTime shotTime)
{
    lask = shot10Times;
    m_x = x;
    m_y = y;
    m_innerTen = innerTen;
    m_shotTime = shotTime;
}

void Lask::clear()
{
    m_innerTen = false;
    lask = -999;
//    flask = -1;
    m_shotTime = QTime();
    m_x = "-999";
    m_y = "-999";
}

bool Lask::equals(const Lask other) const
{
    if(lask == other.get10Lask()
            && m_innerTen == other.isInnerTen()
            && m_x.compare(other.stringX()) == 0
            && m_y.compare(other.stringY()) == 0
            && m_shotTime.toString().compare(other.shotTime().toString()) == 0)
        return true;
    else
        return false;
}

int Lask::getILask() const
{
    if(lask == -999)
        return lask;
    return lask / 10;   //Kuna lasu väärtust hoitakse kümnekordsena, siis on vaja jagada 10'ga, komakohad lõigatakse lihtsalt maha
}

float Lask::getFLask() const
{
    if(lask == -999)
        return lask;
    float flask = lask;
    return flask / 10;  //Kuna lasu väärtust hoitakse kümnekordsena, siis on vaja jagada 10'ga
}

QString Lask::getSLask() const
{
    if(lask == -999)
        return "";  //Kui lasku ei ole, tagastatakse tühi string
    QString l = QString("%1").arg(lask);
    l.insert(l.length() - 1,',');  //Kuna lasu väärtust hoitakse kümnekordsena, siis on vaja lisada koma
    return l;
}

int Lask::get10Lask() const
{
    return lask;
}

bool Lask::isInnerTen() const
{
    return m_innerTen;
}

bool Lask::isEmpty()
{
    if(lask == -999 && m_x == "-999" && m_y == "-999")
        return true;
    else return false;
}

float Lask::X() const
{
    bool onnestus = false;
    float fx = 0;
    fx = m_x.toFloat(&onnestus);
    if(!onnestus){
        QString sx = m_x;
        if(sx.contains(',')){
            sx.replace(',', '.');
        }else if(sx.contains('.')){
            sx.replace('.', ',');
        }
        fx = sx.toFloat(&onnestus);
    }
    return fx;
}

//int Lask::X()
//{
//    return x.toInt();
//}

float Lask::Y() const
{
    bool onnestus = false;
    float fy = 0;
    fy = m_y.toFloat(&onnestus);
    if(!onnestus){
        QString sy = m_y;
        if(sy.contains(',')){
            sy.replace(',', '.');
        }else if(sy.contains('.')){
            sy.replace('.', ',');
        }
        fy = sy.toFloat(&onnestus);
    }
    return fy;
}

void Lask::setInnerTen(bool isInnerTen)
{
    m_innerTen = isInnerTen;
}

//int Lask::Y()
//{
//    return y.toInt();
//}

//QPoint Lask::XY()
//{
//    return QPoint(x, y);
//}

void Lask::setLask(int l)
{
    lask = l * 10;  //Kuna lasu väärtust hoitakse kümnekordsena, siis on vaja korrutada 10'ga
//    flask = l;
}

void Lask::setLask(float l)
{
//    float flask = l * 10; //Kuna lasu väärtust hoitakse kümnekordsena, siis on vaja korrutada 10'ga
//    lask = flask;
    lask = qRound(l * 10);
}

bool Lask::setLask(QString l)
{
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
//    flask *= 10;
    lask = qRound(flask * 10);
    return onnestus;
}

void Lask::set10Lask(int l) //Lasu väärtuse sisestamine 10kordsena
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
//    flask *= 10;
    lask = ilask;
    return onnestus;
}

void Lask::setShotTime(QTime newTime)
{
    m_shotTime = newTime;
}

void Lask::setMX(QString s)
{
    bool onnestus = false;
    float fx = 0;
    fx = s.toFloat(&onnestus);
    if(!onnestus){
        if(s.contains(',')){
            s.replace(',', '.');
        }else if(s.contains('.')){
            s.replace('.', ',');
        }
        fx = s.toFloat(&onnestus);
    }
    fx *= 1000; //Convert meters to millimeters
    setX(fx);
}

void Lask::setX(float k)
{
    m_x = QString("%1").arg(k);
    m_x.replace('.', ',');
//    x = k;
}

void Lask::setX(QString s)
{
    m_x = s;
    m_x.replace('.', ',');
//    bool onnestus = true;
//    x = s.toFloat(&onnestus);
//    if(!onnestus)
//        x = s.replace(".", ",").toFloat(&onnestus);
//    if(!onnestus)
    //        x = s.replace(",", ".").toFloat(&onnestus);
}

void Lask::setMY(QString s)
{
    bool onnestus = false;
    float fy = 0;
    fy = s.toFloat(&onnestus);
    if(!onnestus){
        if(s.contains(',')){
            s.replace(',', '.');
        }else if(s.contains('.')){
            s.replace('.', ',');
        }
        fy = s.toFloat(&onnestus);
    }
    fy *= 1000; //Convert meters to millimeters
    setX(fy);
}

void Lask::setY(float k)
{
    m_y = QString("%1").arg(k);
    m_y.replace('.', ',');
//    y = k;
}

void Lask::setY(QString s)
{
    m_y = s;
    m_y.replace('.', ',');
//    bool onnestus = true;
//    y = s.toFloat(&onnestus);
//    if(!onnestus)
//        y = s.replace(".", ",").toFloat(&onnestus);
//    if(!onnestus)
//        y = s.replace(",", ".").toFloat(&onnestus);
}

void Lask::setXY(QPoint p)  //See ei ole hea, kuna ei võimalda komakohti
{
    m_x = QString("%1").arg(p.x());
    m_y = QString("%1").arg(p.y());
}

void Lask::set(const Lask *l)
{
    this->m_innerTen = l->isInnerTen();
    this->lask = l->lask;
    this->m_shotTime = l->shotTime();
    this->m_x = l->m_x;
    this->m_y = l->m_y;
}

QString Lask::stringX() const
{
    return m_x;
}

QString Lask::stringY() const
{
    return m_y;
}

QTime Lask::shotTime() const
{
    return m_shotTime;
}

QJsonObject Lask::toJson()
{
    QJsonObject shotJson;
    shotJson["shotValue"] = getSLask();
    shotJson["shotX"] = X();
    shotJson["shotY"] = Y();
    shotJson["shotTime"] = shotTime().toString();
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

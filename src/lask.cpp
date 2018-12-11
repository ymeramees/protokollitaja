#include "lask.h"

Lask::Lask(/*QWidget *parent*/)/* : QWidget(parent)*/
{
    clear();
//    hide();
}

void Lask::clear()
{
    m_innerTen = false;
    lask = -999;
//    flask = -1;
    m_shotTime = QTime();
    x = "-999";
    y = "-999";
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
    if(lask == -999 && x == "-999" && y == "-999")
        return true;
    else return false;
}

float Lask::X()
{
    bool onnestus = false;
    float fx = 0;
    fx = x.toFloat(&onnestus);
    if(!onnestus){
        if(x.contains(',')){
            x.replace(',', '.');
        }else if(x.contains('.')){
            x.replace('.', ',');
        }
        fx = x.toFloat(&onnestus);
    }
    return fx;
}

//int Lask::X()
//{
//    return x.toInt();
//}

float Lask::Y()
{
    bool onnestus = false;
    float fy = 0;
    fy = y.toFloat(&onnestus);
    if(!onnestus){
        if(y.contains(',')){
            y.replace(',', '.');
        }else if(y.contains('.')){
            y.replace('.', ',');
        }
        fy = y.toFloat(&onnestus);
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
    x = QString("%1").arg(k);
//    x = k;
}

void Lask::setX(QString s)
{
    x = s;
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
    y = QString("%1").arg(k);
//    y = k;
}

void Lask::setY(QString s)
{
    y = s;
//    bool onnestus = true;
//    y = s.toFloat(&onnestus);
//    if(!onnestus)
//        y = s.replace(".", ",").toFloat(&onnestus);
//    if(!onnestus)
//        y = s.replace(",", ".").toFloat(&onnestus);
}

void Lask::setXY(QPoint p)  //See ei ole hea, kuna ei võimalda komakohti
{
    x = QString("%1").arg(p.x());
    y = QString("%1").arg(p.y());
}

void Lask::set(const Lask *l)
{
    this->m_innerTen = l->isInnerTen();
    this->lask = l->lask;
    this->m_shotTime = l->shotTime();
    this->x = l->x;
    this->y = l->y;
}

QString Lask::stringX()
{
    return x;
}

QString Lask::stringY()
{
    return y;
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

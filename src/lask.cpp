#include "lask.h"

Lask::Lask(/*QWidget *parent*/)/* : QWidget(parent)*/
{
    clear();
//    hide();
}

void Lask::clear()
{
    lask = -999;
//    flask = -1;
    x = "-999";
    y = "-999";
}

int Lask::getILask()
{
    if(lask == -999)
        return lask;
    return lask / 10;   //Kuna lasu väärtust hoitakse kümnekordsena, siis on vaja jagada 10'ga, komakohad lõigatakse lihtsalt maha
}

float Lask::getFLask()
{
    if(lask == -999)
        return lask;
    float flask = lask;
    return flask / 10;  //Kuna lasu väärtust hoitakse kümnekordsena, siis on vaja jagada 10'ga
}

QString Lask::getSLask()
{
    if(lask == -999)
        return "";  //Kui lasku ei ole, tagastatakse tühi string
    QString l = QString("%1").arg(lask);
    l.insert(l.length() - 1,',');  //Kuna lasu väärtust hoitakse kümnekordsena, siis on vaja lisada koma
    return l;
}

int Lask::get10Lask()
{
    return lask;
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
    this->lask = l->lask;
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

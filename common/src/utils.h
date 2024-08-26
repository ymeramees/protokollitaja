#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QStringList>
#include <QHostAddress>
#include <QHostInfo>
#include <QNetworkInterface>

class Utils : public QObject
{
    Q_OBJECT
public:
    static QStringList getLocalIps();
};

#endif // UTILS_H

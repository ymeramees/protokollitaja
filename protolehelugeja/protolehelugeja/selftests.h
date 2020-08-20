#ifndef SELFTESTS_H
#define SELFTESTS_H

#include <QObject>
#include "lask.h"

class SelfTests : public QObject
{
    Q_OBJECT
public:
    explicit SelfTests(QObject *parent = nullptr);
    void generateShots(int noOfShots);

signals:
    void shotRead(Lask shot);
};

#endif // SELFTESTS_H

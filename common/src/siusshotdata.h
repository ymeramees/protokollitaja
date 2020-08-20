#ifndef SIUSSHOTDATA_H
#define SIUSSHOTDATA_H

#include <QMetaType>
#include <QStringList>

#include "lask.h"

struct SiusShotData {
    int id;
    int socketIndex;
    int siusShotNo;
    Lask shot;
    SiusShotData(){}
    SiusShotData(int id, int socketIndex, int siusShotNo, Lask shot):
        id(id), socketIndex(socketIndex), siusShotNo(siusShotNo), shot(shot){}
};

Q_DECLARE_METATYPE(SiusShotData)

#endif // SIUSSHOTDATA_H

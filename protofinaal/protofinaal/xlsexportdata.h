#ifndef XLSEXPORTDATA_H
#define XLSEXPORTDATA_H

#include <QVector>

#include "xlsexportservice.h"

class TeamsTable;

struct XlsExportData {
    int maxShots = 0;
    QVector<XlsTeamBlock> blocks;
};

XlsExportData buildXlsExportData(const QVector<TeamsTable*> &tables);

#endif // XLSEXPORTDATA_H

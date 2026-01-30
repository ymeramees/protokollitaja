#include "xlsexportdata.h"

#include "teamstable.h"

XlsExportData buildXlsExportData(const QVector<TeamsTable*> &tables)
{
    XlsExportData data;

    // First pass: find max shots across all tables
    for (TeamsTable *teamsTable : tables) {
        int shots = teamsTable->lastValidShotIndex() + 1;
        if (shots > data.maxShots) {
            data.maxShots = shots;
        }
    }

    // Second pass: build blocks in reverse order using toXlsData()
    for (int tableIdx = tables.size() - 1; tableIdx >= 0; tableIdx--) {
        TeamsTable *teamsTable = tables.at(tableIdx);
        QVector<XlsTeamBlock> tableBlocks = teamsTable->toXlsData(data.maxShots);
        data.blocks.append(tableBlocks);
    }

    return data;
}

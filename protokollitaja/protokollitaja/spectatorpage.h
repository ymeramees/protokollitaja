#ifndef SPECTATORPAGE_H
#define SPECTATORPAGE_H

#include <QList>
#include <QSet>
#include <QStringList>

#include "leht.h"

namespace SpectatorPage {

constexpr int ColumnCount = 16;

struct SheetInfo {
    bool shown = true;
    bool combinable = false;
    int seriesCount = 0;
    int subtotalStep = 0;
    int competitorRows = 0;
};

struct CombinedPage {
    QList<int> sheetIndexes;
    int consumedSheets = 0;
    int competitorRows = 0;
    int maxTimeMs = 0;
    QString title;
    QList<QStringList> rows;
    QSet<int> headingRows;
};

CombinedPage combinedPage(const QList<Leht*> &sheets, int pageRows);
void fillResult(QStringList &row, Laskur *competitor, bool &finalsShown);
QStringList resultRow(Laskur *competitor, const QString &place, int seriesCount, bool &finalsShown);
SheetInfo sheetInfo(Leht *sheet);
QList<int> sheetsOnPage(const QList<SheetInfo> &sheets, int pageRows);
int sortAndCount(Leht *sheet);

}

#endif // SPECTATORPAGE_H

#include "spectatorpage.h"

#include <algorithm>
#include <iterator>

namespace {

constexpr int MaxSeriesColumns = 6;

}

SpectatorPage::CombinedPage SpectatorPage::combinedPage(const QList<Leht*> &sheets, int pageRows)
{
    QList<SheetInfo> infos;
    std::transform(sheets.begin(), sheets.end(), std::back_inserter(infos), sheetInfo);

    CombinedPage page;
    page.sheetIndexes = sheetsOnPage(infos, pageRows);
    if(page.sheetIndexes.count() < 2)
        return page;

    QStringList blankRow;
    blankRow.fill(" ", ColumnCount);
    page.rows = QList<QStringList>(pageRows, blankRow);
    page.consumedSheets = page.sheetIndexes.last() + 1;

    bool finalsShown = false;   // Sheets with finals results are never combined
    int rowIndex = 0;
    std::for_each(page.sheetIndexes.begin(), page.sheetIndexes.end(), [&](int sheetIndex){
        Leht *sheet = sheets.at(sheetIndex);
        const int competitorRows = infos.at(sheetIndex).competitorRows;

        if(sheetIndex == 0)
            page.title = sheet->ekraaniNimi;
        else{
            rowIndex++;
            page.rows[rowIndex][1] = sheet->ekraaniNimi;
            page.headingRows << rowIndex++;
        }

        const int firstCompetitorRow = rowIndex;
        std::for_each(sheet->reasLaskurid.begin(), sheet->reasLaskurid.begin() + competitorRows, [&](Laskur *competitor){
            page.rows[rowIndex] = resultRow(competitor, QString("%1.").arg(rowIndex - firstCompetitorRow + 1), sheet->seeriateArv, finalsShown);
            rowIndex++;
        });

        page.competitorRows += competitorRows;
        page.maxTimeMs = qMax(page.maxTimeMs, sheet->maxTime());
    });

    return page;
}

void SpectatorPage::fillResult(QStringList &row, Laskur *competitor, bool &finalsShown)
{
    const QString remark = competitor->markus->text();
    const bool outOfCompetition = remark.contains("V.A", Qt::CaseInsensitive);

    row[14] = competitor->getSumma();
    if(remark.isEmpty() || outOfCompetition){
        row[15] = QString("%1").arg(float(competitor->keskmLask) / 10000);
        row[15].replace('.', ',');
    }else
        row[15] = remark;

    if(outOfCompetition)
        row[0] = "v.a.";
    else if(remark.contains("DNF", Qt::CaseInsensitive))
        row[0] = "DNF";
    else if(remark.contains("DSQ", Qt::CaseInsensitive))
        row[0] = "DSQ";

    if(finalsShown)
        row[15] = "";
    if(!competitor->finaal->text().isEmpty()){
        row[15] = competitor->finaal->text();
        finalsShown = true;
    }
}

QStringList SpectatorPage::resultRow(Laskur *competitor, const QString &place, int seriesCount, bool &finalsShown)
{
    QStringList row;
    row.fill(" ", ColumnCount);
    row[0] = place;
    row[1] = competitor->eesNimi->text();
    row[2] = competitor->perekNimi->text();
    row[3] = competitor->sunniAasta->text();
    row[4] = competitor->klubi->text();
    for(int i = 0; i < seriesCount && i < MaxSeriesColumns; i++)
        row[7 + i] = competitor->seeriad[i]->text();
    fillResult(row, competitor, finalsShown);
    return row;
}

SpectatorPage::SheetInfo SpectatorPage::sheetInfo(Leht *sheet)
{
    SheetInfo info;
    const bool individual = sheet->pageType() == Leht::Individual;
    const bool hasFinals = std::any_of(sheet->laskurid.begin(), sheet->laskurid.end(), [](Laskur *competitor){
        return !competitor->finaal->text().isEmpty();
    });

    info.shown = sheet->naidata && (!individual || !sheet->laskurid.isEmpty());
    info.seriesCount = sheet->seeriateArv;
    info.subtotalStep = sheet->vSummadeSamm;
    info.combinable = individual && info.shown && !hasFinals && info.seriesCount <= MaxSeriesColumns && info.subtotalStep < 2;
    if(info.combinable)
        info.competitorRows = sortAndCount(sheet);
    return info;
}

QList<int> SpectatorPage::sheetsOnPage(const QList<SheetInfo> &sheets, int pageRows)
{
    if(sheets.isEmpty())
        return {};

    const SheetInfo &first = sheets.first();
    QList<int> indexes{0};
    if(!first.combinable)
        return indexes;

    int freeRows = pageRows - first.competitorRows;   // The first sheet's title is above the table
    for(int i = 1; i < sheets.count(); i++){
        const SheetInfo &sheet = sheets.at(i);
        if(!sheet.shown)
            continue;

        const int neededRows = 1 + 1 + sheet.competitorRows;    // Separator, heading row and the competitors
        const bool sameLayout = sheet.seriesCount == first.seriesCount && sheet.subtotalStep == first.subtotalStep;
        if(!sheet.combinable || !sameLayout || neededRows > freeRows)
            break;

        indexes << i;
        freeRows -= neededRows;
    }
    return indexes;
}

int SpectatorPage::sortAndCount(Leht *sheet)
{
    const bool hasTargetNumbers = std::any_of(sheet->laskurid.begin(), sheet->laskurid.end(), [](Laskur *competitor){
        return !competitor->rajaNr->text().isEmpty();
    });
    const int withResults = std::count_if(sheet->laskurid.begin(), sheet->laskurid.end(), [](Laskur *competitor){
        return competitor->getSumma() != "0" && competitor->getSumma() != "0,0";
    });

    sheet->sorteeri(hasTargetNumbers && withResults == 0 ? 3 : 0);
    return withResults == 0 ? sheet->laskurid.count() : withResults;
}

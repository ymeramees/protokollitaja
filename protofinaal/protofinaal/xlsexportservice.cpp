#include "xlsexportservice.h"
#include "xlslib.h"

bool XLSExportService::exportResults(
    const QString &fileName,
    const QString &competitionName,
    const QString &timePlace,
    const QString &eventName,
    int maxShots,
    const QVector<XlsTeamBlock> &blocks,
    QString *errorMessage)
{
    xlslib_core::workbook book;

    xlslib_core::font_t *titleFont = book.font("Times New Roman");
    titleFont->SetBoldStyle(xlslib_core::BOLDNESS_BOLD);
    titleFont->SetHeight(20*16);

    xlslib_core::font_t *headerFont = book.font("Times New Roman");
    headerFont->SetBoldStyle(xlslib_core::BOLDNESS_BOLD);
    headerFont->SetHeight(20*12);

    xlslib_core::font_t *underlineFont = book.font("Times New Roman");
    underlineFont->SetUnderlineStyle(xlslib_core::UNDERLINE_SINGLE);
    underlineFont->SetItalic(true);
    underlineFont->SetHeight(20*12);

    xlslib_core::font_t *textFont = book.font("Times New Roman");
    textFont->SetHeight(20*12);

    xlslib_core::font_t *boldFont = book.font("Times New Roman");
    boldFont->SetBoldStyle(xlslib_core::BOLDNESS_BOLD);
    boldFont->SetHeight(20*12);

    xlslib_core::worksheet *sheet = book.sheet(eventName.toStdString());
    if (!sheet) {
        if (errorMessage) {
            *errorMessage = "Failed to create worksheet";
        }
        return false;
    }

    sheet->defaultColwidth(8);

    // Get series structure from first competitor's data
    QVector<int> seriesSizes;
    if (!blocks.isEmpty() && !blocks.first().competitorRows.isEmpty()) {
        const QVector<XlsSeries> &series = blocks.first().competitorRows.first().series;
        for (const XlsSeries &s : series) {
            seriesSizes.append(s.shots.size());
        }
    }

    int row = 0;
    writeHeader(sheet, row, competitionName, timePlace, eventName, seriesSizes, titleFont, headerFont, underlineFont);

    for (const XlsTeamBlock &block : blocks) {
        writeRow(sheet, row++, seriesSizes, block.teamTotalRow, textFont, boldFont);
        for (const XlsShotRow &r : block.competitorRows) {
            writeRow(sheet, row++, seriesSizes, r, textFont, boldFont);
        }
        row++; // Empty row between teams
    }

    if (book.Dump(fileName.toStdString()) != NO_ERRORS) {
        if (errorMessage) {
            *errorMessage = "Failed to write XLS file";
        }
        return false;
    }

    return true;
}

void XLSExportService::writeHeader(
    xlslib_core::worksheet *sheet,
    int &row,
    const QString &competitionName,
    const QString &timePlace,
    const QString &eventName,
    const QVector<int> &seriesSizes,
    xlslib_core::font_t *titleFont,
    xlslib_core::font_t *headerFont,
    xlslib_core::font_t *underlineFont)
{
    int totalShots = 0;
    for (int size : seriesSizes) {
        totalShots += size;
    }
    int totalCols = 2 + totalShots + seriesSizes.size() + 1;  // rank + name + shots + series sums + total

    sheet->label(row, 0, competitionName.toStdString())->font(titleFont);
    sheet->FindCell(row, 0)->halign(xlslib_core::HALIGN_CENTER);
    sheet->rowheight(row, 434);
    if (totalCols > 1) {
        sheet->merge(row, 0, row, totalCols - 1);
    }
    row++;

    sheet->label(row, totalCols - 1, timePlace.toStdString())->font(headerFont);
    sheet->FindCell(row, totalCols - 1)->halign(xlslib_core::HALIGN_RIGHT);
    row += 2;

    sheet->label(row, 1, eventName.toStdString())->font(headerFont);
    row++;

    sheet->colwidth(0, 32*38);  // Rank
    sheet->colwidth(1, 32*180); // Name

    sheet->label(row, 0, "Place")->font(underlineFont);
    sheet->FindCell(row, 0)->halign(xlslib_core::HALIGN_CENTER);
    sheet->label(row, 1, "Name")->font(underlineFont);

    int col = 2;
    int shotNumber = 1;
    
    for (int seriesIdx = 0; seriesIdx < seriesSizes.size(); seriesIdx++) {
        int seriesSize = seriesSizes[seriesIdx];
        
        // Add shot columns for this series
        for (int shotInSeries = 0; shotInSeries < seriesSize; shotInSeries++) {
            sheet->colwidth(col, 32*45);
            sheet->label(row, col, QString::number(shotNumber).toStdString())->font(underlineFont);
            sheet->FindCell(row, col)->halign(xlslib_core::HALIGN_CENTER);
            col++;
            shotNumber++;
        }
        
        // Add series sum column after each series
        sheet->colwidth(col, 32*50);
        sheet->label(row, col, "\u03A3")->font(underlineFont);
        sheet->FindCell(row, col)->halign(xlslib_core::HALIGN_CENTER);
        col++;
    }

    sheet->colwidth(col, 32*60);
    sheet->label(row, col, "\u03A3")->font(underlineFont);
    sheet->FindCell(row, col)->halign(xlslib_core::HALIGN_CENTER);

    row++;
}

void XLSExportService::writeRow(
    xlslib_core::worksheet *sheet,
    int row,
    const QVector<int> &seriesSizes,
    const XlsShotRow &data,
    xlslib_core::font_t *textFont,
    xlslib_core::font_t *boldFont)
{
    sheet->label(row, 0, data.rank.toStdString())->font(textFont);
    sheet->label(row, 1, data.name.toStdString())->font(textFont);

    int col = 2;

    // Write each series with its shots and total
    for (int seriesIdx = 0; seriesIdx < data.series.size(); seriesIdx++) {
        const XlsSeries &series = data.series[seriesIdx];
        
        // Write shots for this series
        for (int shotIdx = 0; shotIdx < series.shots.size(); shotIdx++) {
            if (series.shots[shotIdx] >= 0) {
                double shotValue = series.shots[shotIdx] / 10.0;
                sheet->number(row, col, shotValue)->font(textFont);
                sheet->FindCell(row, col)->halign(xlslib_core::HALIGN_CENTER);
            }
            col++;
        }
        
        // Write series total
        if (!series.total.isEmpty()) {
            double seriesValue = series.total.toDouble();
            sheet->number(row, col, seriesValue)->font(boldFont);
            sheet->FindCell(row, col)->halign(xlslib_core::HALIGN_CENTER);
        }
        col++;
    }

    if (!data.total.isEmpty()) {
        double totalValue = data.total.toDouble();
        sheet->number(row, col, totalValue)->font(boldFont);
        sheet->FindCell(row, col)->halign(xlslib_core::HALIGN_CENTER);
    }
}

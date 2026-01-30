#ifndef XLSEXPORTSERVICE_H
#define XLSEXPORTSERVICE_H

#include <QString>
#include <QVector>

namespace xlslib_core {
class workbook;
class worksheet;
class font_t;
}

struct XlsSeries {
    QVector<int> shots;  // x10 shot values, -1 for no shot
    QString total;       // Series total as string
};

struct XlsShotRow {
    QString rank;
    QString name;
    QVector<XlsSeries> series;
    QString total;
};

struct XlsTeamBlock {
    XlsShotRow teamTotalRow;
    QVector<XlsShotRow> competitorRows;
};

class XLSExportService
{
public:
    bool exportResults(
        const QString &fileName,
        const QString &competitionName,
        const QString &timePlace,
        const QString &eventName,
        int maxShots,
        const QVector<XlsTeamBlock> &blocks,
        QString *errorMessage = nullptr);

private:
    void writeHeader(
        xlslib_core::worksheet *sheet,
        int &row,
        const QString &competitionName,
        const QString &timePlace,
        const QString &eventName,
        const QVector<int> &seriesSizes,
        xlslib_core::font_t *titleFont,
        xlslib_core::font_t *headerFont,
        xlslib_core::font_t *underlineFont);

    void writeRow(
        xlslib_core::worksheet *sheet,
        int row,
        const QVector<int> &seriesSizes,
        const XlsShotRow &data,
        xlslib_core::font_t *textFont,
        xlslib_core::font_t *boldFont);
};

#endif // XLSEXPORTSERVICE_H

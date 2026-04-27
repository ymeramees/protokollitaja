#ifndef SPECTATORWINDOW_H
#define SPECTATORWINDOW_H

#include <QFile>
#include <QEvent>
#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QTextStream>
#include <QTextBrowser>
#include <QResizeEvent>
#include "../../common/src/target.h"

extern bool verbose;

class SpectatorWindow : public QWidget
{
    Q_OBJECT
public:
    explicit SpectatorWindow(QWidget *parent = 0);
    ~SpectatorWindow();

signals:

public slots:
    void addRow(QString rank, QString teamName, QString competitorName, QString shot, QString series, QString sum, QString diff);
    void addTarget(QString name, QString targetNo, int gunType);
    void clearResults();
    void clearTargets();
    void fullscreen();
    bool hasTarget(QString targetNo) const;
    void mouseDoubleClickEvent(QMouseEvent *event);
    void resetTarget(QString targetNo);
    void setHeading(QString competitionName, QString timePlace, QString eventName, QString rankLabel, QString nameLabel, QString seriesLabel, QString pointsLabel, QString diffLabel);
    void setResults(QString resultsHTML);
    void updateTarget(QString targetNo, Lask shot);
//    void setResults(QStringList resultsRows);

private:
    QString originalResultsTemplate;
    QString resultsTemplate;
    QString rowTemplate;
//    QString resultsHTML;
    QString newResultsFontSize;
    QString originalResultsFontSize;
    QString m_resultRows;
    QGridLayout *targetsBox;
    QMap<QString, Target *> m_targets; // Map of targetNo -> Target widget
    QTextBrowser resultsView;
    QVBoxLayout vBox;
    bool eventFilter(QObject *watched, QEvent *event);
    void initializeTemplates();
    void resizeEvent(QResizeEvent *event);
};

#endif // SPECTATORWINDOW_H

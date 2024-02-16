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
    void clearResults();
    void fullscreen();
    void mouseDoubleClickEvent(QMouseEvent *event);
    void setHeading(QString competitionName, QString timePlace, QString eventName, QString rankLabel, QString nameLabel, QString seriesLabel, QString pointsLabel, QString diffLabel);
    void setResults(QString resultsHTML);
//    void setResults(QStringList resultsRows);

private:
    QString originalResultsTemplate;
    QString resultsTemplate;
    QString rowTemplate;
//    QString resultsHTML;
    QString newResultsFontSize;
    QString originalResultsFontSize;
    QString m_resultRows;
    QGridLayout targetsBox;
    QTextBrowser resultsView;
    QVBoxLayout vBox;
    bool eventFilter(QObject *watched, QEvent *event);
    void initializeTemplates();
    void resizeEvent(QResizeEvent *event);
};

#endif // SPECTATORWINDOW_H

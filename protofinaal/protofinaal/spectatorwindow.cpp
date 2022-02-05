#include "spectatorwindow.h"

SpectatorWindow::SpectatorWindow(QWidget *parent) : QWidget(parent)
{
    resultsView.setText("Finaali tulemused");
    vBox.addWidget(&resultsView);
    setLayout(&vBox);

    resultsView.installEventFilter(this);
    resultsView.viewport()->installEventFilter(this);
    resultsView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    vBox.setContentsMargins(0, 0, 0, 0);
//    setContentsMargins(0, 0, 0, 0);
    initializeTemplates();
}

SpectatorWindow::~SpectatorWindow()
{
    if(verbose)
        QTextStream(stdout) << "SpectatorWindow::~SpectatorWindow()" << endl;
}

void SpectatorWindow::addRow(QString rank, QString teamName, QString competitorName, QString s1, QString sum, QString diff)
{
    QString row = rowTemplate;
    row.replace("#no#", rank);
    row.replace("#teamName#", teamName);
    row.replace("#competitorName#", competitorName);
    row.replace("#S1#", s1);
    row.replace("#sum#", sum);
    row.replace("#difference#", diff);
//    row.replace(originalResultsFontSize, newResultsFontSize);

    m_resultRows.prepend(row);

//    if(verbose)
//        QTextStream(stdout) << "resultRows & resultsHTML:" << resultRows << endl << endl << resultsHTML << endl;

//    resultsHTML.arg(resultRows);

//    if(verbose)
//        QTextStream(stdout) << "resultsHTML:" << resultsHTML << endl;
    QString newHTML = resultsTemplate.arg(m_resultRows);
    newHTML.replace(originalResultsFontSize, newResultsFontSize);
    resultsView.setHtml(newHTML);
//    resultsView.setText(resultsHTML.arg(resultRows));
//    resultsHTML = resultsTemplate;
}

bool SpectatorWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        fullscreen();
    }
    // standard event processing
    return QObject::eventFilter(watched, event);
}

void SpectatorWindow::clearResults()
{
//    resultsHTML = resultsTemplate;
    m_resultRows.clear();
}

void SpectatorWindow::fullscreen()
{
    if(windowState() == Qt::WindowFullScreen)
        setWindowState(Qt::WindowNoState);
    else setWindowState(Qt::WindowFullScreen);
}

void SpectatorWindow::initializeTemplates()
{
    QFile templateFile("spectatorView_template.html");
    if(templateFile.open(QIODevice::ReadOnly)){
        originalResultsTemplate = QString(templateFile.readAll());
        int rowStart = originalResultsTemplate.indexOf("<tr>", originalResultsTemplate.lastIndexOf("<tbody>"));
        int rowLength = originalResultsTemplate.lastIndexOf("</tr>") + 5 - rowStart;
        rowTemplate = originalResultsTemplate.mid(rowStart, rowLength);
//        resultsTemplate.remove(rowStart, rowLength);
        originalResultsTemplate.replace(rowTemplate, "%1");
        resultsTemplate = originalResultsTemplate;
//        resultsHTML = resultsTemplate;
        int fontSizeStart = rowTemplate.indexOf("font-size:");
        int fontSizeLength = rowTemplate.indexOf("pt" , fontSizeStart) + 2 - fontSizeStart;
        originalResultsFontSize = rowTemplate.mid(fontSizeStart, fontSizeLength);
        newResultsFontSize = originalResultsFontSize;
        if(verbose){
            QTextStream(stdout) << "rowTemplate:" << rowTemplate << endl;
            QTextStream(stdout) << "resultsTemplate:" << resultsTemplate << endl;
            QTextStream(stdout) << "originalResultsFontSize:" << originalResultsFontSize << endl;
        }
    }else {
        QFile::copy(":/templates/spectatorView_template.html", "spectatorView_template.html");
        QMessageBox::warning(this, tr("Teade"), tr("Tulemuste akna template faili ei leitud!\nProgramm lõi uue, vaikimisi seadistustega."), QMessageBox::Ok);
    }
}

void SpectatorWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    fullscreen();
}

void SpectatorWindow::resizeEvent(QResizeEvent *event)
{
    if(verbose)
        QTextStream(stdout) << "SpectatorWindow::resizeEvent()" << endl;
    bool success = false;
    int fontSize = originalResultsFontSize.mid(11, originalResultsFontSize.indexOf("pt") - 11).toInt(&success);
    if(success){
//        newResultsFontSize = originalResultsFontSize;
        int newFontSize = fontSize * (resultsView.height() - 100) / 800;
        if(verbose)
            QTextStream(stdout) << "SpectatorWindow::resizeEvent(), newFontSize = " << newFontSize << ", resultsView.height() = " << resultsView.height() << endl;
        newResultsFontSize = QString("font-size: %1pt").arg(newFontSize);
        QString newHTML = resultsTemplate.arg(m_resultRows);
        newHTML.replace(originalResultsFontSize, newResultsFontSize);
        resultsView.setHtml(newHTML);
    }
    event->accept();
}

void SpectatorWindow::setHeading(QString competitionName, QString timePlace, QString eventName, QString rankLabel, QString nameLabel, QString seriesLabel, QString pointsLabel, QString totalLabel)
{
    resultsTemplate = originalResultsTemplate;
    resultsTemplate.replace("#competitionName#", competitionName);
    resultsTemplate.replace("#timePlace#", timePlace);
    resultsTemplate.replace("#event#", eventName);
    resultsTemplate.replace("#rank#", rankLabel);
    resultsTemplate.replace("#name#", nameLabel);
    resultsTemplate.replace("#series#", seriesLabel);
    resultsTemplate.replace("#points#", pointsLabel);
    resultsTemplate.replace("#totalPoints#", totalLabel);
}

void SpectatorWindow::setResults(QString resultsHTML)
{
    resultsView.setHtml(resultsHTML);
}

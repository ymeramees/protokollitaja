#include "spectatorwindow.h"

SpectatorWindow::SpectatorWindow(QWidget *parent) : QWidget(parent), targetsBox(new QGridLayout)
{
    resultsView.setText(tr("Finaali tulemused"));
    resultsView.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    vBox.addWidget(&resultsView, 1);   // stretch factor 1 - take half
    vBox.addLayout(targetsBox, 1);     // stretch factor 1 - take half
    setLayout(&vBox);

    resultsView.installEventFilter(this);
    resultsView.viewport()->installEventFilter(this);
    resultsView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    resultsView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    vBox.setContentsMargins(0, 0, 0, 0);
    
    // Set white background for the main widget (which includes targets area)
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
    
//    setContentsMargins(0, 0, 0, 0);
    initializeTemplates();
}

SpectatorWindow::~SpectatorWindow()
{
    if(verbose)
        QTextStream(stdout) << "SpectatorWindow::~SpectatorWindow()" << Qt::endl;
    clearTargets();
}

void SpectatorWindow::addRow(QString rank, QString teamName, QString competitorName, QString shot, QString series, QString sum, QString diff)
{
    QString row = rowTemplate;
    row.replace("#no#", rank);
    row.replace("#teamName#", teamName);
    row.replace("#competitorName#", competitorName);
    row.replace("#shot#", shot);
    row.replace("#series#", series);
    row.replace("#sum#", sum);
    row.replace("#difference#", diff);
//    row.replace(originalResultsFontSize, newResultsFontSize);

    m_resultRows.prepend(row);

//    if(verbose)
//        QTextStream(stdout) << "resultRows & resultsHTML:" << resultRows << endl << endl << resultsHTML << Qt::endl;

//    resultsHTML.arg(resultRows);

//    if(verbose)
//        QTextStream(stdout) << "resultsHTML:" << resultsHTML << Qt::endl;
    QString newHTML = resultsTemplate.arg(m_resultRows);
    newHTML.replace(originalResultsFontSize, newResultsFontSize);
    resultsView.setHtml(newHTML);
//    resultsView.setText(resultsHTML.arg(resultRows));
//    resultsHTML = resultsTemplate;
}

void SpectatorWindow::addTarget(QString name, QString targetNo, int gunType)
{
    if (verbose)
        QTextStream(stdout) << "SpectatorWindow::addTarget(), name = " << name << ", targetNo = " << targetNo << ", gunType = " << gunType << Qt::endl;

    // Check if target already exists
    if (m_targets.contains(targetNo))
    {
        if (verbose)
            QTextStream(stdout) << "SpectatorWindow::addTarget(), target already exists: " << targetNo << Qt::endl;
        return;
    }

    Target *target = new Target(gunType, name, targetNo, this);
    target->setZoomEnabled(true);
    target->init(gunType);

    m_targets[targetNo] = target;

    int index = m_targets.size() - 1;
    int rows = 2;
    int columns = 4;
    int row = index / columns;
    int col = index % columns;

    targetsBox->addWidget(target, row, col);
    target->show();

    if (verbose)
        QTextStream(stdout) << "SpectatorWindow::addTarget(), added target at row=" << row << ", col=" << col << Qt::endl;
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

void SpectatorWindow::clearTargets()
{
    if (verbose)
        QTextStream(stdout) << "SpectatorWindow::clearTargets()" << Qt::endl;

    for (auto it = m_targets.begin(); it != m_targets.end(); ++it)
    {
        it.value()->deleteLater();
    }
    m_targets.clear();
}

void SpectatorWindow::fullscreen()
{
    if(windowState() == Qt::WindowFullScreen)
        setWindowState(Qt::WindowNoState);
    else setWindowState(Qt::WindowFullScreen);
}

bool SpectatorWindow::hasTarget(QString targetNo) const
{
    return m_targets.contains(targetNo);
}

void SpectatorWindow::initializeTemplates()
{
    QFile templateFile(":/spectatorView_template.html");
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
            QTextStream(stdout) << "rowTemplate:" << rowTemplate << Qt::endl;
            QTextStream(stdout) << "resultsTemplate:" << resultsTemplate << Qt::endl;
            QTextStream(stdout) << "originalResultsFontSize:" << originalResultsFontSize << Qt::endl;
        }
    }else {
        QFile::copy(":/spectatorView_template.html", "spectatorView_template.html");
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
        QTextStream(stdout) << "SpectatorWindow::resizeEvent()" << Qt::endl;
    bool success = false;
    int fontSize = originalResultsFontSize.mid(11, originalResultsFontSize.indexOf("pt") - 11).toInt(&success);
    if(success){
        int newFontSize = fontSize * (resultsView.height() - 100) / 800;
        if (newFontSize <= 0) newFontSize = 0;
        if(verbose)
            QTextStream(stdout) << "SpectatorWindow::resizeEvent(), newFontSize = " << newFontSize << ", resultsView.height() = " << resultsView.height() << Qt::endl;
        newResultsFontSize = QString("font-size: %1pt").arg(newFontSize);
        QString newHTML = resultsTemplate.arg(m_resultRows);
        newHTML.replace(originalResultsFontSize, newResultsFontSize);
        resultsView.setHtml(newHTML);
    }
    event->accept();
}

void SpectatorWindow::setHeading(QString competitionName, QString timePlace, QString eventName, QString rankLabel, QString nameLabel, QString shotLabel, QString seriesOrPointsLabel, QString diffLabel)
{
    resultsTemplate = originalResultsTemplate;
    resultsTemplate.replace("#competitionName#", competitionName);
    resultsTemplate.replace("#timePlace#", timePlace);
    resultsTemplate.replace("#event#", eventName);
    resultsTemplate.replace("#rank#", rankLabel);
    resultsTemplate.replace("#name#", nameLabel);
    resultsTemplate.replace("#shot#", shotLabel);
    resultsTemplate.replace("#series#", seriesOrPointsLabel);
    resultsTemplate.replace("#diff#", diffLabel);
}

void SpectatorWindow::setResults(QString resultsHTML)
{
    resultsView.setHtml(resultsHTML);
}

void SpectatorWindow::resetTarget(QString targetNo)
{
    if (verbose)
        QTextStream(stdout) << "SpectatorWindow::resetTarget(), targetNo = " << targetNo << Qt::endl;

    if (!m_targets.contains(targetNo))
    {
        if (verbose)
            QTextStream(stdout) << "SpectatorWindow::resetTarget(), target not found: " << targetNo << Qt::endl;
        return;
    }

    m_targets[targetNo]->reset();

    if (verbose)
        QTextStream(stdout) << "SpectatorWindow::resetTarget(), target reset" << Qt::endl;
}

void SpectatorWindow::updateTarget(QString targetNo, Lask shot)
{
    if (verbose)
        QTextStream(stdout) << "SpectatorWindow::updateTarget(), targetNo = " << targetNo << Qt::endl;

    if (!m_targets.contains(targetNo))
    {
        if (verbose)
            QTextStream(stdout) << "SpectatorWindow::updateTarget(), target not found: " << targetNo << Qt::endl;
        return;
    }

    m_targets[targetNo]->drawAShot(shot);

    if (verbose)
        QTextStream(stdout) << "SpectatorWindow::updateTarget(), shot drawn" << Qt::endl;
}

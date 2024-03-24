/////////////////////////////////////////////////////////////////////////////
/// ToDo list:
///
/////////////////////////////////////////////////////////////////////////////

#include "finalsfileexport.h"
#include "ui_finalsfileexport.h"

FinalsFileExport::FinalsFileExport(QVector<QStringList> inputTable, QString competitionFileLocation, QString competitionName, QString eventName, QString timeAndPlace, int eventType, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FinalsFileExport),
    m_eventFormats(":/eventFormats.json")
{
    ui->setupUi(this);
    connect(ui->drawButton, &QPushButton::clicked, this, &FinalsFileExport::drawStartPositions);

    m_competitionFileLocation = competitionFileLocation;
    m_competitionName = competitionName;
    m_eventName = eventName;
    m_eventType = eventType;
    m_timeAndPlace = timeAndPlace;

    ui->finalsFormatBox->addItems(m_eventFormats.formatIds());

    ui->finalsCompetitorsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->finalsCompetitorsTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->finalsCompetitorsTable->setColumnWidth(0, 70);
    ui->finalsCompetitorsTable->setColumnWidth(1, 70);
    QStringList headers;
    headers << tr("Raja nr") << tr("ID") << tr("Nimi") << tr("Tulemus");
    ui->finalsCompetitorsTable->setHorizontalHeaderLabels(headers);

    //Each "row" in inputTable: target, ID, screen name, result, first name, name, club
    for(int row = 0; row < inputTable.count(); row++){
        ui->finalsCompetitorsTable->insertRow(row);

        for(int column = 0; column < inputTable.at(row).count(); column++){
        QTableWidgetItem *item = new QTableWidgetItem(inputTable.at(row).at(column));

        if(column == 1) //Set ID field to read-only
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);

        ui->finalsCompetitorsTable->setItem(row, column, item);
        }
    }

    //Hide columns which hold data for exporting
    ui->finalsCompetitorsTable->hideColumn(6);
    ui->finalsCompetitorsTable->hideColumn(5);
    ui->finalsCompetitorsTable->hideColumn(4);


}

FinalsFileExport::~FinalsFileExport()
{
    delete ui;
}

void FinalsFileExport::accept()
{
    if(writeFinalsFile()){
        if(ui->exportStartListBox->isChecked()){
            if(writeFinalsStartListFile())
                done(Accepted);
        }else
            done(Accepted);
    }
}

//void FinalsFileExport::closeEvent(QCloseEvent *event)
//{

//}

void FinalsFileExport::drawStartPositions()
{
    for(int i = 0; i < ui->finalsCompetitorsTable->rowCount(); i++)
        ui->finalsCompetitorsTable->item(i, 0)->setText("");

    QVector<int> positionsToDraw;
    for(int i = 0; i < ui->finalsCompetitorsTable->rowCount(); i++)
        positionsToDraw << i;

    for(int i = 0; i < ui->finalsCompetitorsTable->rowCount(); i++){
        int index = QRandomGenerator::global()->generate() % positionsToDraw.count();
        char target = 65 + positionsToDraw.takeAt(index);
        ui->finalsCompetitorsTable->item(i, 0)->setText(QString("%1").arg(target));
    }
}

QString FinalsFileExport::getFinalsFileName()
{
    return finalsFileName;
}

int FinalsFileExport::getRelay()
{
    return ui->relayNumberBox->value();
}

void FinalsFileExport::setRelay(int relay)
{
    ui->relayNumberBox->setValue(relay);
}

bool FinalsFileExport::sortCompetitors()
{
    bool targetNumbersExist = true;
    for(int i = 0; i < ui->finalsCompetitorsTable->rowCount(); i++){
        if(ui->finalsCompetitorsTable->item(i, 0)->text().isEmpty())
            targetNumbersExist = false;
    }
    if(!targetNumbersExist){
        QMessageBox::warning(this, "Protokollitaja", tr("Kõigil laskuritel ei ole rada märgitud! Lisage numbrid (tähed) käsitsi või kasutage loosimise funktsiooni."), QMessageBox::Ok);
        return false;
    }

#ifdef QT_DEBUG
    qDebug() << "Protokollitaja::sortCompetitors()";
#endif
    for(int x = 0; x < ui->finalsCompetitorsTable->rowCount(); x++){
        for(int i = 0; i < (ui->finalsCompetitorsTable->rowCount() - 1); i++){
            if(ui->finalsCompetitorsTable->item(i, 0)->text().localeAwareCompare(ui->finalsCompetitorsTable->item(i+1, 0)->text()) > 0){
                QTableWidgetItem* upperItem;
                QTableWidgetItem* lowerItem;
                for(int column = 0; column < ui->finalsCompetitorsTable->columnCount(); column++){
                    upperItem = ui->finalsCompetitorsTable->takeItem(i, column);
                    lowerItem = ui->finalsCompetitorsTable->takeItem(i + 1, column);
                    ui->finalsCompetitorsTable->setItem(i, column, lowerItem);
                    ui->finalsCompetitorsTable->setItem(i + 1, column, upperItem);
                }
            }
        }
    }
#ifdef QT_DEBUG
        qDebug() << "Protokollitaja::sortCompetitors(), return true";
#endif
    return true;
}

bool FinalsFileExport::writeFinalsFile()
{
    if(!sortCompetitors())
        return false;

    QJsonObject finalsObj;
    finalsObj["fileVersion"] = 301;
    finalsObj["competitionName"] = m_competitionName;
    finalsObj["eventName"] = m_eventName;
    finalsObj["eventType"] = m_eventType;
    finalsObj["timePlace"] = m_timeAndPlace;

    QString fileLocation = m_competitionFileLocation.left(m_competitionFileLocation.lastIndexOf('/') + 1);
    finalsFileName = QFileDialog::getSaveFileName(this, tr("Salvesta finaal"), fileLocation + m_eventName + ".fin", tr("Protofinaali fail (*.fin)"));
    if(finalsFileName.isEmpty()) return false;

    QFile file(finalsFileName);
    if(file.open(QIODevice::ReadOnly))
        if(QMessageBox::critical(this, "Protokollitaja", tr("Sellise nimega fail on juba olemas. Kas "
                "soovite selle üle kirjutada?"), QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel){
            finalsFileName.clear();
            return false;
        }
    file.close();

    if(file.open(QIODevice::WriteOnly)) {
        finalsObj["eventType"] = ui->finalsFormatBox->currentText();
        finalsObj["eventConf"] = m_eventFormats.confById(ui->finalsFormatBox->currentText());
        QJsonArray startList;

        for(int i = 0; i < ui->finalsCompetitorsTable->rowCount(); i++){
            QJsonArray competitorsArray;    // This is an array, to support teams finals in future
            QJsonObject competitor;
            competitor["id"] = ui->finalsCompetitorsTable->item(i, 1)->text().toInt();
            competitor["name"] = ui->finalsCompetitorsTable->item(i, 5)->text() + " " + ui->finalsCompetitorsTable->item(i, 4)->text().left(1) + ".";
            competitor["result"] = ui->finalsCompetitorsTable->item(i, 3)->text();
            competitorsArray.append(competitor);
            startList.append(competitorsArray);
        }

        finalsObj["startList"] = startList;

        QJsonDocument jsonDoc(finalsObj);
        file.write(jsonDoc.toJson());
        file.close();
    } else {
        QMessageBox::critical(this, "Protokollitaja", tr("Ei õnnestu finaali faili luua! Kontrollige, "
                              "kas teil on sinna kausta kirjutamise õigused"), QMessageBox::Ok);
        finalsFileName.clear();
        return false;
    }
    return true;
}

bool FinalsFileExport::writeFinalsStartListFile()
{
    if(finalsFileName.isEmpty())
        return false;

    QVector<StartListWriter::StartListCompetitor> competitorsList;
    //Each row in competitors table: target, ID, screen name, result, first name, name, club
    for(int i = 0; i < ui->finalsCompetitorsTable->rowCount() && i < 8; i++){   //Currently only up to 8 shooters are supported in finals
        competitorsList.append(StartListWriter::StartListCompetitor{
                                   QString("%1").arg(ui->targetNumberBox->value() + i),
                                   ui->finalsCompetitorsTable->item(i, 1)->text(),
                                   ui->finalsCompetitorsTable->item(i, 4)->text(),
                                   ui->finalsCompetitorsTable->item(i, 5)->text(),
                                   ui->finalsCompetitorsTable->item(i, 6)->text(),
                                   ui->finalsCompetitorsTable->item(i, 3)->text(),
                                   m_eventType,
                                   ""  // TODO to be implemented
                               });
    }

    StartListWriter *startListWriter = new StartListWriter(
                competitorsList,
                finalsFileName,
                StartListWriter::SIUS,
                this,
                ui->relayNumberBox->value()
                );
    startListWriter->deleteLater();
    return true;
}

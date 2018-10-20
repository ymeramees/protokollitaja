/////////////////////////////////////////////////////////////////////////////
/// ToDo list:
/// Kohtade loosimine
/// Rajanumbrite järgi sorteerimine enne salvestamist
///
/////////////////////////////////////////////////////////////////////////////

#include "finalsfileexport.h"
#include "ui_finalsfileexport.h"

FinalsFileExport::FinalsFileExport(QVector<QStringList> inputTable, QString competitionFileLocation, QString competitionName, QString eventName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FinalsFileExport)
{
    ui->setupUi(this);

    //Hide some elements, until they are implemented:
    ui->ballotButton->hide();

    this->competitionFileLocation = competitionFileLocation;
    this->competitionName = competitionName;
    this->eventName = eventName;

    ui->finalsCompetitorsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->finalsCompetitorsTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->finalsCompetitorsTable->setColumnWidth(0, 70);
    ui->finalsCompetitorsTable->setColumnWidth(1, 70);
    QStringList headers;
    headers << "Raja nr" << "ID" << "Nimi" << "Tulemus";
    ui->finalsCompetitorsTable->setHorizontalHeaderLabels(headers);

    //Each "row" in inputTable: target, ID, screen name, result, first name, name, club
    for(int row = 0; row < inputTable.count(); row++){
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

bool FinalsFileExport::writeFinalsFile()
{
    QString fileLocation = competitionFileLocation.left(competitionFileLocation.lastIndexOf('/') + 1);
    finalsFileName = QFileDialog::getSaveFileName(this, tr("Salvesta finaal"), fileLocation + eventName + ".fnl", tr("Finaali fail (*.fnl)"));
    if(finalsFileName.isEmpty()) return false;

    QFile file(finalsFileName);
    /*if(file.open(QIODevice::ReadOnly))    //Why this is needed?
        if(QMessageBox::critical(this, "Protokollitaja", tr("Sellise nimega fail on juba olemas. Kas "
                "soovite selle üle kirjutada?"), QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel){
            finalsFileName.clear();
            return false;
        }
    file.close();*/

    if(file.open(QIODevice::WriteOnly)){
        QDataStream out(&file);

        out << (quint32)0x00FA3058;   //Number for verification
        out << (qint32)15;            //Which QFinaal file version it is
        out.setVersion(QDataStream::Qt_4_3);

        out << competitionName << eventName;

        for(int i = 0; i < ui->finalsCompetitorsTable->rowCount() && i < 8; i++){   //Currently only up to 8 shooters are supported in finals
            out << ui->finalsCompetitorsTable->item(i, 1)->text();
            out << ui->finalsCompetitorsTable->item(i, 2)->text();
            out << ui->finalsCompetitorsTable->item(i, 3)->text();
            for(int j = 0; j < 24; j++)
                out << (QString)"00";
            for(int j = 0; j < 24; j++)
                out << (QString)"00";
        }
        file.close();
    }else{
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

//    QString fileLocation = competitionFileLocation.left(competitionFileLocation.lastIndexOf('/') + 1);
//    finalsFileName = QFileDialog::getSaveFileName(this, tr("Salvesta finaali startlist"), fileLocation + eventName + ".csv", tr("Comma separated file (*.csv)"));
//    if(finalsFileName.isEmpty()) return false;

    QVector<QStringList> competitorsList;    //Each "row": target, ID, first name, name, club, result
    //Each row in competitors table: target, ID, screen name, result, first name, name, club
    for(int i = 0; i < ui->finalsCompetitorsTable->rowCount() && i < 8; i++){   //Currently only up to 8 shooters are supported in finals
        QStringList row;
        row << QString("%1").arg(ui->targetNumberBox->value() + i);
        row << ui->finalsCompetitorsTable->item(i, 1)->text();
        row << ui->finalsCompetitorsTable->item(i, 4)->text();
        row << ui->finalsCompetitorsTable->item(i, 5)->text();
        row << ui->finalsCompetitorsTable->item(i, 6)->text();
        row << ui->finalsCompetitorsTable->item(i, 3)->text();
#ifdef PROOV
        qDebug() << "Protokollitaja::writeFinalsStartListFile(), row: " << row;
#endif
        competitorsList.append(row);
    }

    StartListWriter *startListWriter = new StartListWriter(competitorsList, finalsFileName, this, ui->relayNumberBox->value());
    startListWriter->deleteLater();
    return true;
}

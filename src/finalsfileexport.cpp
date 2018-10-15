/////////////////////////////////////////////////////////////////////////////
/// ToDo list:
/// Kui lemasolevat faili üle kirjutada ei taheta, siis kuidas kohe uut asukohta ja nime küsida?
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
    ui->exportStartListBox->hide();
    ui->targetNumberBox->hide();
    ui->targetNumberLabel->hide();

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

    for(int row = 0; row < inputTable.count(); row++){
        for(int column = 0; column < inputTable.at(row).count(); column++){
        QTableWidgetItem *item = new QTableWidgetItem(inputTable.at(row).at(column));

        if(column == 1) //Set ID field to read-only
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);

        ui->finalsCompetitorsTable->setItem(row, column, item);
        }
    }
}

FinalsFileExport::~FinalsFileExport()
{
    delete ui;
}

void FinalsFileExport::accept()
{
    if(writeFinalsFile())
        done(Accepted);
}

//void FinalsFileExport::closeEvent(QCloseEvent *event)
//{

//}

QString FinalsFileExport::getFinalsFileName()
{
    return finalsFileName;
}

bool FinalsFileExport::writeFinalsFile()
{
    QString fileLocation = competitionFileLocation.left(competitionFileLocation.lastIndexOf('/') + 1);
    finalsFileName = QFileDialog::getSaveFileName(this, tr("Salvesta finaal"), fileLocation + eventName + ".fnl", tr("Finaali fail (*.fnl)"));
    if(finalsFileName.isEmpty()) return false;

    QFile file(finalsFileName);
    if(file.open(QIODevice::ReadOnly))
        if(QMessageBox::critical(this, "Protokollitaja", tr("Sellise nimega fail on juba olemas. Kas "
                "soovite selle üle kirjutada?"), QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel){
            finalsFileName.clear();
            return false;
        }
    file.close();

    if(file.open(QIODevice::WriteOnly)){
        QDataStream out(&file);

        out << (quint32)0x00FA3058;   //Number for verification
        out << (qint32)15;            //Which QFinaal file version it is
        out.setVersion(QDataStream::Qt_4_3);

        out << competitionName << eventName;

        for(int i = 0; i < ui->finalsCompetitorsTable->rowCount() && i < 8; i++){   //Currently only up to 8 shooters are supported in finals
            out << ui->finalsCompetitorsTable->itemAt(1, i)->text();
            out << ui->finalsCompetitorsTable->itemAt(2, i)->text();
            out << ui->finalsCompetitorsTable->itemAt(3, i)->text();
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
}

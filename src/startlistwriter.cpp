#include "startlistwriter.h"

StartListWriter::StartListWriter(QVector<QStringList> competitorsList, QString filePath, QObject *parent) : QObject(parent)
{
    this->filePath = filePath;
    this->competitorsList = competitorsList;
}

void StartListWriter::writeStartListFile()
{
    if(!filePath.endsWith(".csv"))
        filePath = QFileDialog::getSaveFileName(this, tr("Salvesta startlist"), filePath + ".csv", tr("Comma separated file (*.csv)"));
    if(filePath.isEmpty()){
        return;
    }else if(!filePath.endsWith(".csv")){
        filePath.append(".csv");
    }

    QFile file(filePath);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&file);
        int eksporditud = 0;
        for(int i = 0; i < competitorsList.count(); i++){
                //ID no;Startno;Name;Firstname;Disp name;Nat;Cat;Group;Team;Bay;Target;Relay;Starttime;BoxTg;Active;Q Tot;Avg;Rank;G1;...;G12;10s;...;0s;Mouches
                out << QString(";%1%2;\"").arg(leht->leheIndeks).arg(leht->reasLaskurid[i]->id);
                out << leht->reasLaskurid[i]->perekNimi->text() << "\";\"";
                out << leht->reasLaskurid[i]->eesNimi->text() << "\";";
                out << leht->reasLaskurid[i]->perekNimi->text() << " " << leht->reasLaskurid[i]->eesNimi->text().left(1);
                out << ".\";;0;0;" << leht->reasLaskurid[i]->klubi->text() << ";;";
                out << leht->reasLaskurid[i]->rajaNr->text().replace('A', '2').replace('B', '3').replace('C', '4').replace('D', '5').replace('E', '6').replace('F', '7').replace('G', '8').replace('H', '9');    //Täheliste raja nr'ite asendamine Siusi jaoks numbritega alates 2st
                out << ";0;;0;0;" << leht->reasLaskurid[i]->getSumma().replace(",", ".") << ";0;\n";
                eksporditud++;
        }
        file.close();
        QMessageBox::information(this, "Protokollitaja", QString("Startlist loodud. Eksporditi %1 laskurit").arg(eksporditud), "Selge");
    }else QMessageBox::critical(this, "Viga", tr("Ei õnnestu faili kirjutada."), QMessageBox::Ok);
}

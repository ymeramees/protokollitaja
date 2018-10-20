#include "startlistwriter.h"

StartListWriter::StartListWriter(QVector<QStringList> competitorsList, QString filePath, QObject *parent, int relay) : QObject(parent)
{
    this->filePath = filePath;
    this->competitorsList = competitorsList;    //Each "row": target, ID, first name, name, club, result
    this->relay = relay;

    writeStartListFile();
}

void StartListWriter::writeStartListFile()
{
    if(filePath.endsWith(".kll") || filePath.endsWith(".fnl"))
        filePath.chop(4);

    if(!filePath.endsWith(".csv"))
        filePath = QFileDialog::getSaveFileName(dynamic_cast<QWidget*>(this->parent()), tr("Salvesta startlist"), filePath + ".csv", tr("Comma separated file (*.csv)"));
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
                out << QString(";%1;\"").arg(competitorsList.at(i).at(1));
                out << competitorsList.at(i).at(3) << "\";\"";  //Name
                out << competitorsList.at(i).at(2) << "\";";    //First name
                out << competitorsList.at(i).at(3) << " " << competitorsList.at(i).at(2).left(1); //Name + first name letter
                out << ".\";;0;0;" << competitorsList.at(i).at(4) << ";;";  //Club
                QString target = competitorsList.at(i).at(0);
                out << target.replace('A', '2').replace('B', '3').replace('C', '4').replace('D', '5').replace('E', '6').replace('F', '7').replace('G', '8').replace('H', '9');    //Täheliste raja nr'ite asendamine Siusi jaoks numbritega alates 2st
                QString result = competitorsList.at(i).at(5);
                out << ";" << relay <<";;0;0;" << result.replace(",", ".") << ";0;\n";  //Relay and result
                eksporditud++;
        }
        file.close();
        QMessageBox::information(dynamic_cast<QWidget*>(this->parent()), "Protokollitaja", QString("Startlist loodud. Eksporditi %1 laskurit\n%2").arg(eksporditud).arg(filePath), "Selge");
    }else QMessageBox::critical(dynamic_cast<QWidget*>(this->parent()), "Viga", tr("Ei õnnestu faili kirjutada."), QMessageBox::Ok);
}

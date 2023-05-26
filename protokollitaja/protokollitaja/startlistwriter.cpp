#include "startlistwriter.h"

StartListWriter::StartListWriter(
        QVector<StartListCompetitor> competitorsList,
        QString filePath,
        StartListType type,
        QObject *parent,
        int relay
        ) : QObject(parent)
{
    m_filePath = filePath;
    m_competitorsList = competitorsList;    //Each "row": target, ID, first name, name, club, result
    m_relay = relay;

    writeStartListFile(type);
}

QString StartListWriter::replaceLetterWithNumber(QString in)
{
    return in.replace('A', '2')  // Replace letters with numbers starting from 2
            .replace('B', '3')
            .replace('C', '4')
            .replace('D', '5')
            .replace('E', '6')
            .replace('F', '7')
            .replace('G', '8')
            .replace('H', '9');
}

int StartListWriter::writeInbandData(QTextStream *out)
{
    int exported = 0;
//    out->setCodec("UTF-8"); // TODO Not needed in Qt 6.5?
    *out << QString::fromUtf8("Erä,Rata,Etunimi,Sukunimi,Seura,Laji,Sarja\n"); // Seems that header is needed
    for(int i = 0; i < m_competitorsList.count(); i++){
        // Relay,Firing point,Family name,First name,Shooting Club,Discipline,Class
        // Each "row": target, ID, first name, name, club, result
        *out << m_relay << ",";  //  Relay, TODO to be checked what happens if this is 0
        *out << replaceLetterWithNumber(m_competitorsList.at(i).target) << ",";
        *out << m_competitorsList.at(i).firstName << ","; // First name
        *out << m_competitorsList.at(i).name << ","; // Last name
        *out << m_competitorsList.at(i).club << ","; // Club
        QString inbandDiscipline;
        switch(m_competitorsList.at(i).discipline) {
        case 0:
        case 1: inbandDiscipline = "Ilmakivääri";
            break;
        case 2:
        case 3: inbandDiscipline = "Ilmapistooli";
            break;
        default : inbandDiscipline = "";
        }

        *out << inbandDiscipline << ","; // Discipline
        *out << m_competitorsList.at(i).competitorClass << "\n"; // Class // FIXME To be implemented
        exported++;
    }
    return exported;
}

int StartListWriter::writeSiusData(QTextStream *out)
{
    int exported = 0;
    for(int i = 0; i < m_competitorsList.count(); i++){
            //ID no;Startno;Name;Firstname;Disp name;Nat;Cat;Group;Team;Bay;Target;Relay;Starttime;BoxTg;Active;Q Tot;Avg;Rank;G1;...;G12;10s;...;0s;Mouches
            *out << QString(";%1;\"").arg(m_competitorsList.at(i).ID);
            *out << m_competitorsList.at(i).name << "\";\"";  //Name
            *out << m_competitorsList.at(i).firstName << "\";";    //First name
            *out << "\"" << m_competitorsList.at(i).name << " " << m_competitorsList.at(i).firstName.left(1); //Name + first name letter
            *out << ".\";;0;0;\"" << m_competitorsList.at(i).club << "\";;";  //Club
            *out << replaceLetterWithNumber(m_competitorsList.at(i).target);
            QString result = m_competitorsList.at(i).result;
            *out << ";" << m_relay <<";;0;0;" << result.replace(",", ".") << ";0;\n";  //Relay and result
            exported++;
    }
    return exported;
}

void StartListWriter::writeStartListFile(StartListType type)
{
    if(m_filePath.endsWith(".kll") || m_filePath.endsWith(".fnl"))
        m_filePath.chop(4);

    if(!m_filePath.endsWith(".csv"))
        m_filePath = QFileDialog::getSaveFileName(
                    dynamic_cast<QWidget*>(this->parent()),
                    tr("Salvesta startlist"),
                    m_filePath + ".csv",
                    tr("Comma separated file (*.csv)")
                    );
    if(m_filePath.isEmpty()){
        return;
    }else if(!m_filePath.endsWith(".csv")){
        m_filePath.append(".csv");
    }

    QFile file(m_filePath);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&file);
        int exported = 0;
        switch (type) {
        case SIUS:
            exported = writeSiusData(&out);
            break;
        case INBAND:
            exported = writeInbandData(&out);
            break;
        default:
            QMessageBox::information(
                        dynamic_cast<QWidget*>(this->parent()),
                        "Protokollitaja",
                        tr("Tundmatu start list'i tüüp, ei ekspordi midagi!")
                        );
        }
        file.close();
        if (exported > 0)
            QMessageBox::information(
                        dynamic_cast<QWidget*>(this->parent()),
                        "Protokollitaja",
                        QString("Startlist loodud. Eksporditi %1 laskurit\n%2").arg(exported).arg(m_filePath),
                        "Selge"
                        );
    } else QMessageBox::critical(dynamic_cast<QWidget*>(this->parent()), "Viga", tr("Ei õnnestu faili kirjutada."), QMessageBox::Ok);
}

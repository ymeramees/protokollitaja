#include "simplekllfilerw.h"

SimpleKllFileRW::SimpleKllFileRW(QObject *parent) : QObject(parent)
{

}

CompetitionSettings SimpleKllFileRW::readCompetitionSettings(QDataStream *inStream, QWidget *parent)
{
    CompetitionSettings competitionSettings;
    quint32 checkPattern, version;
    *inStream >> checkPattern >> version;
    competitionSettings.fileVersion = version;
    if(checkPattern != 0x00FA3848) {
        QMessageBox::critical(parent, tr("Protokollitaja"), tr("Vigane või vale fail!"), QMessageBox::Ok);
    } else if(version >= 100 && version <= 113) {
        *inStream >> competitionSettings.competitionName;
        if(version >= 113) {
            *inStream >> competitionSettings.startDate >> competitionSettings.endDate;
        } else {
            competitionSettings.startDate = QDate(2000, 1, 1);
            competitionSettings.endDate = QDate(2000, 1, 1);
        }
        *inStream >> competitionSettings.place;
    } else QMessageBox::critical(
                parent,
                tr("Protokollitaja"),
                QString("Vale versiooni fail!\n\nVõimalik, et tegu on uuema programmi versiooni failiga.\n\n(KllFileRW::readCompetitionSettingsFromKll())"),
                QMessageBox::Ok
                );
    return competitionSettings;
}

CompetitionSettings SimpleKllFileRW::readCompetitionSettingsFromKll(QString fileName, QWidget *parent)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    CompetitionSettings competitionSettings;
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        competitionSettings = readCompetitionSettings(&in, parent);
    } else QMessageBox::critical(parent, tr("Protokollitaja"), tr("Ei leia faili!"), QMessageBox::Ok);
    QApplication::restoreOverrideCursor();

    competitionSettings.fileName = fileName;
    return competitionSettings;
}

bool SimpleKllFileRW::writeInitialKll(QString fileName, CompetitionSettings data, QWidget *parent)
{
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly)){
            QDataStream out(&file);
            out << (quint32)0x00FA3848;   //Kontrollarv
            out << (qint32)113;           //Millise programmi versiooni failiga on tegu
            out.setVersion(QDataStream::Qt_5_12);
            out << data.competitionName;
            out << data.startDate;
            out << data.endDate;
            out << data.place;
            out << QString("");
            out << 1 << 1 << 5 << 0 << 0 << 0;  //kirjutusabi << autosave << autosave aeg << sakiAsukoht << sakkideArv << järjestamine;
            return true;
    }else {
        QMessageBox::critical(
                parent,
                "Protokollitaja",
                tr("Ei õnnestu faili luua. Kontrollige kas teil on sinna kausta kirjutamise õigus."),
                QMessageBox::Ok
                );
        return false;
    }
}

#include "finalsformats.h"

FinalsFormats::FinalsFormats()
{
    QFile jsonFile(":/eventFormats.json");
    if (jsonFile.open(QIODevice::ReadOnly)) {
        m_finalsFormats = QJsonDocument::fromJson(jsonFile.readAll()).array();
    } else {
        QMessageBox::critical(nullptr, tr("Viga"), tr("Ei leia finaali formaatide faili!"), QMessageBox::Ok);
    }
    jsonFile.close();
}

QJsonObject FinalsFormats::confById(const QString id)
{
    for(QJsonValue eventJson: qAsConst(m_finalsFormats)) {
        if (eventJson.toObject()["id"].toString().compare(id) == 0)
            return eventJson.toObject();
    }
    return QJsonObject();
}

QStringList FinalsFormats::formatIds()
{
    QStringList names;
    for(QJsonValue eventJson: qAsConst(m_finalsFormats)) {
        names << eventJson.toObject()["id"].toString();
    }
    return names;
}

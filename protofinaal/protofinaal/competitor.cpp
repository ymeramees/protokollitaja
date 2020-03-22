#include "competitor.h"

Competitor::Competitor(const QJsonArray configJson, QWidget *parent) : QWidget(parent)
{
    if(verbose)
        QTextStream(stdout) << "Competitor::Competitor(QJsonArray)" << endl;
    QHBoxLayout *hBox = new QHBoxLayout;

    m_id = 0;
    m_idLabel.setToolTip(tr("Võistleja ID"));
    m_idLabel.setText("0");
    m_idLabel.installEventFilter(this);
    hBox->addWidget(&m_idLabel);

    m_nameEdit.setMinimumWidth(75);
    m_nameEdit.setMinimumHeight(22);
    m_nameEdit.setToolTip(tr("Võistleja nimi"));
    hBox->addWidget(&m_nameEdit);

    if(verbose)
        QTextStream(stdout) << "Competitor::configJson.size(): " << configJson.size() << endl;

    for(int i = 0; i < configJson.size(); i++){
        QVector<QLineEdit*> *thisSeries = new QVector<QLineEdit*>;
        for(int j = 0; j < configJson.at(i).toInt(); j++){
//            if(verbose)
//                QTextStream(stdout) << "Competitor::configJson.at(i).toInt(): " << configJson.at(i).toInt() << endl;
            QLineEdit *shotEdit = new QLineEdit;
            shotEdit->setToolTip(tr("Lask"));
            shotEdit->setMinimumWidth(30);
            shotEdit->setMaximumWidth(40);
            shotEdit->setMinimumHeight(22);
            connect(shotEdit, &QLineEdit::editingFinished, this, &Competitor::sum);

            m_shots.append(shotEdit);
            thisSeries->append(shotEdit);
            hBox->addWidget(shotEdit);
        }
        m_series.append(thisSeries);

        QLabel *sumLabel = new QLabel(tr("0,0"));
        m_sumLabels.append(sumLabel);
        hBox->addWidget(sumLabel);
    }

    hBox->setContentsMargins(0, 2, 0, 2);

    if(verbose)
        QTextStream(stdout) << "Competitor::shots: " << m_shots.size() << " , series: " << m_series.size() << " ,sumLabels: " << m_sumLabels.size() << endl;
    setLayout(hBox);
    sum();
}

Competitor::Competitor(const QJsonObject &json, QWidget *parent) : QWidget(parent)
{
    if(verbose)
        QTextStream(stdout) << "Competitor::Competitor(QJsonObject), json[Series].size = " << json["Series"].toArray().size() << endl;
    if(!(json.contains("nameEdit") && json["nameEdit"].isString()) ||
            !(json.contains("Series") && json["Series"].isArray())){
        QMessageBox::critical(this, tr("Viga!"), tr("Vigane fail!"));
        return;
    }

    QHBoxLayout *hBox = new QHBoxLayout;

    m_id = json["id"].toInt();
    m_idLabel.setToolTip(tr("Võistleja ID"));
    m_idLabel.setText(QString("%1").arg(m_id));
    m_idLabel.installEventFilter(this);
    hBox->addWidget(&m_idLabel);

    m_nameEdit.setMinimumWidth(75);
    m_nameEdit.setMinimumHeight(22);
    m_nameEdit.setToolTip(tr("Võistleja nimi"));
    m_nameEdit.setText(json["nameEdit"].toString());
    hBox->addWidget(&m_nameEdit);

    QJsonArray seriesArray = json["Series"].toArray();

    foreach (QJsonValue seriesJson, seriesArray) {
        QVector<QLineEdit*> *thisSeries = new QVector<QLineEdit*>;
        QJsonObject seriesObj = seriesJson.toObject();
        QJsonArray shotsArray = seriesObj["Shots"].toArray();
        foreach (QJsonValue shotJson, shotsArray) {
            QLineEdit *shotEdit = new QLineEdit;
            shotEdit->setText(shotJson.toString());
            shotEdit->setToolTip(tr("Lask"));
            shotEdit->setMinimumWidth(30);
            shotEdit->setMaximumWidth(40);
            shotEdit->setMinimumHeight(22);
            connect(shotEdit, &QLineEdit::editingFinished, this, &Competitor::sum);

            m_shots.append(shotEdit);
            thisSeries->append(shotEdit);
            hBox->addWidget(shotEdit);
        }
        QLabel *sumLabel = new QLabel(seriesObj["Sum"].toString());
        m_sumLabels.append(sumLabel);
        hBox->addWidget(sumLabel);

        m_series.append(thisSeries);
    }

//    QJsonArray shotsArray = json["Shots"].toArray();

//    foreach (QJsonValue shot, shotsArray) {
//        QLineEdit *shotEdit = new QLineEdit;
//        shotEdit->setText(shot.toString());
//        shotEdit->setToolTip(tr("Lask"));
//        shotEdit->setMinimumWidth(30);
//        shotEdit->setMaximumWidth(40);
//        shotEdit->setMinimumHeight(22);
//        connect(shotEdit, &QLineEdit::editingFinished, this, &Competitor::sum);

//        shots.append(shotEdit);
////        thisSeries->append(shotEdit);
//        hBox->addWidget(shotEdit);
//    }

//    QLabel *sumLabel = new QLabel(tr("0,0"));
//    sumLabels.append(sumLabel);
//    hBox->addWidget(sumLabel);
    hBox->setContentsMargins(0, 2, 0, 2);

    setLayout(hBox);
    sum();
}

Competitor::~Competitor()
{
    if(verbose)
        QTextStream(stdout) << "Competitor::~Competitor()" << endl;
    foreach (QLabel *sumLabel, m_sumLabels)
        sumLabel->deleteLater();
    m_sumLabels.clear();

    //series.clear();
    foreach (QVector<QLineEdit*> *serie, m_series) {  //Duplicate pointers to shots
        delete serie;
    }

    foreach (QLineEdit *shot, m_shots)
        shot->deleteLater();
    m_shots.clear();
}

int Competitor::current10Sum() const
{
    return m_sumLabels.at(m_sumLabels.size()-1)->text().remove(',').toInt();
}

int Competitor::id()
{
    return m_id;
}

QString Competitor::lastResult()
{
    if(m_series.size() == 0){
        return "0,0";
    }else if(m_series.size() > 1 && m_series.at(m_series.size() - 1)->at(0)->text().isEmpty()){ //No shot in singleshots, so latest series with a result needs to be found
        if(m_series.at(m_series.size() - 1)->at(0)->text().isEmpty()){
            for(int i = m_series.size() - 2; i >= 0; i--)
                if(m_sumLabels.at(i)->text() != "0,0")
                    return m_sumLabels.at(i)->text();
        }
    }else{  //Only single shots or shot in single shots found, so latest shot needs to be found
        for(int i = m_shots.size() - 1; i >= 0; i--)
            if(!m_shots.at(i)->text().isEmpty())
                return m_shots.at(i)->text();
    }
    return "0,0";
}

QString Competitor::lastSum()
{
    if(m_sumLabels.size() >= 1)
        return m_sumLabels.at(m_sumLabels.size() - 1)->text();
    else
        return "0";
}

void Competitor::mouseDoubleClickEvent(QMouseEvent *event)
{
    int newID = QInputDialog::getInt(this, tr("Võistleja ID muutmine"), tr("Sisestage uus ID:"), m_id, 0);
    if(m_id != newID){
        emit statusInfo(QString(tr("Võistleja ID muudetud, vana: %1 => uus: %2")).arg(m_id).arg(newID));
        m_id = newID;
        m_idLabel.setText(QString("%1").arg(m_id));
    }
    Q_UNUSED(event);
}

QString Competitor::name()
{
    return m_nameEdit.text();
}

QString Competitor::previousSiusRow()
{
    return m_previousSiusRow;
}

void Competitor::setPreviousSiusRow(QString newSiusRow)
{
    m_previousSiusRow = newSiusRow;
}

void Competitor::setShot(int shotNo, QString shotValue)
{
    if(shotNo < m_shots.size())
        m_shots.at(shotNo)->setText(shotValue);
    else{
        QMessageBox::critical(this, tr("Viga"), tr("Laskude arv suurem, kui võimalik! Lask ei läinud kirja!"), QMessageBox::Ok);
        emit statusInfo(m_nameEdit.text() + tr("Laskude arv suurem, kui võimalik! Lask ei läinud kirja!"));
    }
}

void Competitor::sum()
{
    int totalSum = 0;
    for(int i = 0; i < m_series.size(); i++){
        int seriesSum = 0;
        for(int j = 0; j < m_series.at(i)->size(); j++){
            seriesSum += m_series.at(i)->at(j)->text().remove(',').toInt();
        }
        totalSum += seriesSum;
        double dSeriesSum = seriesSum;
        dSeriesSum /= 10;
        m_sumLabels.at(i)->setText(QString("%1").arg(dSeriesSum).replace('.', ','));
        if(!m_sumLabels.at(i)->text().contains(','))
            m_sumLabels.at(i)->setText(m_sumLabels.at(i)->text() + ",0");
    }
    double dTotalSum = totalSum;
    dTotalSum /= 10;
    m_sumLabels.at(m_sumLabels.size() - 1)->setText(QString("%1").arg(dTotalSum).replace('.', ','));
    if(!m_sumLabels.at(m_sumLabels.size() - 1)->text().contains(','))
        m_sumLabels.at(m_sumLabels.size() - 1)->setText(m_sumLabels.at(m_sumLabels.size() - 1)->text() + ",0");
    emit newShot();
//    return dTotalSum;
}

QJsonObject Competitor::toJson() const
{
    QJsonObject json;
    json["nameEdit"] = m_nameEdit.text();
    json["id"] = m_id;
    QJsonArray seriesArray;
    for(int i = 0; i < m_series.size(); i++){
        QJsonObject seriesJson;
        QJsonArray seriesShotsJson;
        for(int j = 0; j < m_series.at(i)->size(); j++){
            seriesShotsJson.append(m_series.at(i)->at(j)->text());
        }
        seriesJson["Shots"] = seriesShotsJson;
        seriesJson["Sum"] = m_sumLabels.at(i)->text();
        seriesArray.append(seriesJson);
    }
    json["Series"] = seriesArray;
    return json;
}

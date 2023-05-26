#include "competitor.h"

Competitor::Competitor(const int id, const QJsonArray configJson, QWidget *parent) : QWidget(parent)
{
    if(verbose)
        QTextStream(stdout) << "Competitor::Competitor(QJsonArray)" << Qt::endl;
    QHBoxLayout *hBox = new QHBoxLayout;

    m_id = id;
    m_idLabel.setToolTip(tr("Võistleja ID"));
    m_idLabel.setText("0");
    m_idLabel.installEventFilter(this);
    hBox->addWidget(&m_idLabel);

    m_nameEdit.setMinimumWidth(75);
    m_nameEdit.setMinimumHeight(22);
    m_nameEdit.setToolTip(tr("Võistleja nimi"));
    hBox->addWidget(&m_nameEdit);

    if(verbose)
        QTextStream(stdout) << "Competitor::configJson.size(): " << configJson.size() << Qt::endl;

    for(int i = 0; i < configJson.size(); i++){
        QVector<ShotEdit*> *thisSeries = new QVector<ShotEdit*>;
        for(int j = 0; j < configJson.at(i).toInt(); j++){
//            if(verbose)
//                QTextStream(stdout) << "Competitor::configJson.at(i).toInt(): " << configJson.at(i).toInt() << Qt::endl;
            ShotEdit *shotEdit = new ShotEdit;
            createShotEditConnections(shotEdit);

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
        QTextStream(stdout) << "Competitor::shots: " << m_shots.size() << " , series: " << m_series.size() << " ,sumLabels: " << m_sumLabels.size() << Qt::endl;
    setLayout(hBox);
    sum();
}

Competitor::Competitor(const QJsonObject &json, QWidget *parent) : QWidget(parent)
{
    if(verbose)
        QTextStream(stdout) << "Competitor::Competitor(QJsonObject), json[Series].size = " << json["series"].toArray().size() << Qt::endl;
    if(!(json.contains("nameEdit") && json["nameEdit"].isString()) ||
            !(json.contains("series") && json["series"].isArray())){
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

    QJsonArray seriesArray = json["series"].toArray();

    foreach (QJsonValue seriesJson, seriesArray) {
        QVector<ShotEdit*> *thisSeries = new QVector<ShotEdit*>;
        QJsonObject seriesObj = seriesJson.toObject();
        QJsonArray shotsArray = seriesObj["shots"].toArray();
        foreach (QJsonValue shotJson, shotsArray) {
            ShotEdit *shotEdit = new ShotEdit(shotJson.toObject());
            createShotEditConnections(shotEdit);

            m_shots.append(shotEdit);
            thisSeries->append(shotEdit);
            hBox->addWidget(shotEdit);
        }
        QLabel *sumLabel = new QLabel(seriesObj["Sum"].toString());
        m_sumLabels.append(sumLabel);
        hBox->addWidget(sumLabel);

        m_series.append(thisSeries);
    }

//    QJsonArray shotsArray = json["shots"].toArray();

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
        QTextStream(stdout) << "Competitor::~Competitor()" << Qt::endl;
    foreach (QLabel *sumLabel, m_sumLabels)
        sumLabel->deleteLater();
    m_sumLabels.clear();

    //series.clear();
    foreach (QVector<ShotEdit*> *serie, m_series) {  //Duplicate pointers to shots
        delete serie;
    }

    foreach (ShotEdit *shot, m_shots)
        shot->deleteLater();
    m_shots.clear();
}

void Competitor::createShotEditConnections(ShotEdit *shotEdit)
{
    connect(shotEdit, &ShotEdit::valueChanged, this, &Competitor::sum);
    connect(shotEdit, &ShotEdit::shotIgnored, this, &Competitor::handleIgnoredShot);
    connect(shotEdit, &ShotEdit::shotUnignored, this, &Competitor::handleUnignoredShot);
}

int Competitor::current10Sum() const
{
    return m_sumLabels.at(m_sumLabels.size()-1)->text().remove(',').toInt();
}

void Competitor::handleIgnoredShot()
{
    ShotEdit *ignoredShotEdit = qobject_cast<ShotEdit*>(sender());
    if(ignoredShotEdit != NULL) {
        ShotEdit *shotEdit = new ShotEdit;
        createShotEditConnections(shotEdit);

        m_shots.append(shotEdit);
        m_series.last()->append(shotEdit);

        QHBoxLayout *hBox = qobject_cast<QHBoxLayout*>(layout());
        hBox->insertWidget(hBox->count() - 1, shotEdit);
        int indexOfIgnoredShot = hBox->indexOf(ignoredShotEdit);
        // Shift following lables forward
        foreach(QLabel *sumLabel, m_sumLabels) {
            int labelIndex = hBox->indexOf(sumLabel);
            if(labelIndex > indexOfIgnoredShot) {
                hBox->insertItem(labelIndex + 1, hBox->takeAt(labelIndex));
            }
        }
        // Find series where ignored shotEdit is
        int indexOfSeries = -1;
        for(int i = 0; i < m_series.size() - 1; i++) {
            if (m_series.at(i)->contains(ignoredShotEdit))
                indexOfSeries = i;
            // Take first shot from each following series and append it to previous one
            if (i >= indexOfSeries && indexOfSeries != -1)
                m_series.at(i)->append(m_series.at(i + 1)->takeFirst());
        }
        sum();
    }
}

void Competitor::handleUnignoredShot()
{
    ShotEdit *unIgnoredShotEdit = qobject_cast<ShotEdit*>(sender());
    if(unIgnoredShotEdit != NULL) {
        ShotEdit *lastShot = m_shots.takeLast();
        m_series.last()->takeLast();

        QHBoxLayout *hBox = qobject_cast<QHBoxLayout*>(layout());
        int indexOfUnIgnoredShot = hBox->indexOf(unIgnoredShotEdit);
        // Shift following lables backwards
        foreach(QLabel *sumLabel, m_sumLabels) {
            int labelIndex = hBox->indexOf(sumLabel);
            if(labelIndex > indexOfUnIgnoredShot) {
                hBox->insertItem(labelIndex - 1, hBox->takeAt(labelIndex));
            }
        }
        // Find series where ignored shotEdit is
        int indexOfSeries = -1;
        for(int i = 0; i < m_series.size() - 1; i++) {
            if (m_series.at(i)->contains(unIgnoredShotEdit))
                indexOfSeries = i;
            // Take the last shot from each following series and prepend it to next one
            if (i >= indexOfSeries && indexOfSeries != -1)
                m_series.at(i + 1)->prepend(m_series.at(i)->takeLast());
        }

        hBox->removeWidget(lastShot);
        lastShot->deleteLater();
        sum();
    }
}

int Competitor::id()
{
    return m_id;
}

QString Competitor::lastResult()
{
    if(m_series.size() == 0){
        return "0,0";
    } else {
        // First check if there are single shots
        QString lastShotValue = "";
        for(int i = m_series.last()->size() - 1; i >= 0; i--)
            if(!m_series.last()->at(i)->text().isEmpty() && !m_series.last()->at(i)->ignored()){
                lastShotValue = m_series.last()->at(i)->text();
                break;
            }

        if(!lastShotValue.isEmpty())
            return lastShotValue;
        else if(m_series.size() > 1){ //No shot in singleshots, so latest series with a result needs to be found
            for(int i = m_series.size() - 2; i >= 0; i--)
                if(m_sumLabels.at(i)->text() != "0,0")
                    return m_sumLabels.at(i)->text();
        }
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

bool Competitor::readSiusShot(SiusShotData shotData)
{
    bool result = false;

    if(shotData.id == m_id){

//        if(vSummadeSamm == 0 || (vSummadeSamm != 0 && shotData.siusShotNo <= vSummadeSamm * 10)) { // Ignore additional shots in all stages
//            int seriesIndex = (competitionStage() * vSummadeSamm * 10 + shotData.siusShotNo - 1) / 10;
            int shotIndex = (shotData.siusShotNo - 1);

            if(shotData.shot.isCompetitionShot() && m_shots.length() > shotIndex){
                if(m_shots.at(shotIndex)->shot().isEmpty()){
                    result = setShot(shotIndex, shotData.shot);
                } else if (m_shots.at(shotIndex)->shot().getSLask().compare(shotData.shot.getSLask()) == 0 &&
                            m_shots.at(shotIndex)->shot().shotTime() == shotData.shot.shotTime()) {
                     result = true;  // Shot already existing, ignore, but return true
                } else
                    result = false;
            } else {
                // TODO draw sighting shots on spectator's screen
            }
//        }
    }
    return result;
}

std::optional<Lask> Competitor::shotAt(int index)
{
    if(m_shots.length() > index)
        return std::optional<Lask>{m_shots.at(index)->shot()};
    else
        return std::nullopt;
}

void Competitor::setPreviousSiusRow(QString newSiusRow)
{
    m_previousSiusRow = newSiusRow;
}

bool Competitor::setShot(int shotNo, Lask newShot)
{
    if(shotNo < m_shots.size()){
        m_shots.at(shotNo)->setShot(newShot);
        return true;
    } else {
        QMessageBox::critical(this, tr("Viga"), tr("Laskude arv suurem, kui võimalik! Lask ei läinud kirja!"), QMessageBox::Ok);
        emit statusInfo(m_nameEdit.text() + tr("Laskude arv suurem, kui võimalik! Lask ei läinud kirja!"));
        return false;
    }
}

void Competitor::setShot(int shotNo, QString siusRow)
{
    if(shotNo < m_shots.size())
        m_shots.at(shotNo)->setSiusShot(siusRow);
    else{
        QMessageBox::critical(this, tr("Viga"), tr("Laskude arv suurem, kui võimalik! Lask ei läinud kirja!"), QMessageBox::Ok);
        emit statusInfo(m_nameEdit.text() + tr("Laskude arv suurem, kui võimalik! Lask ei läinud kirja!"));
    }
}

void Competitor::sum()
{
    // FIXME shot value without decimal places is divided by 10
    int totalSum = 0;
    for(int i = 0; i < m_series.size(); i++){
        int seriesSum = 0;
        for(int j = 0; j < m_series.at(i)->size(); j++){
            if(!m_series.at(i)->at(j)->ignored())
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
            seriesShotsJson.append(m_series.at(i)->at(j)->toJson());
        }
        seriesJson["shots"] = seriesShotsJson;
        seriesJson["Sum"] = m_sumLabels.at(i)->text();
        seriesArray.append(seriesJson);
    }
    json["series"] = seriesArray;
    return json;
}

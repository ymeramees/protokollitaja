#include "competitor2022.h"

Competitor2022::Competitor2022(const int id, const QJsonArray configJson, QWidget *parent) : QWidget(parent)
{
    if(verbose)
        QTextStream(stdout) << "Competitor::Competitor(QJsonArray)" << endl;
    QHBoxLayout *hBox = new QHBoxLayout;

    setupCompetitor(hBox, true, id, "", "");

    if(verbose)
        QTextStream(stdout) << "Competitor::configJson.size(): " << configJson.size() << endl;

    if (configJson.size() > 0)
        for(int i = 0; i < configJson.size(); i++){
            QVector<ShotEdit*> *thisSeries = new QVector<ShotEdit*>;
            for(int j = 0; j < configJson.at(i).toInt(); j++){
                //            if(verbose)
                //                QTextStream(stdout) << "Competitor::configJson.at(i).toInt(): " << configJson.at(i).toInt() << endl;
                ShotEdit *shotEdit = new ShotEdit;
                createShotEditConnections(shotEdit);

                m_shots.append(shotEdit);
                thisSeries->append(shotEdit);
                hBox->addWidget(shotEdit);

                QLabel *pointsLabel = new QLabel(tr(""));
                m_pointsLabels.append(pointsLabel);
                hBox->addWidget(pointsLabel);
            }
            m_series.append(thisSeries);

            QLabel *sumLabel = new QLabel(tr("0"));
            m_pointsTotalLabels.append(sumLabel);
            hBox->addWidget(sumLabel);
        }
//    else
//        for(int i = 0; i < 3; i++){
//            QVector<ShotEdit*> *thisSeries = new QVector<ShotEdit*>;
//            for(int j = 0; j < configJson.at(i).toInt(); j++){
//                //            if(verbose)
//                //                QTextStream(stdout) << "Competitor::configJson.at(i).toInt(): " << configJson.at(i).toInt() << endl;
//                ShotEdit *shotEdit = new ShotEdit;
//                createShotEditConnections(shotEdit);

//                m_shots.append(shotEdit);
//                thisSeries->append(shotEdit);
//                hBox->addWidget(shotEdit);
//            }
//            m_series.append(thisSeries);

//            QLabel *sumLabel = new QLabel(tr("0"));
//            m_sumLabels.append(sumLabel);
//            hBox->addWidget(sumLabel);
//    }

    hBox->setContentsMargins(0, 2, 0, 2);

    if(verbose)
        QTextStream(stdout) << "Competitor::shots: " << m_shots.size() << " , series: " << m_series.size() << " ,sumLabels: " << m_pointsTotalLabels.size() << endl;
    setLayout(hBox);
    sumPoints();
}

Competitor2022::Competitor2022(const QJsonObject &json, QWidget *parent) : QWidget(parent)
{
    if(verbose)
        QTextStream(stdout) << "Competitor::Competitor(QJsonObject), json[Series].size = " << json["series"].toArray().size() << endl;
    if(!(json.contains("nameEdit") && json["nameEdit"].isString()) ||
            !(json.contains("series") && json["series"].isArray())){
        QMessageBox::critical(this, tr("Viga!"), tr("Vigane fail!"));
        return;
    }

    QHBoxLayout *hBox = new QHBoxLayout;

    QString resultText = "";
    if (json.contains("resultEdit"))
        resultText = json["resultEdit"].toString();

    bool isActive = true;
    if (json.contains("isActive"))
        isActive = json["isActive"].toBool();

    setupCompetitor(hBox, isActive, json["id"].toInt(), json["nameEdit"].toString(), resultText);

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

            QLabel *pointsLabel = new QLabel;
            m_pointsLabels.append(pointsLabel);
            hBox->addWidget(pointsLabel);
        }
        QLabel *sumLabel = new QLabel(seriesObj["Sum"].toString());
        m_pointsTotalLabels.append(sumLabel);
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
    sumPoints();
}

Competitor2022::~Competitor2022()
{
    if(verbose)
        QTextStream(stdout) << "Competitor::~Competitor()" << endl;
    foreach (QLabel *sumLabel, m_pointsTotalLabels)
        sumLabel->deleteLater();
    m_pointsTotalLabels.clear();

    //series.clear();
    foreach (QVector<ShotEdit*> *serie, m_series) {  //Duplicate pointers to shots
        delete serie;
    }

    foreach (ShotEdit *shot, m_shots)
        shot->deleteLater();
    m_shots.clear();
}

void Competitor2022::createShotEditConnections(ShotEdit *shotEdit)
{
    connect(shotEdit, &ShotEdit::valueChanged, this, &Competitor2022::newShot);
    connect(shotEdit, &ShotEdit::shotIgnored, this, &Competitor2022::handleIgnoredShot);
    connect(shotEdit, &ShotEdit::shotUnignored, this, &Competitor2022::handleUnignoredShot);
}

int Competitor2022::current10Sum() const
{
    return m_pointsTotalLabels.at(m_pointsTotalLabels.size()-1)->text().remove(',').toInt();
}

void Competitor2022::handleIgnoredShot()
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
        foreach(QLabel *sumLabel, m_pointsTotalLabels) {
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
        sumPoints();
    }
}

void Competitor2022::handleUnignoredShot()
{
    ShotEdit *unIgnoredShotEdit = qobject_cast<ShotEdit*>(sender());
    if(unIgnoredShotEdit != NULL) {
        ShotEdit *lastShot = m_shots.takeLast();
        m_series.last()->takeLast();

        QHBoxLayout *hBox = qobject_cast<QHBoxLayout*>(layout());
        int indexOfUnIgnoredShot = hBox->indexOf(unIgnoredShotEdit);
        // Shift following lables backwards
        foreach(QLabel *sumLabel, m_pointsTotalLabels) {
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
        sumPoints();
    }
}

int Competitor2022::id()
{
    return m_id;
}

bool Competitor2022::isActive() const
{
    return m_active.isChecked();
}

QString Competitor2022::lastResult()
{
    if(m_series.size() == 0){
        return "0,0";
    } else {
        for(int i = m_shots.size() - 1; i >= 0; i--)
            if(!m_shots.at(i)->text().isEmpty() && !m_shots.at(i)->ignored())
                return m_shots.at(i)->text();

//        if(!lastShotValue.isEmpty())
//            return lastShotValue;
//        else if(m_series.size() > 1){ //No shot in singleshots, so latest series with a result needs to be found
//            for(int i = m_series.size() - 2; i >= 0; i--)
//                if(m_sumLabels.at(i)->text() != "0")
//                    return m_sumLabels.at(i)->text();
//        }
    }
    return "0,0";
}

int Competitor2022::lastValidShotIndex() const
{
    for(int i = m_shots.size() - 1; i >= 0; i--)
        if(!m_shots.at(i)->text().isEmpty() && !m_shots.at(i)->ignored())
            return i;

    //        if(!lastShotValue.isEmpty())
    //            return lastShotValue;
    //        else if(m_series.size() > 1){ //No shot in singleshots, so latest series with a result needs to be found
    //            for(int i = m_series.size() - 2; i >= 0; i--)
    //                if(m_sumLabels.at(i)->text() != "0")
    //                    return m_sumLabels.at(i)->text();
    //        }
    return -1;
}

QString Competitor2022::pointsAt(int index) const
{
    if (index >= 0 && index < m_pointsLabels.size())
        return m_pointsLabels.at(index)->text();
    else
        return "";
}

QString Competitor2022::pointsTotal()
{
    if(m_pointsTotalLabels.size() >= 1)
        return m_pointsTotalLabels.at(m_pointsTotalLabels.size() - 1)->text();
    else
        return "0";
}

void Competitor2022::mouseDoubleClickEvent(QMouseEvent *event)
{
    int newID = QInputDialog::getInt(this, tr("Võistleja ID muutmine"), tr("Sisestage uus ID:"), m_id, 0);
    if(m_id != newID){
        emit statusInfo(QString(tr("Võistleja ID muudetud, vana: %1 => uus: %2")).arg(m_id).arg(newID));
        setId(newID);
        emit modified();
    }
    Q_UNUSED(event);
}

QString Competitor2022::name()
{
    return m_nameEdit.text();
}

QString Competitor2022::previousSiusRow()
{
    return m_previousSiusRow;
}

void Competitor2022::setDisplayName(QString newName)
{
    m_nameEdit.setText(newName);
}

bool Competitor2022::readSiusShot(SiusShotData shotData)
{
    bool result = false;

    if(shotData.id == m_id){

//        if(vSummadeSamm == 0 || (vSummadeSamm != 0 && shotData.siusShotNo <= vSummadeSamm * 10)) { // Ignore additional shots in all stages
//            int seriesIndex = (competitionStage() * vSummadeSamm * 10 + shotData.siusShotNo - 1) / 10;
            int shotIndex = (shotData.siusShotNo - 1 + m_siusOffset.value());

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

std::optional<Lask> Competitor2022::shotAt(int index)
{
    if(index >= 0 && m_shots.length() > index)
        return std::optional<Lask>{m_shots.at(index)->shot()};
    else
        return std::nullopt;
}

void Competitor2022::setActive(bool active)
{
    m_active.setChecked(active);
}

void Competitor2022::setId(int newId)
{
    m_id = newId;
    m_idLabel.setText(QString("%1").arg(newId));
}

bool Competitor2022::setPoints(int shotNo, int points)
{
    double dPoints = points;
    dPoints /= 10;
    if (shotNo < m_pointsLabels.size()) {
        m_pointsLabels.at(shotNo)->setText(QString("%1").arg(dPoints).replace('.', ','));
        sumPoints();
        return true;
    } else
        return false;
}

void Competitor2022::setPreviousSiusRow(QString newSiusRow)
{
    m_previousSiusRow = newSiusRow;
}

void Competitor2022::setQualificationResult(QString result)
{
    m_resultEdit.setText(result);
}

bool Competitor2022::setShot(int shotNo, Lask newShot)
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

void Competitor2022::setShot(int shotNo, QString siusRow)
{
    if(shotNo < m_shots.size())
        m_shots.at(shotNo)->setSiusShot(siusRow);
    else{
        QMessageBox::critical(this, tr("Viga"), tr("Laskude arv suurem, kui võimalik! Lask ei läinud kirja!"), QMessageBox::Ok);
        emit statusInfo(m_nameEdit.text() + tr("Laskude arv suurem, kui võimalik! Lask ei läinud kirja!"));
    }
}

void Competitor2022::setupCompetitor(QHBoxLayout *layout, bool active, int id, QString name, QString result)
{
    m_active.setChecked(active);
    connect(&m_active, &QCheckBox::stateChanged, this, &Competitor2022::modified);
    layout->addWidget(&m_active);

    m_id = id;
    m_idLabel.setToolTip(tr("Võistleja ID"));
    m_idLabel.setText(QString("%1").arg(m_id));
    m_idLabel.installEventFilter(this);
    layout->addWidget(&m_idLabel);

    m_nameEdit.setMinimumWidth(75);
    m_nameEdit.setMinimumHeight(22);
    m_nameEdit.setToolTip(tr("Võistleja nimi"));
    m_nameEdit.setPlaceholderText("Nimi");
    m_nameEdit.setText(name);
    connect(&m_nameEdit, &QLineEdit::editingFinished, this, &Competitor2022::modified);
    layout->addWidget(&m_nameEdit);

    m_resultEdit.setMinimumWidth(35);
    m_resultEdit.setMaximumWidth(55);
    m_resultEdit.setMinimumHeight(22);
    m_resultEdit.setToolTip(tr("Tulemus"));
    m_resultEdit.setPlaceholderText("Tulemus");
    m_resultEdit.setText(result);
    connect(&m_resultEdit, &QLineEdit::editingFinished, this, &Competitor2022::modified);
    layout->addWidget(&m_resultEdit);

    m_siusOffset.setMaximumWidth(45);
    m_siusOffset.setMinimum(-99);
    m_siusOffset.setToolTip(tr("Arv, mille võrra Siusist tulnud lasud on nihkes"));
    layout->addWidget(&m_siusOffset);
}

void Competitor2022::sumPoints()
{
    if(verbose)
        QTextStream(stdout) << "Competitor2022::sumPoints()" << endl;

    int pointsTotal = 0;
    for(int i = 0; i < m_series.size(); i++){
        int seriesSum = 0;
        for(int j = 0; j < m_series.at(i)->size(); j++){
            if(!m_series.at(i)->at(j)->ignored()) {
                QString points = m_pointsLabels.at(m_shots.indexOf(m_series.at(i)->at(j)))->text();
//                if (points.contains(','))
//                    seriesSum += points.remove(',').toInt();
//                else
                    seriesSum += qRound(points.replace(',','.').toDouble() * 10);
            }
        }
        pointsTotal += seriesSum;
        double dSeriesSum = seriesSum;
        dSeriesSum /= 10;
        m_pointsTotalLabels.at(i)->setText(QString("%1").arg(dSeriesSum).replace('.', ','));
//        if(!m_sumLabels.at(i)->text().contains(','))
//            m_sumLabels.at(i)->setText(m_sumLabels.at(i)->text() + ",0");
    }
    double dPointsTotal = pointsTotal;
    dPointsTotal /= 10;
    m_pointsTotalLabels.at(m_pointsTotalLabels.size() - 1)->setText(QString("%1").arg(dPointsTotal).replace('.', ','));
//    if(!m_sumLabels.at(m_sumLabels.size() - 1)->text().contains(','))
//        m_sumLabels.at(m_sumLabels.size() - 1)->setText(m_sumLabels.at(m_sumLabels.size() - 1)->text() + ",0");
//    emit newShot();
//    return dTotalSum;
}

QJsonObject Competitor2022::toJson() const
{
    QJsonObject json;
    json["isActive"] = m_active.isChecked();
    json["nameEdit"] = m_nameEdit.text();
    json["resultEdit"] = m_resultEdit.text();
    json["id"] = m_id;
    QJsonArray seriesArray;
    for(int i = 0; i < m_series.size(); i++){
        QJsonObject seriesJson;
        QJsonArray seriesShotsJson;
        for(int j = 0; j < m_series.at(i)->size(); j++){
            seriesShotsJson.append(m_series.at(i)->at(j)->toJson());
        }
        seriesJson["shots"] = seriesShotsJson;
        seriesJson["Sum"] = m_pointsTotalLabels.at(i)->text();
        seriesArray.append(seriesJson);
    }
    json["series"] = seriesArray;
    return json;
}
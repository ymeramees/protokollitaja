#include "lane.h"

Lane::Lane(QStringList disciplines, QWidget *parent) : QWidget(parent)
{
    init(disciplines);
}

Lane::Lane(int laneNo, QStringList disciplines, QString ip, QWidget *parent) : QWidget(parent)
{
    init(disciplines);
    setTargetAndIp(laneNo, ip);
}

void Lane::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        qDebug() << "Language changed!";
        updateUi();
    }
    else {
        QWidget::changeEvent(event);
    }
}

void Lane::clear()
{
    m_idEdit.clear();
    m_firstNameEdit.clear();
    m_lastNameEdit.clear();
    m_clubEdit.clear();
    m_diciplineBox.setCurrentIndex(0);
    m_decimals.setChecked(true);
    m_shotsEdit.setValue(0);
    m_lastShotLabel.clear();
    m_totalLabel.clear();
    m_currentShotIndex = 0;
    m_inCompetition = false;
}

QString Lane::club()
{
    return m_clubEdit.text();
}

QString Lane::decimals()
{
    if (m_decimals.isChecked())
        return "ON";
    else
        return "OFF";
}

QualificationEvents::QualificationEvent Lane::event()
{
    return QualificationEvents::eventData(m_diciplineBox.currentIndex());
}

QString Lane::firstName()
{
    return m_firstNameEdit.text();
}

QString Lane::id()
{
    return m_idEdit.text();
}

bool Lane::inCompetition()
{
    return m_inCompetition;
}

//int Lane::increaseAndGetCurrentShotIndex()
//{
//    return ++m_currentShotIndex;
//}

QString Lane::ip()
{
    return m_ipEdit.text();
}

void Lane::init(QStringList disciplines)
{
    QHBoxLayout *hBox = new QHBoxLayout;

    hBox->addWidget(&m_selected);
    m_targetEdit.setToolTip(tr("Rada"));
    m_targetEdit.setPlaceholderText(tr("Rada"));
    m_targetEdit.setMaximumWidth(30);
    hBox->addWidget(&m_targetEdit);
    m_idEdit.setToolTip("ID");
    m_idEdit.setPlaceholderText("ID");
    m_idEdit.setMaximumWidth(50);
    hBox->addWidget(&m_idEdit);
    m_firstNameEdit.setToolTip(tr("Eesnimi"));
    m_firstNameEdit.setPlaceholderText(tr("Eesnimi"));
    hBox->addWidget(&m_firstNameEdit);
    m_lastNameEdit.setToolTip(tr("Perekonnanimi"));
    m_lastNameEdit.setPlaceholderText(tr("Perekonnanimi"));
    hBox->addWidget(&m_lastNameEdit);
    m_clubEdit.setToolTip(tr("Klubi"));
    m_clubEdit.setPlaceholderText(tr("Klubi"));
    hBox->addWidget(&m_clubEdit);
    m_diciplineBox.setToolTip(tr("Harjutus"));
    m_diciplineBox.addItems(disciplines);
    m_diciplineBox.setFocusPolicy(Qt::StrongFocus);
    connect(&m_diciplineBox, &QComboBox::currentIndexChanged, this, &Lane::updateDecimals);
    connect(&m_diciplineBox, &QComboBox::currentIndexChanged, this, &Lane::updateShots);
    hBox->addWidget(&m_diciplineBox);
    m_decimals.setChecked(true);
    m_decimals.setToolTip(tr("Komadega"));
    hBox->addWidget(&m_decimals);
    m_shotsEdit.setToolTip(tr("Laskude arv"));
    m_shotsEdit.setMaximum(120);
    m_shotsEdit.setFocusPolicy(Qt::StrongFocus);
    hBox->addWidget(&m_shotsEdit);
    m_lastShotLabel.setToolTip(tr("Viimane lask"));
    hBox->addWidget(&m_lastShotLabel);
    m_totalLabel.setToolTip(tr("Summa"));
    hBox->addWidget(&m_totalLabel);
    m_commandButton.setText("Init");
    connect(&m_commandButton, &QPushButton::clicked, [this]() {
        emit commandIssued(this);
    });
    hBox->addWidget(&m_commandButton);
    m_ipEdit.setToolTip(tr("Märgi IP"));
    m_ipEdit.setPlaceholderText(tr("Märgi IP"));
    hBox->addWidget(&m_ipEdit);
    m_statusLabel.setToolTip(tr("Staatus"));
    m_inCompetition = false;
    hBox->addWidget(&m_statusLabel);
    setLayout(hBox);
}

QString Lane::lastName()
{
    return m_lastNameEdit.text();
}

QString Lane::noOfShots()
{
    return m_shotsEdit.text();
}

bool Lane::selected()
{
    return m_selected.isChecked();
}

void Lane::setSiusCompetitorRow(QString competitorRow)
{
    //ID no;Startno;Name;Firstname;Disp name;Nat;Cat;Group;Team;Bay;Target;Relay;Starttime;BoxTg;Active;Q Tot;Avg;Rank;G1;...;G12;10s;...;0s;Mouches
    QStringList parts = competitorRow.replace("\"", "").split(";");
    if(parts.count() >= 10){    // Make sure the row is not too short
        m_selected.setChecked(true);
        m_idEdit.setText(parts.at(1));
        m_firstNameEdit.setText(parts.at(3));
        m_lastNameEdit.setText(parts.at(2));
        m_clubEdit.setText(parts.at(8));
        updateDecimals(m_diciplineBox.currentIndex());
        updateShots(m_diciplineBox.currentIndex());
        m_inCompetition = false;
    }
}

void Lane::setStartListCompetitorRow(QString competitorRow)
{
    // targetNo;id;firstName;lastName;club;eventType;decimals;numberOfShots // TODO Lauri: Update to use new eventType
    QStringList parts = competitorRow.split(";");
    if(parts.count() >= 6){    // Make sure the row is not too short
        m_selected.setChecked(true);
        m_idEdit.setText(parts.at(1));
        m_firstNameEdit.setText(parts.at(2));
        m_lastNameEdit.setText(parts.at(3));
        m_clubEdit.setText(parts.at(4));
        m_diciplineBox.setCurrentText(QualificationEvents::eventData((QualificationEvents::EventType)parts.at(5).toInt()).name);
        m_decimals.setChecked(parts.at(6).toInt());
        m_shotsEdit.setValue(parts.at(7).toInt());
        m_inCompetition = false;
    }
}

void Lane::setCurrentShotIndex(int newIndex)
{
    m_currentShotIndex = newIndex;
}

void Lane::setDiscipline(QString newDiscipline)
{
    m_diciplineBox.setCurrentText(newDiscipline);
}

void Lane::setIp(QString newIp)
{
    m_ipEdit.setText(newIp);
}

void Lane::setLastShotLabel(QString newLabel)
{
    m_lastShotLabel.setText(newLabel);
}

void Lane::setSelected(bool selected)
{
    m_selected.setChecked(selected);
}

void Lane::setShotsNumber(int newNumber)
{
    m_shotsEdit.setValue(newNumber);
}

void Lane::setStatus(QString newStatus)
{
    if (newStatus.compare("ip_match") == 0 || newStatus.compare("match") == 0)
        m_inCompetition = true;
    else
        m_inCompetition = false;

    m_statusLabel.setText(newStatus);
//    switch (TargetStatusTexts.indexOf(newStatus)) {
//    case TargetStatuses::MAIN:
//        m_commandButton.setText(tr("Alusta proovidega"));
//        break;
//    case TargetStatuses::IP_MATCH_WAIT_SIGHTERS:
//        m_commandButton.setText(tr("Alusta proovidega"));
//        break;
//    case TargetStatuses::IP_MATCH_SIGHTERS:
//        m_commandButton.setText(tr("Lõpeta proovid"));
//        break;
//    }
}

void Lane::setTargetAndIp(int targetNo, QString ip)
{
    if (targetNo != -1 && targetNo != 0)
        m_targetEdit.setText(QString("%1").arg(targetNo));
    if (!ip.isEmpty())
        m_ipEdit.setText(ip);
}

QString Lane::target()
{
    return m_targetEdit.text();
}

void Lane::updateDecimals(int newIndex)
{
    if (newIndex >= 0) {
        if (QualificationEvents::eventData(newIndex).decimals) {
            m_decimals.setChecked(true);
        } else
            m_decimals.setChecked(false);
    }
}

void Lane::updateShots(int newIndex)
{
    if (newIndex >= 0)
        m_shotsEdit.setValue(QualificationEvents::eventData(newIndex).numberOfSeries * 10);
}

void Lane::updateUi()
{
    if (m_diciplineBox.count() > 0)
        m_diciplineBox.clear();
    m_diciplineBox.addItems(QualificationEvents::eventNames());
}

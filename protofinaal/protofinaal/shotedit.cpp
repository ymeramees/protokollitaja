#include "shotedit.h"

ShotEdit::ShotEdit()
{
    init();
}
ShotEdit::ShotEdit(QJsonObject json)
{
    init();
    m_shot = Lask(json["shot"].toObject());
    m_ignored = json["ignored"].toBool();
    m_originalShotValue = json["originalShotValue"].toString();
    updateGui();
}

bool ShotEdit::ignored() const
{
    return m_ignored;
}

void ShotEdit::init()
{
    connect(this, &QLineEdit::editingFinished, this, &ShotEdit::setShotFromGui);

    m_standardBackground.setColor(QPalette::Base, palette().base().color());

    setToolTip(tr("Lask"));
    setMinimumWidth(30);
    setMaximumWidth(40);
    setMinimumHeight(22);
}

void ShotEdit::updateBackground()
{
    if (m_ignored){
        QPalette shotIgnoredBackground;
        shotIgnoredBackground.setColor(QPalette::Base, Qt::red);
        setPalette(shotIgnoredBackground);
    } else if (!m_originalShotValue.isEmpty() && m_shot.getSLask().compare(m_originalShotValue) != 0) {
        QPalette shotEditedBackground;
        shotEditedBackground.setColor(QPalette::Base, Qt::yellow);
        setPalette(shotEditedBackground);
    } else {
        setPalette(m_standardBackground);
    }
}

void ShotEdit::setShotFromGui()
{
    m_shot.setLask(this->text());
    updateBackground();
    emit valueChanged();
}

void ShotEdit::setIgnored(bool newIgnored)
{
    m_ignored = newIgnored;
    updateBackground();
    emit valueChanged();
}

void ShotEdit::setShot(Lask shot)
{
    m_shot.set(&shot);
    m_originalShotValue = m_shot.getSLask();
    updateGui();
}

void ShotEdit::setSiusShot(QString siusRow)
{
    m_shot.setSiusShot(siusRow);
    m_originalShotValue = m_shot.getSLask();
    updateGui();
}

Lask ShotEdit::shot()
{
    return m_shot;
}

QJsonObject ShotEdit::toJson() const
{
    QJsonObject json;
    json["shot"] = m_shot.toJson();
    json["ignored"] = ignored();
    json["originalShotValue"] = m_originalShotValue;
    return json;
}

void ShotEdit::updateGui()
{
    this->setText(m_shot.getSLask());
    updateBackground();
    emit valueChanged();
}

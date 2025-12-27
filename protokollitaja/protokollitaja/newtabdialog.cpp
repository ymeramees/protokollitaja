#include "newtabdialog.h"

NewTabDialog::NewTabDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    m_targetType = TargetTypes::AirRifle;
    updateUi();
    connect(ui.harjutus, SIGNAL(currentIndexChanged(int)), this, SLOT(changeEventType(int)));
    connect(ui.laskjad, SIGNAL(currentIndexChanged(int)), this, SLOT(changeScreenName(int)));
    connect(ui.indBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeToTeam(int)));
    connect(ui.okNupp, SIGNAL(clicked()), this, SLOT(close()));

    ui.seeriateSilt->setVisible(false);
    ui.seeriateArv->setVisible(false);
    ui.laskudeSilt->setVisible(false);
    ui.laskudeArv->setVisible(false);
    ui.vSummadeSilt->setVisible(false);
    ui.vSummadeArv->setVisible(false);
    ui.vSummadeSilt2->setVisible(false);
}

void NewTabDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        qDebug() << "Language changed!";
        updateUi();
    }
    else {
        QWidget::changeEvent(event);
    }
}

QualificationEvents::EventType NewTabDialog::eventType() const
{
    return (QualificationEvents::EventType)ui.harjutus->currentIndex();
}

void NewTabDialog::changeScreenName(int)
{
    if (ui.laskjad->currentIndex() >= 0 && ui.harjutus->currentIndex() >= 0) {  // To avoid errors due to empty combobox'es
        QString laskjad = AgeClasses::ageClassName((AgeClasses::AgeClass)ui.laskjad->currentIndex()).name;

        if (ui.indBox->currentIndex() == 0) {
            ui.seeriateSilt->setText(tr("Seeriate arv:"));
            ui.seeriateArv->setMaximum(12);
            ui.kumnendikegaBox->setVisible(true);
            ui.kumnendikegaBox->setCheckable(true);
            ui.seeriateSilt->setVisible(false);
            ui.seeriateArv->setVisible(false);

            int otherEventsStart = QualificationEvents::eventNames().size() - 6;
            if (ui.harjutus->currentIndex() >= otherEventsStart) {
                ui.seeriateSilt->setVisible(true);
                ui.seeriateArv->setVisible(true);
                ui.laskudeSilt->setVisible(true);
                ui.laskudeArv->setVisible(true);
                ui.vSummadeSilt->setVisible(true);
                ui.vSummadeArv->setVisible(true);
                ui.vSummadeSilt2->setVisible(true);
            } else {
                ui.laskudeSilt->setVisible(false);
                ui.laskudeArv->setVisible(false);
                ui.vSummadeSilt->setVisible(false);
                ui.vSummadeArv->setVisible(false);
                ui.vSummadeSilt2->setVisible(false);
            }
            // Overcomplicated, but meant to be future proof in case we allow manual edits to event names
            ui.nimiTulAknas->setText(QualificationEvents::eventData(QualificationEvents::fromEventName(ui.harjutus->currentText())).name + " " + laskjad);
        } else {
            ui.seeriateSilt->setText(tr("Liikmete arv:"));
            ui.seeriateArv->setMaximum(18);
            ui.seeriateSilt->setVisible(true);
            ui.seeriateArv->setVisible(true);
            ui.kumnendikegaBox->setVisible(false);
            ui.kumnendikegaBox->setChecked(false);
            ui.kumnendikegaBox->setCheckable(false);

            laskjad.append(" " + ui.indBox->currentText());
            ui.nimiTulAknas->setText(QualificationEvents::eventData(QualificationEvents::fromEventName(ui.harjutus->currentText())).name + " " + laskjad);
            m_targetType = TargetTypes::TargetType::Other;
        }
    }
}

void NewTabDialog::close()
{
    if (ui.sakiNimi->text() == tr("uus tööleht")) {
        if(QMessageBox::warning(this, "Protokollitaja", tr("Oled kindel, et soovid töölehe nimeks jätta uus tööleht?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No)==QMessageBox::Yes)
        accept();
    } else accept();
}

void NewTabDialog::changeEventType(int)
{
    if (ui.laskjad->currentIndex() >= 0 && ui.harjutus->currentIndex() >= 0) {
        if (ui.indBox->currentIndex() == 0) {
            ui.seeriateArv->setValue(QualificationEvents::eventData(ui.harjutus->currentIndex()).numberOfSeries);
            ui.vSummadeArv->setValue(QualificationEvents::eventData(ui.harjutus->currentIndex()).seriesInSubtotal);
            m_targetType = QualificationEvents::eventData(ui.harjutus->currentIndex()).targetType;
            ui.kumnendikegaBox->setChecked(QualificationEvents::eventData(QualificationEvents::fromEventName(ui.harjutus->currentText())).decimals);
        }

        changeScreenName(0);
    }
}

void NewTabDialog::changeToTeam(int)
{
    if (ui.laskjad->currentIndex() >= 0 && ui.harjutus->currentIndex() >= 0) {
        if (ui.indBox->currentIndex() == 1) {
            ui.seeriateArv->setValue(3);
        }

        changeScreenName(0);
    }
}

void NewTabDialog::updateUi()
{
    if (ui.harjutus->count() > 0)
        ui.harjutus->clear();
    ui.harjutus->addItems(QualificationEvents::eventNames());
    if (ui.laskjad->count() > 0)
        ui.laskjad->clear();
    ui.laskjad->addItems(AgeClasses::classAbbreviations());
}

NewTabDialog::~NewTabDialog()
{

}

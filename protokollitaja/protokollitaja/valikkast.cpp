#include "valikkast.h"

ValikKast::ValikKast(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    m_targetType = TargetTypes::AirRifle;
    updateUi();
    connect(ui.harjutus, SIGNAL(currentIndexChanged(int)), this, SLOT(muudaEkraaniNimi(int)));
    connect(ui.laskjad, SIGNAL(currentIndexChanged(int)), this, SLOT(muudaEkraaniNimi(int)));
    connect(ui.indBox, SIGNAL(currentIndexChanged(int)), this, SLOT(muudaEkraaniNimi(int)));
//    connect(ui.seeriateArv, SIGNAL(valueChanged(int)), this, SLOT(muudaHarjutus(int)));
//    connect(ui.vSummadeArv, SIGNAL(valueChanged(int)), this, SLOT(muudaHarjutus(int)));
    //connect(ui.indBox, SIGNAL(currentIndexChanged(int)), this, SLOT(voimalda(int)));
    connect(ui.okNupp, SIGNAL(clicked()), this, SLOT(sulge()));

    ui.seeriateSilt->setVisible(false);
    ui.seeriateArv->setVisible(false);
    ui.laskudeSilt->setVisible(false);
    ui.laskudeArv->setVisible(false);
    ui.vSummadeSilt->setVisible(false);
    ui.vSummadeArv->setVisible(false);
    ui.vSummadeSilt2->setVisible(false);
}

void ValikKast::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        qDebug() << "Language changed!";
        updateUi();
    }
    else {
        QWidget::changeEvent(event);
    }
}

QualificationEvents::EventType ValikKast::eventType() const
{
    return (QualificationEvents::EventType)ui.harjutus->currentIndex();
}

void ValikKast::muudaEkraaniNimi(int)
{
    if (ui.laskjad->currentIndex() >= 0 && ui.harjutus->currentIndex() >= 0) {  // To avoid errors due to empty combobox'es
        ui.seeriateSilt->setVisible(false);
        ui.seeriateArv->setVisible(false);
        ui.laskudeSilt->setVisible(false);
        ui.laskudeArv->setVisible(false);
        ui.vSummadeSilt->setVisible(false);
        ui.vSummadeArv->setVisible(false);
        ui.vSummadeSilt2->setVisible(false);

        if(ui.indBox->currentIndex() == 0){
            ui.seeriateSilt->setText(tr("Seeriate arv:"));
            ui.seeriateArv->setMaximum(12);
            ui.kumnendikegaBox->setCheckable(true);
        }else{
            ui.seeriateSilt->setText(tr("Liikmete arv:"));
            ui.seeriateArv->setMaximum(8);
            ui.seeriateSilt->setVisible(true);
            ui.seeriateArv->setVisible(true);
            ui.kumnendikegaBox->setChecked(false);
            ui.kumnendikegaBox->setCheckable(false);
        }
        QString laskjad = AgeClasses::ageClassName((AgeClasses::AgeClass)ui.laskjad->currentIndex()).name;

        if(ui.indBox->currentIndex() == 1){
            laskjad.append(" " + ui.indBox->currentText());
            ui.nimiTulAknas->setText(QualificationEvents::eventNames().at(ui.harjutus->currentIndex()) + " " + laskjad);
            m_targetType = TargetTypes::TargetType::Other;
            return;
        }

        ui.seeriateArv->setValue(QualificationEvents::eventData(ui.harjutus->currentIndex()).numberOfSeries);
        ui.vSummadeArv->setValue(QualificationEvents::eventData(ui.harjutus->currentIndex()).seriesInSubtotal);
        m_targetType = QualificationEvents::eventData(ui.harjutus->currentIndex()).targetType;

        int otherEventsStart = QualificationEvents::eventNames().size() - 6;
        if (ui.harjutus->currentIndex() >= otherEventsStart) {
            ui.seeriateSilt->setVisible(true);
            ui.seeriateArv->setVisible(true);
            ui.laskudeSilt->setVisible(true);
            ui.laskudeArv->setVisible(true);
            ui.vSummadeSilt->setVisible(true);
            ui.vSummadeArv->setVisible(true);
            ui.vSummadeSilt2->setVisible(true);

        }
        ui.nimiTulAknas->setText(QualificationEvents::eventNames().at(ui.harjutus->currentIndex()) + " " + laskjad);
    }
}

void ValikKast::voimalda(int indeks)
{
    Q_UNUSED(indeks);
    /*if(indeks == 0)
        ui.leheValikBox->setEnabled(false);
    else ui.leheValikBox->setEnabled(true);*/
}

void ValikKast::sulge()
{
        if(ui.sakiNimi->text() == tr("uus tööleht")){
                if(QMessageBox::warning(this, "Protokollitaja", tr("Oled kindel, et soovid töölehe nimeks jätta uus tööleht?"),
                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No)==QMessageBox::Yes)
                accept();
    }else accept();
}

void ValikKast::muudaHarjutus(int)
{
//    if(ui.indBox->currentIndex() == 0)  //Harjutus muutub ainult, kui tegu on individuaalide lehe valikuga, võistkondlikul tähendab see liikmete arvu valimist
//        harjutus = "Muu";
}

void ValikKast::updateUi()
{
    if (ui.harjutus->count() > 0)
        ui.harjutus->clear();
    ui.harjutus->addItems(QualificationEvents::eventNames());
    if (ui.laskjad->count() > 0)
        ui.laskjad->clear();
    ui.laskjad->addItems(AgeClasses::classAbbreviations());
}

ValikKast::~ValikKast()
{

}

#include "seadedkast.h"

SeadedKast::SeadedKast(QString nim, QString aegK, QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
    Q_UNUSED(nim);
    Q_UNUSED(aegK);
//	ui.voistluseNimi->setText(nim);
//	voistluseNimi = ui.voistluseNimi->text();
//	ui.aegKohtEdit->setText(aegK);
//	aegKoht = ui.aegKohtEdit->text();
//	kirjutusAbi = ui.kirjutusAbiCombo->currentIndex();
//	autosave = ui.aegCombo->currentIndex();
//	aeg = ui.aegEdit->value();
//	sakiAsukoht = ui.sakiBox->currentIndex();

	connect(ui.okNupp, SIGNAL(clicked()), this, SLOT(salvesta()));
	connect(ui.loobuNupp, SIGNAL(clicked()), this, SLOT(close()));
}

void SeadedKast::salvesta()
{
//	voistluseNimi = ui.voistluseNimi->text();
//	aegKoht = ui.aegKohtEdit->text();
//	kirjutusAbi = ui.kirjutusAbiCombo->currentIndex();
//	autosave = ui.aegCombo->currentIndex();
//	aeg = ui.aegEdit->value();
//	sakiAsukoht = ui.sakiBox->currentIndex();
	emit salvestatud();
	this->close();
}

void SeadedKast::closeEvent(QCloseEvent *event)
{
//	ui.voistluseNimi->setText(voistluseNimi);
//	ui.aegKohtEdit->setText(aegKoht);
//	ui.kirjutusAbiCombo->setCurrentIndex(kirjutusAbi);
//	ui.aegCombo->setCurrentIndex(autosave);
//	ui.aegEdit->setValue(aeg);
//	ui.sakiBox->setCurrentIndex(sakiAsukoht);
	event->accept();
}

SeadedKast::~SeadedKast()
{

}

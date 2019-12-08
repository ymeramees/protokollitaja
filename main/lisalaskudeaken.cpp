#include "lisalaskudeaken.h"

LisaLaskudeAken::LisaLaskudeAken(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.ulesNupp, SIGNAL(clicked()), this, SLOT(ules()));
	connect(ui.allaNupp, SIGNAL(clicked()), this, SLOT(alla()));
	connect(ui.eemaldaNupp, SIGNAL(clicked()), this, SLOT(eemalda()));
	connect(ui.lisaNupp, SIGNAL(clicked()), this, SLOT(lisa()));
}

void LisaLaskudeAken::ules()
{
	ui.lisaLasud->insertItem(ui.lisaLasud->currentRow(), ui.lisaLasud->takeItem(ui.lisaLasud->currentRow()));
	ui.lisaLasud->setCurrentRow(ui.lisaLasud->currentRow() - 1);
}

void LisaLaskudeAken::alla()
{
	int rida = ui.lisaLasud->currentRow();
	ui.lisaLasud->insertItem(rida + 1, ui.lisaLasud->takeItem(ui.lisaLasud->currentRow()));
	ui.lisaLasud->setCurrentRow(rida + 1);
}

void LisaLaskudeAken::eemalda()
{
	ui.lisaLasud->takeItem(ui.lisaLasud->currentRow());
}

void LisaLaskudeAken::lisa()
{
    bool onnestus = false;
    double uus = QInputDialog::getDouble(this, "Sisestage uus lisalask", "Uus lisalask:", 0.0, 0, 10.9, 1, &onnestus);
    if(onnestus){
        QString lask = QString("%1").arg(uus);
        if(!lask.contains('.'))
            lask.append(".0");
        lask.replace('.', ',');
        ui.lisaLasud->addItem(lask);
    }
}

LisaLaskudeAken::~LisaLaskudeAken()
{

}

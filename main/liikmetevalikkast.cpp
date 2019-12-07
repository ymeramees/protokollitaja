#include "liikmetevalikkast.h"

LiikmeteValikKast::LiikmeteValikKast(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	maxArv = 4;
	connect(ui.eemaldaNupp, SIGNAL(clicked()), this, SLOT(eemalda()));
	connect(ui.lisaNupp, SIGNAL(clicked()), this, SLOT(lisa()));
}

void LiikmeteValikKast::eemalda()
{
	ui.liikmed->takeItem(ui.liikmed->currentRow());
	//QMessageBox::information(this, "Teade", QString("jupikesi: %1").arg(ui.liikmed->count()), QMessageBox::Ok);
}

void LiikmeteValikKast::lisa()
{
	if(ui.liikmed->count() < maxArv){
		ui.liikmed->addItem(ui.leheLaskurid->currentItem()->text());
	}
	//QMessageBox::information(this, "Teade", QString("jupikesi: %1").arg(ui.leheLaskurid->count()), QMessageBox::Ok);
}

LiikmeteValikKast::~LiikmeteValikKast()
{

}

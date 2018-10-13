#include "valikkast.h"

ValikKast::ValikKast(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
    relv = Ohupuss;
    harjutus = ui.harjutus->currentText();
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

void ValikKast::muudaEkraaniNimi(int)
{
    ui.seeriateSilt->setVisible(false);
    ui.seeriateArv->setVisible(false);
    ui.laskudeSilt->setVisible(false);
    ui.laskudeArv->setVisible(false);
    ui.vSummadeSilt->setVisible(false);
    ui.vSummadeArv->setVisible(false);
    ui.vSummadeSilt2->setVisible(false);

	if(ui.indBox->currentIndex() == 0){
		ui.seeriateSilt->setText("Seeriate arv:");
		ui.seeriateArv->setMaximum(12);
        ui.kumnendikegaBox->setCheckable(true);
    }else{
		ui.seeriateSilt->setText("Liikmete arv:");
		ui.seeriateArv->setMaximum(8);
        ui.seeriateSilt->setVisible(true);
        ui.seeriateArv->setVisible(true);
        ui.kumnendikegaBox->setChecked(false);
        ui.kumnendikegaBox->setCheckable(false);
	}
	QString laskjad;
	switch(ui.laskjad->currentIndex()){
		case 0:{
			laskjad = "Mehed";
			break;
		}
		case 1:{
			laskjad = "Naised";
			break;
		}
		case 2:{
			laskjad = "Meesjuuniorid";
			break;
		}
		case 3:{
			laskjad = "Naisjuuniorid";
			break;
		}
		case 4:{
			laskjad = "Poisid";
			break;
		}
		case 5:{
            laskjad = tr("Tüdrukud");
			break;
		}
		case 6:{
			laskjad = "Meesveteranid";
			break;
		}
		case 7:{
			laskjad = "Naisveteranid";
			break;
		}
	}

    harjutus = ui.harjutus->currentText();

	if(ui.indBox->currentIndex() == 1){
		laskjad.append(" " + ui.indBox->currentText());
		ui.nimiTulAknas->setText(ui.harjutus->currentText() + " " + laskjad);
        relv = Muu;
        return;
	}

	switch(ui.harjutus->currentIndex()){
    case 0:{
        ui.seeriateArv->setValue(4);
        ui.vSummadeArv->setValue(0);
        relv = Ohupuss;
        break;
    }
    case 1:{
        ui.seeriateArv->setValue(6);
        ui.vSummadeArv->setValue(0);
        relv = Ohupuss;
        break;
    }
    case 2:{
        ui.seeriateArv->setValue(4);
        ui.vSummadeArv->setValue(0);
        relv = Ohupustol;
        break;
    }
    case 3:{
        ui.seeriateArv->setValue(6);
        ui.vSummadeArv->setValue(0);
        relv = Ohupustol;
        break;
    }
    case 4:{
        ui.seeriateArv->setValue(3);
        ui.vSummadeArv->setValue(0);
        relv = Sportpuss;
        break;
    }
    case 5:{
        ui.seeriateArv->setValue(6);
        ui.vSummadeArv->setValue(0);
        relv = Sportpuss;
        break;
    }
    case 6:{
        ui.seeriateArv->setValue(3);
        ui.vSummadeArv->setValue(1);
        relv = Sportpuss;
        break;
    }
    case 7:{
        ui.seeriateArv->setValue(6);
        ui.vSummadeArv->setValue(2);
        relv = Sportpuss;
        break;
    }
    case 8:{
        ui.seeriateArv->setValue(12);
        ui.vSummadeArv->setValue(4);
        relv = Sportpuss;
        break;
    }
    case 9:{
        ui.seeriateArv->setValue(6);
        ui.vSummadeArv->setValue(0);
        relv = Spordipustol;
        break;
    }
    case 10:{
        ui.seeriateArv->setValue(6);
        ui.vSummadeArv->setValue(3);
        relv = Spordipustol;
        break;
    }
    case 11:{
        ui.seeriateArv->setValue(6);
        ui.vSummadeArv->setValue(2);
        relv = Spordipustol;
        break;
    }
    case 12:{
        ui.seeriateArv->setValue(6);
        ui.vSummadeArv->setValue(3);
        relv = Spordipustol;
        break;
    }
    case 13:{
        ui.seeriateArv->setValue(4);
        ui.vSummadeArv->setValue(2);
        relv = Puss;
        break;
    }
    case 14:{
        ui.seeriateArv->setValue(6);
        ui.vSummadeArv->setValue(3);
        relv = Puss;
        break;
    }
    case 15:{
        ui.seeriateSilt->setVisible(true);
        ui.seeriateArv->setVisible(true);
        ui.laskudeSilt->setVisible(true);
        ui.laskudeArv->setVisible(true);
        ui.vSummadeSilt->setVisible(true);
        ui.vSummadeArv->setVisible(true);
        ui.vSummadeSilt2->setVisible(true);

        ui.seeriateArv->setValue(2);
        ui.vSummadeArv->setValue(0);
        relv = Ohupuss;
        break;
    }
    case 16:{
        ui.seeriateSilt->setVisible(true);
        ui.seeriateArv->setVisible(true);
        ui.laskudeSilt->setVisible(true);
        ui.laskudeArv->setVisible(true);
        ui.vSummadeSilt->setVisible(true);
        ui.vSummadeArv->setVisible(true);
        ui.vSummadeSilt2->setVisible(true);

        ui.seeriateArv->setValue(2);
        ui.vSummadeArv->setValue(0);
        relv = Ohupustol;
        break;
    }
    case 17:{
        ui.seeriateSilt->setVisible(true);
        ui.seeriateArv->setVisible(true);
        ui.laskudeSilt->setVisible(true);
        ui.laskudeArv->setVisible(true);
        ui.vSummadeSilt->setVisible(true);
        ui.vSummadeArv->setVisible(true);
        ui.vSummadeSilt2->setVisible(true);

        ui.seeriateArv->setValue(4);
        ui.vSummadeArv->setValue(0);
        relv = Sportpuss;
        break;
    }
    case 18:{
        ui.seeriateSilt->setVisible(true);
        ui.seeriateArv->setVisible(true);
        ui.laskudeSilt->setVisible(true);
        ui.laskudeArv->setVisible(true);
        ui.vSummadeSilt->setVisible(true);
        ui.vSummadeArv->setVisible(true);
        ui.vSummadeSilt2->setVisible(true);

        ui.seeriateArv->setValue(2);
        ui.vSummadeArv->setValue(0);
        relv = Spordipustol;
        break;
    }
    default:{
        ui.seeriateSilt->setVisible(true);
        ui.seeriateArv->setVisible(true);
        ui.laskudeSilt->setVisible(true);
        ui.laskudeArv->setVisible(true);
        ui.vSummadeSilt->setVisible(true);
        ui.vSummadeArv->setVisible(true);
        ui.vSummadeSilt2->setVisible(true);

        ui.seeriateArv->setValue(6);
        ui.vSummadeArv->setValue(0);
        relv = Muu;
        break;
    }
	}
	ui.nimiTulAknas->setText(ui.harjutus->currentText() + " " + laskjad);
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

ValikKast::~ValikKast()
{

}

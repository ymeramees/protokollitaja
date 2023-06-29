#include "laskudeaken.h"
#include "ui_laskudeaken.h"

LaskudeAken::LaskudeAken(bool kum, int s, int ls, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LaskudeAken)
{
    ui->setupUi(this);

    kumnendikega = kum;
    laskudeArv = ls;
    seeriateArv = s;
    aktiivneSeeria = 0;
//#ifdef QT_DEBUG
//    qDebug() << "LaskudeAken";
//#endif
//    if(verbose)
//        QTextStream(stdout) << "LaskudeAken::LaskudeAken()" << endl;

    for(int i = 0; i < seeriateArv; i++){
        QList<Lask*> seeriaLas;
        for(int j = 0; j < laskudeArv; j++){
            seeriaLas << new Lask(/*this*/);  //Seeria lasud
        }
        seeriaLas << new Lask(/*this*/);  //Lisaks üks lask karistuse jaoks
        lasud << seeriaLas/*.toVector().toList()*/;   //Seeria lasud laskuri laskude hulka
    }

    connect(ui->paremaleNupp, SIGNAL(clicked()), this, SLOT(paremale()));
    connect(ui->vasakuleNupp, SIGNAL(clicked()), this, SLOT(vasakule()));
    QStringList pais;
    pais << tr("Lask") << "X" << "Y" << tr("Sisekümme");
    ui->laskudeTabel->setColumnCount(4);
    ui->laskudeTabel->setHorizontalHeaderLabels(pais);
    ui->laskudeTabel->setColumnWidth(0, 40);
    ui->laskudeTabel->setColumnWidth(1, 80);
    ui->laskudeTabel->setColumnWidth(2, 80);
    ui->laskudeTabel->setColumnWidth(3, 100);
    ui->laskudeTabel->setRowCount(laskudeArv);
    for(int i = 0; i < laskudeArv; i++){
//        ui->laskudeTabel->setItem(i, 0, new QTableWidgetItem(tr("%1.").arg(i + 1)));
        ui->laskudeTabel->setItem(i, 0, new QTableWidgetItem(lasud[0][i]->getSLask()));
        ui->laskudeTabel->setItem(i, 1, new QTableWidgetItem(tr("%1").arg(lasud[0][i]->stringX())));
        ui->laskudeTabel->setItem(i, 2, new QTableWidgetItem(tr("%1").arg(lasud[0][i]->stringY())));
        ui->laskudeTabel->setItem(i, 3, new QTableWidgetItem());
        if(lasud[0][i]->isInnerTen())
            ui->laskudeTabel->item(i, 3)->setCheckState(Qt::Checked);
        else
            ui->laskudeTabel->item(i, 3)->setCheckState(Qt::Unchecked);
    }
    ui->karistusEdit->setText(lasud[0][lasud[aktiivneSeeria].count() - 1]->getSLask());
    ui->seeriaSilt->setText("1. seeria");
    ui->vasakuleNupp->setEnabled(false);
}

void LaskudeAken::closeEvent(QCloseEvent *event)
{
    kirjutaLasud();
}

void LaskudeAken::kirjutaLasud()    //Kirjutab tabelis olevad lasud seeriasse
{
//    if(verbose)
//        QTextStream(stdout) << "LaskudeAken::kirjutaLasud()" << endl;

    float lask = 0;
    bool onnestus = false;
    for(int i = 0; i < laskudeArv; i++){
        if(ui->laskudeTabel->item(i, 0)->text().isEmpty()){
            lasud[aktiivneSeeria][i]->setLask("");
        } else {
//            ui->laskudeTabel->item(i, 0)->setText("-999");
            lask = ui->laskudeTabel->item(i, 0)->text().toFloat(&onnestus);
            if(!onnestus && ui->laskudeTabel->item(i, 0)->text().contains(','))
                lask = ui->laskudeTabel->item(i, 0)->text().replace(',', '.').toFloat(&onnestus);
            else if(!onnestus && ui->laskudeTabel->item(i, 0)->text().contains('.'))
                lask = ui->laskudeTabel->item(i, 0)->text().replace('.', ',').toFloat(&onnestus);
            if(!onnestus){
                QMessageBox::critical(this, "Viga", "Vigane lasu väärtus tabelis!", QMessageBox::Ok);
                return;
            }
            lasud[aktiivneSeeria][i]->setLask(lask);
        }
        lasud[aktiivneSeeria][i]->setMmX(ui->laskudeTabel->item(i, 1)->text());
        lasud[aktiivneSeeria][i]->setMmY(ui->laskudeTabel->item(i, 2)->text());
        lasud[aktiivneSeeria][i]->setInnerTen(ui->laskudeTabel->item(i, 3)->checkState());
    }
    int karistus = ui->karistusEdit->text().toInt();
    if(ui->karistusEdit->text().isEmpty() || karistus == -999)
        lasud[aktiivneSeeria][lasud[aktiivneSeeria].count() - 1]->set10Lask(-999);
    else
        lasud[aktiivneSeeria][lasud[aktiivneSeeria].count() - 1]->setLask(karistus);
}

void LaskudeAken::loeLasud()    //Loeb seeriast tabelisse uued lasud
{
//#ifdef QT_DEBUG
//    qDebug() << "LaskudeAken::loelasud()";
//#endif
//    if(verbose)
//        QTextStream(stdout) << "LaskudeAken::loelasud()" << endl;

    for(int i = 0; i < laskudeArv; i++){
        ui->laskudeTabel->item(i, 0)->setText(lasud[aktiivneSeeria][i]->getSLask());
        ui->laskudeTabel->item(i, 1)->setText(tr("%1").arg(lasud[aktiivneSeeria][i]->stringX()));
        ui->laskudeTabel->item(i, 2)->setText(tr("%1").arg(lasud[aktiivneSeeria][i]->stringY()));
        if(lasud[aktiivneSeeria][i]->isInnerTen())
            ui->laskudeTabel->item(i, 3)->setCheckState(Qt::Checked);
        else
            ui->laskudeTabel->item(i, 3)->setCheckState(Qt::Unchecked);
    }
    QString karistus = lasud[aktiivneSeeria][lasud[aktiivneSeeria].count() - 1]->getSLask();
    if(karistus.isEmpty() || karistus == "-999")
        karistus.clear();
    ui->karistusEdit->setText(karistus);
    ui->seeriaSilt->setText(tr("%1. seeria").arg(aktiivneSeeria + 1));
}

void LaskudeAken::muudaPealkirja(QString uus)
{
    this->setWindowTitle(uus);
}

void LaskudeAken::paremale()
{
    kirjutaLasud();
    ui->vasakuleNupp->setEnabled(true);
    aktiivneSeeria++;
    if(aktiivneSeeria >= seeriateArv)
        aktiivneSeeria = seeriateArv - 1;
    if(aktiivneSeeria == seeriateArv - 1)
        ui->paremaleNupp->setEnabled(false);

    loeLasud();
}

void LaskudeAken::vasakule()
{
    kirjutaLasud();
    ui->paremaleNupp->setEnabled(true);
    aktiivneSeeria--;
    if(aktiivneSeeria < 0)
        aktiivneSeeria = 0;
    if(aktiivneSeeria == 0)
        ui->vasakuleNupp->setEnabled(false);

    loeLasud();
}

LaskudeAken::~LaskudeAken()
{
    delete ui;
}

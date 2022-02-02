#include "shotswindow.h"
#include "ui_shotswindow.h"

ShotsWindow::ShotsWindow(QVector<CompetitorShot> *shots, /*bool withDecimals, int noOfSeries, int noOfShots,*/ QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShotsWindow),
    m_shotsTableModel(shots)
{
    ui->setupUi(this);
    ui->shotsTable->setModel(&m_shotsTableModel);
    ui->shotsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->shotsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->shotsTable->horizontalHeader()->setMinimumSectionSize(10);
    ui->upButton->setArrowType(Qt::UpArrow);
    ui->downButton->setArrowType(Qt::DownArrow);
    ui->shotsTable->setIconSize(QSize(35, 35));

//    m_withDecimals = withDecimals;
//    m_noOfShots = noOfShots;
//    m_noOfSeries = noOfSeries;
//    m_shots = shots;
//    aktiivneSeeria = 0;
//#ifdef PROOV
//    qDebug() << "ShotsWindow";
//#endif
//    if(verbose)
//        QTextStream(stdout) << "ShotsWindow::ShotsWindow()" << endl;

//    for(int i = 0; i < m_noOfSeries; i++){
//        QList<Lask*> seeriaLas;
//        for(int j = 0; j < m_noOfShots; j++){
//            seeriaLas << new Lask(/*this*/);  //Seeria lasud
//        }
//        seeriaLas << new Lask(/*this*/);  //Lisaks üks lask karistuse jaoks
//        lasud << seeriaLas/*.toVector().toList()*/;   //Seeria lasud laskuri laskude hulka
//    }

    connect(ui->competitionButton, &QPushButton::clicked, [this](){
        markCompetition(true);
    });
    connect(ui->sightersButton, &QPushButton::clicked, [this](){
        markCompetition(false);
    });
    connect(ui->insertShotButton, &QPushButton::clicked, this, &ShotsWindow::insertShot);
    connect(ui->penaltyButton, &QPushButton::clicked, this, &ShotsWindow::insertPenalty);
    connect(ui->deleteButton, &QPushButton::clicked, this, &ShotsWindow::deleteSelectedShot);
    connect(ui->ignoreButton, &QPushButton::clicked, this, &ShotsWindow::ignoreSelectedShot);
    connect(ui->upButton, &QPushButton::clicked, this, &ShotsWindow::moveUp);
    connect(ui->downButton, &QPushButton::clicked, this, &ShotsWindow::moveDown);
//    QStringList pais;
//    pais << tr("Lask") << "X" << "Y" << tr("Sisekümme");
//    ui->shotsTable->setColumnCount(7);  // Competition/sighting/ignored shot, shot value, x, y, isInnerTen, time, origin
//    ui->shotsTable->setHorizontalHeaderLabels(pais);
//    ui->shotsTable->setColumnWidth(0, 40);
//    ui->shotsTable->setColumnWidth(1, 80);
//    ui->shotsTable->setColumnWidth(2, 80);
//    ui->shotsTable->setColumnWidth(3, 100);
//    ui->shotsTable->setRowCount(m_noOfShots);
//    for(int i = 0; i < m_noOfShots; i++){
////        ui->shotsTable->setItem(i, 0, new QTableWidgetItem(tr("%1.").arg(i + 1)));
//        ui->shotsTable->setItem(i, 1, new QTableWidgetItem(lasud[0][i]->getSLask()));
//        ui->shotsTable->setItem(i, 2, new QTableWidgetItem(tr("%1").arg(lasud[0][i]->stringX())));
//        ui->shotsTable->setItem(i, 3, new QTableWidgetItem(tr("%1").arg(lasud[0][i]->stringY())));
//        ui->shotsTable->setItem(i, 4, new QTableWidgetItem());
//        if(lasud[0][i]->isInnerTen())
//            ui->shotsTable->item(i, 4)->setCheckState(Qt::Checked);
//        else
//            ui->shotsTable->item(i, 4)->setCheckState(Qt::Unchecked);
//        ui->shotsTable->setItem(i, 5, new QTableWidgetItem(lasud[0][i]->shotTime().toString()));
//        ui->shotsTable->setItem(i, 6, new QTableWidgetItem(lasud[0][i]->shotOrigin()));
//    }
//    ui->karistusEdit->setText(lasud[0][lasud[aktiivneSeeria].count() - 1]->getSLask());
//    ui->vasakuleNupp->setEnabled(false);
}

void ShotsWindow::closeEvent(QCloseEvent *event)
{
    kirjutaLasud();
    event->accept();
}

bool ShotsWindow::deleteSelectedShot()
{
    if (ui->shotsTable->selectionModel()->selectedRows().size() > 0) {
        int firstSelected = ui->shotsTable->selectionModel()->selectedRows().first().row();
        return m_shotsTableModel.deleteShot(firstSelected);
    } else {
        return false;
    }
}

bool ShotsWindow::ignoreSelectedShot()
{
    QModelIndexList selection = ui->shotsTable->selectionModel()->selectedRows();
    if (selection.size() > 0) {
//        int firstSelected = ui->shotsTable->selectionModel()->selectedRows().first().row();
        foreach(QModelIndex index, selection) {
            m_shotsTableModel.ignoreShot(index.row());
        }
        return true;
    } else {
        return false;
    }
}

void ShotsWindow::insertPenalty()
{
//    int row = insertShot();
//    m_shots[0][row].set10Lask(ui->penaltyEdit->text().toInt() * 10);
    insertShot(ui->penaltyEdit->text().toInt() * 10);
}

bool ShotsWindow::insertShot(int value)
{
    if (ui->shotsTable->selectionModel()->selectedRows().size() > 0) {
        int firstSelected = ui->shotsTable->selectionModel()->selectedRows().first().row();
        return m_shotsTableModel.insertShot(firstSelected, value);
    } else {
        return m_shotsTableModel.insertShot(m_shotsTableModel.size(), value);
    }
}

void ShotsWindow::kirjutaLasud()    //Kirjutab tabelis olevad lasud seeriasse
{
////    if(verbose)
////        QTextStream(stdout) << "ShotsWindow::kirjutaLasud()" << endl;

//    float lask = 0;
//    bool onnestus = false;
//    for(int i = 0; i < m_noOfShots; i++){
//        if(ui->shotsTable->item(i, 0)->text().isEmpty()){
//            lasud[aktiivneSeeria][i]->setLask("");
//        } else {
////            ui->shotsTable->item(i, 0)->setText("-999");
//            lask = ui->shotsTable->item(i, 0)->text().toFloat(&onnestus);
//            if(!onnestus && ui->shotsTable->item(i, 0)->text().contains(','))
//                lask = ui->shotsTable->item(i, 0)->text().replace(',', '.').toFloat(&onnestus);
//            else if(!onnestus && ui->shotsTable->item(i, 0)->text().contains('.'))
//                lask = ui->shotsTable->item(i, 0)->text().replace('.', ',').toFloat(&onnestus);
//            if(!onnestus){
//                QMessageBox::critical(this, "Viga", "Vigane lasu väärtus tabelis!", QMessageBox::Ok);
//                return;
//            }
//            lasud[aktiivneSeeria][i]->setLask(lask);
//        }
//        lasud[aktiivneSeeria][i]->setMmX(ui->shotsTable->item(i, 1)->text());
//        lasud[aktiivneSeeria][i]->setMmY(ui->shotsTable->item(i, 2)->text());
//        lasud[aktiivneSeeria][i]->setInnerTen(ui->shotsTable->item(i, 3)->checkState());
//    }
//    int karistus = ui->karistusEdit->text().toInt();
//    if(ui->karistusEdit->text().isEmpty() || karistus == -999)
//        lasud[aktiivneSeeria][lasud[aktiivneSeeria].count() - 1]->set10Lask(-999);
//    else
//        lasud[aktiivneSeeria][lasud[aktiivneSeeria].count() - 1]->setLask(karistus);
}

void ShotsWindow::loeLasud()    //Loeb seeriast tabelisse uued lasud
{
////#ifdef PROOV
////    qDebug() << "ShotsWindow::loelasud()";
////#endif
////    if(verbose)
////        QTextStream(stdout) << "ShotsWindow::loelasud()" << endl;

//    for(int i = 0; i < m_noOfShots; i++){
//        ui->shotsTable->item(i, 0)->setText(lasud[aktiivneSeeria][i]->getSLask());
//        ui->shotsTable->item(i, 1)->setText(tr("%1").arg(lasud[aktiivneSeeria][i]->stringX()));
//        ui->shotsTable->item(i, 2)->setText(tr("%1").arg(lasud[aktiivneSeeria][i]->stringY()));
//        if(lasud[aktiivneSeeria][i]->isInnerTen())
//            ui->shotsTable->item(i, 3)->setCheckState(Qt::Checked);
//        else
//            ui->shotsTable->item(i, 3)->setCheckState(Qt::Unchecked);
//    }
//    QString karistus = lasud[aktiivneSeeria][lasud[aktiivneSeeria].count() - 1]->getSLask();
//    if(karistus.isEmpty() || karistus == "-999")
//        karistus.clear();
//    ui->karistusEdit->setText(karistus);
}

void ShotsWindow::moveDown()
{
    QModelIndexList selection = ui->shotsTable->selectionModel()->selectedRows();

    qDebug() << "ShotsWindow::moveDown";
    m_shotsTableModel.moveDown(selection);
}

void ShotsWindow::moveUp()
{
    QItemSelectionModel *selectionModel = ui->shotsTable->selectionModel();
    QModelIndexList selection = selectionModel->selectedRows();

    qDebug() << "ShotsWindow::moveUp";
    selectionModel->select(m_shotsTableModel.moveUp(selection), QItemSelectionModel::Select);
//    ui->shotsTable->setSelectionModel(selectionModel);
}

void ShotsWindow::muudaPealkirja(QString uus)
{
    this->setWindowTitle(uus);
}

void ShotsWindow::markCompetition(bool isCompetition)
{
//    qDebug() << "isCompetition = " << isCompetition;
    QModelIndexList selection = ui->shotsTable->selectionModel()->selectedRows();

    foreach(QModelIndex index, selection) {
        m_shotsTableModel.markCompetition(index.row(), isCompetition);
//        Lask current = m_shots->at(index.row());
//        current.setCompetitionShot(isCompetition);
//        m_shots[0][index.row()].setCompetitionShot(isCompetition);
//        emit ui->shotsTable->model()->dataChanged(index, index, {Qt::DisplayRole});
//        m_shots[0][] = current;
//        m_shots->replace(index.row(), current);
    }
//    kirjutaLasud();
//    ui->vasakuleNupp->setEnabled(true);
//    aktiivneSeeria++;
//    if(aktiivneSeeria >= m_noOfSeries)
//        aktiivneSeeria = m_noOfSeries - 1;
//    if(aktiivneSeeria == m_noOfSeries - 1)
//        ui->paremaleNupp->setEnabled(false);

//    loeLasud();
//    ui->shotsTable
}

ShotsWindow::~ShotsWindow()
{
    delete ui;
}

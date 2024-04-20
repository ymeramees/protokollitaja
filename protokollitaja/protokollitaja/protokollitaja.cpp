#include <QtGui>
#include "protokollitaja.h"
//#include "libxl.h"
//using namespace libxl;

/////////////////////////////////////////////////////////////////////////////
/// ToDo list:
/// Pooleli:
/// Print2() on pooleli, asendite pealkirju ei ole
/// Protokollitaja lehelugeja vajab uuendamist:
/// 1. Uue masinaga lugemine ei tööta?
/// 2. Sisekümneid ei märgistata
/// 3. Kll faili lugeja-kirjutaja testide tegemine
///
///
/////////////////////////////////////////////////////////////////////////////

extern QString organization;
extern QString programmiNimi;
extern QString versioon;
extern QString aasta;
extern QString argument;
extern bool verbose;
//extern QDir asukoht;

Protokollitaja::Protokollitaja(QWidget *parent)
    : QMainWindow(parent), m_settings(programmiNimi.mid(0, programmiNimi.indexOf(' ')), "Protokollitaja conf")
{
    setWindowTitle(programmiNimi); // TODO uuendada nime
    setWindowIcon(QIcon(":/images/Protokollitaja.ico"));

    prindiEelvaade = 0;
    protoUuendaja = 0;
    server = 0;
    siusDataSocket = 0;
//    socket = 0;
    blockSize = 0;
    uhendumiseAken = 0; //SiusDataga ühendumiseks
    lasuVSiusis = 10;   //Loetakse seadete failist üle - ei ole enam kasutuses
    lasuNrSiusis = 13;   //Loetakse seadete failist üle
    autoUuendus = true;

    if (m_settings.language().isEmpty()) {
        changeLanguage(true);
    }

    setupTranslator();

        udpSocket = new QUdpSocket(this);
        udpSocket->bind(45743, QUdpSocket::ShareAddress);
        connect(udpSocket, SIGNAL(readyRead()), this, SLOT(loeBroadcast()));

        uusAct = new QAction(QIcon(":/images/new.png"), tr("&Uus"), this);
        uusAct->setShortcut(tr("Ctrl+N"));
        uusAct->setStatusTip(tr("Loob uue faili või avab vastava faili kui sellenimeline on juba olemas"));
        connect(uusAct, SIGNAL(triggered()), this, SLOT(uus()));
        avaAct = new QAction(QIcon(":/images/open.png"), tr("&Ava..."), this);
        avaAct->setShortcut(tr("Ctrl+o"));
        avaAct->setStatusTip(tr("Ava fail"));
        connect(avaAct, SIGNAL(triggered()), this, SLOT(ava()));
        salvestaAct = new QAction(QIcon(":/images/save.png"), tr("&Salvesta"), this);
        salvestaAct->setShortcut(tr("Ctrl+s"));
        salvestaAct->setStatusTip(tr("Salvesta fail"));
        connect(salvestaAct, SIGNAL(triggered()), this, SLOT(salvesta()));
        salvestaKuiAct = new QAction(QIcon(":/images/save.png"), tr("&Salvesta nimega..."), this);
        salvestaKuiAct->setShortcut(tr("Ctrl+a"));
        salvestaKuiAct->setStatusTip(tr("Salvesta fail määratud nimega"));
        connect(salvestaKuiAct, SIGNAL(triggered()), this, SLOT(salvestaKui()));
        prindiAct = new QAction(QIcon(":/images/print.png"), tr("Print..."), this);
        prindiAct->setShortcut(tr("Ctrl+p"));
        prindiAct->setStatusTip(tr("Prindib selle lehe sisu"));
//#ifdef QT_DEBUG
//        connect(prindiAct, SIGNAL(triggered()), this, SLOT(prindi2()));
//#else
        connect(prindiAct, SIGNAL(triggered()), this, SLOT(prindi()));
//#endif
        eelvaadeAct = new QAction(QIcon(":/images/print.png"), tr("Lehekülje eelvaade"), this);
        eelvaadeAct->setStatusTip(tr("Näitab selle lehekülje eelvaate"));
        connect(eelvaadeAct, SIGNAL(triggered()), this, SLOT(eelvaade()));
        valjuAct = new QAction(QIcon(":/images/exit.png"), tr("&Välju"), this);
        valjuAct->setShortcut(tr("Ctrl+q"));
        valjuAct->setStatusTip(tr("Välju programmist"));
        connect(valjuAct, SIGNAL(triggered()), this, SLOT(close()));
        uusLaskurAct = new QAction(QIcon(":/images/uusLaskur.png"), tr("Uus &laskur"), this);
        uusLaskurAct->setShortcut(tr("Ctrl+u"));
        uusLaskurAct->setStatusTip(tr("Loob uue laskuri rea"));
        connect(uusLaskurAct, SIGNAL(triggered()), this, SLOT(uusLaskur()));
        liigutaAct = new QAction(QIcon(":/images/teisaldaLaskur.png"), tr("&Teisalda valitud laskurid"), this);
        liigutaAct->setStatusTip(tr("Viib valitud laskurite read teisele töölehele"));
        connect(liigutaAct, SIGNAL(triggered()), this, SLOT(liiguta()));
        eemaldaLaskurAct = new QAction(QIcon(":/images/eemaldaLaskur.png"), tr("&Eemalda valitud laskurid"), this);
        eemaldaLaskurAct->setStatusTip(tr("Eemaldab valitud laskurite read"));
        connect(eemaldaLaskurAct, SIGNAL(triggered()), this, SLOT(eemaldaLaskur()));
        uusTabAct = new QAction(QIcon(":/images/uusTab.png"), tr("Uus &tööleht"), this);
        uusTabAct->setStatusTip(tr("Loob uue lehe"));
        connect(uusTabAct, SIGNAL(triggered()), this, SLOT(uusTab()));
        eemaldaTabAct = new QAction(QIcon(":/images/eemaldaTab.png"), tr("Eemalda see tööleht"), this);
        eemaldaTabAct->setStatusTip(tr("Eemaldab aktiivse töölehe"));
        connect(eemaldaTabAct, SIGNAL(triggered()), this, SLOT(eemaldaTab()));
        naitaTulAkenAct = new QAction(QIcon(":/images/tulemuse_aken.png"), tr("Tulemuste aken"), this);
        naitaTulAkenAct->setStatusTip(tr("Avab tulemuste akna"));
        connect(naitaTulAkenAct, SIGNAL(triggered()), this, SLOT(naitaTulAken()));
        importAct = new QAction(QIcon(":/images/import.png"), tr("Import..."), this);
        importAct->setStatusTip(tr("Impordib sellele lehele tulemused .txt või .csv failist"));
        connect(importAct, SIGNAL(triggered()), this, SLOT(import()));
        eksportTXTAct = new QAction(QIcon(":/images/eksport.png"), tr("Eksport txt..."), this);
        eksportTXTAct->setStatusTip(tr("Ekspordib selle lehe tulemused .txt faili"));
        connect(eksportTXTAct, SIGNAL(triggered()), this, SLOT(eksportTXT()));
        eksportCSVAct = new QAction(QIcon(":/images/eksport.png"), tr("Eksport csv..."), this);
        eksportCSVAct->setStatusTip(tr("Ekspordib selle lehe tulemused .csv faili"));
        connect(eksportCSVAct, SIGNAL(triggered()), this, SLOT(eksportCSV()));
//        eksportInbandStartListAct = new QAction(tr("Eksport Inband startlist..."), this); // There is no reason to use this anymore
//        eksportInbandStartListAct->setStatusTip(tr("Ekspordib valitud laskurid Inband'i startlisti faili"));
//        connect(eksportInbandStartListAct, &QAction::triggered, [this]{exportStartList(StartListWriter::INBAND);});
        eksportSiusStartListAct = new QAction(tr("Eksport Sius startlist..."), this);
        eksportSiusStartListAct->setStatusTip(tr("Ekspordib valitud laskurid Sius'i startlisti faili"));
        connect(eksportSiusStartListAct, &QAction::triggered, [this]{exportStartList(StartListWriter::SIUS);});
        eksportFSiusStartListAct = new QAction(tr("Eksport finaali Sius startlist..."), this);
        eksportFSiusStartListAct->setStatusTip(tr("Ekspordib esimesed 8 laskurit Sius'i startlisti faili"));
        connect(eksportFSiusStartListAct, SIGNAL(triggered()), this, SLOT(exportFinalsSiusStartList()));
        eksportXLSAct = new QAction(QIcon(":/images/eksport.png"), tr("Eksport xls..."), this);
        eksportXLSAct->setStatusTip(tr("Ekspordib selle lehe tulemused Excel'i .xls faili"));
        connect(eksportXLSAct, SIGNAL(triggered()), this, SLOT(eksportXLS()));
        kaivitaServerAct = new QAction(tr("Käivita server lehtede lugemiseks"), this);
        kaivitaServerAct->setStatusTip(tr("Käivitab serveri, et saaks teises arvutis lehti lugeda"));
        connect(kaivitaServerAct, SIGNAL(triggered()), this, SLOT(kaivitaServer()));
        kontrolliUuendusiAct = new QAction(tr("Kontrolli uuendusi"), this);
        kontrolliUuendusiAct->setStatusTip(tr("Kontrollib Protokollitaja programmi uuenduste olemasolu internetist"));
        connect(kontrolliUuendusiAct, SIGNAL(triggered()), this, SLOT(checkForUpdates()));
        kopeeriLaskuridAct = new QAction(QIcon(":/images/kopeeriLaskur.png"), tr("Kopeeri valitud laskurid..."), this);
        kopeeriLaskuridAct->setStatusTip(tr("Kopeerib valitud laskurid teisele lehele"));
        connect(kopeeriLaskuridAct, SIGNAL(triggered()), this, SLOT(kopeeriLaskurid()));
        kopeeriVahAct = new QAction(tr("Kõik vahemällu"), this);
        kopeeriVahAct->setStatusTip(tr("Kopeerib selle lehe laskurid vahemällu"));
        connect(kopeeriVahAct, SIGNAL(triggered()), this, SLOT(kopeeriVah()));
        kopeeriValitudVahAct = new QAction(tr("Valitud vahemällu"), this);
        kopeeriValitudVahAct->setStatusTip(tr("Kopeerib selle lehe valitud laskurid vahemällu"));
        connect(kopeeriValitudVahAct, SIGNAL(triggered()), this, SLOT(kopeeriValitudVah()));
        naitaSeadedAct = new QAction(tr("Seaded"), this);
        naitaSeadedAct->setStatusTip(tr("Avab seadete akna"));
        connect(naitaSeadedAct, SIGNAL(triggered()), this, SLOT(naitaSeaded()));
        reastaAct = new QAction(QIcon(":/images/reasta.png"), tr("Sorteeri laskurid"), this);
        reastaAct->setStatusTip(tr("Sorteerib laskurid sellel lehel"));
        connect(reastaAct, SIGNAL(triggered()), this, SLOT(reasta()));
        reastaPAct = new QAction(tr("Sorteeri perekonnanimede järgi"), this);
        reastaPAct->setStatusTip(tr("Sorteerib laskurid sellel lehel perekonnanimede järgi"));
        connect(reastaPAct, SIGNAL(triggered()), this, SLOT(reastaP()));
        reastaSAct = new QAction(tr("Sorteeri sünniaasta järgi"), this);
        reastaSAct->setStatusTip(tr("Sorteerib laskurid sellel lehel sünniaasta järgi"));
        connect(reastaSAct, SIGNAL(triggered()), this, SLOT(reastaS()));
        reastaRAct = new QAction(tr("Sorteeri rajanumbri järgi"), this);
        reastaRAct->setStatusTip(tr("Sorteerib laskurid sellel lehel rajanumbri järgi"));
        connect(reastaRAct, SIGNAL(triggered()), this, SLOT(reastaR()));
        reastaSiAct = new QAction(tr("Sorteeri sifri järgi"), this);
        reastaSiAct->setStatusTip(tr("Sorteerib laskurid sellel lehel sifri järgi"));
        connect(reastaSiAct, SIGNAL(triggered()), this, SLOT(reastaSi()));
        margiKoikAct = new QAction(QIcon(":/images/margista.png"), tr("Märgi kõik"), this);
        margiKoikAct->setStatusTip(tr("Paneb linnukese kõigile laskuritele"));
        connect(margiKoikAct, SIGNAL(triggered()), this, SLOT(margi()));
        eemaldaMargidAct = new QAction(QIcon(":/images/eemaldaMargid.png"), tr("Eemalda märgistused"), this);
        eemaldaMargidAct->setStatusTip(tr("Eemaldab kõigilt laskuritelt linnukesed"));
        connect(eemaldaMargidAct, SIGNAL(triggered()), this, SLOT(eemaldaMargid()));
        taiendaBaasAct = new QAction(tr("Täienda andmebaasi"), this);
        taiendaBaasAct->setStatusTip(tr("Lisab uued laskurid andmebaasi"));
        connect(taiendaBaasAct, SIGNAL(triggered()), this, SLOT(taiendaAndmebaas()));

        finaalAct = new QAction(QIcon(":/images/finaal.png"), tr("Finaal"), this);
        finaalAct->setStatusTip(tr("Käivitab selle lehe andmetega Finaali programmi"));
        connect(finaalAct, SIGNAL(triggered()), this, SLOT(runFinals()));

        programmistAct = new QAction(tr("Programmist"), this);
        programmistAct->setStatusTip(tr("Kuvab infot programmi kohta"));
        connect(programmistAct, SIGNAL(triggered()), this, SLOT(naitaInfot()));

        naitaWAbiAct = new QAction(tr("Abi"), this);
        naitaWAbiAct->setStatusTip(tr("Näitab abi, kuidas kahe ekraani jaoks seadistada"));
        connect(naitaWAbiAct, SIGNAL(triggered()), this, SLOT(naitaWAbi()));

        QAction *changeLanguageAct = new QAction(tr("Programmi keel"), this);
        changeLanguageAct->setStatusTip(tr("Programmi keele valik"));
        connect(changeLanguageAct, &QAction::triggered, [this](){
            changeLanguage(false);
        });

        impordiFinaalAct = new QAction(tr("Impordi finaal..."), this);
        impordiFinaalAct->setStatusTip(tr("Lisab finaali tulemused vastavast failist"));
        connect(impordiFinaalAct, SIGNAL(triggered()), this, SLOT(impordiFinaal()));

        finaaliFailAct = new QAction(tr("Finaali fail..."), this);
        finaaliFailAct->setStatusTip(tr("Loob selle lehe kohta finaali faili"));
        connect(finaaliFailAct, SIGNAL(triggered()), this, SLOT(exportFinalsFile()));

        lehelugejaAct = new QAction(tr("Loe lehti..."), this);
        lehelugejaAct->setStatusTip(tr("Lehtede lugemine otse masinast"));
        connect(lehelugejaAct, SIGNAL(triggered()), this, SLOT(lehelugeja()));

        uhenduSiusDatagaAct = new QAction(("SiusData/Range Control"), this);
        uhenduSiusDatagaAct->setStatusTip(tr("Ühendub SiusDataga"));
        connect(uhenduSiusDatagaAct, SIGNAL(triggered()), this, SLOT(uhenduSiusDataga()));

        sendCompetitorsToRangeAct = new QAction(tr("Saada valitud Range Control'i"), this);
        sendCompetitorsToRangeAct->setStatusTip(tr("Saadab valitud laskurid Range Control'i"));
        connect(sendCompetitorsToRangeAct, SIGNAL(triggered()), this, SLOT(sendCompetitorsToRange()));

        uploadAct = new QAction(tr("Lae veebi"), this);
        uploadAct->setStatusTip(tr("Laeb tulemused veebi"));
        connect(uploadAct, SIGNAL(triggered()), this, SLOT(uploadResults()));

        sifriLisaAct = new QAction(tr("Lisa sifrid..."), this);
        sifriLisaAct->setStatusTip(tr("Sifrite lisamine valitud laskuritele"));
        connect(sifriLisaAct, SIGNAL(triggered()), this, SLOT(sifriLisa()));

        seiskaServerAct = new QAction(tr("Seiska server"), this);
        seiskaServerAct->setStatusTip(tr("Seiskab lehtede lugemise serveri"));
        connect(seiskaServerAct, SIGNAL(triggered()), this, SLOT(seiskaServer()));

#ifdef QT_DEBUG
        deleteAllShotsAct = new QAction(tr("Kustuta lasud"), this);
        deleteAllShotsAct->setStatusTip(tr("Kustutab märgitud laskurite kõik lasud"));
        connect(deleteAllShotsAct, SIGNAL(triggered()), this, SLOT(deleteAllShots()));
#endif

        failMenu = menuBar()->addMenu(tr("&Fail"));
        failMenu->addAction(uusAct);
        failMenu->addAction(avaAct);
        failMenu->addAction(salvestaAct);
        failMenu->addAction(salvestaKuiAct);
        failMenu->addSeparator();
        failMenu->addAction(prindiAct);
//        failMenu->addAction(eelvaadeAct); //See ei toimi õigesti, seega ei ole mõtet menüüsse lisada
        failMenu->addSeparator();
        failMenu->addAction(valjuAct);

        tooriistadMenu = menuBar()->addMenu(tr("&Tööriistad"));
        tooriistadMenu->addAction(uusLaskurAct);
        tooriistadMenu->addAction(kopeeriLaskuridAct);
        tooriistadMenu->addAction(liigutaAct);
        tooriistadMenu->addAction(eemaldaLaskurAct);
        tooriistadMenu->addSeparator();
        tooriistadMenu->addAction(uusTabAct);
        tooriistadMenu->addAction(eemaldaTabAct);
        tooriistadMenu->addSeparator();
        tooriistadMenu->addAction(reastaAct);
        tooriistadMenu->addAction(reastaPAct);
        tooriistadMenu->addAction(reastaSAct);
        tooriistadMenu->addAction(reastaRAct);
        tooriistadMenu->addAction(reastaSiAct);
        tooriistadMenu->addSeparator();
        tooriistadMenu->addAction(margiKoikAct);
        tooriistadMenu->addAction(eemaldaMargidAct);
        tooriistadMenu->addSeparator();
        tooriistadMenu->addAction(sifriLisaAct);
        tooriistadMenu->addSeparator();
        tooriistadMenu->addAction(taiendaBaasAct);
        tooriistadMenu->addSeparator();
        tooriistadMenu->addAction(naitaSeadedAct);
        tooriistadMenu->addSeparator();
        tooriistadMenu->addAction(kaivitaServerAct);
        tooriistadMenu->addAction(seiskaServerAct);
        tooriistadMenu->addSeparator();
        tooriistadMenu->addAction(uhenduSiusDatagaAct);
        tooriistadMenu->addAction(sendCompetitorsToRangeAct);

        tulemusedMenu = menuBar()->addMenu(tr("T&ulemused"));
        tulemusedMenu->addAction(importAct);
        tulemusedMenu->addSeparator();
        tulemusedMenu->addAction(uploadAct);
        tulemusedMenu->addAction(eksportTXTAct);
        tulemusedMenu->addAction(eksportCSVAct);
        tulemusedMenu->addAction(eksportXLSAct);
        tulemusedMenu->addSeparator();
//        tulemusedMenu->addAction(eksportInbandStartListAct);
        tulemusedMenu->addAction(eksportSiusStartListAct);
        tulemusedMenu->addAction(eksportFSiusStartListAct);
        tulemusedMenu->addSeparator();
        tulemusedMenu->addAction(kopeeriVahAct);
        tulemusedMenu->addAction(kopeeriValitudVahAct);
        tulemusedMenu->addSeparator();
//        tulemusedMenu->addAction(lehelugejaAct);  // Not tested lately and probably not working properly
//        tulemusedMenu->addSeparator();
        tulemusedMenu->addAction(naitaTulAkenAct);
        tulemusedMenu->addAction(finaalAct);
        tulemusedMenu->addAction(finaaliFailAct);
        tulemusedMenu->addAction(impordiFinaalAct);

        abiMenu = menuBar()->addMenu(tr("&Abi"));
        abiMenu->addAction(naitaWAbiAct);
        abiMenu->addAction(changeLanguageAct);
        abiMenu->addAction(programmistAct);
        abiMenu->addAction(kontrolliUuendusiAct);

#ifdef QT_DEBUG
        QMenu *testMenu = menuBar()->addMenu(tr("&Testimine"));
        testMenu->addAction(deleteAllShotsAct);
#endif

        (void) new QShortcut(Qt::Key_Escape, this, SLOT(close()));
        setStatusBar(statusBar());

        toolBar = addToolBar(tr("failMenu"));
        toolBar->addAction(uusAct);
        toolBar->addAction(avaAct);
        toolBar->addAction(salvestaAct);
        toolBar->addAction(prindiAct);

        toolBar = addToolBar(tr("tooRiistad"));
        toolBar->addAction(uusLaskurAct);
        toolBar->addAction(eemaldaLaskurAct);
        toolBar->addAction(liigutaAct);
        toolBar->addAction(kopeeriLaskuridAct);
        toolBar->addAction(reastaAct);
        toolBar->addAction(margiKoikAct);
        toolBar->addAction(eemaldaMargidAct);
        toolBar->addAction(uusTabAct);
        toolBar->addAction(eemaldaTabAct);

        toolBar = addToolBar(tr("tooRiistad2"));
        toolBar->addAction(lehelugejaAct);
        toolBar->addAction(uhenduSiusDatagaAct);
        toolBar->addAction(naitaTulAkenAct);
        toolBar->addAction(finaalAct);

        aValik = new InitialDialog(this);
        aValik->setWindowModality(Qt::ApplicationModal);
        aValik->setWindowIcon(QIcon(":/images/Protokollitaja.ico"));
        valik = new ValikKast(this);
        valik->setWindowIcon(QIcon(":/images/Protokollitaja.ico"));
        valik->setWindowModality(Qt::ApplicationModal);
        tulemus = new TulemuseAken();
        tulemus->setWindowIcon(QIcon(":/images/Protokollitaja.ico"));
        connect(tulemus, SIGNAL(naitaJargmist()), this, SLOT(naitaTul()));
        importAken = new ImportAken(this);
        importAken->setWindowIcon(QIcon(":/images/Protokollitaja.ico"));
        lValik = new LiikmeteValikKast(this);
        connect(lValik->ui.leheBox, SIGNAL(currentIndexChanged(int)), this, SLOT(uuendaLiikmeteNimekirja(int)));
        connect(lValik->ui.lisaNupp, SIGNAL(clicked()), this, SLOT(uuendaJalgitavaid()));
        lehelugejaAken = 0;
        sifriLisaAken = new SifriSeade(this);

        progress = 0;

        progressTimer = new QTimer(this);
        progressTimer->setSingleShot(true);
        progressTimer->setInterval(3000);
        connect(progressTimer, SIGNAL(timeout()), this, SLOT(peataProgress()));

        sulgeja = new QTimer(this);
        connect(sulgeja, SIGNAL(timeout()), this, SLOT(sulge()));
        sulgeja->setSingleShot(true);

        naitaja = new QTimer(this);
        connect(naitaja, SIGNAL(timeout()), this, SLOT(naitaTul()));
        naitaja->setInterval(10000);

        viiLopuni = new QTimer(this);
        viiLopuni->setSingleShot(true);
        viiLopuni->setInterval(500);
        connect(viiLopuni, SIGNAL(timeout()), this, SLOT(viiLoppu()));

        salvestaja = new QTimer(this);
        connect(salvestaja, SIGNAL(timeout()), this, SLOT(autosave()));
        salvestaja->setInterval(300000);

        uploadTimer.setInterval(60000);
        connect(&uploadTimer, &QTimer::timeout, this, &Protokollitaja::uploadResults);

        voistkondadele = new QTimer(this);  //Kasutatakse võistkondade andmete uuendamiseks, peale liikmete valiku kasti sulgemist
        voistkondadele->setSingleShot(true);
        voistkondadele->setInterval(50);
        connect(voistkondadele, SIGNAL(timeout()), this, SLOT(uuendaVoistkondi()));
        connect(lValik, SIGNAL(accepted()), voistkondadele, SLOT(start()));

        tabWidget = new QTabWidget(this);
        tabWidget->setTabPosition(QTabWidget::West);
        setCentralWidget(tabWidget);

        seaded = new SeadedKast(m_competitionName, m_place, this);
        seaded->setWindowModality(Qt::ApplicationModal);
        connect(seaded, SIGNAL(salvestatud()), this, SLOT(uuendaSeaded()));
        connect(seaded->ui.sakid, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(muudaTab(const QModelIndex&)));

        pilt = new QPixmap(1600, 2450);
        pilt2 = new QPixmap(2450, 1600);

        algseaded();    //Seadistab algsed väärtused
        readSettings();

        QFile fail(qApp->applicationDirPath() + "/Data/Laskuritenimekiri Puss.txt");
        if(fail.open(QIODevice::ReadOnly | QIODevice::Text)){
                QTextStream sisse(&fail);
//                sisse.setCodec("UTF-8");
                while(!sisse.atEnd()){
                        QString str;
                        QStringList list;
                        str = sisse.readLine();
                        list = str.split(";");
                        if(list.count() < 2){
                            QMessageBox::warning(this, "Protokollitaja", tr("Püssilaskurite nimekirjas on tühi rida!"), QMessageBox::Ok);
                            continue;
                        }
                        andmebaas.nimekiriPuss << new Andmebaas::LaskuriNimi;
                        andmebaas.nimekiriPuss[andmebaas.nimekiriPuss.count()-1]->eesnimi = list.takeFirst().trimmed();
                        if(andmebaas.nimekiriPuss.isEmpty()){
                                QMessageBox::critical(this, "Protokollitaja", tr("Püssilaskurite failis on %1. rida vigane!").arg(andmebaas.nimekiriPuss.count()), QMessageBox::Ok);
                                return;
                        }
                        andmebaas.nimekiriPuss[andmebaas.nimekiriPuss.count()-1]->perekonnanimi = list.takeFirst().trimmed();
                        if(andmebaas.nimekiriPuss.isEmpty()){
                                QMessageBox::critical(this, "Protokollitaja", tr("Püssilaskurite failis on %1. rida vigane!").arg(andmebaas.nimekiriPuss.count()), QMessageBox::Ok);
                                return;
                        }
                        andmebaas.nimekiriPuss[andmebaas.nimekiriPuss.count()-1]->sunniaasta = list.takeFirst().trimmed();
                        if(andmebaas.nimekiriPuss.isEmpty()){
                                QMessageBox::critical(this, "Protokollitaja", tr("Püssilaskurite failis on %1. rida vigane!").arg(andmebaas.nimekiriPuss.count()), QMessageBox::Ok);
                                return;
                        }
                        andmebaas.nimekiriPuss[andmebaas.nimekiriPuss.count()-1]->klubi = list.takeFirst().trimmed();
                }
                andmebaas.kirjutusabiPuss = true;
                fail.close();
                if(verbose)
                    QTextStream(stdout) << "Püssilaskurite nimekiri loetud, leitud " << andmebaas.nimekiriPuss.size() << " rida" << Qt::endl;
        }else{
                QMessageBox::warning(this, tr("Hoiatus"), tr("Püssilaskurite faili ei leitud! Püssilaskurite kirjutusabi pole võimaldatud"), QMessageBox::Ok);
                andmebaas.kirjutusabiPuss = false;
        }

        fail.setFileName(qApp->applicationDirPath() + "/Data/Laskuritenimekiri Pustol.txt");
        if(fail.open(QIODevice::ReadOnly | QIODevice::Text)){
                QTextStream sisse(&fail);
//                sisse.setCodec("UTF-8");
                while(!sisse.atEnd()){
                        QString str;
                        QStringList list;
                        str = sisse.readLine();
                        list = str.split(";");
                        if(list.count() < 2){
                            QMessageBox::warning(this, "Protokollitaja", tr("Püstolilaskurite nimekirjas on tühi rida!"), QMessageBox::Ok);
                            continue;
                        }
                        andmebaas.nimekiriPustol << new Andmebaas::LaskuriNimi;
                        andmebaas.nimekiriPustol[andmebaas.nimekiriPustol.count()-1]->eesnimi = list.takeFirst().trimmed();
                        if(andmebaas.nimekiriPustol.isEmpty()){
                                QMessageBox::critical(this, "Protokollitaja", tr("Püstolilaskurite failis on %1. rida vigane!").arg(andmebaas.nimekiriPustol.count()), QMessageBox::Ok);
                                return; // FIXME kas on õige siin ja järgmises kahes if'is return'ida?
                        }
                        andmebaas.nimekiriPustol[andmebaas.nimekiriPustol.count()-1]->perekonnanimi = list.takeFirst().trimmed();
                        if(andmebaas.nimekiriPustol.isEmpty()){
                                QMessageBox::critical(this, "Protokollitaja", tr("Püstolilaskurite failis on %1. rida vigane!").arg(andmebaas.nimekiriPustol.count()), QMessageBox::Ok);
                                return;
                        }
                        andmebaas.nimekiriPustol[andmebaas.nimekiriPustol.count()-1]->sunniaasta = list.takeFirst().trimmed();
                        if(andmebaas.nimekiriPustol.isEmpty()){
                                QMessageBox::critical(this, "Protokollitaja", tr("Püstolilaskurite failis on %1. rida vigane!").arg(andmebaas.nimekiriPustol.count()), QMessageBox::Ok);
                                return;
                        }
                        andmebaas.nimekiriPustol[andmebaas.nimekiriPustol.count()-1]->klubi = list.takeFirst().trimmed();
                }
                andmebaas.kirjutusabiPustol = true;
                fail.close();
                if(verbose)
                        QTextStream(stdout) << "Püstolilaskurite nimekiri loetud, leitud " << andmebaas.nimekiriPustol.size() << " rida" << Qt::endl;
        }else{
                QMessageBox::warning(this, tr("Hoiatus"), tr("Püstolilaskurite faili ei leitud! Püstolilaskurite kirjutusabi pole võimaldatud"), QMessageBox::Ok);
                andmebaas.kirjutusabiPustol = false;
        }

#ifdef Q_OS_WIN
#if QT_VERSION >= 0x050000
        //this->show();
        this->showMaximized();
#else
        this->showMaximized();
#endif
#else
        this->showMaximized();
#endif

#ifdef QT_DEBUG
    qDebug() << "Vahepunkt1" << " Salvestaja aeg: " << salvestaja->interval();
        QMessageBox::information(this, tr("Teade"), tr("Debug versioon!"), QMessageBox::Ok);
#endif

        argument = qApp->arguments().last();
        if(argument.endsWith(".kll", Qt::CaseInsensitive) /*!argument.isEmpty() && !argument.endsWith(".exe") && !argument.endsWith("Protokollitaja", Qt::CaseInsensitive)*/){
            //qDebug() << "Protokollitaja().arguments().last()";
            seeFail = argument;
            setWindowTitle(programmiNimi + " - " + seeFail);
            valik->ui.indBox->setCurrentIndex(0);
            loefail();
            tulemus->voistluseNimi = m_competitionName;
            writeSettings();
            voibSulgeda = true;
        }else{
            if(aValik->exec() == QDialog::Accepted){
                setDataFromInitialDialog(); // FIXME not working, as will be later overwritten by data read from file
                        QString logsDir = QDir(QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).filePath("Protokollitaja")).filePath("Protokollitaja logs");
                        QDir dir;
                        dir.mkpath(logsDir);

                        QString logFileName = QString("Protokollitaja log %1.log").arg(QDate::currentDate().toString(Qt::ISODate));
                logi = new QFile(QDir(logsDir).filePath(logFileName));
                kaivitaServer();

                if(logi->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){ //Muudatuste ja laskude logifail
                    logiValja.setDevice(logi);
                    logiValja << "/////////////////////" << m_competitionName << ", " << QDateTime::currentDateTime().toString() << ", " << programmiNimi << " (" << versioon <<  ")/////////////////////\n";
                }else{
                    QMessageBox::warning(this, tr("Hoiatus"), tr("Logi faili kirjutamine ei õnnestunud! Kontrollige, et teil oleks kirjutamisõigus sinna kausta, kus asub võistluste fail."), QMessageBox::Ok);
                }
            }else{
#ifdef QT_DEBUG
                qDebug() << "aValik declined";
#endif
                m_competitionName = "ProtokollitajaKood::Väljumine<Välju>return code 0021947";
                sulgeja->start(50);
                return;
            }

            if(m_competitionName != "ProtokollitajaKood::Väljumine<Välju>return code 0021947" && !m_competitionName.isEmpty()){
                loefail();
                setDataFromInitialDialog(); // A hack to implement changes from initial dialog
                tulemus->voistluseNimi = m_competitionName;
            }
        }
        if(m_competitionName.isEmpty()){
            sulgeja->start(50);
            return;
        }
#ifdef QT_DEBUG
        qDebug() << "Vahepunkt2; " << QString((QChar)10).length();
#endif

        checkForUpdates(true);

        tulemus->joonista();

        if (QGuiApplication::screens().size() >= 2)
            naitaTulAken();
}

void Protokollitaja::algseaded()    //Seadistab algsed väärtused kas programmi käivitamisel või uue faili tegemisel
{
    voibSulgeda = true;
    kasNaidataTul = true;
    voibUuendadaNimekirja = true;
    m_competitionName.clear();
    m_startDate = QDate::currentDate();
    m_endDate = QDate::currentDate();
    m_place.clear();
    kirjutusAbi = true;
    abi = TargetTypes::TargetType::AirRifle;
    m_ranking = KumneteArvuga;
    laskuriId = 0;
    leheIndeks = 0;
    lehelugejaLeht = 0;
    lehelugejaLaskur = 0;
//    vorguLeht = 0;
//    vorguLaskur = 0;
    uhendusAutoriseeritud = false;

    salvestaja->setInterval(300000);

    seaded->ui.voistluseNimi->setText(m_competitionName);
    seaded->ui.startDateEdit->setDate(m_startDate);
    seaded->ui.endDateEdit->setDate(m_endDate);
//    seaded->voistluseNimi = m_competitionName;
    seaded->ui.kohtEdit->setText(m_place);
//    seaded->aegKoht = aegKoht;
    seaded->ui.sakiBox->setCurrentIndex(0);
//    seaded->sakiAsukoht = 0;
    seaded->ui.aegCombo->setCurrentIndex(1);
//    seaded->autosave = seaded->ui.aegCombo->currentIndex();
    seaded->ui.aegEdit->setValue(salvestaja->interval() / 60000);
//    seaded->aeg = seaded->ui.aegEdit->value();
    seaded->ui.kirjutusAbiCombo->setCurrentIndex(1);
//    seaded->kirjutusAbi = seaded->ui.kirjutusAbiCombo->currentIndex();
    seaded->ui.jarjestamiseBox->setCurrentIndex(m_ranking);

    seaded->ui.sakid->clear();
    seaded->ui.competitionTypesEdit->setText(m_settings.competitionShotTypesString());
    seaded->ui.sighterTypesEdit->setText(m_settings.sighterShotTypesString());
}

void Protokollitaja::autosave()
{
    if(verbose)
        QTextStream(stdout) << "Protokollitaja::autosave(), seeFail: " << seeFail << ", salvestaja->interval(): " << salvestaja->interval() << Qt::endl;

    if (salvestaja->interval() < 60000)  // TO avoid too frequent saves
        salvestaja->setInterval(300000);

    if(seeFail.isEmpty())
        return;
    QString failiNimi = seeFail;
    if(failiNimi.endsWith(".kll"))
        failiNimi.chop(4);
    failiNimi.append("_autosave.kll");
    kirjutaFail(failiNimi);

        /*
        }else QMessageBox::critical(this, "Protokollitaja", tr("Ei õnnestu autosave faili luua. Varukoopia "
                        "tegemine on võimatu"), QMessageBox::Ok);*/
}

void Protokollitaja::ava()
{
        if(!voibSulgeda){
        int vastus = QMessageBox::question(this, "Protokollitaja", tr("Kas soovid muudatused salvestada?"),
                        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
                if(vastus == QMessageBox::Save)	salvesta();
                else if(vastus == QMessageBox::Cancel) return;
        }
        QString failiNimi = QFileDialog::getOpenFileName(this, tr("Ava fail"), seeFail, tr("Protokollitaja fail (*.kll)"));
        if(failiNimi.isEmpty()) return;
        for(int i = 0; i < tabWidget->count(); i++)
                tabWidget->widget(i)->deleteLater();
        seeFail = failiNimi;
        setWindowTitle(programmiNimi + " - " + seeFail);
        valik->ui.indBox->setCurrentIndex(0);
        loefail();
        writeSettings();
        voibSulgeda = true;
}

void Protokollitaja::changeLanguage(bool atStartup)
{
    QDir dir(":/protokollitaja/languages/");
    QStringList fileNames = dir.entryList(QStringList("*.qm"));
    QStringList languages;
    for (QString language : fileNames) {
        languages.append(language.remove(".qm"));
    }
    bool ok = false;
    QString newLanguage = QInputDialog::getItem(this, "Choose a language", "Language:", languages, languages.indexOf(m_settings.language()), false, &ok);

    if (ok) {
        m_settings.setLanguage(newLanguage);
        m_settings.writeSettings();

        setupTranslator();

        if (!atStartup)
            QMessageBox::information(this, tr("Teade"), tr("Keele vahetus rakendub programmi uuesti käivitamisel"));
    }
}

void Protokollitaja::checkForUpdates()
{
    checkForUpdates(false);
}

void Protokollitaja::checkForUpdates(bool autoCheck)
{
    autoUuendus = autoCheck;
    UpdateChecker *checker = new UpdateChecker(versioon, &logiValja);
    connect(checker, &UpdateChecker::versionInfoResponse, this, &Protokollitaja::receivedVersionInfo);

    checker->getLatestVersionInfo("ymeramees", "protokollitaja");
}

void Protokollitaja::closeEvent(QCloseEvent *event)
{
#ifdef QT_DEBUG
    qDebug() << "closeEvent()";
#endif
    if(voibSulgeda){
        if(protoUuendaja == 0)  //Kui on uuendamise kaudu sulgemine, siis ei ole vaja küsida, kui salvestama ei pea
            if(QMessageBox::warning(this, tr("Protokollitaja"), tr("Oled kindel, et soovid programmist väljuda?"), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok)==QMessageBox::Cancel)
                event->ignore();
    }else{
        int vastus = QMessageBox::question(this, "Protokollitaja", tr("Kas soovid muudatused salvestada ja programmist väljuda?"),	QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if(vastus == QMessageBox::Save){
            salvesta();
            //tulemus->close();
            //lehelugejaAken->sulgeUhendus();
            //qApp->quit();
            event->accept();
        }else if(vastus == QMessageBox::Cancel){
            event->ignore();
        }else{
            /*tulemus->close();
                lehelugejaAken->sulgeUhendus();
                qApp->quit();*/
            event->accept();
        }
    }
    if(event->isAccepted()){
        /*QFile logifail(asukoht.absolutePath() + QString("/Data/Logi %1.log").arg(QDate::currentDate().toString(Qt::ISODate)));
                if(logifail.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
                    QTextStream valja(&logifail);
                    valja << lehelugejaAken->m_ui.logi->toPlainText() << "\n";
                    logifail.close();
                }*/
        tulemus->close();
        if(lehelugejaAken)
            lehelugejaAken->sulgeUhendus();
        if(server != 0)
            server->closeConnections();
//                if(siusDataConnections != nullptr)
//                    siusDataConnections
    }
}

#ifdef QT_DEBUG
void Protokollitaja::deleteAllShots()
{
    Leht* seeLeht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->
                    widget());
    seeLeht->deleteAllShotsFromSelectedCompetitors();
}
#endif

void Protokollitaja::eelvaade()
{
        if(tabWidget->count() < 1) return;
        if(prindiEelvaade == 0)
                prindiEelvaade = new QLabel;
        if(tabWidget->count() > 0){
                Leht* seeLeht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->
                                widget());
                pilt->fill();
                QPainter painter(pilt);
                QFont vanaFont = this->font();
                QFont kirjaFont;
                QFont paiseFont;
                QFont pealkirjaFont;

                kirjaFont.setPointSize(24);
                kirjaFont.setBold(false);
                setFont(kirjaFont);
                paiseFont.setPointSize(20);
                paiseFont.setBold(false);
                pealkirjaFont.setPointSize(32);
                pealkirjaFont.setBold(true);

                painter.setFont(pealkirjaFont);
                painter.drawText(60, 50, m_competitionName);
                painter.setFont(paiseFont);
                painter.drawText(1345, 130, "Summa");
                if(!seeLeht->voistk)
                        painter.drawText(1100, 130, "Seeriad");
                painter.setFont(kirjaFont);
                painter.drawText(800, 50, 700, 50, Qt::AlignRight, timeAndPlaceString());
                kirjaFont.setBold(true);
                painter.setFont(kirjaFont);
                painter.drawText(75, 125, seeLeht->ekraaniNimi);
                kirjaFont.setBold(false);
                painter.setFont(kirjaFont);
                if(seeLeht->voistk){
                        int lopp = seeLeht->voistkonnad.count();
                        if(seeLeht->voistkonnad.count() > (56 / (seeLeht->seeriateArv + 1)))
                                lopp = 56 / (seeLeht->seeriateArv + 1);
                        int i = 0;
                        for(int alg = 0; alg < lopp; alg++){
                                painter.drawText(25, 175 + i * 41, QString("%1.").arg(alg+1));
                                painter.drawText(85, 175 + i * 41, seeLeht->voistkonnad[alg]->nimi->text());
                                for(int j = 0; j < seeLeht->seeriateArv; j++){
                                        if(j != 0) i++;
                                        painter.drawText(585, 175 + i * 41, seeLeht->voistkonnad[alg]->voistlejad[j]->
                                                        eesNimi);
                                        painter.drawText(800, 175 + i * 41, seeLeht->voistkonnad[alg]->voistlejad[j]->
                                                        perekNimi);
                                        painter.drawText(1100, 175 + i * 41, seeLeht->voistkonnad[alg]->voistlejad[j]->
                                                        summa);
                                }
                                painter.drawText(1375, 175 + i * 41, seeLeht->voistkonnad[alg]->summa->text());
                                if(!seeLeht->voistkonnad[alg]->markus->text().isEmpty())
                                        painter.drawText(1460, 175 + i * 41, seeLeht->voistkonnad[alg]->markus->text());
                        i += 2;
                        }
                }else{
                        for(int i = 0; i < seeLeht->laskurid.count() && i < 55; i++){
                                if(seeLeht->laskurid[i]->markus->text().contains("V.A", Qt::CaseInsensitive))
                                        painter.drawText(25, 175 + i * 41, "v.a.");
                                else painter.drawText(25, 175 + i * 41, QString("%1.").arg(i+1));
                                painter.drawText(85, 175 + i * 41, seeLeht->laskurid[i]->eesNimi->text());
                                painter.drawText(315, 175 + i * 41, seeLeht->laskurid[i]->perekNimi->text());
                                painter.drawText(585, 175 + i * 41, seeLeht->laskurid[i]->sunniAasta->text());
                                painter.drawText(685, 175 + i * 41, seeLeht->laskurid[i]->klubi->text());
                                for(int j = 0; j < seeLeht->laskurid[i]->seeriateArv; j++)
                                        painter.drawText(985 + j * 65, 175 + i * 41, seeLeht->laskurid[i]->seeriad[j]->text());
                                painter.drawText(1375, 175 + i * 41, seeLeht->laskurid[i]->getSumma());
                                if(!seeLeht->laskurid[i]->finaal->text().isEmpty()){
                                        painter.drawText(1450, 175 + i * 41, seeLeht->laskurid[i]->finaal->text());
//                                        kirjaFont.setBold(true);
//                                        painter.setFont(kirjaFont);
//                                        painter.drawText(1520, 175 + i * 41, seeLeht->laskurid[i]->koguSumma->text());
                                        kirjaFont.setBold(false);
                                        painter.setFont(kirjaFont);
                                }else if(!seeLeht->laskurid[i]->markus->text().isEmpty() &&
                                            !seeLeht->laskurid[i]->markus->text().contains("V.A", Qt::CaseInsensitive))
                                        painter.drawText(1460, 175 + i * 41, seeLeht->laskurid[i]->markus->text());
                        }
                }
                painter.setFont(vanaFont);
                this->setFont(vanaFont);
        }
        prindiEelvaade->setPixmap(*pilt);
        prindiEelvaade->setScaledContents(true);
        prindiEelvaade->resize(0.3 * prindiEelvaade->pixmap().size());
        prindiEelvaade->show();
}

void Protokollitaja::eemaldaLaskur()
{
        if(tabWidget->count() > 0){
                dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>
                        (tabWidget->currentWidget())->widget())->eemaldaLaskur();
                voibSulgeda = false;
        }
}

void Protokollitaja::eemaldaMargid()
{
        if(tabWidget->count() < 1) return;
        if(tabWidget->count() > 0){
                Leht* seeLeht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->
                                widget());
                if(!seeLeht->laskurid.isEmpty())
                        for(int i = 0; i < seeLeht->laskurid.count(); i++)
                                seeLeht->laskurid[i]->linnuke->setChecked(false);
                if(!seeLeht->voistkonnad.isEmpty())
                        for(int i = 0; i < seeLeht->voistkonnad.count(); i++)
                                seeLeht->voistkonnad[i]->linnuke->setChecked(false);
        }
}

void Protokollitaja::eemaldaTab()
{
        if(tabWidget->count() > 0){
                Leht* leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
                bool jalgitakse = false;
                for(int i = 0; i < tabWidget->count(); i++){
                        Leht* leht2 = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->
                                        widget());
                        for(int j = 0; j < leht2->jalgitavad.count(); j++){
                                if(leht->leheIndeks == leht2->jalgitavad[j])
                                        jalgitakse = true;
                        }
                }
                if(jalgitakse){
                        QMessageBox::warning(this, "Protokollitaja", tr("Seda lehte ei ole võimalik kustutada, kuna "
                                        "selle järgi on tehtud võistkondlik arvestus. Kustutage enne võistkondlik leht.")
                                        , QMessageBox::Ok);
                        return;
                }
                if(QMessageBox::warning(this, tr("Küsimus"), tr("Oled sa kindel, et tahad selle töölehe kustutada? Kui sa "
                                "seda teed, siis kaotad kõik andmed, mis sellel on!"), QMessageBox::Yes | QMessageBox::No)
                                == QMessageBox::Yes){
                        tabWidget->removeTab(tabWidget->currentIndex());
                        leht->deleteLater();
                }
        }
        if(tabWidget->count() < 1)
                valik->ui.indBox->setCurrentIndex(0);
}

void Protokollitaja::eksportCSV()
{
        Leht* leht = 0;
        if(tabWidget->count() > 0)
            leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
        else{
            QMessageBox::critical(this, tr("Viga"), tr("Töölehed puuduvad, ei ole midagi eksportida!"), QMessageBox::Ok);
            return;
        }
        QString failiNimi = QFileDialog::getSaveFileName(this, tr("Ekspordi"), seeFail.left(seeFail.length() - 3),
                        tr("Comma separated file (*.csv)"));
        if(failiNimi.isEmpty()) return;
        if(!failiNimi.endsWith(".csv"))
                failiNimi.append(".csv");
        QFile fail(failiNimi);
        if(fail.open(QIODevice::WriteOnly | QIODevice::Text)){
                QTextStream valja(&fail);
                if(leht->voistk){
                        for(int i = 0; i < leht->voistkonnad.count(); i++){
                                valja << QString("%1.;").arg(i+1) << leht->voistkonnad[i]->nimi->text() << ";";
                                for(int j = 0; j < leht->seeriateArv; j++){
                                        if(j != 0)
                                                valja << "\n;;";
                                        valja << leht->voistkonnad[i]->voistlejad[j]->eesNimi << ";";
                                        valja << leht->voistkonnad[i]->voistlejad[j]->perekNimi << ";";
                                        valja << leht->voistkonnad[i]->voistlejad[j]->summa << ";";
                                }
                                valja << leht->voistkonnad[i]->summa->text() << ";";
                                valja << leht->voistkonnad[i]->markus->text();
                                valja << "\n\n";
                        }
                }else{
                        for(int i = 0; i < leht->laskurid.count(); i++){
                                if(leht->laskurid[i]->markus->text().contains("V.A", Qt::CaseInsensitive)){
                                        valja << "v.a.;";
                                }else valja << QString("%1.;").arg(i+1);
                                valja << leht->laskurid[i]->eesNimi->text() << ";";
                                valja << leht->laskurid[i]->perekNimi->text();
                                valja << ";" << leht->laskurid[i]->sunniAasta->text() << ";";
                                valja << leht->laskurid[i]->klubi->text() << ";";
                                int indeks = 0;
                                for(int j = 0; j < leht->seeriateArv; j++){
                                        if(leht->laskurid[i]->seeriad[j]->text().isEmpty())
                                                valja << "0" << ";";
                                        else valja << leht->laskurid[i]->seeriad[j]->text() << ";";
                                        if(leht->vSummadeSamm != 0)
                                                if((j + 1) % leht->vSummadeSamm == 0){
                                                        valja << leht->laskurid[i]->vSummad[indeks]->text() << ";";
                                                        indeks++;
                                                }
                                }
                                valja << leht->laskurid[i]->getSumma() << ";";
                                if(!leht->laskurid[i]->finaal->text().isEmpty()){
                                        valja << leht->laskurid[i]->finaal->text() << ";";
//                                        valja << leht->laskurid[i]->koguSumma->text() << ";";
                                }
                                if(!leht->laskurid[i]->markus->text().isEmpty() &&
                                        !leht->laskurid[i]->markus->text().contains("V.A", Qt::CaseInsensitive))
                                    valja << leht->laskurid[i]->markus->text();
                                valja << "\n";
                        }
                }
                fail.close();
                QMessageBox::information(this, "Protokollitaja", tr("Tulemused eksporditud"), QMessageBox::Ok);
        }else QMessageBox::critical(this, tr("Viga"), tr("Ei õnnestu faili kirjutada."), QMessageBox::Ok);
}

void Protokollitaja::exportStartList(StartListWriter::StartListType type)
{
    Leht* leht = 0;
    if(tabWidget->count() > 0)
        leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
    else{
        QMessageBox::critical(this, tr("Viga"), tr("Töölehed puuduvad, ei ole midagi eksportida!"), QMessageBox::Ok);
        return;
    }

    if(leht->voistk){
        QMessageBox::critical(this, tr("Viga"), tr("Võistkondade lehest ei saa teha Sius'i stardinimekirja!!"),
                QMessageBox::Ok);
        return;
    }else if(leht->laskurid.count() <= 0){
        QMessageBox::critical(this, tr("Viga"), tr("Laskurid puuduvad, ei ole midagi eksportida!"), QMessageBox::Ok);
        return;
    }

    QVector<StartListWriter::StartListCompetitor> competitorsList;

    for(int i = 0; i < leht->laskurid.count(); i++){
        if(leht->laskurid[i]->linnuke->isChecked()){
            QStringList row;    //target, ID, first name, name, club, result
            if(leht->laskurid[i]->rajaNr->text().contains("A") || leht->laskurid[i]->rajaNr->text().contains("B") || leht->laskurid[i]->rajaNr->text().contains("C") || leht->laskurid[i]->rajaNr->text().contains("D") || leht->laskurid[i]->rajaNr->text().contains("E") || leht->laskurid[i]->rajaNr->text().contains("F") || leht->laskurid[i]->rajaNr->text().contains("G") || leht->laskurid[i]->rajaNr->text().contains("H"))
                QMessageBox::critical(this, tr("Viga"), tr("Raja numbrites on tähti. Kui soovite finaali start listi eksportida, kasutage teist funktsiooni!"), QMessageBox::Ok);

            competitorsList.append(StartListWriter::StartListCompetitor{
                leht->laskurid[i]->rajaNr->text(),
                QString("%1").arg(leht->laskurid[i]->id),
                leht->laskurid[i]->eesNimi->text(),
                leht->laskurid[i]->perekNimi->text(),
                leht->laskurid[i]->klubi->text(),
                leht->laskurid[i]->getSumma().replace(",", "."),
                static_cast<int>(leht->eventType()),
                                       ""  // TODO to be implemented
                                   });
        }
    }

    if(competitorsList.size() <= 0){
        QMessageBox::critical(this, tr("Viga"), tr("Märgitud laskurid puuduvad, ei ole midagi eksportida!"), QMessageBox::Ok);
        return;
    } else {
        StartListWriter *startListWriter = new StartListWriter(competitorsList, seeFail, type, this);
        startListWriter->deleteLater();
    }
}

void Protokollitaja::exportFinalsSiusStartList()
{
    Leht* leht = 0;
    if(tabWidget->count() > 0)
        leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
    else{
        QMessageBox::critical(this, tr("Viga"), tr("Töölehed puuduvad, ei ole midagi eksportida!"), QMessageBox::Ok);
        return;
    }

    if(leht->voistk){
        QMessageBox::critical(this, tr("Viga"), tr("Võistkondade lehest ei saa teha Sius'i stardinimekirja!!"),
                QMessageBox::Ok);
        return;
    }else if(leht->laskurid.count() <= 0){
        QMessageBox::critical(this, tr("Viga"), tr("Laskurid puuduvad, ei ole midagi eksportida!"), QMessageBox::Ok);
        return;
    }

    bool rajaNrOlemas = true;   //Kontrollimaks, kas kõigil on raja nr'id olemas või mitte
    leht->sorteeri(0);   //Et laskurid oleks raja nr'ite järgi reas ja oleks olemas reaslaskurid

    int competitorsInFinal = 8;
    if(leht->reasLaskurid.count() < 8)
        competitorsInFinal = leht->reasLaskurid.count();

    for(int i = 0; i < competitorsInFinal; i++)
        if(leht->reasLaskurid[i]->rajaNr->text().isEmpty())
            rajaNrOlemas = false;

    if(rajaNrOlemas)    //Kui raja nr on olemas, siis saab selle järgi sorteerida
        leht->sorteeri(3);   //Sorteerida raja nr'i järgi

    QString failiNimi = QFileDialog::getSaveFileName(this, tr("Ekspordi"), seeFail.left(seeFail.lastIndexOf("\\")),
            tr("Comma separated file (*.csv)"));

    if(failiNimi.isEmpty() || leht == 0) return;

    if(!failiNimi.endsWith(".csv"))
        failiNimi.append(".csv");

    QFile fail(failiNimi);
    if(fail.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream valja(&fail);
        int eksporditud = 0;
        for(int i = 0; i < leht->reasLaskurid.count() && i < competitorsInFinal; i++){
                //ID no;Startno;Name;Firstname;Disp name;Nat;Cat;Group;Team;Bay;Target;Relay;Starttime;BoxTg;Active;Q Tot;Avg;Rank;G1;...;G12;10s;...;0s;Mouches
                valja << QString(";%1%2;\"").arg(leht->leheIndeks).arg(leht->reasLaskurid[i]->id);
                valja << leht->reasLaskurid[i]->perekNimi->text() << "\";\"";
                valja << leht->reasLaskurid[i]->eesNimi->text() << "\";";
                valja << leht->reasLaskurid[i]->perekNimi->text() << " " << leht->reasLaskurid[i]->eesNimi->text().left(1);
                valja << ".\";;0;0;" << leht->reasLaskurid[i]->klubi->text() << ";;";
                valja << leht->reasLaskurid[i]->rajaNr->text().replace('A', '2').replace('B', '3').replace('C', '4').replace('D', '5').replace('E', '6').replace('F', '7').replace('G', '8').replace('H', '9');    //Täheliste raja nr'ite asendamine Siusi jaoks numbritega alates 2st
                valja << ";0;;0;0;" << leht->reasLaskurid[i]->getSumma().replace(",", ".") << ";0;\n";
                eksporditud++;
        }
        fail.close();
        QMessageBox::information(this, "Protokollitaja", tr("Startlist loodud. Eksporditi %1 laskurit").arg(eksporditud), QMessageBox::Ok);
    }else QMessageBox::critical(this, tr("Viga"), tr("Ei õnnestu faili kirjutada."), QMessageBox::Ok);
}

void Protokollitaja::eksportTXT()
{
        Leht* leht = 0;
        if(tabWidget->count() > 0)
            leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
        else{
            QMessageBox::critical(this, tr("Viga"), tr("Töölehed puuduvad, ei ole midagi eksportida!"), QMessageBox::Ok);
            return;
        }
        QString failiNimi = QFileDialog::getSaveFileName(this, tr("Ekspordi"), seeFail.left(seeFail.length() - 3), tr("Teksti fail (*.txt)"));
        if(failiNimi.isEmpty()) return;
        if(!failiNimi.endsWith(".txt"))
                failiNimi.append(".txt");
        QFile fail(failiNimi);
        if(fail.open(QIODevice::WriteOnly | QIODevice::Text)){
                QTextStream valja(&fail);
                if(leht->voistk){
                        for(int i = 0; i < leht->voistkonnad.count(); i++){
                                valja << QString("%1.\t").arg(i+1) << leht->voistkonnad[i]->nimi->text() << "\t";
                                for(int j = 0; j < leht->seeriateArv; j++){
                                        if(j != 0)
                                                valja << "\n\t\t";
                                        valja << leht->voistkonnad[i]->voistlejad[j]->eesNimi << "\t";
                                        valja << leht->voistkonnad[i]->voistlejad[j]->perekNimi << "\t";
                                        valja << leht->voistkonnad[i]->voistlejad[j]->summa << "\t";
                                }
                                valja << leht->voistkonnad[i]->summa->text() << "\t";
                                valja << leht->voistkonnad[i]->markus->text();
                                valja << "\n\n";
                        }
                }else{
                        for(int i = 0; i < leht->laskurid.count(); i++){
                                if(leht->laskurid[i]->markus->text().contains("V.A", Qt::CaseInsensitive)){
                                        valja << "v.a.\t";
                                }else valja << QString("%1.\t").arg(i+1);
                                valja << leht->laskurid[i]->eesNimi->text() << "\t";
                                valja << leht->laskurid[i]->perekNimi->text();
                                valja << "\t" << leht->laskurid[i]->sunniAasta->text() << "\t";
                                valja << leht->laskurid[i]->klubi->text() << "\t";
                                int indeks = 0;
                                for(int j = 0; j < leht->seeriateArv; j++){
                                        if(leht->laskurid[i]->seeriad[j]->text().isEmpty())
                                                valja << "0" << "\t";
                                        else valja << leht->laskurid[i]->seeriad[j]->text() << "\t";
                                        if(leht->vSummadeSamm != 0)
                                                if((j + 1) % leht->vSummadeSamm == 0){
                                                        valja << leht->laskurid[i]->vSummad[indeks]->text() << "\t";
                                                        indeks++;
                                                }
                                }
                                valja << leht->laskurid[i]->getSumma() << "\t";
                                if(!leht->laskurid[i]->finaal->text().isEmpty()){
                                        valja << leht->laskurid[i]->finaal->text() << "\t";
//                                        valja << leht->laskurid[i]->koguSumma->text() << "\t";
                                }
                                if(!leht->laskurid[i]->markus->text().isEmpty() &&
                                        !leht->laskurid[i]->markus->text().contains("V.A", Qt::CaseInsensitive))
                                    valja << leht->laskurid[i]->markus->text();
                                valja << "\n";
                        }
                }
                fail.close();
                QMessageBox::information(this, "Protokollitaja", tr("Tulemused eksporditud"), QMessageBox::Ok);
        }else QMessageBox::critical(this, tr("Viga"), tr("Ei õnnestu faili kirjutada."), QMessageBox::Ok);
}

void Protokollitaja::eksportXLS()
{
    if(tabWidget->count() > 0){
        QString failiAsukoht = seeFail.left(seeFail.length() - 3);
        QString failiNimi = QFileDialog::getSaveFileName(this, tr("Ekspordi"), failiAsukoht + "xls",
                        tr("Excel workbook file (*.xls)"));
        if(failiNimi.isEmpty()) return;
        if(!failiNimi.endsWith(".xls"))
                failiNimi.append(".xls");
        xlslib_core::workbook book;

        QFile file(failiNimi);
        if(file.open(QIODevice::ReadOnly))
            if(QMessageBox::critical(this, "Protokollitaja", tr("Sellise nimega fail on juba olemas. Kas "
                "soovite selle üle kirjutada?"), QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel){
                failiNimi.clear();
                return;
            }
        file.close();

        xlslib_core::font_t *tiitelFont = book.font("Times New Roman");   //font, millega kirjutatakse võistluse nimi
        tiitelFont->SetBoldStyle(xlslib_core::BOLDNESS_BOLD);
        tiitelFont->SetHeight(20*16); //teksti kõrgus 16

        xlslib_core::font_t *pealkiriFont = book.font("Times New Roman"); //font, millega kirjutatakse "Finaal"
        pealkiriFont->SetBoldStyle(xlslib_core::BOLDNESS_BOLD);
        pealkiriFont->SetHeight(20*14); //teksti kõrgus 14

        xlslib_core::font_t *paiseFont = book.font("Times New Roman"); //font, millega kirjutatakse rasvane tekst ja päised
        paiseFont->SetBoldStyle(xlslib_core::BOLDNESS_BOLD);
        paiseFont->SetHeight(20*12); //teksti kõrgus 12

        xlslib_core::font_t *underlineFont = book.font("Times New Roman"); //font, millega kirjutatakse koht ja nimi
        underlineFont->SetUnderlineStyle(xlslib_core::UNDERLINE_SINGLE);
        underlineFont->SetItalic(true);
        underlineFont->SetHeight(20*12); //teksti kõrgus 12

        xlslib_core::font_t *tekstiFont = book.font("Times New Roman");    //tavalise teksti font
        tekstiFont->SetHeight(20*12);   //teksti kõrgus 12

        //Book *book = xlCreateBook();
        for(int i = 0; i < tabWidget->count(); i++){
            xlslib_core::worksheet *sheet = book.sheet(QString(tabWidget->tabText(i).toUtf8()).toStdString());
            //Sheet* sheet = book->addSheet(tabWidget->tabText(i).toLocal8Bit().data());
            if(sheet){
                Leht* leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());
                //if(leht->seeriateArv > 6)
                    //sheet->setLandscape(true);

                for(int i = 0; i < 100; i++)    //tabel kõik Times New Romaniga ja suurusega 10
                    for(int j = 0; j < 50; j++)
                        sheet->blank(i, j)->font(tekstiFont);

                sheet->defaultColwidth(8);  //millegipärast vajalik, et saaks muuta tulpade laiust
                sheet->colwidth(0, 32*38); //38 pikslit
                /*for(int i = 3; i <= 12; i++)
                    sheet->colwidth(i, 32*40); //40 pikslit
                sheet->colwidth(13, 32*32); //32 pikslit*/

                int lisa = 0;
                if(leht->vSummadeSamm > 0)
                    lisa = leht->seeriateArv / leht->vSummadeSamm;
                sheet->merge(0, 0, 0, 6 + leht->seeriateArv + lisa);
                sheet->rowheight(0, 434);

                //sheet->setCol(0, 0, 4.71);
                if(leht->voistk){
                    sheet->colwidth(1, 32*110); //110 pikslit
                    sheet->colwidth(2, 32*110); //45 pikslit
                    sheet->colwidth(3, 32*138); //120 pikslit
                    sheet->colwidth(4, 32*61);
                    sheet->colwidth(5, 32*61);
                    //sheet->setCol(1, 1, 15);
                    //sheet->setCol(2, 2, 15);
                    //sheet->setCol(3, 3, 19);
                    //sheet->setCol(4, 4, 8);
                    //sheet->setCol(5, 5, 8);
                }else{
                    sheet->colwidth(1, 32*110); //110 pikslit
                    sheet->colwidth(2, 32*138);
                    sheet->colwidth(3, 32*45);
                    sheet->colwidth(4, 32*110);
                    //sheet->setCol(1, 1, 15);
                    //sheet->setCol(2, 2, 19);
                    //sheet->setCol(3, 3, 6);
                    //sheet->setCol(4, 4, 15);
                }
                //sheet->writeStr(1, 0, m_competitionName.toLocal8120Bit().data(), pealKiri);
                //sheet->label(0, 0, m_competitionName.toLocal8Bit().data())->font(tiitelFont);
                sheet->label(0, 0, QString(m_competitionName.toUtf8()).toStdString())->font(tiitelFont);
                sheet->FindCell(0, 0)->halign(xlslib_core::HALIGN_CENTER);

                sheet->label(4, 1, QString(leht->ekraaniNimi.toUtf8()).toStdString())->font(paiseFont);
                sheet->label(5, 0, tr("Koht").toStdString())->font(underlineFont);
                sheet->FindCell(5, 0)->halign(xlslib_core::HALIGN_CENTER);
                std::string sigma = "\u03A3";    //Summa märk
                if(leht->voistk){
                    sheet->label(1, 6, QString(timeAndPlaceString().toUtf8()).toStdString())->font(paiseFont);
                    sheet->FindCell(1, 6)->halign(xlslib_core::HALIGN_RIGHT);
                    sheet->label(5, 1, tr("Võistkond").toStdString())->font(underlineFont);
                    sheet->label(5, 2, tr("Eesnimi").toStdString())->font(underlineFont);
                    sheet->label(5, 3, tr("Perenimi").toStdString())->font(underlineFont);
                    sheet->label(5, 4, tr("Tulemus").toStdString())->font(underlineFont);
                    sheet->label(5, 5, sigma)->font(underlineFont);
                    sheet->FindCell(5, 5)->halign(xlslib_core::HALIGN_CENTER);
                }else{
                    if (leht->vSummadeSamm > 0) {
                        sheet->label(1, 6 + leht->seeriateArv + leht->laskurid[0]->vSummad.count(), QString(timeAndPlaceString().toUtf8()).toStdString())->font(paiseFont);
                        sheet->FindCell(1, 6 + leht->seeriateArv + leht->laskurid[0]->vSummad.count())->halign(xlslib_core::HALIGN_RIGHT);
                    } else {
                        sheet->label(1, 6 + leht->seeriateArv, QString(timeAndPlaceString().toUtf8()).toStdString())->font(paiseFont);
                        sheet->FindCell(1, 6 + leht->seeriateArv)->halign(xlslib_core::HALIGN_RIGHT);
                    }
                    sheet->label(5, 1, tr("Eesnimi").toStdString())->font(underlineFont);
                    sheet->label(5, 2, tr("Perenimi").toStdString())->font(underlineFont);
                    sheet->label(5, 3, tr("S.a.").toStdString())->font(underlineFont);
                    sheet->label(5, 4, tr("Klubi").toStdString())->font(underlineFont);

                    if(leht->eventType() == QualificationEvents::EventType::AirRifle40 || leht->eventType() == QualificationEvents::EventType::AirPistol40){
                        sheet->merge(5, 5, 5, 8);
                        sheet->label(5, 5, tr("Seeriad").toStdString())->font(underlineFont);
                    }else if(leht->eventType() == QualificationEvents::EventType::AirRifle60 || leht->eventType() == QualificationEvents::EventType::AirPistol60
                             || leht->eventType() == QualificationEvents::EventType::RifleProne60_50m || leht->eventType() == QualificationEvents::EventType::FreePistol60_50m
                             || leht->eventType() == QualificationEvents::EventType::RifleProne60_300m){
                        sheet->merge(5, 5, 5, 10);
                        sheet->label(5, 5, tr("Seeriad").toStdString())->font(underlineFont);
                    }else if(leht->eventType() == QualificationEvents::EventType::Rifle3x20_50m || leht->eventType() == QualificationEvents::EventType::Rifle3x20_300m){
                        sheet->merge(5, 5, 5, 7);
                        sheet->label(5, 5, tr("Põlvelt").toStdString())->font(underlineFont);
                        sheet->merge(5, 8, 5, 10);
                        sheet->label(5, 8, tr("Lamades").toStdString())->font(underlineFont);
                        sheet->merge(5, 11, 5, 13);
                        sheet->label(5, 11, tr("Püsti").toStdString())->font(underlineFont);
                    }else if(leht->eventType() == QualificationEvents::EventType::Rifle3x40_50m || leht->eventType() == QualificationEvents::EventType::Rifle3x40_300m){
                        sheet->merge(5, 5, 5, 9);
                        sheet->label(5, 5, tr("Põlvelt").toStdString())->font(underlineFont);
                        sheet->merge(5, 10, 5, 14);
                        sheet->label(5, 10, tr("Lamades").toStdString())->font(underlineFont);
                        sheet->merge(5, 15, 5, 19);
                        sheet->label(5, 15, tr("Püsti").toStdString())->font(underlineFont);
                    }else if(leht->eventType() == QualificationEvents::EventType::Pistol_25m){
                        sheet->merge(5, 5, 5, 8);
                        sheet->label(5, 5, tr("Ringmärk").toStdString())->font(underlineFont);
                        sheet->merge(5, 9, 5, 12);
                        sheet->label(5, 9, tr("Ilmuv märk").toStdString())->font(underlineFont);
                    }else if(leht->eventType() == QualificationEvents::EventType::RapidFirePistol){
                        sheet->merge(5, 5, 5, 8);
                        sheet->label(5, 5, tr("I pool").toStdString())->font(underlineFont);
                        sheet->merge(5, 9, 5, 12);
                        sheet->label(5, 9, tr("II pool").toStdString())->font(underlineFont);
                    }else if(leht->eventType() == QualificationEvents::EventType::StandardPistol){
                        sheet->merge(5, 5, 5, 7);
                        sheet->label(5, 5, "150\"")->font(underlineFont);
                        sheet->merge(5, 8, 5, 10);
                        sheet->label(5, 8, "20\"")->font(underlineFont);
                        sheet->merge(5, 11, 5, 13);
                        sheet->label(5, 11, "10\"")->font(underlineFont);
                    }else if(leht->eventType() == QualificationEvents::EventType::RunningTarget_50m || leht->eventType() == QualificationEvents::EventType::RunningTarget_10m){
                        sheet->merge(5, 5, 5, 8);
                        sheet->label(5, 5, tr("Aeglane jooks").toStdString())->font(underlineFont);
                        sheet->merge(5, 9, 5, 12);
                        sheet->label(5, 9, tr("Kiire jooks").toStdString())->font(underlineFont);
                    }
                }
                for(int i = 0; i < 15; i++)
                    sheet->FindCell(5, i)->halign(xlslib_core::HALIGN_CENTER);

                if(!leht->voistk){
                    if(leht->laskurid.count() > 0){
                        if(leht->vSummadeSamm > 0){
                            sheet->label(5, 5 + leht->seeriateArv + leht->laskurid[0]->vSummad.count(), sigma)->font(underlineFont);
                            sheet->FindCell(5, 5 + leht->seeriateArv + leht->laskurid[0]->vSummad.count())->halign(xlslib_core::HALIGN_CENTER);
                            sheet->label(5, 6 + leht->seeriateArv + leht->laskurid[0]->vSummad.count(), "10*")->font(underlineFont);
                            sheet->colwidth(6 + leht->seeriateArv + leht->laskurid[0]->vSummad.count(), 32*31);
                            sheet->FindCell(5, 6 + leht->seeriateArv + leht->laskurid[0]->vSummad.count())->halign(xlslib_core::HALIGN_CENTER);
                        }else{
                            sheet->label(5, 5 + leht->seeriateArv, sigma)->font(underlineFont);
                            sheet->FindCell(5, 5 + leht->seeriateArv)->halign(xlslib_core::HALIGN_CENTER);
                            sheet->label(5, 6 + leht->seeriateArv, "10*")->font(underlineFont);
                            sheet->colwidth(6 + leht->seeriateArv, 32*31);
                            sheet->FindCell(5, 6 + leht->seeriateArv)->halign(xlslib_core::HALIGN_CENTER);
                        }
                    }
                    for(int j = 0; j < leht->laskurid.count(); j++){
                        int rida = 6;
                        if(!(leht->laskurid[j]->eesNimi->text().isEmpty() && leht->laskurid[j]->perekNimi->text().isEmpty() && leht->laskurid[j]->getSumma().toInt() == 0)){
                            if(j < 3){
                                if(leht->laskurid[j]->markus->text().contains("V.A", Qt::CaseInsensitive)){
                                    rida++;
                                    sheet->label(j + rida, 0, QString("v.a.").toStdString())->font(paiseFont);
                                    sheet->FindCell(j + rida, 0)->halign(xlslib_core::HALIGN_CENTER);
                                    sheet->label(j + rida, 1, QString(leht->laskurid[j]->eesNimi->text().toUtf8()).toStdString())->font(tekstiFont);
                                    sheet->label(j + rida, 2, QString(leht->laskurid[j]->perekNimi->text().toUtf8()).toStdString())->font(tekstiFont);
                                }else{
                                    switch(j){
                                    case 0: {
                                            sheet->label(j + rida, 0, "I")->font(paiseFont);
                                            sheet->FindCell(j + rida, 0)->halign(xlslib_core::HALIGN_CENTER);
                                            break;
                                        }
                                    case 1: {
                                            sheet->label(j + rida, 0, "II")->font(paiseFont);
                                            sheet->FindCell(j + rida, 0)->halign(xlslib_core::HALIGN_CENTER);
                                            break;
                                        }
                                    case 2: {
                                            sheet->label(j + rida, 0, "III")->font(paiseFont);
                                            sheet->FindCell(j + rida, 0)->halign(xlslib_core::HALIGN_CENTER);
                                            break;
                                        }
                                    default: {
                                            sheet->label(j + rida, 0, QString("%1.").arg(j+1).toStdString())->font(tekstiFont);
                                            sheet->FindCell(j + rida, 0)->halign(xlslib_core::HALIGN_CENTER);
                                            break;
                                        }
                                    }
                                    sheet->label(j + rida, 1, QString(leht->laskurid[j]->eesNimi->text().toUtf8()).toStdString())->font(paiseFont);
                                    sheet->label(j + rida, 2, QString(leht->laskurid[j]->perekNimi->text().toUtf8()).toStdString())->font(paiseFont);
                                }
                            }else{
                                if(leht->laskurid[j]->markus->text().contains("V.A", Qt::CaseInsensitive)){
                                    rida++;
                                    sheet->label(j + rida, 0, QString("v.a.").toStdString())->font(tekstiFont);
                                    sheet->FindCell(j + rida, 0)->halign(xlslib_core::HALIGN_CENTER);
                                }else{
                                    sheet->label(j + rida, 0, QString("%1.").arg(j+1).toStdString())->font(tekstiFont);
                                    sheet->FindCell(j + rida, 0)->halign(xlslib_core::HALIGN_CENTER);
                                }
                                sheet->label(j + rida, 1, QString(leht->laskurid[j]->eesNimi->text().toUtf8()).toStdString())->font(tekstiFont);
                                sheet->label(j + rida, 2, QString(leht->laskurid[j]->perekNimi->text().toUtf8()).toStdString())->font(tekstiFont);
                            }
                            sheet->number(j + rida, 3, leht->laskurid[j]->sunniAasta->text().toInt())->font(tekstiFont);
                            sheet->FindCell(j + rida, 3)->halign(xlslib_core::HALIGN_CENTER);
                            sheet->label(j + rida, 4, QString(leht->laskurid[j]->klubi->text().toUtf8()).toStdString())->font(tekstiFont);
                            if(leht->vSummadeSamm > 0){
                                bool onnestus = true;
                                double arv;
                                for(int v = 0; v < leht->laskurid[j]->vSummad.count(); v++){
                                    for(int x = 0; x < leht->vSummadeSamm; x++){
                                        arv = leht->laskurid[j]->seeriad[v * leht->vSummadeSamm + x]->text().toDouble(&onnestus);
                                        if(!onnestus)
                                            arv = leht->laskurid[j]->seeriad[v * leht->vSummadeSamm + x]->text().replace(',', '.').toDouble(&onnestus);
                                        if(onnestus)
                                            sheet->number(j + rida, 5 + v * leht->vSummadeSamm + v + x, arv)->font(tekstiFont);
                                        else {
                                            sheet->label(j + rida, 5 + v * leht->vSummadeSamm + v + x, QString(leht->laskurid[j]->seeriad[v * leht->vSummadeSamm + x]->text().toUtf8()).toStdString())->font(tekstiFont);
                                        }
                                        sheet->FindCell(j + rida, 5 + v * leht->vSummadeSamm + v + x)->halign(xlslib_core::HALIGN_CENTER);
                                        sheet->colwidth(5 + v * leht->vSummadeSamm + v + x, 32*31);
                                    }
                                    sheet->colwidth(5 + (v + 1) * leht->vSummadeSamm + v, 32*31);

                                    arv = leht->laskurid[j]->vSummad[v]->text().toDouble(&onnestus);
                                    if(!onnestus)
                                        arv = leht->laskurid[j]->vSummad[v]->text().replace(',', '.').toDouble(&onnestus);
                                    sheet->number(j + rida, 5 + (v + 1) * leht->vSummadeSamm + v, arv)->font(paiseFont);
                                    sheet->FindCell(j + rida, 5 + (v + 1) * leht->vSummadeSamm + v)->halign(xlslib_core::HALIGN_CENTER);
                                }
                                sheet->colwidth(5 + leht->seeriateArv + leht->laskurid[j]->vSummad.count(), 32*61);
                                arv = leht->laskurid[j]->getSumma().toDouble(&onnestus);
                                if(!onnestus)
                                    arv = leht->laskurid[j]->getSumma().replace(',', '.').toDouble(&onnestus);
                                sheet->number(j + rida, 5 + leht->seeriateArv + leht->laskurid[j]->vSummad.count(), arv)->font(paiseFont);
                                sheet->FindCell(j + rida, 5 + leht->seeriateArv + leht->laskurid[j]->vSummad.count())->halign(xlslib_core::HALIGN_CENTER);
                                if(!leht->laskurid[j]->finaal->text().isEmpty()){
                                    sheet->colwidth(5 + leht->seeriateArv + leht->laskurid[j]->vSummad.count(), 32*47);
                                    sheet->colwidth(6 + leht->seeriateArv + leht->laskurid[j]->vSummad.count(), 32*47);

                                    arv = leht->laskurid[j]->finaal->text().toDouble(&onnestus);
                                    if(!onnestus)
                                        arv = leht->laskurid[j]->finaal->text().replace(',', '.').toDouble(&onnestus);
                                    sheet->number(j + rida, 7 + leht->seeriateArv + leht->laskurid[j]->vSummad.count(), arv)->font(tekstiFont);

//                                    arv = leht->laskurid[j]->koguSumma->text().toDouble(&onnestus);
//                                    if(!onnestus)
//                                        arv = leht->laskurid[j]->koguSumma->text().replace(',', '.').toDouble(&onnestus);
//                                    sheet->number(j + rida, 6 + leht->seeriateArv + leht->laskurid[j]->vSummad.count(), arv)->font(tekstiFont);
//                                    sheet->number(j + rida, 7 + leht->seeriateArv + leht->laskurid[j]->vSummad.count(), arv)->font(paiseFont);
                                }
                                if(!leht->laskurid[j]->markus->text().isEmpty() &&
                                        !leht->laskurid[j]->markus->text().contains("V.A", Qt::CaseInsensitive)){
                                    if(leht->laskurid[j]->finaal->text().isEmpty())
                                        sheet->label(j + rida, 7 + leht->seeriateArv + leht->laskurid[j]->vSummad.count(), QString(leht->laskurid[j]->markus->text().toUtf8()).toStdString())->font(tekstiFont);
                                    else sheet->label(j + rida, 9 + leht->seeriateArv + leht->laskurid[j]->vSummad.count(), QString(leht->laskurid[j]->markus->text().toUtf8()).toStdString())->font(tekstiFont);
                                }
                            }else{
                                for(int x = 0; x < leht->seeriateArv; x++){
                                    bool onnestus = true;
                                    double seeria = leht->laskurid[j]->seeriad[x]->text().toDouble(&onnestus);
                                    if(!onnestus)
                                        seeria = leht->laskurid[j]->seeriad[x]->text().replace(',', '.').toDouble(&onnestus);
//#ifdef QT_DEBUG
//                                    qDebug() << "Õnnestus: " << leht->laskurid[j]->seeriad[x]->text() << " " << leht->laskurid[j]->seeriad[x]->text().toDouble() << " " << leht->laskurid[j]->seeriad[x]->text().replace(',', '.').toDouble() << onnestus;
//#endif
                                    if(onnestus)
                                        sheet->number(j + rida, x + 5, seeria)->font(tekstiFont);
                                    else
                                        sheet->label(j + rida, x + 5, QString(leht->laskurid[j]->seeriad[x]->text().toUtf8()).toStdString())->font(tekstiFont);
                                    sheet->FindCell(j + rida, x + 5)->halign(xlslib_core::HALIGN_CENTER);
                                    sheet->colwidth(x + 5, 32*31);
                                }
                                sheet->colwidth(5 + leht->seeriateArv, 32*61);
                                bool onnestus = true;
                                double arv = leht->laskurid[j]->getSumma().toDouble(&onnestus);
                                if(!onnestus)
                                    arv = leht->laskurid[j]->getSumma().replace(',', '.').toDouble(&onnestus);
                                sheet->number(j + rida, 5 + leht->seeriateArv, arv)->font(paiseFont);
                                sheet->FindCell(j + rida, 5 + leht->seeriateArv)->halign(xlslib_core::HALIGN_CENTER);

                                sheet->label(j + rida, 6 + leht->seeriateArv, leht->laskurid[j]->kumned->text().toStdString())->font(tekstiFont);
                                sheet->FindCell(j + rida, 6 + leht->seeriateArv)->halign(xlslib_core::HALIGN_CENTER);
                                if(!leht->laskurid[j]->finaal->text().isEmpty()){
                                    sheet->colwidth(7 + leht->seeriateArv, 32*47);
                                    sheet->colwidth(8 + leht->seeriateArv, 32*47);

                                    arv = leht->laskurid[j]->finaal->text().toDouble(&onnestus);
                                    if(!onnestus)
                                        arv = leht->laskurid[j]->finaal->text().replace(',', '.').toDouble(&onnestus);
                                    sheet->number(j + rida, 7 + leht->seeriateArv, arv)->font(tekstiFont);

//                                    arv = leht->laskurid[j]->koguSumma->text().toDouble(&onnestus);
//                                    if(!onnestus)
//                                        arv = leht->laskurid[j]->koguSumma->text().replace(',', '.').toDouble(&onnestus);
//                                    sheet->number(j + rida, 7 + leht->seeriateArv, arv)->font(paiseFont);

                                    sheet->FindCell(j + rida, 7 + leht->seeriateArv)->halign(xlslib_core::HALIGN_CENTER);
                                    sheet->FindCell(j + rida, 8 + leht->seeriateArv)->halign(xlslib_core::HALIGN_CENTER);
                                }
                                if(!leht->laskurid[j]->markus->text().isEmpty() &&
                                        !leht->laskurid[j]->markus->text().contains("V.A", Qt::CaseInsensitive)){
                                    if(leht->laskurid[j]->finaal->text().isEmpty())
                                        sheet->label(j + rida, 7 + leht->seeriateArv, QString(leht->laskurid[j]->markus->text().toUtf8()).toStdString())->font(tekstiFont);
                                    else sheet->label(j + rida, 9 + leht->seeriateArv, QString(leht->laskurid[j]->markus->text().toUtf8()).toStdString())->font(tekstiFont);
                                }
                            }
                        }
                    }
                }else{
                    if(leht->voistkonnad.count() > 0){
                        int rida = 6;
                        for(int i = 0; i < leht->voistkonnad.count(); i++){
                            rida++;
                            sheet->label(rida, 0, QString("%1.").arg(i+1).toStdString())->font(tekstiFont);
                            sheet->FindCell(rida, 0)->halign(xlslib_core::HALIGN_CENTER);
                            sheet->label(rida, 1, QString(leht->voistkonnad[i]->nimi->text().toUtf8()).toStdString())->font(tekstiFont);
                            rida--;
                            bool onnestus = true;
                            double arv;
                            for(int j = 0; j < leht->voistkonnad[i]->voistlejad.count(); j++){
                                rida++;
                                sheet->label(rida, 2, QString(leht->voistkonnad[i]->voistlejad[j]->eesNimi.toUtf8()).toStdString())->font(tekstiFont);
                                sheet->label(rida, 3, QString(leht->voistkonnad[i]->voistlejad[j]->perekNimi.toUtf8()).toStdString())->font(tekstiFont);
                                arv = leht->voistkonnad[i]->voistlejad[j]->summa.toDouble(&onnestus);
                                if(!onnestus)
                                    arv = leht->voistkonnad[i]->voistlejad[j]->summa.replace(',', '.').toDouble(&onnestus);
                                sheet->number(rida, 4, arv)->font(paiseFont);
                                sheet->FindCell(rida, 4)->halign(xlslib_core::HALIGN_CENTER);
                            }
                            arv = leht->voistkonnad[i]->summa->text().toDouble(&onnestus);
                            if(!onnestus)
                                arv = leht->voistkonnad[i]->summa->text().replace(',', '.').toDouble(&onnestus);
                            sheet->number(rida, 5, arv)->font(paiseFont);
                            sheet->FindCell(rida, 5)->halign(xlslib_core::HALIGN_CENTER);
                            rida++;
                        }
                    }
                }
            }
        }

        if(book.Dump(/*QString(*/failiNimi.toLatin1().toStdString()) == 0){
            QMessageBox::information(this, tr("Teade"), tr("Võistlus eksporditud .xls faili\n%1").arg(failiNimi), QMessageBox::Ok);
        }
    }
}

void Protokollitaja::exportFinalsFile()
{
        if(tabWidget->count() < 1){
                QMessageBox::critical(this, "Protokollitaja", tr("Ei ole ühtegi lehte, mille põhjal finaali teha!"),
                                QMessageBox::Ok);
                return;
        }
        Leht* seeLeht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
        if(seeLeht->voistk){
                QMessageBox::critical(this, "Protokollitaja", tr("Tahate võistkondliku arvestuse kohta finaali teha? "
                                "Mida te endast küll mõtlete? Ühesõnaga selline asi unustage kohe ära."), QMessageBox::Ok);
                return;
        }

        if (seeLeht->finalsData().has_value()) {
            QJsonObject finalsObj = seeLeht->finalsData().value();
            QString fileLocation = seeFail.left(seeFail.lastIndexOf('/') + 1);
            QString finalsFileName = QFileDialog::getSaveFileName(this, tr("Salvesta finaal"), fileLocation + finalsObj["eventName"].toString("") + ".fin", tr("Protofinaali fail (*.fin)"));
            if(finalsFileName.isEmpty()) return;

            QFile file(finalsFileName);
            if(file.open(QIODevice::ReadOnly))
                if(QMessageBox::critical(this, "Protokollitaja", tr("Sellise nimega fail on juba olemas. Kas "
                        "soovite selle üle kirjutada?"), QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel){
                    finalsFileName.clear();
                    return;
                }
            file.close();

            if(file.open(QIODevice::WriteOnly)) {
                QJsonDocument jsonDoc(finalsObj);
                file.write(jsonDoc.toJson());
                file.close();
                m_finalsFileName = finalsFileName;
                return;
            } else {
                QMessageBox::critical(this, "Protokollitaja", tr("Ei õnnestu finaali faili luua! Kontrollige, "
                                    "kas teil on sinna kausta kirjutamise õigused"), QMessageBox::Ok);
                finalsFileName.clear();
                return;
            }
        }

        m_finalsFileName.clear();

        // bool rajaNrOlemas = false;   // Check if all competitors have target numbers
        bool selected = false;   // If some competitior is selected, then the selected competitors will be sent to finals
        int competitorsInFinal = 8;
        if(seeLeht->laskurid.count() < 8)
            competitorsInFinal = seeLeht->laskurid.count();

        for(int i = 0; i < competitorsInFinal; i++){ //Piisab, kui on osadel raja nr, mitte kõigil
            // if(!seeLeht->reasLaskurid[i]->rajaNr->text().isEmpty())
                // rajaNrOlemas = true;
            if(seeLeht->laskurid[i]->linnuke->isChecked())
               selected = true;
        }

        // if(rajaNrOlemas)    //Kui raja nr on olemas, siis saab selle järgi sorteerida
        seeLeht->sorteeri(0);   //Sorteerida raja nr'i järgi
// #ifdef QT_DEBUG
//         qDebug() << "Protokollitaja::exportFinalsFile(), Rajanr: " << seeLeht->reasLaskurid[0]->rajaNr->text();
// #endif

//        QString teateTekst; //Tekst, kus öeldakse, mille järgi laskurid finaali sorteeriti
//        if(seeLeht->reasLaskurid[0]->rajaNr->text() != "A"){
//            seeLeht->sorteeri(0);   //Kui radade nr'id ei alga A'ga, siis ilmselt ei ole finaali jaoks radu sisestatud ja tuleb summade järgi reastada
//            teateTekst = tr("Rajanumbreid ei leitud, laskurid reastati finaaliks tulemuse järgi.");
//        }else {
//            seeLeht->sorteeri(3);
//            teateTekst = tr("Leitud rajanumbrid, laskurid reastati finaaliks rajanumbri järgi.");
//        }

//#ifdef QT_DEBUG
//        qDebug() << "Protokollitaja::exportFinalsFile(), Rajanr2: " << seeLeht->reasLaskurid[0]->rajaNr->text() << ", perekNimi: " << seeLeht->reasLaskurid[0]->perekNimi->text();
//#endif
        QVector<QStringList> finalsTable; //Each "row": target, ID, screen name, result, first name, name, club
        if (selected) {
            for(int i = 0; i < seeLeht->laskurid.count() && i < competitorsInFinal; i++){
                if (seeLeht->reasLaskurid[i]->linnuke->isChecked()) {
                    QStringList finalsRow;
                    finalsRow << seeLeht->reasLaskurid[i]->rajaNr->text();
                    finalsRow << QString("%1%2").arg(seeLeht->leheIndeks, 2, 10, QChar('0')).arg(seeLeht->reasLaskurid[i]->id, 4, 10, QChar('0'));
                    finalsRow << QString("%1 %2.").arg(seeLeht->reasLaskurid[i]->perekNimi->text()).arg(seeLeht->reasLaskurid[i]->eesNimi->text().left(1));
                    finalsRow << seeLeht->reasLaskurid[i]->getSumma();
                    finalsRow << seeLeht->reasLaskurid[i]->eesNimi->text();
                    finalsRow << seeLeht->reasLaskurid[i]->perekNimi->text();
                    finalsRow << seeLeht->reasLaskurid[i]->klubi->text();
                    finalsTable << finalsRow;
                }
            }
        } else {
            for(int i = 0; i < seeLeht->laskurid.count() && i < competitorsInFinal; i++){
                QStringList finalsRow;
                finalsRow << seeLeht->reasLaskurid[i]->rajaNr->text();
                finalsRow << QString("%1%2").arg(seeLeht->leheIndeks, 2, 10, QChar('0')).arg(seeLeht->reasLaskurid[i]->id, 4, 10, QChar('0'));
                finalsRow << QString("%1 %2.").arg(seeLeht->reasLaskurid[i]->perekNimi->text()).arg(seeLeht->reasLaskurid[i]->eesNimi->text().left(1));
                finalsRow << seeLeht->reasLaskurid[i]->getSumma();
                finalsRow << seeLeht->reasLaskurid[i]->eesNimi->text();
                finalsRow << seeLeht->reasLaskurid[i]->perekNimi->text();
                finalsRow << seeLeht->reasLaskurid[i]->klubi->text();
                finalsTable << finalsRow;
            }
        }

        FinalsFileExport *finalsFileExport = new FinalsFileExport(
                    finalsTable,
                    seeFail,
                    m_competitionName,
                    seeLeht->ekraaniNimi,
                    timeAndPlaceString(),
                    seeLeht->eventType(),
                    this
                    );
        finalsFileExport->setRelay(10 + seeLeht->leheIndeks);
        if(finalsFileExport->exec() == QDialog::Accepted){
            //Lisa kood, et lugeda finaalifailinimi
            m_finalsFileName = finalsFileExport->getFinalsFileName();
            //QMessageBox::information(this, "Protokollitaja", teateTekst, QMessageBox::Ok);
            QMessageBox::information(this, "Protokollitaja", tr("Finaali fail kirjutatud!\n%1").arg(m_finalsFileName), QMessageBox::Ok);
        }
        finalsFileExport->deleteLater();
}

void Protokollitaja::finalsFinished(const int exitCode, const QProcess::ExitStatus exitStatus)
{
    // TODO: Check exit code and status in future?
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);

    readFinalsFile(m_finalsFileName);
}

void Protokollitaja::impordiFinaal()
{
    QString failiNimi = QFileDialog::getOpenFileName(this, tr("Ava"), seeFail.left(seeFail.length() - 3), tr("Finaali fail (*.fin)"));
    if(failiNimi.isEmpty() || failiNimi == ".fin")
            return;
    readFinalsFile(failiNimi);
}

void Protokollitaja::import()
{
        if(tabWidget->count() < 1) return;
        importAken->ui.sakiBox->clear();
        importAken->setCurrentCompetitorId(laskuriId);
        for(int i = 0; i < tabWidget->count(); i++)
                importAken->ui.sakiBox->addItem(tabWidget->tabText(i));
        importAken->ui.sakiBox->setCurrentIndex(tabWidget->currentIndex());
        if(importAken->exec() == QDialog::Accepted && importAken->leht != nullptr){
                Leht* leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(importAken->
                                ui.sakiBox->currentIndex()))->widget());
                if(leht->voistk){
                QMessageBox::critical(this, tr("Viga!"), tr("Võistkondade lehele ei saa laskureid importida!"), QMessageBox::Ok);
                    return;
                }
                for(int i = 0; i < importAken->leht->laskurid.count(); i++){
                        if(importAken->leht->laskurid[i]->linnuke->isChecked()){
                                leht->uusLaskur(++laskuriId);
                                leht->laskurid[leht->laskurid.count() - 1]->set(importAken->leht->laskurid[i]);
                        }
                }
                voibSulgeda = false;
        }
        importAken->clearSheet();
}

void Protokollitaja::runFinals()
{
    exportFinalsFile();
    if(m_finalsFileName.isEmpty()) return;

    //finaal.setWorkingDirectory("/home/lauri/Katsepolygon/QFinaal");

    if(lehelugejaAken){ //Finaali käivitades tasub lugemismasinaga ühendus katkestada, et Finaal saaks lehti lugeda
        lehelugejaAken->sulgeUhendus();
    }

    m_protoFinaalProcess = new QProcess(this);
    m_protoFinaalProcess->start(qApp->applicationDirPath() + "/Protofinaal", QStringList() << "-f" << m_finalsFileName);
    connect(m_protoFinaalProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finalsFinished(int, QProcess::ExitStatus)));

    if(!m_protoFinaalProcess->waitForStarted(5000)){   //Kontrollimaks, kas õnnestub käivitada
//        switch(finaal->state()){
//        case QProcess::NotRunning : {
//                QMessageBox::critical(this, "Protokollitaja", "QProcess::NotRunning", QMessageBox::Ok);
//                break;
//        }
//        case QProcess::Starting : {
//                QMessageBox::critical(this, "Protokollitaja", "QProcess::Starting", QMessageBox::Ok);
//                break;
//        }
//        case QProcess::Running : {
//                QMessageBox::critical(this, "Protokollitaja", "QProcess::Running", QMessageBox::Ok);
//                break;
//        }
//        default : QMessageBox::critical(this, "Protokollitaja", "QProcess::Info puudub", QMessageBox::Ok);
//        }
        switch(m_protoFinaalProcess->error()){
        case QProcess::FailedToStart : {
            if(m_protoFinaalProcess->errorString() == "No such file or directory")
                QMessageBox::critical(this, tr("Viga"), tr("Finaali käivitamine ei õnnestunud!\nProtofinaal.exe faili ei leitud!"), QMessageBox::Ok);
            else
                QMessageBox::critical(this, tr("Viga"), tr("Finaali käivitamine ei õnnestunud!\n\n%1").arg(m_protoFinaalProcess->errorString()), QMessageBox::Ok);
            break;
        }
        case QProcess::Crashed : {
            QMessageBox::critical(this, tr("Viga"), tr("Finaali programm jooksis käivitamisel kokku!\n\n%1").arg(m_protoFinaalProcess->errorString()), QMessageBox::Ok);
            break;
        }
        case QProcess::Timedout : {
            QMessageBox::critical(this, tr("Viga"), tr("Finaali käivitamine ei õnnestunud! Võttis liiga palju aega!\n\n%1").arg(m_protoFinaalProcess->errorString()), QMessageBox::Ok);
            break;
        }
        default : QMessageBox::critical(this, tr("Viga"), tr("Finaali käivitamine ei õnnestunud! Põhjus teadmata!\n\n%1").arg(m_protoFinaalProcess->errorString()), QMessageBox::Ok);
        }
    }
}

void Protokollitaja::kaivitaServer()
{
    if(server == 0){
//        server = new QTcpServer(this);
        server = new ProtolehelugejaServer(this);
        if (!server->listen(QHostAddress::Any, 50005)) {
            QMessageBox::critical(this, tr("Viga"), tr("Unable to start the server: %1.").arg(server->errorString()));
            return;
        }
        ipAadress.clear();
        int mituOiget = 0;  //Mitu aadressi algab 192.'ga
        //ipAadress = server->serverAddress().toString();
        QList<QHostAddress> ipAadressid = QNetworkInterface::allAddresses();
        //Mõnes arvutis on mitu IPv4 aadressi, vaatame, mitu neist algavad 192.'ga, kui ainult üks, kasutame seda,
        //kui algab mitu, kuvame kõik
        for(int i = 0; i < ipAadressid.size(); ++i){
            if (ipAadressid.at(i) != QHostAddress::LocalHost && ipAadressid.at(i).toIPv4Address()){
               if(ipAadressid.at(i).toString().startsWith("192.")){
                   mituOiget++;
                   ipAadress = ipAadressid.at(i).toString();
               }
            }
        }
        if(mituOiget > 1 || mituOiget == 0){    //Kui on rohkem kui üks aadress 192.'ga või mitte ühtegi
            ipAadress.clear();
            for (int i = 0; i < ipAadressid.size(); ++i) {
                if (ipAadressid.at(i) != QHostAddress::LocalHost && ipAadressid.at(i).toIPv4Address()) {
                    //ipAadress = ipAadressid.at(i).toString();
                    ipAadress.append("\n" + ipAadressid.at(i).toString());  //Mõnes arvutis on mitu IP aadressi
                    //break;
                }
            }
        }
        // kui ei leia uhtegi sellist, kasutame IPv4 localhost'i
        if (ipAadress.isEmpty())
            ipAadress = QHostAddress(QHostAddress::LocalHost).toString();
        statusBar()->showMessage(tr("Server käivitatud, ip: %1").arg(ipAadress), 10000);
        connect(server, &ProtolehelugejaServer::save, this, &Protokollitaja::salvesta);
        connect(server, &ProtolehelugejaServer::renewWithTargetNumber, this, &Protokollitaja::uuendaVorkuSifriga);
        connect(server, &ProtolehelugejaServer::shotInfoRead, this, &Protokollitaja::readShotInfo);
    }else QMessageBox::information(this, tr("Teade"), tr("Server töötab, \naadress: %1").arg(ipAadress), QMessageBox::Ok);
}

void Protokollitaja::kasNaitaTul(bool naitamine)
{
        if(naitamine)
                naitaja->start();
        else naitaja->stop();
}

void Protokollitaja::kirjutaFail(QString failiNimi)
{
    QFile fail(failiNimi);
    if(fail.open(QIODevice::WriteOnly)){
        QDataStream valja(&fail);
        valja << quint32(0x00FA3848);	//Kontrollarv
        valja << qint32(113);			//Millise faili versiooniga on tegu
        valja.setVersion(QDataStream::Qt_6_5);

        QJsonObject jsonObj;
        jsonObj["competitionName"] = m_competitionName/*.toUtf8()*/;
        jsonObj["startDate"] = m_startDate.toString();
        jsonObj["endDate"] = m_endDate.toString();
        jsonObj["place"] = m_place/*.toUtf8()*/;
        jsonObj["country"] = m_country;
        jsonObj["webCompetitionId"] = webCompetitionId;
        jsonObj["autoComplete"] = seaded->ui.kirjutusAbiCombo->currentIndex();
        jsonObj["autosaveEnabled"] = seaded->ui.aegCombo->currentIndex();
        jsonObj["autosaveInterval"] = seaded->ui.aegEdit->value();
        jsonObj["tabsLocation"] = seaded->ui.sakiBox->currentIndex();
        jsonObj["tabCount"] = tabWidget->count();	//Töölehtede arv, et pärast teaks mitu lehte on vaja lugeda
        jsonObj["ranking"] = m_ranking;  //Kas järjestamine käib sisekümnetega või viimase seeria järgi
        jsonObj["uploadInterval"] = seaded->ui.uploadTimeBox->value();

        QJsonArray tabsArray;
        for(int i = 0; i < tabWidget->count(); i++){
            QJsonObject tabJson;
            tabJson["name"] = tabWidget->tabText(i)/*.toUtf8()*/;	//Töölehe nimi

            Leht* leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());
            tabJson["seriesCount"] = leht->seeriateArv;
            tabJson["shotsCount"] = leht->laskudeArv;
            tabJson["seriesInSubTotal"] = leht->vSummadeSamm;
            tabJson["autocomplete"] = leht->abi;
            tabJson["displayName"] = leht->ekraaniNimi/*.toUtf8()*/;

            #ifdef QT_DEBUG
                qDebug() << "leht->laskudeArv: " << leht->laskudeArv;
            #endif

            tabJson["weaponType"] = leht->m_targetType;
            tabJson["eventType"] = leht->eventType()/*.toUtf8()*/;
            tabJson["decimals"] = leht->kumnendikega;
            tabJson["toBeShown"] = leht->naidata;
            tabJson["toBeUploaded"] = leht->toBeUploaded();
            tabJson["minTime"] = leht->minTime();
            tabJson["maxTime"] = leht->maxTime();
            tabJson["teamEvent"] = leht->voistk;
            tabJson["tabIndex"] = leht->leheIndeks;

            if(leht->voistk){
                tabJson["teamsCount"] = leht->voistkonnad.count();
                tabJson["followeesCount"] = leht->jalgitavad.count();

                #ifdef QT_DEBUG
                    qDebug() << "leht->jalgitavad.count(): = " << leht->jalgitavad.count();
                #endif

                QJsonArray followeesArray;
                for (int followeeIndex : leht->jalgitavad) {
                    followeesArray.append(followeeIndex);
                }
                tabJson["followees"] = followeesArray;

                QJsonArray teamsArray;
                for (Voistkond *team : leht->voistkonnad) {
                    teamsArray.append(team->toJson());
                }
                tabJson["teams"] = teamsArray;
            } else {
                tabJson["competitorsCount"] = leht->laskurid.count();

                #ifdef QT_DEBUG
                    qDebug() << "leht->laskurid.count(): = " << leht->laskurid.count();
                #endif

                QJsonArray competitorsArray;
                for (int i = 0; i < leht->laskurid.count(); i++) {
                    competitorsArray.append(leht->laskurid[i]->toJson());
                }
                tabJson["competitors"] = competitorsArray;
            }
            tabsArray.append(tabJson);
        }
        jsonObj["tabs"] = tabsArray;
        valja << jsonObj;
        voibSulgeda = true;
        statusBar()->showMessage("Fail salvestatud", 3000);
    } else QMessageBox::critical(this, "Protokollitaja", tr("Ei õnnestu faili luua. Kontrollige kas "
                    "teil on sinna kausta kirjutamise õigus."), QMessageBox::Ok);
}

void Protokollitaja::kontrolliIdKordusi()   //Kuna see funktsioon peaks muutma midagi ainult siis, kui avatakse vana faili, siis lisab see uued ID'd automaatselt
{
    for(int i = 0; i < tabWidget->count(); i++){    //Kõigepealt tuleb leida suurim olemasolev ID, et sealt nummerdamist jätkata
        Leht* leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());

        if(!leht->voistk)   //Ainult individuaalsetel laskuritel on ID
            for(int j = 0; j < leht->laskurid.count(); j++){    //Kõik laskurid igalt lehelt tuleb üle kontrollida
                if(leht->laskurid[j]->id > laskuriId)
                    laskuriId = leht->laskurid[j]->id;
            }
    }

    for(int i = 0; i < tabWidget->count(); i++){    //Kõik lehed tuleb üle kontrollida
        Leht* leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());

        if(!leht->voistk)   //Ainult individuaalsetel laskuritel on ID
            for(int j = 0; j < leht->laskurid.count(); j++){    //Kõik laskurid igalt lehelt tuleb üle kontrollida
                for(int x = 0; x < tabWidget->count(); x++){    //Võrreldes neid kõigil lehtedel olevate kõigi laskuritega
                    Leht* leht2 = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(x))->widget());
                    if(!leht2->voistk)
                        for(int y = 0; y < leht2->laskurid.count(); y++){
                            if(!(i == x && j == y) && leht->laskurid[j]->id == leht2->laskurid[y]->id)
                                leht2->laskurid[y]->id = ++laskuriId;   //Teisele laskurile omistatakse uus ID
                        }
                }
            }
    }
}

void Protokollitaja::kontrolliIdKordust(int uusId, Laskur* las)   //Kontrollib laskuri uue ID varasemat olemasolu
{
    bool olemas = false;
    for(int i = 0; i < tabWidget->count(); i++){    //Kõigepealt tuleb leida suurim olemasolev ID, et sealt nummerdamist jätkata
        Leht* leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());

        if(!leht->voistk)   //Ainult individuaalsetel laskuritel on ID
            for(int j = 0; j < leht->laskurid.count(); j++){    //Kõik laskurid igalt lehelt tuleb üle kontrollida
                if(leht->laskurid[j]->id > laskuriId)
                    laskuriId = leht->laskurid[j]->id;
                if(leht->laskurid[j]->id == uusId && leht->laskurid[j] != las)
                    olemas = true;
            }
    }
    if(!olemas){
        las->id = uusId;
        voibSulgeda = false;
    }else{
        QMessageBox::critical(this, tr("Viga"), "Selline ID on juba olemas!\nID'd ei muudetud!", QMessageBox::Ok);
    }
}

void Protokollitaja::kopeeriLaskurid()
{
    if(tabWidget->count() < 2) return;  //Kui lehti on vähem kui kaks, ei saa kopeerida

    Leht* leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
    int currentIndex = tabWidget->currentIndex();

    if(leht->voistk){
        QMessageBox::critical(this, tr("Viga"), tr("Võistkondade kopeerimine lehtede vahel pole võimalik")
                , QMessageBox::Ok);
        return;
    }

    if(leht->laskurid.count() < 1) return;  //Kui Laskureid ei ole, ei saa kopeerida

    voibSulgeda = false;
    QStringList sakid;
    for(int i = 0; i < tabWidget->count(); i++)
        if (i != currentIndex)  // Exclude current tab
            sakid << tabWidget->tabText(i);

    bool Ok;
    QString valik = QInputDialog::getItem(this, tr("Vali leht, millele laskurid kopeerida"), tr("Töölehe nimi:"), sakid, 0, false, &Ok);

    if(Ok && !valik.isEmpty()){
        for(int i = 0; i < tabWidget->count(); i++)
            if(i != currentIndex && tabWidget->tabText(i) == valik){
                Leht* leht2 = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());
                if(leht2->voistk){
                    QMessageBox::critical(this, tr("Viga"), tr("Laskureid ei saa kopeerida võistkonna lehele"), QMessageBox::Ok);
                    return;
                } else if (leht->leheIndeks == leht2->leheIndeks) {
                    QMessageBox::critical(this, "Protokollitaja", tr("Laskureid ei saa kopeerida samale lehele"), QMessageBox::Ok);
                    return;
                } else if(leht->seeriateArv > leht2->seeriateArv){
                    if(QMessageBox::warning(this, tr("Hoiatus!"), tr("Lehel, kuhu laskureid kopeerida tahetakse, on seeriate arv väiksem! See"
                                                                  "tähendab, et osad seeriad lähevad kaduma.\n\nKas soovite jätkata"), QMessageBox::Yes | QMessageBox::No)
                            == QMessageBox::No)
                        return;
                }
                for(int j = 0; j < leht->laskurid.count(); j++){
                    if(leht->laskurid[j]->linnuke->isChecked()){
                        leht2->uusLaskur(0);
                        leht2->laskurid[leht2->laskurid.count() - 1]->set(leht->laskurid[j]);
                    }
                }
            }
    }
}

void Protokollitaja::kopeeriVah()
{
        if(tabWidget->count() < 1) return;
        Leht* leht = 0;
        if(tabWidget->count() > 0)
                leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
        QString tekst;
        if(leht->voistk){
                for(int i = 0; i < leht->voistkonnad.count(); i++){
                        tekst.append(leht->voistkonnad[i]->nimi->text() + "\t");
                        for(int j = 0; j < leht->seeriateArv; j++){
                                if(j != 0)
                                        tekst.append("\n\t");
                                tekst.append(leht->voistkonnad[i]->voistlejad[j]->eesNimi + "\t");
                                tekst.append(leht->voistkonnad[i]->voistlejad[j]->perekNimi + "\t");
                                tekst.append(leht->voistkonnad[i]->voistlejad[j]->summa + "\t");
                        }
                        tekst.append(leht->voistkonnad[i]->summa->text() + "\t");
                        tekst.append(leht->voistkonnad[i]->markus->text());
                        tekst.append("\n\n");
                }
        }else{
                for(int i = 0; i < leht->laskurid.count(); i++){
                        tekst.append(leht->laskurid[i]->eesNimi->text() + "\t" + leht->laskurid[i]->perekNimi->text() +
                                        + "\t" + leht->laskurid[i]->sunniAasta->text() + "\t" + leht->laskurid[i]->klubi->text()
                                        + "\t");
                        int indeks = 0;
                        for(int j = 0; j < leht->seeriateArv; j++){
                                if(leht->laskurid[i]->seeriad[j]->text().isEmpty())
                                        tekst.append("0\t");
                                else tekst.append(leht->laskurid[i]->seeriad[j]->text() + "\t");
                                if(leht->vSummadeSamm != 0)
                                        if((j + 1) % leht->vSummadeSamm == 0){
                                                tekst.append(leht->laskurid[i]->vSummad[indeks]->text() + "\t");
                                                indeks++;
                                        }
                        }
                        tekst.append(leht->laskurid[i]->getSumma() + "\t");
                        tekst.append(leht->laskurid[i]->kumned->text() + "\t");
                        if(!leht->laskurid[i]->finaal->text().isEmpty()){
                                tekst.append(leht->laskurid[i]->finaal->text() + "\t" /*+ leht->laskurid[i]->koguSumma->text() + "\t"*/);
                        }
                        tekst.append(leht->laskurid[i]->markus->text());
                        tekst.append("\n");
                }
        }
        QClipboard *vahemalu = QApplication::clipboard();
        vahemalu->setText(tekst);
}

void Protokollitaja::kopeeriValitudVah()
{
        if(tabWidget->count() < 1) return;
        Leht* leht = 0;
        if(tabWidget->count() > 0)
                leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
        QString tekst;
        if(leht->voistk){
                for(int i = 0; i < leht->voistkonnad.count(); i++){
                        if(leht->voistkonnad[i]->linnuke->isChecked()){
                                tekst.append(leht->voistkonnad[i]->nimi->text() + "\t");
                                for(int j = 0; j < leht->seeriateArv; j++){
                                        if(j != 0)
                                                tekst.append("\n\t");
                                        tekst.append(leht->voistkonnad[i]->voistlejad[j]->eesNimi + "\t");
                                        tekst.append(leht->voistkonnad[i]->voistlejad[j]->perekNimi + "\t");
                                        tekst.append(leht->voistkonnad[i]->voistlejad[j]->summa + "\t");
                                }
                                tekst.append(leht->voistkonnad[i]->summa->text() + "\t");
                                tekst.append(leht->voistkonnad[i]->markus->text());
                                tekst.append("\n\n");
                        }
                }
        }else{
                for(int i = 0; i < leht->laskurid.count(); i++){
                        if(leht->laskurid[i]->linnuke->isChecked()){
                                tekst.append(leht->laskurid[i]->eesNimi->text() + "\t" + leht->laskurid[i]->perekNimi->
                                                text() + "\t" + leht->laskurid[i]->sunniAasta->text() + "\t" + leht->laskurid[i]->
                                                klubi->text() + "\t");
                                int indeks = 0;
                                for(int j = 0; j < leht->seeriateArv; j++){
                                        if(leht->laskurid[i]->seeriad[j]->text().isEmpty())
                                                tekst.append("0\t");
                                        else tekst.append(leht->laskurid[i]->seeriad[j]->text() + "\t");
                                        if(leht->vSummadeSamm != 0)
                                                if((j + 1) % leht->vSummadeSamm == 0){
                                                        tekst.append(leht->laskurid[i]->vSummad[indeks]->text() + "\t");
                                                        indeks++;
                                                }
                                }
                                tekst.append(leht->laskurid[i]->getSumma() + "\t");
                                tekst.append(leht->laskurid[i]->kumned->text() + "\t");
                                if(!leht->laskurid[i]->finaal->text().isEmpty()){
                                        tekst.append(leht->laskurid[i]->finaal->text() + "\t" /*+ leht->laskurid[i]->koguSumma->text() + "\t"*/);
                                }
                                tekst.append(leht->laskurid[i]->markus->text());
                                tekst.append("\n");
                        }
                }
        }
        QClipboard *vahemalu = QApplication::clipboard();
        vahemalu->setText(tekst);
}

void Protokollitaja::lehedLoetud()
{
    if(lehelugejaAken){
        if(!lehelugejaLaskur->onLehelugejaLaskur){    //Kui vahepeal sorteerimisega on pointer valeks läinud, tuleb leida õige laskur
            for(int i = 0; i < lehelugejaLeht->laskurid.count(); i++){  //Sorteerimine toimub ainult ühel lehel, otsida on vaja seega ainult sealt
                if(lehelugejaLeht->laskurid[i]->onLehelugejaLaskur){
                    lehelugejaLaskur = lehelugejaLeht->laskurid[i];
                }
            }
        }
        if(!lehelugejaLaskur->onLehelugejaLaskur){    //Kui ei õnnestunud leida õige laskur
            logiValja << "#Viga: Lehelugemise laskur kadunud!\n";
            QMessageBox::critical(this, tr("Viga"), tr("Ei leitud laskurid, kellele need tulemused minema oleks pidanud!"), QMessageBox::Ok);
            return;
        }
        for(int i = 0; i < lehelugejaLaskur->seeriateArv; i++){  //Selle laskuri uuendamine, kelle lehti loeti
            lehelugejaLaskur->seeriad[i]->setText(lehelugejaAken->seeriad[i]->text());
            for(int j = 0; j < lehelugejaLaskur->laskudeArv; j++){
                //if(lehelugejaAken->lasud[i][j]->getILask() != -999) //Igal juhul on ilmselt vaja lasud üle kirjutada?
                    lehelugejaLaskur->lasud[i][j]->set(lehelugejaAken->lasud[i][j]);    //Laskude kopeerimine
            }
        }
        lehelugejaLaskur->liida();
        lehelugejaAken->m_ui.summaEdit->setText(lehelugejaLaskur->getSumma());
        muudaSalvestamist();

    }
}

void Protokollitaja::lehelugeja()
{
    if(tabWidget->count() > 0){
        Leht* leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
        lehelugejaLeht = leht;
        if(leht->voistk){
            leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(0))->widget());
            lehelugejaLeht = leht;
        }
        if(lehelugejaLeht->seeriateArv > 6){
            QMessageBox::critical(this, tr("Viga"), tr("Kahjuks see versioon Protokollitajast ei toeta lehelugemist 3x40 lasku standardi harjutustes!"), QMessageBox::Ok);
            return;
        }
//        if(!leht->harjutus.contains(tr("õhu"), Qt::CaseInsensitive) && !leht->harjutus.contains(tr("standard"), Qt::CaseInsensitive) && !leht->harjutus.contains(tr("lamades"), Qt::CaseInsensitive)){
//            QMessageBox::critical(this, tr("Viga"), tr("Kahjuks see versioon Protokollitajast toetab lehelugemist ainult sportpüssi, õhupüssi ja õhupüstoli harjutustes!"), QMessageBox::Ok);
//            return;
//        }
        if(!lehelugejaAken){
            lehelugejaAken = new LehelugejaAken(this, Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
            lehelugejaAken->m_ui.nimeBox->hide();
            lehelugejaAken->m_ui.nimeSilt->hide();
            connect(lehelugejaAken, SIGNAL(uutNime(QString)), this, SLOT(uuendaLehelugejat(QString)));
            connect(lehelugejaAken, SIGNAL(lehedLoetud()), this, SLOT(lehedLoetud()));
            connect(lehelugejaAken, SIGNAL(uutSifrit(int)), this, SLOT(uuendaLehelugejatSifriga(int)));
            connect(lehelugejaAken, SIGNAL(salvesta()), this, SLOT(salvesta()));
        }

        if(!leht->voistk && leht->laskurid.count() > 0){
            lehelugejaAken->m_ui.nimeBox->clear();
            for(int i = 0; i < lehelugejaAken->seeriad.count(); i++)
                lehelugejaAken->seeriad[i]->show();
            for(int i = 0; i < leht->laskurid.count(); i++)
                lehelugejaAken->m_ui.nimeBox->addItem(leht->laskurid[i]->eesNimi->text() + " " + leht->laskurid[i]->perekNimi->text());
            //lehelugejaLeht = leht;
            if(lehelugejaLaskur != 0)
                lehelugejaLaskur->onLehelugejaLaskur = false;   //Vana laskur tuleb "vabastada"
            lehelugejaLaskur = lehelugejaLeht->laskurid[0]; //Pointer laskurile, keda vaja uuendada kui lehed on loetud (hetkel nimekirjas esimene)
            lehelugejaLaskur->onLehelugejaLaskur = true;
            lehelugejaAken->m_ui.sifriEdit->setText(leht->laskurid[0]->sifriAlgus->text());
            if(!leht->laskurid[0]->sifriAlgus->text().isEmpty() && !leht->laskurid[0]->sifriLopp->text().isEmpty())
                lehelugejaAken->m_ui.sifriLabel->setText(leht->laskurid[0]->sifriAlgus->text() + " - " +
                        leht->laskurid[0]->sifriLopp->text());
            if(leht->seeriateArv <= 6){
                for(int i = 0; i < leht->seeriateArv; i++){
                    lehelugejaAken->seeriad[i]->setText(leht->laskurid[0]->seeriad[i]->text());
                }
                lehelugejaAken->m_ui.summaEdit->setText(leht->laskurid[0]->getSumma());
                for(int j = leht->seeriateArv; j < lehelugejaAken->seeriad.count(); j++)
                    lehelugejaAken->seeriad[j]->hide();
            }else{
                QMessageBox::critical(this, tr("Viga"), tr("Liiga palju seeriaid, lugemine ei ole võimalik!"), QMessageBox::Ok);
                return;
            }
        }else{
            QMessageBox::critical(this, tr("Viga"), tr("Ei ole ühtegi laskurit, kelle lehti lugeda!"), QMessageBox::Ok);
            return;
        }
        /*QFile logifail(asukoht.absolutePath() + QString("/Data/Logi %1.log").arg(QDate::currentDate()
                                                                                 .toString(Qt::ISODate)));
            if(logifail.open(QIODevice::ReadOnly | QIODevice::Text)){
                QTextStream sisse(&logifail);
                lehelugejaAken->m_ui.logi->setText(sisse.readAll());
                logifail.close();
            }*/
        uuendaLehelugejat(lehelugejaAken->m_ui.nimeBox->currentText());
        lehelugejaAken->exec();
        QFile logifail(qApp->applicationDirPath() + QString("/Data/Logi %1.log").arg(QDate::currentDate().toString(Qt::ISODate)));
        if(logifail.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
            QTextStream valja(&logifail);
            valja << lehelugejaAken->m_ui.logi->toPlainText() << "\n";
            logifail.close();
        }
    }
}

void Protokollitaja::liiguta()
{
    if(tabWidget->count() < 2) return;  //Kui lehti on vähem kui kaks, ei saa teisaldada

    Leht* leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
    int currentIndex = tabWidget->currentIndex();

    if(leht->voistk){
        QMessageBox::critical(this, "Protokollitaja", tr("Võistkondade liigutamine lehtede vahel pole võimalik"),
                QMessageBox::Ok);
        return;
    }

    if(leht->laskurid.count() < 1) return;  //Kui Laskureid ei ole, ei saa teisaldada

    voibSulgeda = false;
    QStringList sakid;
    for(int i = 0; i < tabWidget->count(); i++)
        if (i != currentIndex)  // Exclude current tab
            sakid << tabWidget->tabText(i);
    bool Ok;
    QString valik = QInputDialog::getItem(this, tr("Vali leht, millele laskurid teisaldada"), tr("Töölehe nimi:"), sakid, 0, false, &Ok);
    if(Ok && !valik.isEmpty()){
        for(int i = 0; i < tabWidget->count(); i++)
            if(i != currentIndex && tabWidget->tabText(i) == valik){
                Leht* leht2 = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());

                if(leht2->voistk){
                    QMessageBox::critical(this, "Protokollitaja", tr("Laskureid ei saa teisaldada võistkonna lehele"), QMessageBox::Ok);
                    return;
                } else if (leht->leheIndeks == leht2->leheIndeks) {
                    QMessageBox::critical(this, "Protokollitaja", tr("Laskureid ei saa teisaldada samale lehele"), QMessageBox::Ok);
                    return;
                } else if(leht->seeriateArv > leht2->seeriateArv){
                    if(QMessageBox::warning(this, tr("Hoiatus!"), tr("Lehel, kuhu laskureid teisaldada tahetakse, on seeriate arv väiksem! See"
                                                                  "tähendab, et osad seeriad lähevad kaduma.\n\nKas soovite jätkata"), QMessageBox::Yes | QMessageBox::No)
                            == QMessageBox::No)
                    return;
                }
                for(int j = 0; j < leht->laskurid.count(); j++){
                    if(leht->laskurid[j]->linnuke->isChecked()){
                        if(!leht->laskurid[j]->onLehelugejaLaskur && /*!leht->laskurid[j]->onVorguLaskur &&*/ leht->laskurid[j]->isEnabled()){
                            leht2->uusLaskur(leht->laskurid[j]->id);
                            leht2->laskurid[leht2->laskurid.count() - 1]->set(leht->laskurid[j]);
                        }else{
                            leht->laskurid[j]->linnuke->setChecked(false);  //Linnuke ära, et seda laskurit ära ei kustutataks
                            QMessageBox teade;
                            teade.setIcon(QMessageBox::Warning);
                            teade.setWindowTitle("Protokollitaja");
                            teade.setText(tr("Ühele valitud laskurile loetakse parasjagu tulemusi, seega ei saa teda teisaldada"));
                            teade.show();
                        }
                    }
                }
                leht->eemaldaLaskur();
            }
    }
}

void Protokollitaja::loeBroadcast()
{
    while (udpSocket->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
//        statusLabel->setText(tr("Received datagram: \"%1\"").arg(datagram.data()));
//        QMessageBox::information(this, tr("Teade"), tr("Saabus broadcastitud datagram: \"%1\"").arg(datagram.data()), QMessageBox::Ok);
        QString rida(datagram);
        if(rida == "Protok?"){
            statusBar()->showMessage(tr("Küsiti IP aadressi"), 3000);
            datagram = "Protok:";
            QList<QHostAddress> ipAadressid = QNetworkInterface::allAddresses();
            //Mõnes arvutis on mitu IPv4 aadressi, saata tuleb kõik, mis ei alga 255 või 127'ga
            for(int i = 0; i < ipAadressid.size(); ++i){
                if (ipAadressid.at(i) != QHostAddress::LocalHost && ipAadressid.at(i).toIPv4Address()){
                   if(!ipAadressid.at(i).toString().startsWith("255.")){
                        datagram.append((ipAadressid.at(i).toString() + ";").toLatin1());
                   }
                }
            }
//            datagram = "Protok:" + QByteArray()
#ifdef QT_DEBUG
            qDebug() << "Vastan broadcastile: " << datagram.data();
#endif
            udpSocket->writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, 45744);
        }
    }
}

void Protokollitaja::loefail()
{
#ifdef QT_DEBUG
    qDebug() << "loefail()";
#endif
    KllFileRW reader(&andmebaas, &kirjutusAbi, &m_ranking, lValik, this, this);
    TabWidgetWithSettings kllData = reader.readKllFile(seeFail, 0);

    if (kllData.tabWidget != nullptr) {
        tabWidget->deleteLater();
        tabWidget = kllData.tabWidget;
        setCentralWidget(kllData.tabWidget);

        m_competitionName = kllData.competitionName;
        m_startDate = kllData.startDate;
        m_endDate = kllData.endDate;
        m_place = kllData.place;
        m_country = kllData.country;
        webCompetitionId = kllData.webCompetitionId;

        seaded->ui.voistluseNimi->setText(m_competitionName);
        seaded->ui.kohtEdit->setText(m_place);
        seaded->ui.startDateEdit->setDate(m_startDate);
        seaded->ui.endDateEdit->setDate(m_endDate);
        seaded->ui.kirjutusAbiCombo->setCurrentIndex(kllData.autoComplete);
        seaded->ui.aegCombo->setCurrentIndex(kllData.autosaveEnabled);
        seaded->ui.aegEdit->setValue(kllData.autosaveInterval);
        seaded->ui.sakiBox->setCurrentIndex(kllData.tabLocation);
        seaded->ui.jarjestamiseBox->setCurrentIndex(m_ranking);
        seaded->ui.uploadTimeBox->setValue(kllData.uploadInterval);

        if(tabWidget->count() > 0) {
            for (int i = 0; i < tabWidget->count(); i++) {
                Leht* leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());
                connect(leht, SIGNAL(uuendaLiikmeid()), this, SLOT(uuendaLiikmeteKast()));
                connect(leht, SIGNAL(uuendaVoistkondi()), this, SLOT(uuendaVoistkondi()));
                connect(leht, SIGNAL(muudatus()), this, SLOT(muudaSalvestamist()));
                connect(leht, SIGNAL(idMuutus(int,Laskur*)), this, SLOT(kontrolliIdKordust(int,Laskur*)));
                leheIndeks = leht->leheIndeks;
            }
        } else {
            uusTab();
        }
        uuendaSeaded();
    }

#ifdef QT_DEBUG
    qDebug() << "loefail() lõpp";
#endif
}

void Protokollitaja::readFinalsFile(QString fileName)
{
#ifdef QT_DEBUG
    qDebug() << "readFinalsFile fileName: " << fileName;
#endif
    QFile file(fileName);
    QJsonDocument fileJson;
    QJsonObject jsonObj;

    if(file.open(QIODevice::ReadOnly)){
        fileJson = QJsonDocument::fromJson(file.readAll());
        jsonObj = fileJson.object();

        if(jsonObj["fileVersion"].toInt() > 301)
            QMessageBox::warning(this, tr("Viga!"), tr("Faili versioon on uuem, kui see versioon programmist. Faili avamisel võib tekkida vigu!"), QMessageBox::Ok);

        if(!(jsonObj.contains("relays") && jsonObj["relays"].isArray())) {
            QMessageBox::critical(this, tr("Viga!"), tr("Finaali fail vigane või ilma tulemusteta!"));
        } else {
            Leht* currentSheet = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
            int addedCount = 0;
            for (const QJsonValue &relayJson: jsonObj["relays"].toArray()) {
                QJsonObject relayObj = relayJson.toObject();
                if (relayObj["teams"].isArray()) {
                    for (const QJsonValue &teamJson: relayObj["teams"].toArray()) {
                        QJsonObject teamObj = teamJson.toObject();
                        if (teamObj["membersInTeam"].isArray()) {
                            for (const QJsonValue &competitorJson: teamObj["membersInTeam"].toArray()) {
                                QJsonObject competitorObj = competitorJson.toObject();
                                if (competitorObj.contains("id") && competitorObj["id"].isDouble() &&
                                    competitorObj.contains("nameEdit") && competitorObj["nameEdit"].isString() &&
                                    competitorObj.contains("series") && competitorObj["series"].isArray()) {

                                    QString id = QString("%1").arg(competitorObj["id"].toInt());
                                    QString name = competitorObj["nameEdit"].toString().toUpper();
                                    QJsonObject seriesObj = competitorObj["series"].toArray().last().toObject();
                                    if (seriesObj.contains("Sum") && seriesObj["Sum"].isString()) {
                                        QString finalsResult = seriesObj["Sum"].toString();

                                        if (name.at(1) == '.') {
                                            name.remove(0, 2);
                                            name = name.trimmed();
                                        } else if(name.at(name.length() - 1) == '.') {
                                            name.remove(name.length() - 3, 3);
                                            name = name.trimmed();
                                        }
#ifdef QT_DEBUG
                                        qDebug() << "Finals name: " << name << "result: " << finalsResult;
#endif
                                        foreach(Laskur* currentCompetitor, currentSheet->laskurid) {
#ifdef QT_DEBUG
                                            qDebug() << "currentCompetitor.name: " << currentCompetitor->perekNimi->text() << ", id: " << currentCompetitor->id << ", finals name: " << name << ", id: " << id;
#endif
                                            if(currentCompetitor->perekNimi->text().trimmed() == name && id.endsWith(QString("%1").arg(currentCompetitor->id))) {
                                                currentCompetitor->finaal->setText(finalsResult);
                                                addedCount++;
                                                currentCompetitor->liida();
                                            }
                                        }
                                    } else {
                                        QMessageBox::critical(this, tr("Viga!"), tr("No Sum in series!"));
                                    }
                                } else {
                                    QMessageBox::critical(this, tr("Viga!"), tr("Faulty competitorObj!"));
                                }
                            }
                        } else {
                            QMessageBox::critical(this, tr("Viga!"), tr("membersInTeam not an array!"));
                        }
                    }
                } else {
                    QMessageBox::critical(this, tr("Viga!"), tr("Teams not an array!"));
                }
            }
            if(addedCount < 1)
                QMessageBox::warning(this, tr("Protokollitaja"), tr("Ei leidnud ühelegi laskurile finaali tulemust. Kontrollige faili õigsust."), QMessageBox::Ok);
            else {
                voibSulgeda = false;
                currentSheet->setFinalsData(jsonObj);
                QMessageBox::information(this, tr("Protokollitaja"), tr("Lisatud %1'le laskurile finaali tulemus.").arg(addedCount), QMessageBox::Ok);
            }
        }
    } else {
        QMessageBox::critical(this, tr("Viga!"), tr("Faili avamine ei ole võimalik!\n\nAsukoht: ") + file.fileName(), QMessageBox::Ok);
    }
}

void Protokollitaja::margi()
{
        if(tabWidget->count() < 1) return;
        if(tabWidget->count() > 0){
                Leht* seeLeht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->
                                widget());
                if(!seeLeht->laskurid.isEmpty())
                        for(int i = 0; i < seeLeht->laskurid.count(); i++)
                                seeLeht->laskurid[i]->linnuke->setChecked(true);
                if(!seeLeht->voistkonnad.isEmpty())
                        for(int i = 0; i < seeLeht->voistkonnad.count(); i++)
                                seeLeht->voistkonnad[i]->linnuke->setChecked(true);
        }
}

void Protokollitaja::muudaSalvestamist()
{
    voibSulgeda = false;
}

void Protokollitaja::muudaTab(const QModelIndex &indeks)
{
        QString uusNimi;
        bool ok = false;
        switch(indeks.column()){
        case 0: {
                uusNimi = QInputDialog::getText(this, tr("Sisestage uus lehe nimi"), tr("Töölehe nimi:"),
                        QLineEdit::Normal, seaded->ui.sakid->currentItem()->text(0), &ok);
                break;
        }
        case 1: {
                uusNimi = QInputDialog::getText(this, tr("Sisestage uus ekraaninimi"), tr("Ekraaninimi:"),
                                        QLineEdit::Normal, seaded->ui.sakid->currentItem()->text(1), &ok);
                break;
        }
        case 2: {
                uusNimi = QString("%1").arg(QInputDialog::getInt(this, tr("Sisestage uus min näitamise aeg"),
                                tr("Aeg sekundites:"), seaded->ui.sakid->currentItem()->text(2).toInt(),
                                1, seaded->ui.sakid->currentItem()->text(3).toInt(), 1, &ok));
                break;
        }
        case 3: {
                uusNimi = QString("%1").arg(QInputDialog::getInt(this, tr("Sisestage uus max näitamise aeg"),
                                tr("Aeg sekundites:"), seaded->ui.sakid->currentItem()->text(3).toInt(),
                                seaded->ui.sakid->currentItem()->text(2).toInt(), 99, 1, &ok));
                break;
        }
        case 5: {
                int indeks = 0;
                for(int i = 0; i < TargetTypes::targetNames().size(); i++)
                    if(TargetTypes::targetNames().at(i) == seaded->ui.sakid->currentItem()->text(5))
                        indeks = i;
                uusNimi = QInputDialog::getItem(this, tr("Valige uus märkleht"), tr("Märkleht:"), TargetTypes::targetNames(), indeks, false, &ok);
                break;
        }
        case 6: {
                int indeks = 0;
                for(int i = 0; i < QualificationEvents::eventNames().count(); i++)
                    if(QualificationEvents::eventNames().at(i) == seaded->ui.sakid->currentItem()->text(6))
                        indeks = i;
                uusNimi = QInputDialog::getItem(this, tr("Sisestage uus harjutus"), tr("Harjutus:"), QualificationEvents::eventNames(), indeks, false, &ok);
                break;
        }
        }
        if(!uusNimi.isEmpty() && ok){
                seaded->ui.sakid->currentItem()->setText(indeks.column(), uusNimi);
        }
        voibSulgeda = false;
}

void Protokollitaja::naitaInfot()
{
        QMessageBox::about(this, programmiNimi, "Versioon: " + versioon + "\n\n" + programmiNimi + tr(" on "
                        "vabavaraline programm, mis on loodud kasutamiseks laskevõistlustel protokollide tegemiseks "
                        "ning tulemuste näitamiseks. Kasutamine omal vastutusel. Autor ei võta endale mingit vastutust "
                        "kahjude eest, mis programmi kasutamisega võivad kaasneda! Tegu on beta versiooniga ehk siis "
                        "alles katsetamisjärgus oleva programmiga.\n\nMuidu kasutage, seda nii kuidas ise heaks "
                        "arvate. Välja arvatud programmi looja maine ning heaolu kahjustamise või rahalise kasu "
                        "saamise eesmärkidel. Samuti pole kasutamine lubatud juhul, kui autor on selle ära keelanud!"
                        "\n\nTeadmiseks teile, et seda programmi on vahelduva eduga autori vabast ajast arendatud juba "
                        "aastast 2008 ning Finaali programmi, koos vanemate versioonidega aastast 2007!\n\nKüsimused, "
                        "ettepanekud, leitud vead, arvamused jms võib saata allolevale e-postile\n\nAutor: Ümeramees\n"
                        "ymeramees@gmail.com\nTallinn ") + aasta + "\n\nQt versioon:\n\tCompile: " + QT_VERSION_STR +
                        "\n\tRun time: " + qVersion());
}

void Protokollitaja::naitaSeaded()
{
    // From new settings class
    seaded->ui.sighterTypesEdit->setText(m_settings.sighterShotTypesString());
    seaded->ui.competitionTypesEdit->setText(m_settings.competitionShotTypesString());

        seaded->ui.voistluseNimi->setText(m_competitionName);
        seaded->ui.startDateEdit->setDate(m_startDate);
        seaded->ui.endDateEdit->setDate(m_endDate);
//        seaded->voistluseNimi = m_competitionName;
        seaded->ui.kohtEdit->setText(m_place);
//        seaded->aegKoht = aegKoht;
        seaded->ui.sakiBox->setCurrentIndex(tabWidget->tabPosition());
//        seaded->sakiAsukoht = tabWidget->tabPosition();
        if(salvestaja->isActive())
            seaded->ui.aegCombo->setCurrentIndex(1);
        else seaded->ui.aegCombo->setCurrentIndex(0);
//        seaded->autosave = seaded->ui.aegCombo->currentIndex();
        seaded->ui.aegEdit->setValue(salvestaja->interval() / 60000);
//        seaded->aeg = seaded->ui.aegEdit->value();
        if(kirjutusAbi)
            seaded->ui.kirjutusAbiCombo->setCurrentIndex(1);
        else seaded->ui.kirjutusAbiCombo->setCurrentIndex(0);
//        seaded->kirjutusAbi = seaded->ui.kirjutusAbiCombo->currentIndex();
        seaded->ui.jarjestamiseBox->setCurrentIndex(m_ranking);

        seaded->ui.uploadTimeBox->setValue(uploadTimer.interval() / 1000);

        seaded->ui.sakid->clear();
        for(int i = 0; i < tabWidget->count(); i++){
                QStringList sakiAndmed;
                sakiAndmed << tabWidget->tabText(i);
                Leht* leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());
                sakiAndmed << leht->ekraaniNimi << QString("%1").arg(leht->minTime() / 1000) << QString("%1")
                                .arg(leht->maxTime() / 1000);
                QTreeWidgetItem *item = new QTreeWidgetItem(seaded->ui.sakid, sakiAndmed);
                if(leht->naidata)
                        item->setCheckState(4, Qt::Checked);
                else item->setCheckState(4, Qt::Unchecked);
                item->setText(5, TargetTypes::targetNames().at(leht->m_targetType));
                item->setText(6, QualificationEvents::eventNames().at(leht->eventType()));

                if(leht->kumnendikega)
                    item->setCheckState(7, Qt::Checked);
                else item->setCheckState(7, Qt::Unchecked);

                if(leht->toBeUploaded())
                    item->setCheckState(8, Qt::Checked);
                else item->setCheckState(8, Qt::Unchecked);

                seaded->ui.sakid->insertTopLevelItem(-1, item);
        }
        seaded->show();
}

void Protokollitaja::naitaTul()
{
    if(verbose)
        QTextStream(stdout) << "Protokollitaja::naitaTul()" << Qt::endl;
    static int leheNr = 0;
    static int reaNr = 0;
    static int areaNr = 0;
    static bool naidatud = false;
    tulemus->ind = true;
    if(tulemus->windowState() == Qt::WindowMaximized)
        tulemus->fullScreen();
        //tulemus->loplik = false;
    if(areaNr > reaNr){
        areaNr = 0;
        reaNr = 0;
    }

    if(tabWidget->count() > leheNr){
        if(leheNr >= 0){
            Leht *seeLeht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(leheNr))->widget());
            while(!seeLeht->naidata){
                leheNr++;
                if(leheNr >= tabWidget->count())
                    leheNr = 0;
                reaNr = 0;
                areaNr = 0;
                naidatud = false;
                seeLeht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(leheNr))->widget());
            }
            if(seeLeht->voistk){
                tulemus->ind = false;
                if(seeLeht->voistkonnad.count() < 1 || seeLeht->voistkonnad.count() < areaNr || seeLeht->voistkonnad.count() < reaNr){
                    leheNr++;
                    areaNr = 0;
                    reaNr = 0;
                    naitaja->setInterval(100);  //Et kiiremini uut lehte näitaks, muidu jääb vahepeal paus väga pikaks
                    return;
                }

                uuendaVoistkondi();

                seeLeht->sorteeri(0);

                if(seeLeht->voistkonnad.count() > 25){
                    if(seeLeht->voistkonnad.count() - reaNr > 25){
                        areaNr = reaNr;
                        reaNr += 25;
                        naidatud = false;
                        naitaja->setInterval(seeLeht->maxTime());
                    }else{
                        areaNr = reaNr;
                        reaNr = seeLeht->voistkonnad.count();
                        naidatud = true;
                        naitaja->setInterval(seeLeht->minTime() + 2000*(reaNr-areaNr));
                        leheNr++;
                    }

                }else{
                    areaNr = reaNr;
                    reaNr = seeLeht->voistkonnad.count();
                    naidatud = true;
                    naitaja->setInterval(10000 + 2000*(reaNr-areaNr));
                    leheNr++;
                }
                if(naitaja->interval() > seeLeht->maxTime()) naitaja->setInterval(seeLeht->maxTime());
//                if(reaNr > 25) i = (reaNr - 25);
                tulemus->voistluseNimi = m_competitionName;
                tulemus->aegKoht = timeAndPlaceString();
                tulemus->pealKiri = seeLeht->ekraaniNimi;
                for(int j = 0; j < tulemus->getRidadeArv(); j++)
                    for(int k = 0; k < 16; k++)
                        tulemus->read[j][k] = " ";
                if(seeLeht->voistkonnad.count() > 1 && (reaNr - areaNr) > 1){
                    for(int i = areaNr; i < reaNr; i++){
                        tulemus->read[i-areaNr][0] = QString("%1.").arg(i + 1);
                        tulemus->read[i-areaNr][1] = seeLeht->reasVoistkonnad[i]->nimi->text();
                        //tulemus->read[i-areaNr][2] = seeLeht->reasVoistkonnad[i]->perekNimi->text();
                        //tulemus->read[i-areaNr][3] = seeLeht->reasVoistkonnad[i]->sunniAasta->text();
                        //tulemus->read[i-areaNr][4] = seeLeht->reasVoistkonnad[i]->klubi->text();
                        for(int j = 0; j < seeLeht->seeriateArv && j < 8; j++)
                            tulemus->read[i-areaNr][3 + j] = seeLeht->reasVoistkonnad[i]->voistlejad[j]->summa;
                        tulemus->read[i-areaNr][11] = seeLeht->reasVoistkonnad[i]->summa->text();
                        if(seeLeht->reasVoistkonnad[i]->markus->text().isEmpty()){
                            tulemus->read[i-areaNr][12] = QString("%1").arg(float(seeLeht->reasVoistkonnad[i]->keskmLask) / 10);
                            tulemus->read[i-areaNr][12].replace('.', ',');
                        }else tulemus->read[i-areaNr][12] = seeLeht->reasVoistkonnad[i]->markus->text();
                    }
                }else{
                    tulemus->read[0][0] = QString("%1.").arg(areaNr + 1);
                    tulemus->read[0][1] = seeLeht->reasVoistkonnad[areaNr]->nimi->text();
                    //tulemus->read[0][2] = seeLeht->reasVoistkonnad[areaNr]->perekNimi->text();
                    //tulemus->read[0][3] = seeLeht->reasVoistkonnad[areaNr]->sunniAasta->text();
                    //tulemus->read[0][4] = seeLeht->reasVoistkonnad[areaNr]->klubi->text();
                    for(int j = 0; j < seeLeht->seeriateArv && j < 8; j++)
                        tulemus->read[0][3 + j] = seeLeht->reasVoistkonnad[areaNr]->voistlejad[j]->summa;
                    tulemus->read[0][11] = seeLeht->reasVoistkonnad[areaNr]->summa->text();
                    if(seeLeht->reasVoistkonnad[areaNr]->markus->text().isEmpty()){
                        tulemus->read[0][12] = QString("%1").arg(float(seeLeht->reasVoistkonnad[areaNr]->keskmLask) / 10);
                        tulemus->read[0][12].replace('.', ',');
                    }else tulemus->read[0][12] = seeLeht->voistkonnad[areaNr]->markus->text();
                }
                tulemus->joonista();
                if(naidatud){
                    reaNr = 0;
                    naidatud = false;
                }
                if(leheNr >= tabWidget->count()) leheNr = 0;

                return;
            }
            if(seeLeht->laskurid.count() < 1 || seeLeht->laskurid.count() < areaNr || seeLeht->laskurid.count() < reaNr){
                leheNr++;
                areaNr = 0;
                reaNr = 0;
                naitaja->setInterval(100);  //Et kiiremini uut lehte näitaks, muidu jääb vahepeal paus väga pikaks
                return;
            }
            if(verbose)
                QTextStream(stdout) << "Protokollitaja::naitaTul(): individual" << Qt::endl;
            bool kasOnRajaNr = false;
            bool kasOnSummasid = false;
            for(int i = 0; i < seeLeht->laskurid.count(); i++){
                if(seeLeht->laskurid[i]->getSumma() != "0" && seeLeht->laskurid[i]->getSumma() != "0,0")
                    kasOnSummasid = true;
                if(!seeLeht->laskurid[i]->rajaNr->text().isEmpty())
                    kasOnRajaNr = true;
            }
            if(kasOnRajaNr && !kasOnSummasid){   //Kui on raja numbreid ja ei ole veel tulemusi, siis sorteerida rajanumbrite järgi
                if(verbose)
                    QTextStream(stdout) << "Protokollitaja::naitaTul(): Sorting by target numbers" << Qt::endl;
                seeLeht->sorteeri(3);
            }else{
                if(verbose)
                    QTextStream(stdout) << "Protokollitaja::naitaTul(): Sorting by results" << Qt::endl;
                seeLeht->sorteeri(0);
            }
            if(verbose)
                QTextStream(stdout) << "Protokollitaja::naitaTul(): Sorted" << Qt::endl;

            int tulemusegaArv = 0;
            for(int i = 0; i < seeLeht->laskurid.count(); i++)
                if(seeLeht->laskurid[i]->getSumma() != "0" && seeLeht->laskurid[i]->getSumma() != "0,0")
                    tulemusegaArv++;
            if(tulemusegaArv == 0)
                tulemusegaArv = seeLeht->laskurid.count();
            if(tulemusegaArv > 25){
                if(tulemusegaArv - reaNr > 25){
                    areaNr = reaNr;
                    reaNr += 25;
                    naidatud = false;
                    naitaja->setInterval(seeLeht->maxTime());
                }else{
                    areaNr = reaNr;
                    reaNr = tulemusegaArv;
                    naidatud = true;
                    naitaja->setInterval(seeLeht->minTime() + 2000*(reaNr-areaNr));
                    leheNr++;
                }

            }else{
                areaNr = reaNr;
                reaNr = tulemusegaArv;
                naidatud = true;
                naitaja->setInterval(10000 + 2000*(reaNr-areaNr));
                leheNr++;
            }
            if(verbose)
                QTextStream(stdout) << "Protokollitaja::naitaTul(): vahepunkt" << Qt::endl;
            if(naitaja->interval() > seeLeht->maxTime()) naitaja->setInterval(seeLeht->maxTime());
            //if(reaNr > 25) i = (reaNr - 25);
            tulemus->voistluseNimi = m_competitionName;
            tulemus->aegKoht = timeAndPlaceString();
            tulemus->pealKiri = seeLeht->ekraaniNimi;
            for(int j = 0; j < tulemus->getRidadeArv(); j++)
                for(int k = 0; k < 16; k++)
                    tulemus->read[j][k] = " ";
            if(seeLeht->seeriateArv <= 6 && seeLeht->vSummadeSamm == 0){ //Ilma vahesummadeta harjutused
                if(tulemusegaArv > 1 && (reaNr - areaNr) > 1){  //Kui on mitu laskurit lehel
                    if(verbose)
                        QTextStream(stdout) << "Protokollitaja::naitaTul(): tulemusegaArv > 1 && (reaNr - areaNr) > 1" << Qt::endl;
                    for(int i = areaNr; i < reaNr; i++){
                        tulemus->read[i-areaNr][0] = QString("%1.").arg(i + 1);
                        tulemus->read[i-areaNr][1] = seeLeht->reasLaskurid[i]->eesNimi->text();
                        tulemus->read[i-areaNr][2] = seeLeht->reasLaskurid[i]->perekNimi->text();
                        tulemus->read[i-areaNr][3] = seeLeht->reasLaskurid[i]->sunniAasta->text();
                        tulemus->read[i-areaNr][4] = seeLeht->reasLaskurid[i]->klubi->text();
                        for(int j = 0; j < seeLeht->seeriateArv && j < 6; j++)
                            tulemus->read[i-areaNr][7 + j] = seeLeht->reasLaskurid[i]->seeriad[j]->text();
                        tulemus->read[i-areaNr][14] = seeLeht->reasLaskurid[i]->getSumma();
                        if(seeLeht->reasLaskurid[i]->markus->text().isEmpty() || seeLeht->reasLaskurid[i]->markus->text().contains("V.A", Qt::CaseInsensitive)){
                            tulemus->read[i-areaNr][15] = QString("%1").arg(float(seeLeht->reasLaskurid[i]->keskmLask) / 10000);
                            tulemus->read[i-areaNr][15].replace('.', ',');
                        }else tulemus->read[i-areaNr][15] = seeLeht->reasLaskurid[i]->markus->text();
                        if(seeLeht->reasLaskurid[i]->markus->text().contains("V.A", Qt::CaseInsensitive))
                            tulemus->read[i-areaNr][0] = "v.a.";
                        else if(seeLeht->reasLaskurid[i]->markus->text().contains("DNF", Qt::CaseInsensitive))
                            tulemus->read[i-areaNr][0] = "DNF";
                        else if(seeLeht->reasLaskurid[i]->markus->text().contains("DSQ", Qt::CaseInsensitive))
                            tulemus->read[i-areaNr][0] = "DSQ";
                        if(tulemus->loplik)
                            tulemus->read[i-areaNr][15] = "";
                        if(!seeLeht->reasLaskurid[i]->finaal->text().isEmpty()){
                            //Enam kogusummat koos finaaliga ei näidata:
                            tulemus->read[i-areaNr][15] = seeLeht->reasLaskurid[i]->finaal->text()/* + " " + seeLeht->reasLaskurid[i]->koguSumma->text()*/;
                            tulemus->loplik = true;
                        }
                    }
                }else{  //Kui on ainult üks laskur lehel
                    if(verbose)
                        QTextStream(stdout) << "Protokollitaja::naitaTul(): One shooter on the page" << Qt::endl;
                    tulemus->read[0][0] = QString("%1.").arg(areaNr + 1);
                    tulemus->read[0][1] = seeLeht->reasLaskurid[areaNr]->eesNimi->text();
                    tulemus->read[0][2] = seeLeht->reasLaskurid[areaNr]->perekNimi->text();
                    tulemus->read[0][3] = seeLeht->reasLaskurid[areaNr]->sunniAasta->text();
                    tulemus->read[0][4] = seeLeht->reasLaskurid[areaNr]->klubi->text();
                    for(int j = 0; j < seeLeht->seeriateArv && j < 6; j++)
                        tulemus->read[0][7 + j] = seeLeht->reasLaskurid[areaNr]->seeriad[j]->text();
                    tulemus->read[0][14] = seeLeht->reasLaskurid[areaNr]->getSumma();
                    if(seeLeht->reasLaskurid[areaNr]->markus->text().isEmpty() || seeLeht->reasLaskurid[areaNr]->markus->text().contains("V.A", Qt::CaseInsensitive)){
                        tulemus->read[0][15] = QString("%1").arg(float(seeLeht->reasLaskurid[areaNr]->keskmLask)	/ 10000);
                        tulemus->read[0][15].replace('.', ',');
                    }else tulemus->read[0][15] = seeLeht->laskurid[areaNr]->markus->text();
                    if(seeLeht->reasLaskurid[areaNr]->markus->text().contains("V.A", Qt::CaseInsensitive))
                        tulemus->read[0][0] = "v.a.";
                    else if(seeLeht->reasLaskurid[areaNr]->markus->text().contains("DNF", Qt::CaseInsensitive))
                        tulemus->read[0][0] = "DNF";
                    else if(seeLeht->reasLaskurid[areaNr]->markus->text().contains("DSQ", Qt::CaseInsensitive))
                        tulemus->read[0][0] = "DSQ";
                    if(tulemus->loplik)
                        tulemus->read[0][15] = "";
                    if(!seeLeht->reasLaskurid[areaNr]->finaal->text().isEmpty()){
                        //Enam kogusummat koos finaaliga ei näidata:
                        tulemus->read[0][15] = seeLeht->reasLaskurid[areaNr]->finaal->text()/* + " " + seeLeht->reasLaskurid[areaNr]->koguSumma->text()*/;
                        tulemus->loplik = true;
                    }
                }
            }else{	//kui on tegu vahesummadega, mis muidu ekraanile ära ei mahu
                if(seeLeht->vSummadeSamm == 0){
                    QMessageBox::critical(this, tr("Viga!"), tr("Mis harjutus see nii suure seeriate arvuga on ja seejuures ilma vahesummadeta? Selle näitamine ei ole võimalik."), QMessageBox::Ok);
                    return;
                }
                tulemus->mitmeJarel = seeLeht->vSummadeSamm;
                enum{ Lamades = 0, Pusti = 1, Polvelt = 2 };
                int asend = Lamades;
                for(int x = 0; x < seeLeht->reasLaskurid.count(); x++)
                    for(int i = 0; i < seeLeht->seeriateArv; i += seeLeht->vSummadeSamm)
                        if(seeLeht->reasLaskurid[x]->seeriad[i]->text().toInt() != 0)
                            if(asend < i / seeLeht->vSummadeSamm)
                                asend = i / seeLeht->vSummadeSamm;
                if(tulemusegaArv > 1 && (reaNr - areaNr) > 1){
                    for(int i = areaNr; i < reaNr; i++){
                        tulemus->read[i-areaNr][0] = QString("%1.").arg(i + 1);
                        tulemus->read[i-areaNr][1] = seeLeht->reasLaskurid[i]->eesNimi->text();
                        tulemus->read[i-areaNr][2] = seeLeht->reasLaskurid[i]->perekNimi->text();
                        tulemus->read[i-areaNr][3] = seeLeht->reasLaskurid[i]->sunniAasta->text();
                        tulemus->read[i-areaNr][4] = seeLeht->reasLaskurid[i]->klubi->text();
                        if(seeLeht->seeriateArv > 6){
                            switch(asend){
                            case Lamades : {
                                for(int v = 0; v < seeLeht->vSummadeSamm; v++)
                                    tulemus->read[i-areaNr][9 + v] = seeLeht->reasLaskurid[i]->seeriad[v]->text();
                                tulemus->read[i-areaNr][13] = seeLeht->reasLaskurid[i]->vSummad[0]->text();
                                break;
                            }
                            case Pusti : {
                                tulemus->read[i-areaNr][6] = seeLeht->reasLaskurid[i]->vSummad[0]->text();
                                for(int v = 0; v < seeLeht->vSummadeSamm; v++)
                                    tulemus->read[i-areaNr][9 + v] = seeLeht->reasLaskurid[i]->seeriad[v + seeLeht->vSummadeSamm]->text();
                                tulemus->read[i-areaNr][13] = seeLeht->reasLaskurid[i]->vSummad[1]->text();
                                break;
                            }
                            case Polvelt : {
                                tulemus->read[i-areaNr][6] = seeLeht->reasLaskurid[i]->vSummad[0]->text();
                                tulemus->read[i-areaNr][7] = seeLeht->reasLaskurid[i]->vSummad[1]->text();
                                for(int v = 0; v < seeLeht->vSummadeSamm; v++)
                                    tulemus->read[i-areaNr][9 + v] = seeLeht->reasLaskurid[i]->seeriad[v + (asend * seeLeht->vSummadeSamm)]->text();
                                tulemus->read[i-areaNr][13] = seeLeht->reasLaskurid[i]->vSummad[2]->text();
                                break;
                            }
                            }
                            /*for(int v = 0; v < seeLeht->vSummadeSamm; v++)
                                tulemus->read[i-areaNr][6 + v] = seeLeht->reasLaskurid[i]->seeriad[v]->text();
                            tulemus->read[i-areaNr][5 + seeLeht->vSummadeSamm] = seeLeht->reasLaskurid[i]->vSummad[0]->text();
                            QMessageBox::information(this, "Viga!", seeLeht->reasLaskurid[i]->vSummad[0]->text(), QMessageBox::Ok);
                            for(int v = 1; v < seeLeht->laskurid[i-areaNr]->vSummad.count(); v++)
                                if(seeLeht->reasLaskurid[i]->vSummad[v]->text().toInt() == 0){
                                    tulemus->read[i-areaNr][5 + v] = seeLeht->reasLaskurid[i]->vSummad[v - 1]->text();
                                    for(int s = 0; s < seeLeht->vSummadeSamm; s++)
                                        tulemus->read[i-areaNr][5 + s + v] = seeLeht->reasLaskurid[i]->seeriad[s + seeLeht->vSummadeSamm * v]->text();
                                }*/
                        }else{
                            int s = 0;
                            for(int j = 0; j < seeLeht->seeriateArv; j++){
                                tulemus->read[i-areaNr][5 + j + s] = seeLeht->reasLaskurid[i]->seeriad[j]->text();
                                if((j + 1) % seeLeht->vSummadeSamm == 0){
                                    s++;
                                    tulemus->read[i-areaNr][5 + j + s] = seeLeht->reasLaskurid[i]->vSummad[j / seeLeht->vSummadeSamm]->text();
                                }
                            }
                        }
                        tulemus->read[i-areaNr][14] = seeLeht->reasLaskurid[i]->getSumma();
                        if(seeLeht->reasLaskurid[i]->markus->text().isEmpty() || seeLeht->reasLaskurid[i]->markus->text().contains("V.A", Qt::CaseInsensitive)){
                            tulemus->read[i-areaNr][15] = QString("%1").arg(float(seeLeht->reasLaskurid[i]->keskmLask) / 10000);
                            tulemus->read[i-areaNr][15].replace('.', ',');
                        }else tulemus->read[i-areaNr][15] = seeLeht->reasLaskurid[i]->markus->text();
                        if(seeLeht->reasLaskurid[i]->markus->text().contains("V.A", Qt::CaseInsensitive))
                            tulemus->read[i-areaNr][0] = "v.a.";
                        else if(seeLeht->reasLaskurid[i]->markus->text().contains("DNF", Qt::CaseInsensitive))
                            tulemus->read[i-areaNr][0] = "DNF";
                        else if(seeLeht->reasLaskurid[i]->markus->text().contains("DSQ", Qt::CaseInsensitive))
                            tulemus->read[i-areaNr][0] = "DSQ";
                        if(tulemus->loplik)
                            tulemus->read[i-areaNr][15] = "";
                        if(!seeLeht->reasLaskurid[i]->finaal->text().isEmpty()){
                            //Enam kogusummat koos finaaliga ei näidata:
                            tulemus->read[i-areaNr][15] = seeLeht->reasLaskurid[i]->finaal->text() /*+ " " +
                                    seeLeht->reasLaskurid[i]->koguSumma->text()*/;
                            tulemus->loplik = true;
                        }

                    }
                }else{
                    tulemus->read[0][0] = QString("%1.").arg(areaNr + 1);
                    tulemus->read[0][1] = seeLeht->reasLaskurid[areaNr]->eesNimi->text();
                    tulemus->read[0][2] = seeLeht->reasLaskurid[areaNr]->perekNimi->text();
                    tulemus->read[0][3] = seeLeht->reasLaskurid[areaNr]->sunniAasta->text();
                    tulemus->read[0][4] = seeLeht->reasLaskurid[areaNr]->klubi->text();
                    if(seeLeht->seeriateArv > 6){
                        switch(asend){
                        case Lamades : {
                            for(int v = 0; v < seeLeht->vSummadeSamm; v++)
                                tulemus->read[0][9 + v] = seeLeht->reasLaskurid[areaNr]->seeriad[v]->text();
                            tulemus->read[0][13] = seeLeht->reasLaskurid[areaNr]->vSummad[0]->text();
                            break;
                        }
                        case Pusti : {
                            tulemus->read[0][6] = seeLeht->reasLaskurid[areaNr]->vSummad[0]->text();
                            for(int v = 0; v < seeLeht->vSummadeSamm; v++)
                                tulemus->read[0][9 + v] = seeLeht->reasLaskurid[areaNr]->seeriad[v + seeLeht->vSummadeSamm]->text();
                            tulemus->read[0][13] = seeLeht->reasLaskurid[areaNr]->vSummad[1]->text();
                            break;
                        }
                        case Polvelt : {
                            tulemus->read[0][6] = seeLeht->reasLaskurid[areaNr]->vSummad[0]->text();
                            tulemus->read[0][7] = seeLeht->reasLaskurid[areaNr]->vSummad[1]->text();
                            for(int v = 0; v < seeLeht->vSummadeSamm; v++)
                                tulemus->read[0][9 + v] = seeLeht->reasLaskurid[areaNr]->seeriad[v + (asend * seeLeht->vSummadeSamm)]->text();
                            tulemus->read[0][13] = seeLeht->reasLaskurid[areaNr]->vSummad[2]->text();
                            break;
                        }
                        }
                    }else{
                        int s = 0;
                        for(int j = 0; j < seeLeht->seeriateArv; j++){
                            tulemus->read[0][5 + j + s] = seeLeht->reasLaskurid[areaNr]->seeriad[j]->text();
                            if((j + 1) % seeLeht->vSummadeSamm == 0){
                                s++;
                                tulemus->read[0][5 + j + s] = seeLeht->reasLaskurid[areaNr]->vSummad[j / seeLeht->vSummadeSamm]->text();
                            }
                        }
                    }
                    tulemus->read[0][14] = seeLeht->reasLaskurid[areaNr]->getSumma();
                    if(seeLeht->reasLaskurid[areaNr]->markus->text().isEmpty() || seeLeht->reasLaskurid[areaNr]->markus->text().contains("V.A", Qt::CaseInsensitive)){
                        tulemus->read[0][15] = QString("%1").arg(float(seeLeht->reasLaskurid[areaNr]->keskmLask)	/ 10000);
                        tulemus->read[0][15].replace('.', ',');
                    }else tulemus->read[0][15] = seeLeht->laskurid[areaNr]->markus->text();
                    if(seeLeht->reasLaskurid[areaNr]->markus->text().contains("V.A", Qt::CaseInsensitive))
                        tulemus->read[0][0] = "v.a.";
                    else if(seeLeht->reasLaskurid[areaNr]->markus->text().contains("DNF", Qt::CaseInsensitive))
                        tulemus->read[0][0] = "DNF";
                    else if(seeLeht->reasLaskurid[areaNr]->markus->text().contains("DSQ", Qt::CaseInsensitive))
                        tulemus->read[0][0] = "DSQ";
                    if(tulemus->loplik)
                        tulemus->read[0][15] = "";
                    if(!seeLeht->reasLaskurid[areaNr]->finaal->text().isEmpty()){
                        //Enam kogusummat koos finaaliga ei näidata:
                        tulemus->read[0][15] = seeLeht->reasLaskurid[areaNr]->finaal->text()/* + " " + seeLeht->reasLaskurid[areaNr]->koguSumma->text()*/;
                        tulemus->loplik = true;
                    }
                }
            }
            tulemus->joonista();
            if(naidatud){
                reaNr = 0;
                naidatud = false;
                tulemus->loplik = false;
            }
            if(leheNr >= tabWidget->count()) leheNr = 0;
        }
    }else leheNr = 0;
}

void Protokollitaja::naitaTulAken()
{
    QList<QScreen *> screens = QGuiApplication::screens();

    if (screens.size() >= 2) {
        if (QMessageBox::question(this, "Protokollitaja", tr("Kas tahate tulemused näidata teisel ekraanil?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            QScreen *currentScreen = QGuiApplication::screenAt(this->geometry().center());
            int currentIndex = screens.indexOf(currentScreen);
            QScreen *otherScreen = screens.at((currentIndex + 1) % screens.size());
            tulemus->setScreen(otherScreen);
            tulemus->move(otherScreen->geometry().center() - tulemus->rect().center());
            tulemus->showFullScreen();
            QMessageBox::information(this, tr("Teade"), tr("Tulemuse aken näidatud teisel ekraanil"), QMessageBox::Ok);
        } else {
            tulemus->show();
        }
    } else {
        tulemus->show();
    }
    kasNaidataTul = true;
    kasNaitaTul(kasNaidataTul);
}

void Protokollitaja::naitaWAbi()
{
        QPixmap *pildike = new QPixmap(":/images/Abi.bmp");
        QLabel *pilt = new QLabel();
        pilt->setPixmap(*pildike);
        pilt->setGeometry(pildike->rect());
        //pilt->setWindowModality(Qt::ApplicationModal);
        pilt->show();
}

void Protokollitaja::peataProgress()
{
    if(progress !=0){
        if(progress->wasCanceled()){
            siusDataSocket->disconnectFromHost();   //Ühendamine katkestati
        }else progress->accept();
    }
}

void Protokollitaja::prindi()
{
    if(seaded->ui.prindiBox->currentIndex() == 0){  //Kui valitud on uus printimine
        prindi2();
        return;
    }

    if(tabWidget->count() > 0){
        Leht* seeLeht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
        if(seeLeht->laskurid.count() < 1 && seeLeht->voistkonnad.count() < 1) return;
        QPrinter printer(QPrinter::HighResolution);
        printer.setPageSize(QPageSize::A4);
        if(seeLeht->seeriateArv > 8) printer.setPageOrientation(QPageLayout::Landscape);
        else printer.setPageOrientation(QPageLayout::Portrait);
        QPrintDialog dialog(&printer, this);
/*          bool valitud = false;
                for(int i = 0; i < seeLeht->laskurid.count(); i++)
                        if(seeLeht->laskurid[i]->linnuke->isChecked())
                                valitud = true;
                if (valitud)
                        dialog.addEnabledOption(QAbstractPrintDialog::PrintSelection);*/
        if(dialog.exec() == QDialog::Accepted){
            QPainter painter2;
            bool koik = false;
            painter2.begin(&printer);
            int w = printer.pageRect(QPrinter::DevicePixel).width();
            int h = printer.pageRect(QPrinter::DevicePixel).height();
            QRect page(0, 0, w, h);
            if(seeLeht->voistk){
                int alg = 0, lopp = seeLeht->voistkonnad.count();
                if(seeLeht->voistkonnad.count() > (56 / (seeLeht->seeriateArv + 1)))
                    lopp = 56 / (seeLeht->seeriateArv + 1);
                do{
                    pilt->fill();
                    QPainter painter(pilt);
                    QFont vanaFont = this->font();
                    QFont kirjaFont;
                    QFont paiseFont;
                    QFont pealkirjaFont;

                    kirjaFont.setPointSize(24);
                    kirjaFont.setBold(false);
                    setFont(kirjaFont);
                    paiseFont.setPointSize(20);
                    paiseFont.setBold(false);
                    pealkirjaFont.setPointSize(32);
                    pealkirjaFont.setBold(true);

                    painter.setFont(pealkirjaFont);
                    painter.drawText(60, 50, m_competitionName);
                    painter.setFont(paiseFont);
                    painter.drawText(1345, 130, tr("Summa"));
                    //painter.drawText(1100, 130, "Seeriad");
                    painter.setFont(kirjaFont);
                    painter.drawText(800, 50, 700, 50, Qt::AlignRight, timeAndPlaceString());
                    kirjaFont.setBold(true);
                    painter.setFont(kirjaFont);
                    painter.drawText(75, 125, seeLeht->ekraaniNimi);
                    kirjaFont.setBold(false);
                    painter.setFont(kirjaFont);
                    for(int i = 0; alg < lopp; alg++){
                        painter.drawText(25, 175 + i * 41, QString("%1.").arg(alg+1));
                        painter.drawText(85, 175 + i * 41, seeLeht->voistkonnad[alg]->nimi->text());
                        for(int j = 0; j < seeLeht->seeriateArv; j++){
                            if(seeLeht->voistkonnad[alg]->voistlejad[j]->eesNimi != "Võistleja 1"){
                                if(j != 0) i++; //Pliiats uuele reale
                                painter.drawText(585, 175 + i * 41, seeLeht->voistkonnad[alg]->voistlejad[j]->eesNimi);
                                painter.drawText(800, 175 + i * 41, seeLeht->voistkonnad[alg]->voistlejad[j]->perekNimi);
                                painter.drawText(1100, 175 + i * 41, seeLeht->voistkonnad[alg]->voistlejad[j]->summa);
                            }
                        }
                        painter.drawText(1375, 175 + i * 41, seeLeht->voistkonnad[alg]->summa->text());
                        if(!seeLeht->voistkonnad[alg]->markus->text().isEmpty())
                            painter.drawText(1460, 175 + i * 41, seeLeht->voistkonnad[alg]->markus->text());
                        i += 2;
                    }
                    painter.setFont(vanaFont);
                    this->setFont(vanaFont);
                    painter2.drawPixmap(page, *pilt);
                    koik = true;
                    if(lopp != seeLeht->voistkonnad.count()){
                        if(lopp + (56 / (seeLeht->seeriateArv + 1)) > seeLeht->voistkonnad.count())
                            lopp = seeLeht->voistkonnad.count();
                        else lopp += (56 / (seeLeht->seeriateArv + 1));
                        printer.newPage();
                        koik = false;
                    }
                }while(!koik);
                painter2.end();
            }else if(seeLeht->seeriateArv > 8){
                //pilt2->scaled(2450, 1600);
                int lopp = seeLeht->laskurid.count();
                if(seeLeht->laskurid.count() > 30)
                    lopp = 30;
                do{
                    pilt2->fill();
                    QPainter painter(pilt2);
                    QFont vanaFont = this->font();
                    QFont kirjaFont;
                    QFont summaF;
                    QFont paiseFont;
                    QFont pealkirjaFont;

                    kirjaFont.setPointSize(24);
                    kirjaFont.setBold(false);
                    setFont(kirjaFont);
                    summaF.setPointSize(24);
                    summaF.setBold(true);
                    paiseFont.setPointSize(20);
                    paiseFont.setBold(false);
                    pealkirjaFont.setPointSize(32);
                    pealkirjaFont.setBold(true);

                    painter.setFont(pealkirjaFont);
                    painter.drawText(60, 50, m_competitionName);
                    painter.setFont(paiseFont);
                    painter.drawText(2015, 130, tr("Summa"));
                    painter.drawText(1050, 130, tr("Põlvelt"));
                        painter.drawText(1400, 130, tr("Lamades"));
                    painter.drawText(1750, 130, tr("Püsti"));
                    painter.drawText(2120, 130, tr("Finaal"));
                    painter.drawText(2220, 130, tr("Kokku"));
                    painter.setFont(kirjaFont);
                    painter.drawText(1500, 50, 700, 50, Qt::AlignRight, timeAndPlaceString());
                    kirjaFont.setBold(true);
                    painter.setFont(kirjaFont);
                    painter.drawText(75, 125, seeLeht->ekraaniNimi);
                    kirjaFont.setBold(false);
                    painter.setFont(kirjaFont);
                    int i = 0;
                    for(int alg = 0; alg < lopp; alg++){
                        painter.setFont(kirjaFont);
                        if(seeLeht->laskurid[alg]->markus->text().contains("V.A", Qt::CaseInsensitive)){
                            painter.drawText(15, 175 + i * 41, "v.a.");
                        }else if(seeLeht->laskurid[alg]->markus->text().contains("DNF", Qt::CaseInsensitive)){
                            painter.drawText(10, 175 + i * 41, "DNF");
                        }else if(seeLeht->laskurid[alg]->markus->text().contains("DSQ", Qt::CaseInsensitive)){
                            painter.drawText(10, 175 + i * 41, "DSQ");
                        }else painter.drawText(15, 175 + i * 41, QString("%1.").arg(alg+1));
                        painter.drawText(75, 175 + i * 41, seeLeht->laskurid[alg]->eesNimi->text());
                        painter.drawText(290, 175 + i * 41, seeLeht->laskurid[alg]->perekNimi->text());
                        painter.drawText(550, 175 + i * 41, seeLeht->laskurid[alg]->sunniAasta->text());
                        painter.drawText(650, 175 + i * 41, seeLeht->laskurid[alg]->klubi->text());
                        for(int v = 0; v < seeLeht->laskurid[alg]->vSummad.count(); v++){
                            for(int j = 0; j < seeLeht->vSummadeSamm; j++)
                                painter.drawText(950 + (j + v * seeLeht->vSummadeSamm) * 65 + v * 100, 175 + i * 41,	seeLeht->laskurid[alg]->seeriad[j + v * seeLeht->vSummadeSamm]->text());
                            painter.setFont(summaF);
                            painter.drawText(950 + (v + 1) * seeLeht->vSummadeSamm * 65 + v * 100, 175 + i * 41, seeLeht->laskurid[alg]->vSummad[v]->text());
                            painter.setFont(kirjaFont);
                        }
                        painter.setFont(summaF);
                        painter.drawText(2025, 175 + i * 41, seeLeht->laskurid[alg]->getSumma());
                        //painter.setFont(kirjaFont);
                        if(!seeLeht->laskurid[alg]->finaal->text().isEmpty()){
                            //painter.setFont(kirjaFont);
                            painter.drawText(2120, 175 + i * 41, seeLeht->laskurid[alg]->finaal->text());
                            //painter.setFont(summaF);
                            //painter.drawText(2210, 175 + i * 41, seeLeht->laskurid[alg]->koguSumma->text());  //Kogusummat enam ei kasutata
                            painter.setFont(kirjaFont);
                        }else if(!seeLeht->laskurid[alg]->markus->text().isEmpty() && !seeLeht->laskurid[alg]->markus->text().contains("V.A", Qt::CaseInsensitive) && !seeLeht->laskurid[alg]->markus->text().contains("DNF", Qt::CaseInsensitive) && !seeLeht->laskurid[alg]->markus->text().contains("DSQ", Qt::CaseInsensitive)){
                            painter.setFont(kirjaFont);
                            painter.drawText(2300, 175 + i * 41, seeLeht->laskurid[alg]->markus->text());
                        }
                        painter.setFont(kirjaFont);
                        i++;
                    }
                    painter.setFont(vanaFont);
                    this->setFont(vanaFont);
                    painter2.drawPixmap(page, *pilt2);
                    koik = true;
                    if(lopp != seeLeht->laskurid.count()){
                        if(lopp + 30 > seeLeht->laskurid.count())
                            lopp = seeLeht->laskurid.count();
                        else lopp += 30;
                        printer.newPage();
                        koik = false;
                    }
                }while(!koik);
                painter2.end();
                //printer.setOrientation(QPrinter::Portrait);
            }else if(seeLeht->vSummadeSamm > 0){
                int lopp = seeLeht->laskurid.count();
                if(seeLeht->laskurid.count() > 55)
                    lopp = 55;
                do{
                    pilt->fill();
                    QPainter painter(pilt);
                    QFont vanaFont = this->font();
                    QFont kirjaFont;
                    QFont summaF;
                    QFont paiseFont;
                    QFont pealkirjaFont;

                    kirjaFont.setPointSize(22);
                    kirjaFont.setBold(false);
                    setFont(kirjaFont);
                    summaF.setPointSize(22);
                    summaF.setBold(true);
                    paiseFont.setPointSize(18);
                    paiseFont.setBold(false);
                    pealkirjaFont.setPointSize(32);
                    pealkirjaFont.setBold(true);

                    painter.setFont(pealkirjaFont);
                    painter.drawText(60, 50, m_competitionName);
                    painter.setFont(paiseFont);
                    painter.drawText(1345, 130, tr("Summa"));
                    //painter.drawText(1010, 130, "Seeriad");
                    painter.setFont(kirjaFont);
                    painter.drawText(800, 50, 700, 50, Qt::AlignRight, timeAndPlaceString());
                    kirjaFont.setBold(true);
                    painter.setFont(kirjaFont);
                    painter.drawText(75, 125, seeLeht->ekraaniNimi);
                    kirjaFont.setBold(false);
                    painter.setFont(kirjaFont);
                    int i = 0;
                    for(int alg = 0; alg < lopp; alg++){
                        painter.setFont(kirjaFont);
                        if(seeLeht->laskurid[alg]->markus->text().contains("V.A", Qt::CaseInsensitive)){
                            painter.drawText(15, 175 + i * 41, "v.a.");
                        }else if(seeLeht->laskurid[alg]->markus->text().contains("DNF", Qt::CaseInsensitive)){
                            painter.drawText(10, 175 + i * 41, "DNF");
                        }else if(seeLeht->laskurid[alg]->markus->text().contains("DSQ", Qt::CaseInsensitive)){
                            painter.drawText(10, 175 + i * 41, "DSQ");
                        }else painter.drawText(15, 175 + i * 41, QString("%1.").arg(alg+1));
                        painter.drawText(75, 175 + i * 41, seeLeht->laskurid[alg]->eesNimi->text());
                        painter.drawText(260, 175 + i * 41, seeLeht->laskurid[alg]->perekNimi->text());
                        painter.drawText(510, 175 + i * 41, seeLeht->laskurid[alg]->sunniAasta->text());
                        painter.drawText(600, 175 + i * 41, seeLeht->laskurid[alg]->klubi->text());
                        for(int v = 0; v < seeLeht->laskurid[alg]->vSummad.count(); v++){
                            for(int j = 0; j < seeLeht->vSummadeSamm; j++)
                                painter.drawText(830 + (j + v * seeLeht->vSummadeSamm) * 55 + v * 75, 175 + i * 41, seeLeht->laskurid[alg]->seeriad[j + v * seeLeht->vSummadeSamm]->text());
                            painter.setFont(summaF);
                            painter.drawText(830 + (v + 1) * seeLeht->vSummadeSamm * 55 + v * 75, 175 + i * 41, seeLeht->laskurid[alg]->vSummad[v]->text());
                            painter.setFont(kirjaFont);
                        }
                        painter.setFont(summaF);
                        painter.drawText(1375, 175 + i * 41, seeLeht->laskurid[alg]->getSumma());
                        //painter.setFont(kirjaFont);
                        if(!seeLeht->laskurid[alg]->finaal->text().isEmpty()){
                            //painter.setFont(kirjaFont);
                            painter.drawText(1440, 175 + i * 41, seeLeht->laskurid[alg]->finaal->text());
                            //painter.setFont(summaF);
                            //painter.drawText(1520, 175 + i * 41, seeLeht->laskurid[alg]->koguSumma->text());  //Kogusummat enam ei kasutata
                            painter.setFont(kirjaFont);
                        }else if(!seeLeht->laskurid[alg]->markus->text().isEmpty() && !seeLeht->laskurid[alg]->markus->text().contains("V.A", Qt::CaseInsensitive) && !seeLeht->laskurid[alg]->markus->text().contains("DNF", Qt::CaseInsensitive) && !seeLeht->laskurid[alg]->markus->text().contains("DSQ", Qt::CaseInsensitive)){
                            painter.setFont(kirjaFont);
                            painter.drawText(1460, 175 + i * 41, seeLeht->laskurid[alg]->markus->text());
                        }
                        painter.setFont(kirjaFont);
                        i++;
                    }
                    painter.setFont(vanaFont);
                    this->setFont(vanaFont);
                    painter2.drawPixmap(page, *pilt);
                    koik = true;
                    if(lopp != seeLeht->laskurid.count()){
                        if(lopp + 55 > seeLeht->laskurid.count())
                            lopp = seeLeht->laskurid.count();
                        else lopp += 55;
                        printer.newPage();
                        koik = false;
                    }
                }while(!koik);
                painter2.end();
            }else{
                //pilt->scaled(1600, 2450);
                int lopp = seeLeht->laskurid.count();
                if(seeLeht->laskurid.count() > 55)
                    lopp = 55;
                do{
                    pilt->fill();
                    QPainter painter(pilt);
                    QFont vanaFont = this->font();
                    QFont kirjaFont;
                    QFont summaF;
                    QFont paiseFont;
                    QFont pealkirjaFont;

                    kirjaFont.setPointSize(24);
                    kirjaFont.setBold(false);
                    setFont(kirjaFont);
                    summaF.setPointSize(24);
                    summaF.setBold(true);
                    paiseFont.setPointSize(20);
                    paiseFont.setBold(false);
                    pealkirjaFont.setPointSize(32);
                    pealkirjaFont.setBold(true);

                    painter.setFont(pealkirjaFont);
                    painter.drawText(60, 50, m_competitionName);
                    painter.setFont(paiseFont);
                    painter.drawText(1345, 130, tr("Summa"));
                    painter.drawText(1100, 130, tr("Seeriad"));
                    painter.setFont(kirjaFont);
                    painter.drawText(800, 50, 700, 50, Qt::AlignRight, timeAndPlaceString());
                    kirjaFont.setBold(true);
                    painter.setFont(kirjaFont);
                    painter.drawText(75, 125, seeLeht->ekraaniNimi);
                    kirjaFont.setBold(false);
                    painter.setFont(kirjaFont);
                    int i = 0;
                    for(int alg = 0; alg < lopp; alg++){
                        if(seeLeht->laskurid[alg]->markus->text().contains("V.A", Qt::CaseInsensitive)){
                            painter.drawText(25, 175 + i * 41, "v.a.");
                        }else if(seeLeht->laskurid[alg]->markus->text().contains("DNF", Qt::CaseInsensitive)){
                            painter.drawText(20, 175 + i * 41, "DNF");
                        }else if(seeLeht->laskurid[alg]->markus->text().contains("DSQ", Qt::CaseInsensitive)){
                            painter.drawText(20, 175 + i * 41, "DSQ");
                        }else painter.drawText(25, 175 + i * 41, QString("%1.").arg(alg+1));
                        painter.drawText(85, 175 + i * 41, seeLeht->laskurid[alg]->eesNimi->text());
                        painter.drawText(315, 175 + i * 41, seeLeht->laskurid[alg]->perekNimi->text());
                        painter.drawText(585, 175 + i * 41, seeLeht->laskurid[alg]->sunniAasta->text());
                        painter.drawText(685, 175 + i * 41, seeLeht->laskurid[alg]->klubi->text());
                        for(int j = 0; j < seeLeht->laskurid[alg]->seeriateArv; j++)
                            painter.drawText(985 + j * 65, 175 + i * 41, seeLeht->laskurid[alg]->seeriad[j]->text());
                        painter.setFont(summaF);
                        painter.drawText(1375, 175 + i * 41, seeLeht->laskurid[alg]->getSumma());
//                        painter.setFont(kirjaFont);
                        if(!seeLeht->laskurid[alg]->finaal->text().isEmpty()){
                            //painter.setFont(kirjaFont);
                            painter.drawText(1450, 175 + i * 41, seeLeht->laskurid[alg]->finaal->text());
//                            painter.setFont(summaF);
//                            painter.drawText(1520, 175 + i * 41, seeLeht->laskurid[alg]->koguSumma->text());  //Kogusummat enam ei kasutata
                            painter.setFont(kirjaFont);
                        }else if(!seeLeht->laskurid[alg]->markus->text().isEmpty() && !seeLeht->laskurid[alg]->markus->text().contains("V.A", Qt::CaseInsensitive) && !seeLeht->laskurid[alg]->markus->text().contains("DNF", Qt::CaseInsensitive) && !seeLeht->laskurid[alg]->markus->text().contains("DSQ", Qt::CaseInsensitive)){
                            painter.setFont(kirjaFont);
                            painter.drawText(1460, 175 + i * 41, seeLeht->laskurid[alg]->markus->text());}
                        i++;
                        painter.setFont(kirjaFont);
                    }
                    painter.setFont(vanaFont);
                    this->setFont(vanaFont);
                    painter2.drawPixmap(page, *pilt);
                    koik = true;
                    if(lopp != seeLeht->laskurid.count()){
                        if(lopp + 55 > seeLeht->laskurid.count())
                            lopp = seeLeht->laskurid.count();
                        else lopp += 55;
                        printer.newPage();
                        koik = false;
                    }
                }while(!koik);
                painter2.end();
            }
        }
    }
}

void Protokollitaja::prindi2()
{
    if(tabWidget->count() > 0){
        Leht* seeLeht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
        if(seeLeht->laskurid.count() < 1 && seeLeht->voistkonnad.count() < 1) return;

        QString pohi;   //Printimise template
        QFile htmlFail(qApp->applicationDirPath() + "/Data/Print_template.html");
        if(htmlFail.open(QIODevice::ReadOnly | QIODevice::Text)){
                QTextStream sisse(&htmlFail);
                sisse.setAutoDetectUnicode(true);
//                sisse.setCodec(QTextCodec::codecForName("UTF-8"));
                pohi = sisse.readAll();
        }else{
            QMessageBox::warning(this, "Hoiatus", tr("Print_template.html faili ei leitud! Printimine ei toimi!"), QMessageBox::Ok);
            return;
        }

        QString pTekst = pohi;  //Tekst, mis reaalselt prinditud saab

        //Kõigepealt üldandmed
        pTekst.replace("#voistlusenimi#", m_competitionName);
        pTekst.replace("#aegkoht#", timeAndPlaceString());

        pTekst.replace("#harjutus#", seeLeht->ekraaniNimi);

        if(!seeLeht->voistk){   //Individuaalarvestuse lehe printimine
            //Kõigepealt võistkonna osa välja lõikamine
            int algus = pTekst.lastIndexOf("<table", pTekst.indexOf("#voistkond#"));
            int pikkus = pTekst.indexOf("</table>", pTekst.indexOf("#voistkond#")) + 8 - algus;
            pTekst.remove(algus, pikkus);

        //Tabeli rea välja lõikamine
        algus = pTekst.indexOf("<tr>", pTekst.indexOf("<tbody>"));
        pikkus = pTekst.lastIndexOf("</tr>", pTekst.indexOf("</tbody>")) + 5 - algus;
        if(algus == -1 || pTekst.lastIndexOf("</tr>", pTekst.indexOf("</tbody>")) == -1)
                QMessageBox::critical(this, tr("Viga"), tr("Ei leidnud Print_template.html failist <tr>, <tbody>, </tr> või </tbody> tag'e. Tabeli printimine ei ole võimalik"), QMessageBox::Ok);

        QString origRida = pTekst.mid(algus, pikkus);   //Tabeli rea template
        QString rida;    //Tabeli rida, mis korduma hakkab
        pTekst.remove(algus, pikkus);

        algus = origRida.lastIndexOf("<td", origRida.indexOf("#S2#"));
        pikkus = origRida.indexOf("</td>", origRida.indexOf("#S6#")) + 5 - algus;
        if(algus == -1 || origRida.indexOf("</td>", origRida.indexOf("#S6#")) == -1)
            QMessageBox::critical(this, tr("Viga"), tr("Ei leidnud Print_template.html failist <td>, #S2#, </td> või #S6# tag'e. Tabeli printimine ei ole võimalik"), QMessageBox::Ok);
        origRida.remove(algus, pikkus); //Seeriate 2-6 eemaldamine template'ist

        algus = origRida.lastIndexOf("<td", origRida.indexOf("#S1#"));
        pikkus = origRida.indexOf("</td>", origRida.indexOf("#S1#")) + 5 - algus;
        if(algus == -1 || origRida.indexOf("</td>", origRida.indexOf("#S1#")) == -1)
            QMessageBox::critical(this, tr("Viga"), tr("Ei leidnud Print_template.html failist <td>, #S1# või </td> tag'e. Tabeli printimine ei ole võimalik"), QMessageBox::Ok);
        QString origSeeriaRida = origRida.mid(algus, pikkus); //Tabeli rea seeriate osa (alates 2. seeriast), mis korduma hakkab
        QString seeriaRida = origSeeriaRida;

        algus = origRida.lastIndexOf("<td", origRida.indexOf("#summa#"));
        pikkus = origRida.indexOf("</td>", origRida.indexOf("#summa#")) + 5 - algus;
        if(algus == -1 || origRida.indexOf("</td>", origRida.indexOf("#summa#")) == -1)
            QMessageBox::critical(this, tr("Viga"), tr("Ei leidnud Print_template.html failist <td>, #summa# või </td> tag'e. Tabeli printimine ei ole võimalik"), QMessageBox::Ok);
        QString origVSummaRida = origRida.mid(algus, pikkus);   //Tabeli vahesumma rea template
        QString vSummaRida = origVSummaRida;
//        QMessageBox::information(this, "Protokollitaja", "pTekst: " + pTekst, QMessageBox::Ok);

        //Õige arvu seeriate ja vahesummade lisamine template'i reale
        if(seeLeht->vSummadeSamm > 1){
            algus = origRida.indexOf("</td>", origRida.indexOf("#S1#")) + 5;    //Sisestuspunkt
            for(int j = 1; j <= seeLeht->laskurid[0]->vSummad.count(); j++){
                for(int i = 1; i <= seeLeht->vSummadeSamm; i++){
                    if(j == 1 && i == 1)    //Esimese seeria lisamise vahele jätmine (see on juba seal olemas)
                        i++;
                    seeriaRida = origSeeriaRida;
                    seeriaRida.replace("#S1#", QString("#S%1#").arg(i + (j - 1) * seeLeht->vSummadeSamm));
                    origRida.insert(algus, seeriaRida);
                    algus += seeriaRida.length();
                }
                vSummaRida = origVSummaRida;
                vSummaRida.replace("#summa#", QString("#vSumma%1#").arg(j));
                origRida.insert(algus, vSummaRida);
                algus += vSummaRida.length();
            }
        }else{
            for(int i = 2; i <= seeLeht->seeriateArv; i++){
                seeriaRida = origSeeriaRida;
                seeriaRida.replace("#S1#", QString("#S%1#").arg(i));
                algus = origRida.indexOf("</td>", origRida.indexOf(QString("#S%1#").arg(i - 1))) + 5;    //Sisestuspunkt
                origRida.insert(algus, seeriaRida);
            }
        }

        int colspan = 0;
        if(seeLeht->vSummadeSamm > 1)
            colspan = seeLeht->seeriateArv + seeLeht->seeriateArv / seeLeht->vSummadeSamm;   //Tulpade arv koos vahesummadega
        else colspan = seeLeht->seeriateArv;    //Tulpade arv ainult seeriatega
        pTekst.replace("colspan=\"6\"", QString("colspan=\"%1\"").arg(colspan));   //Seeriate pealkiri üle kõigi seeriate tulpade

        bool finalsResultExist = false;

        //Laskurid tabelis
        for (int i = seeLeht->laskurid.count() - 1; i >= 0 ; i--) {

            if(seeLeht->laskurid[i]->eesNimi->text().isEmpty())
                continue;

            rida = origRida;

            if(seeLeht->laskurid[i]->markus->text().contains("V.A", Qt::CaseInsensitive)){
                rida.replace("#nr#", tr("V.A."));
            }else if(seeLeht->laskurid[i]->markus->text().contains("DNF", Qt::CaseInsensitive)){
                rida.replace("#nr#", "DNF");
            }else if(seeLeht->laskurid[i]->markus->text().contains("DSQ", Qt::CaseInsensitive)){
                rida.replace("#nr#", "DSQ");
            }else rida.replace("#nr#", QString("%1").arg(i + 1));

            rida.replace("#eesnimi#", seeLeht->laskurid[i]->eesNimi->text());
            rida.replace("#perekonnanimi#", seeLeht->laskurid[i]->perekNimi->text());
            rida.replace("#sunniaasta#", seeLeht->laskurid[i]->sunniAasta->text());
            rida.replace("#klubi#", seeLeht->laskurid[i]->klubi->text());
            for (int column = 0; column < seeLeht->seeriateArv; column++) { //Seeriate sisestamine, seeriate numeratsioon algab 1'st
                rida.replace(QString("#S%1#").arg(column + 1), seeLeht->laskurid[i]->seeriad[column]->text());
            }
            for (int column = 0; column < seeLeht->laskurid[i]->vSummad.count(); column++) {    //Vahesummade sisestamine, numeratsioon algab 1'st
                rida.replace(QString("#vSumma%1#").arg(column + 1), seeLeht->laskurid[i]->vSummad[column]->text());
            }
            if(seeLeht->laskurid[i]->kumned->text() != "0" && !(seeLeht->laskurid[i]->getSumma().contains(",") || seeLeht->laskurid[i]->getSumma().contains(".")))
                rida.replace("#summa#", seeLeht->laskurid[i]->getSumma() + "-" + seeLeht->laskurid[i]->kumned->text() + "x");
            else rida.replace("#summa#", seeLeht->laskurid[i]->getSumma());

            if(!seeLeht->laskurid[i]->finaal->text().isEmpty()){
                rida.replace("#finaal#", seeLeht->laskurid[i]->finaal->text());
                finalsResultExist = true;
            }else{
                rida.remove("#finaal#");
            }

            if(!seeLeht->laskurid[i]->markus->text().isEmpty() && !seeLeht->laskurid[i]->markus->text().contains("V.A", Qt::CaseInsensitive) && !seeLeht->laskurid[i]->markus->text().contains("DNF", Qt::CaseInsensitive) && !seeLeht->laskurid[i]->markus->text().contains("DSQ", Qt::CaseInsensitive)){
            rida.replace("#markus#", seeLeht->laskurid[i]->markus->text());
            }else rida.remove("#markus#");
            pTekst.insert(pTekst.indexOf("<tbody>") + 7, rida);
        }
        if(!finalsResultExist){
            int start = pTekst.lastIndexOf("<th", pTekst.indexOf(">Finaal<"));
            int length = pTekst.indexOf("</th>", pTekst.indexOf(">Finaal<")) - start;
#ifdef QT_DEBUG
            qDebug() << "!finalsResultExist: " << pTekst.mid(start, length);
#endif
            pTekst.remove(start, length);
        }

        }else{  //Võistkondliku arvestuse lehe printimine
            //Kõigepealt individuaalarvestuse osa välja lõikamine
            int algus = pTekst.lastIndexOf("<table", pTekst.indexOf("#S1#"));
            int pikkus = pTekst.indexOf("</table>", pTekst.indexOf("#S1#")) + 8 - algus;
            pTekst.remove(algus, pikkus);

            //Tabeli võistkonna rea välja lõikamine
            algus = pTekst.indexOf("<tr>", pTekst.indexOf("<tbody>"));
            pikkus = pTekst.lastIndexOf("</tr>", pTekst.indexOf("</tbody>")) + 5 - algus;
            if(algus == -1 || pTekst.lastIndexOf("</tr>", pTekst.indexOf("</tbody>")) == -1)
            QMessageBox::critical(this, tr("Viga"), tr("Ei leidnud Print_template.html failist <tr>, <tbody>, </tr> või </tbody> tag'e. Tabeli printimine on vigane!"), QMessageBox::Ok);

            QString origRida = pTekst.mid(algus, pikkus);   //Tabeli võistkonna rea template
            QString rida;    //Tabeli võistkonna rida, mis korduma hakkab
            pTekst.remove(algus, pikkus);

            for(int i = 0; i < seeLeht->voistkonnad.count(); i++){  //Võistkonna lisamine

                if(seeLeht->voistkonnad[i]->nimi->text().isEmpty())
                    continue;

                rida = origRida;    //Esimene liige
                rida.replace("#nr#", QString("%1").arg(i + 1));
                rida.replace("#voistkond#", seeLeht->voistkonnad[i]->nimi->text());
                rida.replace("#eesnimi#", seeLeht->voistkonnad[i]->voistlejad[0]->eesNimi);
                rida.replace("#perekonnanimi#", seeLeht->voistkonnad[i]->voistlejad[0]->perekNimi);
                rida.replace("#tulemus1#", seeLeht->voistkonnad[i]->voistlejad[0]->summa);
                rida.replace("#summa#", seeLeht->voistkonnad[i]->summa->text());
                if(!seeLeht->voistkonnad[i]->markus->text().isEmpty())
                    rida.replace("#markus#", seeLeht->voistkonnad[i]->markus->text());
                else rida.replace("#markus#", "");
                pTekst.insert(algus, rida);
                algus += rida.length();

                for(int j = 1; j < seeLeht->voistkonnad[i]->voistlejad.count(); j++){   //Teiste liikmete lisamine
                    if(!seeLeht->voistkonnad[i]->voistlejad[j]->eesNimi.contains(tr("Võistleja 1"))){
                        rida = origRida;
                        rida.replace("#nr#", "");
                        rida.replace("#voistkond#", "");
                        rida.replace("#eesnimi#", seeLeht->voistkonnad[i]->voistlejad[j]->eesNimi);
                        rida.replace("#perekonnanimi#", seeLeht->voistkonnad[i]->voistlejad[j]->perekNimi);
                        rida.replace("#tulemus1#", seeLeht->voistkonnad[i]->voistlejad[j]->summa);
                        rida.replace("#summa#", "");
                        rida.replace("#markus#", "");
                        pTekst.insert(algus, rida);
                        algus += rida.length();
                    }
                }
                rida = origRida;    //Tühja rea lisamine, et võistkondade vahel vahed oleks
                rida.replace("#nr#", "");
                rida.replace("#voistkond#", "");
                rida.replace("#eesnimi#", "");
                rida.replace("#perekonnanimi#", "");
                rida.replace("#tulemus1#", "");
                rida.replace("#summa#", "");
                rida.replace("#markus#", "");
                pTekst.insert(algus, rida);
                algus += rida.length();
            }
        }

//        QTextStream valja(&tulemusteTekst);

        /*valja <<  "<html>\n"
                  "<head>\n"
                  "<meta Content=\"Text/html; charset=utf-8\">\n"
              <<  QString("<title>%1</title>\n").arg(m_competitionName)
              <<  "</head>\n"
                  "<body bgcolor=#ffffff link=#5000A0>\n"
              <<  QString("<h1>%1</h1>\n").arg(m_competitionName)
              <<  QString("<h2>%1</h2>\n").arg(seeLeht->harjutus)
              <<  "<table width=\"100%\" border=0 cellspacing=0 cellpadding=5>\n";
        // headers
        valja << "<thead><tr bgcolor=#ffffff>"
                 "<th>Eesnimi</th>"
                 "<th>Perekonnanimi</th>"
                 "<th>Klubi</th>";
        for (int column = 0; column < seeLeht->seeriateArv; column++)
            valja << QString("<th>S%1</th>").arg(column);
        valja << "<th>Summa</th>";
        valja << "</tr></thead>\n";

        // data table
        for (int row = 0; row < seeLeht->laskurid.count(); row++) {
            valja << "<tr>";
            valja << QString("<td bkcolor=0>%1</td>").arg(seeLeht->laskurid[row]->eesNimi->text());
            valja << QString("<td bkcolor=0>%1</td>").arg(seeLeht->laskurid[row]->perekNimi->text());
            valja << QString("<td bkcolor=0>%1</td>").arg(seeLeht->laskurid[row]->klubi->text());
            for (int column = 0; column < seeLeht->seeriateArv; column++) {
                valja << QString("<td bkcolor=0>%1</td>").arg(seeLeht->laskurid[row]->seeriad[column]->text());
            }
            valja << QString("<td bkcolor=0>%1</td>").arg(seeLeht->laskurid[row]->getSumma());
            valja << "</tr>\n";
        }
        valja <<  "</table>\n"
                "</body>\n"
                "</html>\n";*/

        QPrinter printer;
        printer.setPageSize(QPageSize::A4);
        printer.setPageMargins(QMarginsF(20, 10, 10, 10), QPageLayout::Millimeter);
        if(seeLeht->seeriateArv > 6)    //3x40 standard las olla landscape'is
            printer.setPageOrientation(QPageLayout::Landscape);

//        QWebView *dokument = new QWebView();
        QTextDocument *dokument = new QTextDocument();
        dokument->setPageSize(QSizeF(printer.pageRect(QPrinter::DevicePixel).size()));
        dokument->setHtml(pTekst);

//#if QT_VERSION >= 0x050000
//        printer.setOutputFileName("Print2.pdf");
//        dokument->print(&printer);
//#else
        QPrintDialog *dialog = new QPrintDialog(&printer, this);
        if (dialog->exec() == QDialog::Accepted) {
            dokument->print(&printer);
        }
//#endif

        QFile fail("Print.html");
        if(fail.open(QIODevice::WriteOnly | QIODevice::Text)){
            QTextStream valja(&fail);
            valja << pTekst;
        }

        delete dokument;
    }
}

void Protokollitaja::readSettings()
{
#ifdef QT_DEBUG
    qDebug() << "readSettings()";
#endif

    seaded->ui.competitionTypesEdit->setText(m_settings.competitionShotTypesString());
    seaded->ui.sighterTypesEdit->setText(m_settings.sighterShotTypesString());
    lasuVSiusis = m_settings.shotValueIndexInSius();
    lasuNrSiusis = m_settings.shotNoIndexInSius();
    aValik->setFileName(m_settings.lastOpenFileName());
    aValik->setData(SimpleKllFileRW::readCompetitionSettingsFromKll(aValik->fileName(), this));
}

void Protokollitaja::readShotInfo(QString data, int socketIndex)
{
    if(data.isEmpty() || !data.startsWith("Laskur:")){  //This should not happen
        logiValja << "Saabus tühi lasuinfo, socketIndex = " << socketIndex;
        return;
    }
    //OLD: "Laskur:siffer - siffer;Eesnimi;Perekonnanimi;seeriate arv;laskude arv;seeriad;selle seeria lasud; x; y; summa;aktiivne seeria;harjutus;lasku lehes;kümnendikega lugemine (true/false)
    //NEW: "Laskur:siffer - siffer;Eesnimi;Perekonnanimi;seeriate arv;laskude arv;loetud seeria nr (0-5); loetud seeria; loetud seeria lasud; x; y;

    data.remove(0, 7);
    QStringList dataList = data.split(";", Qt::KeepEmptyParts);
    QString targetNumbersStart = dataList.at(0).left(dataList.at(0).indexOf('-')).trimmed();
    QString targetNumbersEnd = dataList.at(0).mid(dataList.at(0).indexOf('-') + 1, dataList.at(0).length()).trimmed();
    dataList.takeFirst(); //Target numbers have been taken already
    dataList.takeFirst(); //First name not needed
    dataList.takeFirst(); //Name not needed
    Laskur *thisCompetitor = 0;
    Leht* sheet = 0;

#ifdef QT_DEBUG
        qDebug() << "targetNumbersStart: " << targetNumbersStart << ", targetNumbersEnd: " << targetNumbersEnd;
#endif

    //Locate competitor whose results arrived
    bool found = false;
    for(int i = 0; i < tabWidget->count(); i++){
        sheet = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());
        for(int j = 0; j < sheet->laskurid.count(); j++){
            if(targetNumbersStart.compare(sheet->laskurid[j]->sifriAlgus->text().trimmed()) == 0 && targetNumbersEnd.compare(sheet->laskurid[j]->sifriLopp->text().trimmed()) == 0){
                thisCompetitor = sheet->laskurid[j];
                found = true;
                logiValja << "#seeLaskur: " << thisCompetitor->id << " " << thisCompetitor->eesNimi->text() << " " << thisCompetitor->perekNimi->text() << "\n";
                j = sheet->laskurid.count(); //Loop'ist väljumiseks
                i = tabWidget->count(); //Loop'ist väljumiseks
                break;
            }
        }
    }

    if(!found){
        saadaVorku("Viga:" + tr("Ei leitud sellist sifrit!\n\nTulemusi ei uuendatud!"), socketIndex);
        return;
    }else if(sheet->seeriateArv != dataList.takeFirst().toInt()){   //Check number of series
        saadaVorku("Viga:" + tr("Seeriate arv ei ühti Protokollitajaga!\n\nTulemusi ei uuendatud!"), socketIndex);
        return;
    }

#ifdef QT_DEBUG
        qDebug() << "#seeLaskur: " << thisCompetitor->id << " " << thisCompetitor->eesNimi->text() << " " << thisCompetitor->perekNimi->text() << "\n";
#endif

    dataList.takeFirst();   //Number of shots currently defaults to 10
    int numberOfShots = 10;

    int seriesNo = dataList.takeFirst().toInt();
    QString series = dataList.takeFirst();

#ifdef QT_DEBUG
        qDebug() << "series = " << series << ", seriesNo = " << seriesNo << "\n";
#endif

    if(!thisCompetitor->seeriad[seriesNo]->text().isEmpty() && thisCompetitor->seeriad[seriesNo]->text() != series){   //Add error message and ask what to do?
        saadaVorku("Viga:" + tr("Sellel seerial on juba tulemus olemas!\n\nTulemusi ei uuendatud, kui tahate üle kirjutada, kustutage Protokollitajast eelmine seeria ära!"), socketIndex);
        logiValja << "#lehelugemisel seeria muutus: " << thisCompetitor->id << " " << thisCompetitor->eesNimi->text() << " "
                  << thisCompetitor->perekNimi->text() << ", vana: " << thisCompetitor->seeriad[seriesNo]->text() << " uus: " << series << "\n";
    }else if(!series.isEmpty()){    //Only new results will be sent and read
        thisCompetitor->seeriad[seriesNo]->setText(series);
        for(int j = 0; j < numberOfShots; j++){
            thisCompetitor->lasud[seriesNo][j]->setLask(dataList.takeFirst());
            thisCompetitor->lasud[seriesNo][j]->setNanoX(dataList.takeFirst());
            thisCompetitor->lasud[seriesNo][j]->setNanoY(dataList.takeFirst());
            thisCompetitor->lasud[seriesNo][j]->setInnerTen(
                Lask::calcIfInnerTen(
                    sheet->m_targetType,
                    thisCompetitor->lasud[seriesNo][j]->X(),
                    thisCompetitor->lasud[seriesNo][j]->Y()
                    )
                );
        }
    }

#ifdef QT_DEBUG
        qDebug() << "liida()";
#endif
        thisCompetitor->liida();
        saadaVorku("Summa:" + thisCompetitor->getSumma(), socketIndex);

    muudaSalvestamist();
}

void Protokollitaja::readSiusInfo(SiusShotData shotData)
{
    if(verbose)
        QTextStream(stdout) << "readSiusInfo()" << Qt::endl;

    Laskur* thisCompetitor = nullptr;
    Leht* sheet = nullptr;
        if(shotData.shot.isCompetitionShot()) // TODO: process and save also sighting shots
            for(int i = 0; i < tabWidget->count(); i++){
                sheet = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());
                for(int j = 0; j < sheet->laskurid.count(); j++){
                    if(shotData.id == sheet->laskurid[j]->id && (shotData.socketIndex == sheet->laskurid[j]->siusConnectionIndex() || sheet->laskurid[j]->siusConnectionIndex() == -1)){ //To avoid different Sius connections reading into one competitor
                        thisCompetitor = sheet->laskurid[j];
                        thisCompetitor->readSiusShot(shotData);
                        logiValja << "#thisCompetitor: " << thisCompetitor->id << " " << thisCompetitor->eesNimi->text() << " " << thisCompetitor->perekNimi->text() << "\n";
                        j = sheet->laskurid.count(); //To break out from all loops
                        i = tabWidget->count(); //To break out from all loops

                        break;
                    }
                }
            }
}

void Protokollitaja::reasta()   //Tulemuste järgi reastamine
{
        if(tabWidget->count() < 1) return;
        if(tabWidget->count() > 0){
                QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget())->reasta(0);
                voibSulgeda = false;
                QApplication::restoreOverrideCursor();
        }
}

void Protokollitaja::reastaP()  //Perekonnanimede järgi reastamine
{
        if(tabWidget->count() < 1) return;
        if(tabWidget->count() > 0){
                QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget())->reasta(1);
                voibSulgeda = false;
                QApplication::restoreOverrideCursor();
        }
}

void Protokollitaja::reastaR()  //Raja nr'ite järgi reastamine
{
        if(tabWidget->count() < 1) return;
        if(tabWidget->count() > 0){
                QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget())->reasta(3);
                voibSulgeda = false;
                QApplication::restoreOverrideCursor();
        }
}

void Protokollitaja::reastaS()  //Sünniaastate järgi reastamine
{
        if(tabWidget->count() < 1) return;
        if(tabWidget->count() > 0){
                QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget())->reasta(2);
                voibSulgeda = false;
                QApplication::restoreOverrideCursor();
        }
}

void Protokollitaja::reastaSi() //Sifrite järgi reastamine
{
        if(tabWidget->count() < 1) return;
        if(tabWidget->count() > 0){
                QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget())->reasta(4);
                voibSulgeda = false;
                QApplication::restoreOverrideCursor();
        }
}

void Protokollitaja::receivedVersionInfo(bool updateExists, QString versionString)
{
    if(updateExists){  //Saadaval on uuem versioon
        if(QMessageBox::information(
                    this,
                    tr("Teade"),
                    tr("Programmist on saadaval uuem versioon. Praegune: %1\nUus versioon: %2\n\n"
                            "Uus versioon on saadaval Drive'is: "
                            "https://drive.google.com/drive/folders/1SpWxxP-E12XytEFT0VmYz_QpLLFzq1nd\n\n"
                            "Kas soovite selle kausta avada?"
                            ).arg(versioon).arg(versionString),
                    QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok
                ){
            QDesktopServices::openUrl(QUrl("https://drive.google.com/drive/folders/1SpWxxP-E12XytEFT0VmYz_QpLLFzq1nd"));
        }
    } else if(!autoUuendus){
        QMessageBox::information(
                    this,
                    tr("Teade"),
                    tr("Teil on kõige uuem versioon programmist.\n\nLeitud uusim versioon: %1").arg(versionString),
                    QMessageBox::Ok
                    );
    }
}

void Protokollitaja::dataUploaderFinished(bool success, QString reply, QString errorString)
{
    if(!success){
        logiValja << "#ERROR: Web upload failed, errorString: " << errorString << "\n#reply: " << reply << "\n";
        statusBarInfoChanged("Error with upload: " + errorString + " " + reply);
        if(errorString.contains("Authentication", Qt::CaseInsensitive) || errorString.contains("Connection closed", Qt::CaseInsensitive)){
            m_restHeaderData = ""; // In case of login error, clear login data
            uploadTimer.stop();  // No point to try again if login data was incorrect
        }
    }else{
        statusBarInfoChanged("Upload successful: " + reply);
        if(webCompetitionId.isEmpty()) {
            webCompetitionId = reply;
            if(webCompetitionId.contains(QRegularExpression(QStringLiteral("[^\\x{0000}-\\x{007F}]")))){
                if(QMessageBox::warning(
                            this,
                            tr("Viga!"),
                            tr("Võistluse veebi ID (%1) sisaldab kummalisi tähemärke ja on ilmselt vigane!"
                               "\n\nKas soovite selle kustutada? See ei mõjuta muud, kui ainult veebi laadimist.").arg(webCompetitionId),
                            QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
                webCompetitionId = "";
            }
            voibSulgeda = false;
            logiValja << "#Web upload success: " << reply << "\n#webCompetitionId: " << webCompetitionId << "\n";
        }
    }
}

void Protokollitaja::saadaVorku(QString saadetis, int socketIndex)
{
    server->send(saadetis, socketIndex);
    if(verbose)
        QTextStream(stdout) << "Protokollitaja::saadaVorku(): " << saadetis << ", socketIndex = " << socketIndex << Qt::endl;
}

void Protokollitaja::salvesta()
{
#ifdef QT_DEBUG
    qDebug() << "Salvestamine1, seeFail: " << seeFail;
#endif
        if(seeFail.isEmpty())
#if QT_VERSION >= 0x050000
                seeFail = QFileDialog::getSaveFileName(this, tr("Salvesta nimega"),
                        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), tr("Protokollitaja fail (*.kll)"));
#else
            seeFail = QFileDialog::getSaveFileName(this, tr("Salvesta nimega"),
                    QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation), tr("Protokollitaja fail (*.kll)"));
#endif
        if(seeFail.isEmpty())
                return;
#ifdef QT_DEBUG
    qDebug() << "Salvestamine";
#endif
    setWindowTitle(programmiNimi + " - " + seeFail);
    kirjutaFail(seeFail);
}

void Protokollitaja::salvestaKui()
{
        QString uusSeeFail = QFileDialog::getSaveFileName(this, tr("Salvesta nimega"), seeFail,
                tr("Protokollitaja fail (*.kll)"));
        if(uusSeeFail.isEmpty())
            return;
        else seeFail = uusSeeFail;
        if(!seeFail.endsWith(".kll"))
                seeFail.append(".kll");
        setWindowTitle(programmiNimi + " - " + seeFail);
        salvesta();
        writeSettings();
}

void Protokollitaja::seiskaServer()
{
    if(server != 0){
        server->closeConnections();
        server->close();
        statusBar()->showMessage(tr("Server seisatud"), 5000);
        server->deleteLater();
        server = 0;
    }
}

void Protokollitaja::sendCompetitorsToRange()
{
    if(tabWidget->count() < 1) return;  // Make sure there is at least 1 tab

    Leht* sheet = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());

    if(sheet->voistk){
        QMessageBox::critical(this, "Protokollitaja", tr("Võistkondade saatmine Range Control'i ei ole võimalik"),
                QMessageBox::Ok);
        return;
    }

    auto data = sheet->exportStartList();
    if (data) {

        if(siusDataConnections == nullptr)
            uhenduSiusDataga();

        siusDataConnections->sendDataToRangeControl(data.value());
    }
}

void Protokollitaja::setDataFromInitialDialog()
{
    m_competitionName = aValik->competitionName();
    seeFail = aValik->fileName();
    m_startDate = aValik->startDate();
    m_endDate = aValik->endDate();
    m_place = aValik->place();
    m_country = aValik->country();
    setWindowTitle(programmiNimi + " - " + seeFail);
    writeSettings();
}

void Protokollitaja::setupTranslator()
{

    // QTranslator translator;
    // bool loadResult = translator.load(m_settings.language(), ":/protokollitaja/languages");
    // if(!loadResult) {
    //     QTextStream(stdout) << "Translation file loading failed!, working dir: " << QDir::currentPath() << Qt::endl;
    // }
    // qApp->installTranslator(&translator);

    qApp->removeTranslator(&m_translator);

    bool loadResult = m_translator.load(m_settings.language(), ":/protokollitaja/languages");
    if(!loadResult) {
        QTextStream(stdout) << "Translation file loading failed!, working dir: " << QDir::currentPath() << Qt::endl;
    }
    qApp->installTranslator(&m_translator);
}

void Protokollitaja::sifriLisa()
{
    if(tabWidget->count() < 1) return;
    if(sifriLisaAken->exec() == QDialog::Accepted){
        int siffer = sifriLisaAken->ui.sifriAlgus->text().toInt();
        Leht *seeLeht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
        if(!seeLeht->voistk){
            for(int j = 0; j < seeLeht->laskurid.count(); j++)
                if(seeLeht->laskurid[j]->linnuke->isChecked())
                    if(seeLeht->laskurid[j]->sifriAlgus->text().isEmpty() || (!seeLeht->laskurid[j]->sifriAlgus->text().isEmpty() &&
                            QMessageBox::warning(this, "Protokollitaja", tr("Siffer on juba olemas, kas soovite üle kirjutada?"), QMessageBox::Ok,
                            QMessageBox::Cancel) == QMessageBox::Ok)){
                        seeLeht->laskurid[j]->sifriAlgus->setText(QString("%1").arg(siffer));
                        siffer += seeLeht->seeriateArv * 10 / sifriLisaAken->ui.laskudeArv->value();
                        seeLeht->laskurid[j]->sifriLopp->setText(QString("%1").arg(--siffer));
                        siffer++;
                    }
        }
    }
}

void Protokollitaja::statusBarInfoChanged(QString newStatusInfo)
{
    statusBar()->showMessage(newStatusInfo, 5000);
}

void Protokollitaja::sulge()
{
#ifdef QT_DEBUG
    qDebug() << "Väljumine";
#endif
    qApp->quit();

}

void Protokollitaja::sulgeUhendus()
{
    if(server != 0)
        server->closeConnections();
//    uhendusAutoriseeritud = false;
//    socket = 0;
}

void Protokollitaja::taiendaAndmebaas()
{
        if(tabWidget->count() < 1) return;
        int lisatud = 0, plisatud = 0;
        QFile fail(qApp->applicationDirPath() + "/Data/Laskuritenimekiri Puss.txt");
        if(fail.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
                QTextStream valja(&fail);
//                valja.setCodec("UTF-8");
                valja.setAutoDetectUnicode(false);
//                static bool esimene = true;
                for(int i = 0; i < tabWidget->count(); i++){
                        Leht* seeLeht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());
            if(TargetTypes::targetData(seeLeht->m_targetType).isRifleDB)
                                for(int j = 0; j < seeLeht->laskurid.count(); j++){
                                        bool olemas = false;
                                        for(int i = 0; i < andmebaas.nimekiriPuss.count(); i++){
                                                if(seeLeht->laskurid[j]->eesNimi->text().trimmed() == andmebaas.nimekiriPuss[i]->eesnimi.trimmed() &&
                                                        seeLeht->laskurid[j]->perekNimi->text().trimmed() == andmebaas.nimekiriPuss[i]->perekonnanimi.trimmed() &&
                                                        seeLeht->laskurid[j]->sunniAasta->text().trimmed() == andmebaas.nimekiriPuss[i]->sunniaasta.trimmed())
                                                    olemas = true;
                                        }
                                        if(!olemas && !seeLeht->laskurid[j]->eesNimi->text().trimmed().isEmpty() &&
                                                !seeLeht->laskurid[j]->perekNimi->text().trimmed().isEmpty() &&
                                                !seeLeht->laskurid[j]->sunniAasta->text().trimmed().isEmpty() &&
                                                !seeLeht->laskurid[j]->klubi->text().trimmed().isEmpty()){
                                            QString rida;
//#ifndef Q_WS_WIN
//                                            if(!esimene) rida = "\n";
//#else
                                            rida = "\n";
//#endif
                                            rida.append(seeLeht->laskurid[j]->eesNimi->text().trimmed() + ";");
                                            rida.append(seeLeht->laskurid[j]->perekNimi->text().trimmed() + ";");
                                            rida.append(seeLeht->laskurid[j]->sunniAasta->text().trimmed() + ";");
                                            rida.append(seeLeht->laskurid[j]->klubi->text().trimmed());
                                            valja << rida;
//                                            esimene = false;
                                            lisatud++;
                                            rida.remove("\n");
                                            QStringList list = rida.split(";");
                                            andmebaas.nimekiriPuss << new Andmebaas::LaskuriNimi;
                                            andmebaas.nimekiriPuss[andmebaas.nimekiriPuss.count()-1]->eesnimi =
                                                    list.takeFirst().trimmed();
                                            andmebaas.nimekiriPuss[andmebaas.nimekiriPuss.count()-1]->perekonnanimi =
                                                    list.takeFirst().trimmed();
                                            andmebaas.nimekiriPuss[andmebaas.nimekiriPuss.count()-1]->sunniaasta =
                                                    list.takeFirst().trimmed();
                                            andmebaas.nimekiriPuss[andmebaas.nimekiriPuss.count()-1]->klubi =
                                                    list.takeFirst().trimmed();
                                        }
                                }
                }
                fail.close();
        }

        fail.setFileName(qApp->applicationDirPath() + "/Data/Laskuritenimekiri Pustol.txt");
        if(fail.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
                QTextStream valja(&fail);
//                valja.setCodec("UTF-8");
                valja.setAutoDetectUnicode(false);
//                static bool esimene = true;
                for(int i = 0; i < tabWidget->count(); i++){
                        Leht* seeLeht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());
            if(!TargetTypes::targetData(seeLeht->m_targetType).isRifleDB)
                                for(int j = 0; j < seeLeht->laskurid.count(); j++){
                                        bool olemas = false;
                                        for(int i = 0; i < andmebaas.nimekiriPustol.count(); i++){
                                                if(seeLeht->laskurid[j]->eesNimi->text().trimmed() == andmebaas.nimekiriPustol[i]->eesnimi.trimmed() &&
                                                        seeLeht->laskurid[j]->perekNimi->text().trimmed() == andmebaas.nimekiriPustol[i]->perekonnanimi.trimmed() &&
                                                        seeLeht->laskurid[j]->sunniAasta->text().trimmed() == andmebaas.nimekiriPustol[i]->sunniaasta.trimmed())
                                                    olemas = true;
                                        }
                                        if(!olemas && !seeLeht->laskurid[j]->eesNimi->text().trimmed().isEmpty() &&
                                                !seeLeht->laskurid[j]->perekNimi->text().trimmed().isEmpty() &&
                                                !seeLeht->laskurid[j]->sunniAasta->text().trimmed().isEmpty() &&
                                                !seeLeht->laskurid[j]->klubi->text().trimmed().isEmpty()){
                                            QString rida;
//#ifndef Q_WS_WIN
//                                            if(!esimene) rida = "\n";
//#else
                                            rida = "\n";
//#endif
                                            rida.append(seeLeht->laskurid[j]->eesNimi->text().trimmed() + ";");
                                            rida.append(seeLeht->laskurid[j]->perekNimi->text().trimmed() + ";");
                                            rida.append(seeLeht->laskurid[j]->sunniAasta->text().trimmed() + ";");
                                            rida.append(seeLeht->laskurid[j]->klubi->text().trimmed());
                                            valja << rida;
                                            plisatud++;
//                                            esimene = false;
                                            rida.remove("\n");
                                            QStringList list = rida.split(";");
                                            andmebaas.nimekiriPustol << new Andmebaas::LaskuriNimi;
                                            andmebaas.nimekiriPustol[andmebaas.nimekiriPustol.count()-1]->eesnimi =
                                                    list.takeFirst().trimmed();
                                            andmebaas.nimekiriPustol[andmebaas.nimekiriPustol.count()-1]->perekonnanimi =
                                                    list.takeFirst().trimmed();
                                            andmebaas.nimekiriPustol[andmebaas.nimekiriPustol.count()-1]->sunniaasta =
                                                    list.takeFirst().trimmed();
                                            andmebaas.nimekiriPustol[andmebaas.nimekiriPustol.count()-1]->klubi =
                                                    list.takeFirst().trimmed();
                                        }
                                }
                }
                fail.close();
        }
        if(lisatud != 0 && plisatud == 0)
                QMessageBox::information(this, "Protokollitaja", tr("%1 uut püssilaskurit lisatud andmebaasi")
                                .arg(lisatud), QMessageBox::Ok);
        else if(lisatud == 0 && plisatud !=0)
                QMessageBox::information(this, "Protokollitaja", tr("%1 uut püstolilaskurit lisatud "
                                "andmebaasi").arg(plisatud), QMessageBox::Ok);
        else if(lisatud != 0 && plisatud !=0)
                QMessageBox::information(this, "Protokollitaja", tr("%1 uut püssilaskurid ja %2 uut "
                                "püstolilaskurit lisatud andmebaasi").arg(lisatud).arg(plisatud), QMessageBox::Ok);
                    else QMessageBox::information(this, "Protokollitaja", tr("Uusi laskureid ei leitud"), QMessageBox::Ok);
}

QString Protokollitaja::timeAndPlaceString()
{
    QString format = "dd.MM.yyyy";

    if (m_startDate == m_endDate) {
        return QString("%1 %3")
            .arg(m_startDate.toString(format))
            .arg(m_place);
    } else {
        if (m_startDate.year() == m_endDate.year()) {
            format.remove(".yyyy");

            if (m_startDate.month() == m_endDate.month()) {
                format.remove(".MM");
            }
        }

        return QString("%1-%2 %3")
            .arg(m_startDate.toString(format))
            .arg(m_endDate.toString("dd.MM.yyyy"))
            .arg(m_place);
    }
}

QJsonObject Protokollitaja::toExportJson()
{
    QJsonObject json;
    json["token"] = "placeholder for future token34";
//#ifdef QT_DEBUG
//    json["id"] = "5c48c8334cfa122159c9fe49";
//#endif
    if(!webCompetitionId.isEmpty()) {
        json["id"] = webCompetitionId;
    }
    json["competitionName"] = m_competitionName;
    json["startDate"] = m_startDate.toString(Qt::ISODate);
    json["endDate"] = m_endDate.toString(Qt::ISODate);
    json["place"] = m_place;

    json["timeAndPlace"] = timeAndPlaceString();

    Leht *sheet = nullptr;
    QJsonArray sheetsArray;
    for(int i = 0; i < tabWidget->count(); i++){
        sheet = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());
        if(sheet->toBeUploaded())
            sheetsArray.append(sheet->toExportJson());
    }
    json["events"] = sheetsArray;

    return json;
}

void Protokollitaja::uploadResults()
{
    QUrl url;
    url.setScheme("https");
#ifdef QT_DEBUG
    url.setHost("localhost");
    url.setPath("/api/v1/competitions");
    url.setPort(3005);
#else
    url.setHost("ymeramees.no-ip.org");
    url.setPath("/api/v1/competitions");
    url.setPort(3005);
#endif

    if(m_restHeaderData.isEmpty()) {
        bool isOk = false;
        QString userName = QInputDialog::getText(this, tr("Kasutajanimi andmebaasis"), tr("Kasutajanimi:"), QLineEdit::Normal, "", &isOk).toLower();
        if(isOk) {
            QString passWord = QInputDialog::getText(this, tr("Kasutaja parool andmebaasis"), tr("Parool:"), QLineEdit::Password, "", &isOk);
            QString concatenated = userName + ":" + passWord;
            QByteArray data = concatenated.toLocal8Bit().toBase64();
            m_restHeaderData = "Basic " + data;
        }
        if(!isOk)
            return;
    }

    // Sometimes needed for quick fixes:
//    webCompetitionId = QInputDialog::getText(this, "webCompetitionId", "id:", QLineEdit::Normal, webCompetitionId);

    QJsonDocument jsonDoc(toExportJson());

#ifdef QT_DEBUG
    // For development purposes:
    QFile file("lastUpload.json");
    if(file.open(QIODevice::WriteOnly)){
        file.write(jsonDoc.toJson());
        QTextStream(stdout) << "Json written to file: " << file.fileName() << Qt::endl;
    }else
        QTextStream(stdout) << "Unable to open file" << Qt::endl;
#endif

    if(dataUploader == nullptr)
        dataUploader = new DataUploader(verbose, this);

    connect(dataUploader, &DataUploader::uploadFinished, this, &Protokollitaja::dataUploaderFinished);

    dataUploader->uploadResults(url, m_restHeaderData, webCompetitionId, jsonDoc);

    uploadTimer.start();
}

void Protokollitaja::uuendaJalgitavaid()
{
        Leht* leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(lValik->ui.leheBox->
                        currentIndex()))->widget());
        lValik->jalgitavad.prepend(leht->leheIndeks);
}

void Protokollitaja::uuendaLehelugejat(QString nimi)
{
    if(tabWidget->count() > 0 && lehelugejaAken){
        int laskudeArv = 10;
//        QString nimi = lehelugejaAken->m_ui.nimeBox->currentText();
        if(lehelugejaLeht == 0)
            lehelugejaLeht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
        for(int i = 0; i < lehelugejaLeht->laskurid.count(); i++)
            if(nimi == (lehelugejaLeht->laskurid[i]->eesNimi->text() + " " + lehelugejaLeht->laskurid[i]->perekNimi->text())){
                if(lehelugejaLeht->seeriateArv > 6){
                    QMessageBox::critical(this, tr("Viga"), tr("Kahjuks see versioon Protokollitajast ei toeta lehelugemist 3x40 lasku standardi harjutustes!"), QMessageBox::Ok);
                    return;
                }/*else if(lehelugejaLeht->laskurid[i]->onVorguLaskur){
                    QMessageBox::critical(this, tr("Viga"), tr("Sellele laskurile juba loetakse lehti Lehelugejas!"), QMessageBox::Ok);
                    return;
                }*/
                for(int k = 0; k < lehelugejaAken->seeriad.count(); k++){
                    lehelugejaAken->seeriad[k]->show();
                    lehelugejaAken->seeriad[k]->clear();
                    for(int l = 0; l < laskudeArv; l++){
                        lehelugejaAken->lasud[k][l]->clear();
                    }
                }
                if(lehelugejaLaskur != 0)
                    lehelugejaLaskur->onLehelugejaLaskur = false;   //Vana laskur tuleb "vabastada"
                lehelugejaLaskur = lehelugejaLeht->laskurid[i]; //Pointer laskurile, keda vaja uuendada kui lehed on loetud
                lehelugejaLaskur->onLehelugejaLaskur = true;
                lehelugejaAken->m_ui.sifriEdit->setText(lehelugejaLeht->laskurid[i]->sifriAlgus->text());
                lehelugejaAken->m_ui.sifriLabel->setText(lehelugejaLeht->laskurid[i]->sifriAlgus->text() + " - " +
                        lehelugejaLeht->laskurid[i]->sifriLopp->text());
                for(int j = 0; j < lehelugejaLeht->seeriateArv; j++){
                    lehelugejaAken->seeriad[j]->setText(lehelugejaLeht->laskurid[i]->seeriad[j]->text());
                    for(int l = 0; l < laskudeArv; l++){
                        lehelugejaAken->lasud[j][l]->set(lehelugejaLeht->laskurid[i]->lasud[j][l]);
                    }
                }
                lehelugejaAken->m_ui.summaEdit->setText(lehelugejaLeht->laskurid[i]->getSumma());
                int k = 0;
                while(!lehelugejaAken->seeriad[k]->text().isEmpty()){
                    k++;
                if(k >= lehelugejaAken->seeriad.count()){
                    k = lehelugejaAken->seeriad.count() - 1;
                    break;
                }
                }
                for(int j = lehelugejaLeht->seeriateArv; j < lehelugejaAken->seeriad.count(); j++)
                    lehelugejaAken->seeriad[j]->hide();
                lehelugejaAken->seeriad[k]->setFocus();
                if(QualificationEvents::eventData(lehelugejaLeht->eventType()).targetType == TargetTypes::AirRifle)
                    lehelugejaAken->m_ui.leheCombo->setCurrentIndex(0);
                else if(QualificationEvents::eventData(lehelugejaLeht->eventType()).targetType == TargetTypes::AirPistol)
                    lehelugejaAken->m_ui.leheCombo->setCurrentIndex(1);
                else if(QualificationEvents::eventData(lehelugejaLeht->eventType()).targetType == TargetTypes::SmallboreRifle)
                    lehelugejaAken->m_ui.leheCombo->setCurrentIndex(2);
                //laskude arv lehes
                if(!lehelugejaLeht->laskurid[i]->sifriAlgus->text().isEmpty() && !lehelugejaLeht->
                        laskurid[i]->sifriLopp->text().isEmpty()){
                    int laskuLehes = lehelugejaLeht->seeriateArv * 10 / (lehelugejaLeht->laskurid[i]->
                            sifriLopp->text().toInt() - lehelugejaLeht->laskurid[i]->sifriAlgus->text().toInt() + 1);
                    lehelugejaAken->m_ui.laskudeBox->setValue(laskuLehes);
                }else lehelugejaAken->m_ui.laskudeBox->setValue(1);
                lehelugejaAken->m_ui.kumnendikegaBox->setChecked(lehelugejaLeht->kumnendikega);
                lehelugejaAken->alustaUuesti();
//                lehelugejaAken->seadista();
            }
    }
}

void Protokollitaja::uuendaLehelugejatSifriga(int siffer)
{
    if(tabWidget->count() > 0){
        int laskudeArv = 10;
        for(int i = 0; i < tabWidget->count(); i++){
            Leht* leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());
            if(!leht->voistk){
                for(int j = 0; j < leht->laskurid.count(); j++)
                    if(!leht->laskurid[j]->sifriAlgus->text().isEmpty())
                        if(siffer >= leht->laskurid[j]->sifriAlgus->text().toInt() && siffer < leht->laskurid[j]->sifriLopp->text().toInt()){
                            if(leht->seeriateArv > 6){
                                QMessageBox::critical(this, tr("Viga"), tr("Kahjuks see versioon Protokollitajast ei toeta lehelugemist 3x40 lasku standardi harjutustes!"), QMessageBox::Ok);
                                return;
                            }/*else if(leht->laskurid[j]->onVorguLaskur){
                                QMessageBox::critical(this, tr("Viga"), tr("Sellele laskurile juba loetakse lehti Lehelugejas!"), QMessageBox::Ok);
                                return;
                            }*/
    //                        int siffer = lehelugejaAken->m_ui.sifriEdit->text().toInt();
                            lehelugejaAken->m_ui.logi->append(QString("Siffer: %1").arg(siffer));
                            lehelugejaLeht = leht;
                            if(lehelugejaLaskur != 0)
                                lehelugejaLaskur->onLehelugejaLaskur = false;   //Vana laskur tuleb "vabastada"
                            lehelugejaLaskur = leht->laskurid[j]; //Pointer laskurile, keda vaja uuendada kui lehed on loetud
                            lehelugejaLaskur->onLehelugejaLaskur = true;
                            lehelugejaAken->m_ui.nimeBox->clear();
                            for(int k = 0; k < lehelugejaAken->seeriad.count(); k++){
                                lehelugejaAken->seeriad[k]->show();
                                lehelugejaAken->seeriad[k]->clear();
                                for(int l = 0; l < laskudeArv; l++){
                                    lehelugejaAken->lasud[k][l]->clear();
                                }
                            }
                            for(int k = 0; k < leht->laskurid.count(); k++)
                                lehelugejaAken->m_ui.nimeBox->addItem(leht->laskurid[k]->eesNimi->text() + " " +
                                                                      leht->laskurid[k]->perekNimi->text());
                            if(leht->seeriateArv <= 6){
                                for(int k = 0; k < leht->seeriateArv; k++){
                                    lehelugejaAken->seeriad[k]->setText(leht->laskurid[j]->seeriad[k]->text());
                                    for(int l = 0; l < laskudeArv; l++){
                                        lehelugejaAken->lasud[k][l]->set(leht->laskurid[j]->lasud[k][l]);
                                    }
                                }
                            }else{
                                QMessageBox::critical(this, tr("Viga"), tr("Liiga palju seeriaid, lugemine ei ole võimalik!"), QMessageBox::Ok);
                                return;
                            }
                            lehelugejaAken->m_ui.summaEdit->setText(leht->laskurid[j]->getSumma());
                            lehelugejaAken->m_ui.sifriLabel->setText(leht->laskurid[j]->sifriAlgus->text() + " - " + leht->laskurid[j]->sifriLopp->text());
                            lehelugejaAken->m_ui.nimeBox->setCurrentIndex(j);
                            int k = 0; //loetav seeria
                            /*while(!lehelugejaAken->seeriad[k]->text().isEmpty()){
                                k++;
                                if(k >= lehelugejaAken->seeriad.count()){
                                    k = lehelugejaAken->seeriad.count() - 1;
                                    break;
                                }
                            }*/
                            //laskude arv lehes
                            int laskuLehes = leht->seeriateArv * 10 / (leht->laskurid[j]->sifriLopp->text().toInt() -
                                                                       leht->laskurid[j]->sifriAlgus->text().toInt() + 1);
                            //mitmes leht (seni lastud lehtede arv)
                            int lehti = siffer - leht->laskurid[j]->sifriAlgus->text().toInt();
                            //lastud laskude arv ja sealt mitmendat seeriat hakatakse lugema
                            k = lehti * laskuLehes / 10;
                            for(int j = leht->seeriateArv; j < lehelugejaAken->seeriad.count(); j++)
                                lehelugejaAken->seeriad[j]->hide();
                            lehelugejaAken->seeriad[k]->selectAll();
                            lehelugejaAken->seeriad[k]->setFocus();
                            if(QualificationEvents::eventData(lehelugejaLeht->eventType()).targetType == TargetTypes::AirRifle)
                                lehelugejaAken->m_ui.leheCombo->setCurrentIndex(0);
                            else if(QualificationEvents::eventData(lehelugejaLeht->eventType()).targetType == TargetTypes::AirPistol)
                                lehelugejaAken->m_ui.leheCombo->setCurrentIndex(1);
                            else if(QualificationEvents::eventData(lehelugejaLeht->eventType()).targetType == TargetTypes::SmallboreRifle)
                                lehelugejaAken->m_ui.leheCombo->setCurrentIndex(2);
                            lehelugejaAken->m_ui.laskudeBox->setValue(laskuLehes);
                            lehelugejaAken->m_ui.kumnendikegaBox->setChecked(lehelugejaLeht->kumnendikega);
//                            lehelugejaAken->seadista();
                            lehelugejaAken->fookus->start();
                            return;
                        }
            }
        }
        QMessageBox::warning(this, tr("Hoiatus!"), "Sellist sifrit ei leitud!", QMessageBox::Ok);
    }
}

void Protokollitaja::uuendaLiikmeteKast()
{
        voibUuendadaNimekirja = false;
        lValik->jalgitavad.clear();
        lValik->ui.leheBox->clear();
        for(int i = 0; i < tabWidget->count(); i++)
                lValik->ui.leheBox->addItem(tabWidget->tabText(i));
        lValik->ui.leheBox->setCurrentIndex(0);
        voibUuendadaNimekirja = true;
        uuendaLiikmeteNimekirja(0);
}

void Protokollitaja::uuendaLiikmeteNimekirja(int)
{
        if(voibUuendadaNimekirja){
                lValik->ui.leheLaskurid->clear();
                Leht* leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(lValik->ui.leheBox->
                                currentIndex()))->widget());
                for(int i = 0; i < leht->laskurid.count(); i++){
                        QString rida = leht->laskurid[i]->perekNimi->text().trimmed() + ", ";
                        rida.append(leht->laskurid[i]->eesNimi->text().trimmed() + ", ");
                        rida.append(leht->laskurid[i]->klubi->text().trimmed() + ", ");
                        rida.append(leht->ekraaniNimi);
                        lValik->ui.leheLaskurid->addItem(rida);
                }
                lValik->ui.leheLaskurid->setCurrentRow(0);
        }
}

void Protokollitaja::uuendaSeaded()
{
    // From new settings class
    m_settings.setCompetitionShotTypes(seaded->ui.competitionTypesEdit->text());
    m_settings.setSighterShotTypes(seaded->ui.sighterTypesEdit->text());

        m_competitionName = seaded->ui.voistluseNimi->text();
        m_startDate = seaded->ui.startDateEdit->date();
        m_endDate = seaded->ui.endDateEdit->date();
        m_place = seaded->ui.kohtEdit->text();
        salvestaja->setInterval(seaded->ui.aegEdit->value() * 60000);
        switch(seaded->ui.sakiBox->currentIndex()){
        case 0: {
                tabWidget->setTabPosition(QTabWidget::North);
                break;
        }
        case 1: {
                tabWidget->setTabPosition(QTabWidget::South);
                break;
        }
        case 2: {
                tabWidget->setTabPosition(QTabWidget::West);
                break;
        }
        case 3: {
                tabWidget->setTabPosition(QTabWidget::East);
                break;
        }
        }
        if(seaded->ui.aegCombo->currentIndex() == 0)
                salvestaja->stop();
        else salvestaja->start();
        if(seaded->ui.kirjutusAbiCombo->currentIndex() == 1)
                kirjutusAbi = true;
        else kirjutusAbi = false;
        m_ranking = seaded->ui.jarjestamiseBox->currentIndex();

        uploadTimer.setInterval(seaded->ui.uploadTimeBox->value() * 1000);

        if(tabWidget->count() > 0){
                for(int i = 0; i < seaded->ui.sakid->topLevelItemCount(); i++){
                        seaded->ui.sakid->setCurrentItem(seaded->ui.sakid->topLevelItem(i));
                        if(tabWidget->tabText(i) != seaded->ui.sakid->currentItem()->text(0))
                                tabWidget->setTabText(i, seaded->ui.sakid->currentItem()->text(0));
                        Leht* leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());
                        if(leht->ekraaniNimi != seaded->ui.sakid->currentItem()->text(1))
                                leht->ekraaniNimi =	seaded->ui.sakid->currentItem()->text(1);
                        if(leht->minTime() != seaded->ui.sakid->currentItem()->text(2).toInt() * 1000)
                                leht->setMinTimeMs(seaded->ui.sakid->currentItem()->text(2).toInt() * 1000);
                        if(leht->maxTime() != seaded->ui.sakid->currentItem()->text(3).toInt() * 1000)
                                leht->setMaxTimeMs(seaded->ui.sakid->currentItem()->text(3).toInt() * 1000);
                        if(seaded->ui.sakid->currentItem()->checkState(4) == Qt::Unchecked)
                                leht->naidata = false;
                        else leht->naidata = true;
                        if(seaded->ui.sakid->currentItem()->text(5) == tr("Õhupüss"))
                                leht->m_targetType = TargetTypes::AirRifle;
                        else if(seaded->ui.sakid->currentItem()->text(5) == tr("Õhupüstol"))
                                leht->m_targetType = TargetTypes::AirPistol;
                        else if(seaded->ui.sakid->currentItem()->text(5) == tr("Sportpüss"))
                                leht->m_targetType = TargetTypes::SmallboreRifle;
                        else if(seaded->ui.sakid->currentItem()->text(5) == tr("Spordipüstol"))
                                leht->m_targetType = TargetTypes::FreePistol;
                        else if(seaded->ui.sakid->currentItem()->text(5) == tr("Muu püss"))
                                leht->m_targetType = TargetTypes::OtherRifle;
                        else if(seaded->ui.sakid->currentItem()->text(5) == tr("Muu püstol"))
                                leht->m_targetType = TargetTypes::OtherPistol;
                        else leht->m_targetType = TargetTypes::Other;
                        if(leht->eventType() != QualificationEvents::fromString(seaded->ui.sakid->currentItem()->text(6)))
                                leht->setEventType(QualificationEvents::fromString(seaded->ui.sakid->currentItem()->text(6)));
                        leht->kumnendikega = seaded->ui.sakid->currentItem()->checkState(7);
                        leht->setToBeUploaded(seaded->ui.sakid->currentItem()->checkState(8));
                }
        }
        voibSulgeda = false;
        writeSettings();
}

void Protokollitaja::uuendaVoistkondi() //Uuendadakse võistkondade tulemusi enne ekraanil näitamist ja peale liikmete valiku kasti sulgemist
{
    if(tabWidget->count() < 1) return;
        for(int i = 0; i < tabWidget->count(); i++){
            Leht *seeLeht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());
                if(seeLeht->voistk){
#ifdef QT_DEBUG
    qDebug() << "uuendaVoistkondi(): leheIndeks = " << seeLeht->leheIndeks;
#endif
                    for(int i = 0; i < seeLeht->voistkonnad.count(); i++)
                        for(int j = 0; j < seeLeht->voistkonnad[i]->voistlejad.count(); j++)
                            for(int m = 0; m < seeLeht->jalgitavad.count(); m++)
                                for(int n = 0; n < tabWidget->count(); n++){
                                    Leht *andmed = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(n))->widget());
                                    if(andmed->leheIndeks == seeLeht->jalgitavad[m])
                                        for(int b = 0; b < andmed->laskurid.count(); b++)
                                            if(andmed->laskurid[b]->eesNimi->text().trimmed() == seeLeht->voistkonnad[i]->voistlejad[j]->eesNimi.trimmed()
                                                    && andmed->laskurid[b]->perekNimi->text().trimmed() == seeLeht->voistkonnad[i]->voistlejad[j]->perekNimi.trimmed()
                                                    && andmed->laskurid[b]->klubi->text().trimmed() == seeLeht->voistkonnad[i]->voistlejad[j]->klubi.trimmed()
                                                    && andmed->ekraaniNimi == seeLeht->voistkonnad[i]->voistlejad[j]->harjutus){
                                                seeLeht->voistkonnad[i]->voistlejad[j]->summa = andmed->laskurid[b]->getSumma().trimmed();
                                                seeLeht->voistkonnad[i]->voistlejad[j]->silt->setText(seeLeht->voistkonnad[i]->voistlejad[j]->perekNimi.trimmed()
                                                                                                      + ": " + seeLeht->voistkonnad[i]->voistlejad[j]->summa);
                                            }
                                }
                    for(int i = 0; i < seeLeht->voistkonnad.count(); i++)
                        seeLeht->voistkonnad[i]->liida();
                }
        }
}

void Protokollitaja::uuendaVorkuSifriga(int siffer, int socketIndex)
{
    if(tabWidget->count() > 0){
        int laskudeArv = 10;
//        if(vorguLaskur != 0){
//            vorguLaskur->onVorguLaskur = false;    //Vana laskur tuleb "vabastada"
//            vorguLaskur->setEnabled(true);  //Eelmise laskuri võib nüüd vabastada, enne kui uus blokeeritakse
//        }
    for(int i = 0; i < tabWidget->count(); i++){
        Leht* leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());
        if(!leht->voistk){
            for(int j = 0; j < leht->laskurid.count(); j++)
                if(!leht->laskurid[j]->sifriAlgus->text().isEmpty())
                    if(siffer >= leht->laskurid[j]->sifriAlgus->text().toInt() && siffer < leht->laskurid[j]->sifriLopp->text().toInt()){
                        if(leht->laskurid[j]->onLehelugejaLaskur){
                                        saadaVorku("Viga:" + tr("Sellele laskurile juba loetakse lehti Protokollitajas!"), socketIndex);
                            return;
                        }
                        Laskur *seeLaskur = leht->laskurid[j];
//                        if(leht->seeriateArv > 6){
//                            QMessageBox::critical(this, tr("Viga"), tr("Kahjuks see versioon Protokollitajast toetab "
//                                    "lehelugemist ainult õhupüssi ja õhupüstoli harjutustes!"), QMessageBox::Ok);
//                            return;
//                        }
//                        int siffer = lehelugejaAken->m_ui.sifriEdit->text().toInt();
//                        lehelugejaAken->m_ui.logi->append(QString("Siffer: %1").arg(siffer));
                        QString pakett = "Laskur:"; //Protolehelugejasse saadetav pakett
                                //"Laskur:siffer - siffer;Eesnimi;Perekonnanimi;seeriate arv;laskude arv;seeriad;selle seeria lasud; x; y; summa;aktiivne seeria;harjutus;lasku lehes;kümnendikega lugemine (true/false)
//                        vorguLeht = leht;

//                        vorguLaskur = vorguLeht->laskurid[j]; //Pointer laskurile, keda vaja uuendada kui lehed on loetud
//                        vorguLaskur->onVorguLaskur = true; //Uus laskur tuleb "märgistada"
//                        lehelugejaAken->m_ui.nimeBox->clear();
//                        for(int k = 0; k < lehelugejaAken->seeriad.count(); k++){
//                            lehelugejaAken->seeriad[k]->show();
//                            lehelugejaAken->seeriad[k]->clear();
//                        }
                        pakett.append(seeLaskur->sifriAlgus->text() + " - " + seeLaskur->sifriLopp->text() + ";");
                        pakett.append(seeLaskur->eesNimi->text() + ";" + seeLaskur->perekNimi->text() + ";");

//                        for(int k = 0; k < leht->laskurid.count(); k++)
//                            lehelugejaAken->m_ui.nimeBox->addItem(leht->laskurid[k]->eesNimi->text() + " " +
//                                                                  leht->laskurid[k]->perekNimi->text());
                        pakett.append(QString("%1;%2;").arg(leht->seeriateArv).arg(laskudeArv));    //Laskude arv on hetkel ainult 10
                        if(leht->seeriateArv <= 12){
                            for(int k = 0; k < leht->seeriateArv; k++){
                                pakett.append(seeLaskur->seeriad[k]->text() + ";");
                                for(int j = 0; j < laskudeArv; j++){
                                    pakett.append(QString("%1;%2;%3;").arg(seeLaskur->lasud[k][j]->getSLask())
                                                  .arg(seeLaskur->lasud[k][j]->stringX()).arg(seeLaskur->lasud[k][j]->stringY()));
                                }
                            }
                        }else{
                            saadaVorku("Viga:" + tr("Sellist harjutust ei toetata!\nSeeriate arv liiga suur."), socketIndex);
//                            vorguLaskur->onVorguLaskur = false;
//                            vorguLaskur = 0;
//                            vorguLeht = 0;
                            return;
                        }
//                                lehelugejaAken->seeriad[k]->setText(leht->laskurid[j]->seeriad[k]->text());
                        pakett.append(seeLaskur->getSumma() + ";");
//                        lehelugejaAken->m_ui.summaEdit->setText(leht->laskurid[j]->summa->text());
//                        lehelugejaAken->m_ui.sifriLabel->setText(leht->laskurid[j]->sifriAlgus->text() + " - " +
//                                                                 leht->laskurid[j]->sifriLopp->text());
//                        lehelugejaAken->m_ui.nimeBox->setCurrentIndex(j);
                        int k = 0; //loetav seeria
                        /*while(!lehelugejaAken->seeriad[k]->text().isEmpty()){
                            k++;
                            if(k >= lehelugejaAken->seeriad.count()){
                                k = lehelugejaAken->seeriad.count() - 1;
                                break;
                            }
                        }*/
                        //laskude arv lehes
                        int laskuLehes = leht->seeriateArv * 10 / (seeLaskur->sifriLopp->text().toInt() -
                                                                   seeLaskur->sifriAlgus->text().toInt() + 1);
                        //mitmes leht (seni lastud lehtede arv)
                        int lehti = siffer - seeLaskur->sifriAlgus->text().toInt();
                        //lastud laskude arv ja sealt mitmendat seeriat hakatakse lugema
                        k = lehti * laskuLehes / 10;
//                        for(int j = leht->seeriateArv; j < lehelugejaAken->seeriad.count(); j++)
//                            lehelugejaAken->seeriad[j]->hide();
//                        lehelugejaAken->seeriad[k]->selectAll();
//                        lehelugejaAken->seeriad[k]->setFocus();
                        pakett.append(QString("%1;").arg(k));
                        pakett.append(QString("%1;").arg(leht->m_targetType));
//                        if(leht->harjutus.contains(tr("Õhupüss")))
//                            lehelugejaAken->m_ui.leheCombo->setCurrentIndex(0);
//                        else if(leht->harjutus.contains(tr("Õhupüstol")))
//                            lehelugejaAken->m_ui.leheCombo->setCurrentIndex(1);
//                        else if(leht->harjutus.contains("Standard", Qt::CaseInsensitive) || leht->harjutus.contains("Lamades", Qt::CaseInsensitive))
//                            lehelugejaAken->m_ui.leheCombo->setCurrentIndex(2);
                        pakett.append(QString("%1;").arg(laskuLehes));
//                        lehelugejaAken->m_ui.laskudeBox->setValue(laskuLehes);
                        if(leht->kumnendikega)
                            pakett.append("true");
                        else pakett.append("false");
//                        lehelugejaAken->m_ui.kumnendikegaBox->setChecked(lehelugejaLeht->kumnendikega);
//                        lehelugejaAken->seadista();
                        saadaVorku(pakett, socketIndex);
//                        for(int i = 0; i < leht->seeriateArv; i++){ //Välistamaks, et samal ajal, kui lehti loetakse, muudab keegi käsitsi seeriaid, tuleb muutmine blokeerida
//                            vorguLaskur->seeriad[i]->setEnabled(false);
//                        }
//                        vorguLaskur->summa->setEnabled(false);
//                        vorguLaskur->setEnabled(false); //Välistamaks, et samal ajal, kui lehti loetakse, muudab keegi käsitsi
                                                        //seeriaid, tuleb muutmine blokeerida
                        return;
                    }
        }
    }
//    QMessageBox::warning(this, tr("Hoiatus!"), "Sellist sifrit ei leitud!", QMessageBox::Ok);
    saadaVorku("Hoiatus:" + tr("Sellist sifrit ei leitud!"), socketIndex);
//    vorguLaskur = 0;
//    vorguLeht = 0;
    }
}

void Protokollitaja::uhenduSiusDataga()
{
    QString logsDir = QDir(QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).filePath("Protokollitaja")).filePath("Protokollitaja logs");
    QDir dir;
    dir.mkpath(logsDir);

    QString incomingFileName = QString("Protokollitaja incoming %1.log").arg(QDate::currentDate().toString(Qt::ISODate));
    siusLogi = new QFile(QDir(logsDir).filePath(incomingFileName));

    if(siusDataConnections == nullptr){
        siusDataConnections = new SiusDataConnections(
                    siusLogi,
                    &logiValja,
                    &m_settings,
                    this
                    );
        connect(siusDataConnections, &SiusDataConnections::statusInfo, this, &Protokollitaja::statusBarInfoChanged);
        connect(siusDataConnections, &SiusDataConnections::shotRead, this, &Protokollitaja::readSiusInfo);
        connect(siusDataConnections, &SiusDataConnections::disconnectedFromSius, this, &Protokollitaja::uhendusSiusigaKatkes);
    }

    if(siusLogi->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){ //Saabunud võrguliikluse logi
        QTextStream valja(siusLogi);
        valja << "/////////////////////" << m_competitionName << ", " << QDateTime::currentDateTime().toString() << ", " << programmiNimi << " (" << versioon << ")/////////////////////\n";
        siusLogi->close();
    }

    siusDataConnections->exec();
}

void Protokollitaja::uhendusSiusigaKatkes(int connectionIndex)
{
    logiValja << "#Ühendus SiusData'ga katkes, buffer.length(): " << siusiBuffer.length() << "\n";
    for(int i = 0; i < tabWidget->count(); i++){
        Leht* leht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(i))->widget());
        leht->siusiReset(connectionIndex);
    }
//    teatekast.setText(tr("Ühendus SiusData'ga katkes!"));
//    teatekast.show();
}

void Protokollitaja::uus()
{
        if(!voibSulgeda){
        int vastus = QMessageBox::question(this, "Protokollitaja", tr("Kas soovid muudatused salvestada?"),
                        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
                if(vastus == QMessageBox::Save)	salvesta();
                else if(vastus == QMessageBox::Cancel) return;
        }
        for(int i = 0; i < tabWidget->count(); i++)
                tabWidget->widget(i)->deleteLater();
        tulemus->voistluseNimi = " ";
        tulemus->pealKiri = " ";

        algseaded();

        for(int j = 0; j < tulemus->getRidadeArv(); j++)
                for(int k = 0; k < 12; k++)
                        tulemus->read[j][k] = " ";
        tulemus->joonista();
        valik->ui.indBox->setCurrentIndex(0);
        if(aValik->exec() == QDialog::Accepted){
            setDataFromInitialDialog();
        }else{
            sulgeja->start(50);
            return;
        }
        if(m_competitionName.isEmpty()) sulgeja->start(50);
        loefail();
}

void Protokollitaja::uusLaskur()    //Uue laskuri loomine,  koos uue ID'ga
{
    if(tabWidget->count() > 0){
        kontrolliIdKordusi();
        Leht* seeLeht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
        seeLeht->uusLaskur(++laskuriId);
        if(seeLeht->voistk)
            seeLeht->voistkonnad[seeLeht->voistkonnad.count() - 1]->nimi->setFocus();
        else seeLeht->laskurid[seeLeht->laskurid.count() -1]->eesNimi->setFocus();
        viiLopuni->start();
    }
}

void Protokollitaja::uusLaskur(int i)   //Uue laskuri loomine, koos olemasoleva ID'ga (kasutatakse näiteks faili avamisel
{
        if(tabWidget->count() > 0){
                Leht* seeLeht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
                seeLeht->uusLaskur(i);
                if(seeLeht->voistk)
                        seeLeht->voistkonnad[seeLeht->voistkonnad.count() - 1]->nimi->setFocus();
                else seeLeht->laskurid[seeLeht->laskurid.count() -1]->eesNimi->setFocus();
                viiLopuni->start();
        }
}

void Protokollitaja::uusLaskur(QJsonObject json)
{
    if (tabWidget->count() > 0) {
        Leht* seeLeht = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget());
        seeLeht->uusLaskur(json);
        if (seeLeht->voistk)
            seeLeht->voistkonnad[seeLeht->voistkonnad.count() - 1]->nimi->setFocus();
        else
            seeLeht->laskurid[seeLeht->laskurid.count() -1]->eesNimi->setFocus();
        viiLopuni->start();
    }
}

void Protokollitaja::uusTab()
{
        int a = abi;
        /*for(int i = 0; i < tabWidget->count(); i++)
                valik->ui.leheValikBox->addItem(tabWidget->tabText(i));*/
        if(tabWidget->count() < 1){
                //valik->ui.leheValikBox->setEnabled(false);
                valik->ui.indBox->setEnabled(false);
        }else{
                valik->ui.indBox->setEnabled(true);
        }
        if(valik->exec() == QDialog::Accepted){
                if(valik->ui.sakiNimi->text().isEmpty()){
                        QMessageBox::warning(this, "Protokollitaja", tr("Töölehe nime lahter on tühi. Ei saa luua uut töölehte ilma nimeta."), QMessageBox::Ok);
                        return;
                }
                if(valik->ui.nimiTulAknas->text().isEmpty()){
                        QMessageBox::warning(this, "Protokollitaja", tr("Tulemuse aknas näidatava tabeli pealkirja lahter on tühi. Paluks see ka ikka täita"), QMessageBox::Ok);
                        return;
                }
                leheIndeks++;
                voibSulgeda = false;
                if((valik->m_targetType == TargetTypes::AirRifle ||
                    valik->m_targetType == TargetTypes::SmallboreRifle ||
                    valik->m_targetType == TargetTypes::OtherRifle) &&
                    abi != (int)TargetTypes::Other)
                        a = (int)TargetTypes::AirRifle;
                else if((valik->m_targetType == TargetTypes::AirPistol ||
                    valik->m_targetType == TargetTypes::FreePistol ||
                    valik->m_targetType == TargetTypes::OtherPistol) &&
                    abi != (int)TargetTypes::Other)
                        a = (int)TargetTypes::AirPistol;
                bool voistk = false;
                if(valik->ui.indBox->currentIndex() == 0)
                        voistk = false;
                else voistk = true;
                bool kumnendikega = false;
                if(valik->ui.kumnendikegaBox->isChecked())
                    kumnendikega = true;
                /*Leht *individualistid = 0;
                if(voistk)
                        individualistid = dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->widget(valik->ui.
                                        leheValikBox->currentIndex()))->widget());
                individualistid->jalgijad++;*/
                QScrollArea *area = new QScrollArea(tabWidget);
                area->setWidgetResizable(true);
                area->setWidget(new Leht(&andmebaas, valik->ui.seeriateArv->value(), valik->ui.vSummadeArv->value(), a, &kirjutusAbi, valik->ui.nimiTulAknas->text(), valik->m_targetType, valik->eventType(), kumnendikega, &m_ranking, tabWidget, voistk, lValik, leheIndeks, valik->ui.laskudeArv->value()));
                int newTabIndex = tabWidget->addTab(area, valik->ui.sakiNimi->text());
                tabWidget->setCurrentIndex(newTabIndex);
                connect(dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget())
                                , SIGNAL(uuendaLiikmeid()), this, SLOT(uuendaLiikmeteKast()));
                connect(dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget())
                                , SIGNAL(uuendaVoistkondi()), this, SLOT(uuendaVoistkondi()));
                connect(dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget())
                                , SIGNAL(muudatus()), this, SLOT(muudaSalvestamist()));
                connect(dynamic_cast<Leht*>(dynamic_cast<QScrollArea*>(tabWidget->currentWidget())->widget())
                                , SIGNAL(idMuutus(int,Laskur*)), this, SLOT(kontrolliIdKordust(int,Laskur*)));
        }
}

void Protokollitaja::viiLoppu()
{
        QScrollArea* area = dynamic_cast<QScrollArea*>(tabWidget->currentWidget());
        area->verticalScrollBar()->setValue(area->verticalScrollBar()->maximum());
}

void Protokollitaja::writeSettings()
{
    m_settings.setLastOpenFileName(seeFail);
    m_settings.writeSettings();
}

Protokollitaja::~Protokollitaja()
{
    if(siusDataConnections != nullptr)
        siusDataConnections->deleteLater();
}

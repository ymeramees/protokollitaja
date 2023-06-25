#include <QtGui>
#include "tulemuseaken.h"

extern QString versioon;

TulemuseAken::TulemuseAken(QWidget *parent) : QWidget(parent)
{
        if(this->objectName().isEmpty()){
                this->setObjectName("TulemuseAken");
        }
        setWindowTitle("Protokollitaja - tulemused");
        //setWindowIcon(QIcon(":/images/Finaal.ico"));
        ind = true;
        loplik = false;
        mitmeJarel = 0;
        est = new QLocale(QLocale::Estonian, QLocale::Estonia);
        kirjaFont.setPointSize(20);
        //kirjaFont.setBold(true);
        summaF.setPointSize(20);
        summaF.setBold(true);
        setFont(kirjaFont);
        paiseFont.setPointSize(16);
        paiseFont.setBold(false);
        pealkirjaFont.setPointSize(32);
        pealkirjaFont.setBold(true);
        pisike.setPointSize(12);
        setBackgroundRole(QPalette::Base);
        setAutoFillBackground(true);
        pealKiri = "Pealkiri";
        ridadeArv = 25;
        for(int i = 0; i < ridadeArv; i++){
                QStringList rida;
                for(int j = 0; j < 16; j++)
                        rida << " ";
                read << rida;
        }
        fontAct = new QAction(tr("Muuda kirja font..."), this);
        fontAct->setStatusTip(tr("Muuda selle akna kirja"));
        connect(fontAct, SIGNAL(triggered()), this, SLOT(muudaKirjaFont()));
        pealkirjaFontAct = new QAction(tr("Muuda pealkirja font..."), this);
        pealkirjaFontAct->setStatusTip(tr("Muuda selle akna pealkirja"));
        connect(pealkirjaFontAct, SIGNAL(triggered()), this, SLOT(muudaPealkirjaFont()));
        paiseFontAct = new QAction(tr("Muuda päise font..."), this);
        paiseFontAct->setStatusTip(tr("Muuda selle akna päise kirja"));
        connect(paiseFontAct, SIGNAL(triggered()), this, SLOT(muudaPaiseFont()));
        fullScreenAct = new QAction(tr("Täisekraan vaade"), this);
        fullScreenAct->setShortcut(tr("F"));
        fullScreenAct->setStatusTip(tr("Näita seda akent täisekraan vaates"));
        connect(fullScreenAct, SIGNAL(triggered()), this, SLOT(fullScreen()));

//        connect(this, SIGNAL(QGuiApplication::screenAdded(QScreen*)), this, SLOT(TulemuseAken::fullScreen(QScreen*)));    // FIXME To be fixed
//        connect(this, SIGNAL(QGuiApplication::screenRemoved(QScreen*)), this, SLOT(TulemuseAken::fullScreen(QScreen*)));

        popup = new QMenu(this);
        popup->addAction(fontAct);
        popup->addAction(pealkirjaFontAct);
        popup->addAction(paiseFontAct);
        popup->addAction(fullScreenAct);
        (void) new QShortcut(Qt::Key_F, this, SLOT(fullScreen()));
        pilt = new QPixmap(1600, 1200/*, QImage::Format_ARGB32*/);
        painter = new QPainter(pilt);
        silt = new QLabel(this);
        silt->setBackgroundRole(QPalette::Base);
        silt->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        silt->setScaledContents(true);
        silt->setGeometry(0, 0, this->width(), this->height());
        painter->setFont(kirjaFont);
        silt->setPixmap(/*QPixmap::fromImage(*/*pilt);
}

TulemuseAken::~TulemuseAken()
{
}

void TulemuseAken::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
        silt->setGeometry(0, 0, this->width(), this->height());
}

void TulemuseAken::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event);
        popup->exec(event->globalPos());
}

void TulemuseAken::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
        fullScreen();
}

void TulemuseAken::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
        emit naitaJargmist();
}

void TulemuseAken::fullScreen()
{
        if(this->windowState() == Qt::WindowFullScreen)
                this->setWindowState(Qt::WindowNoState);
        else this->setWindowState(Qt::WindowFullScreen);
}

void TulemuseAken::fullScreen(QScreen *s)
{
        Q_UNUSED(s);
        if(this->windowState() == Qt::WindowFullScreen && QGuiApplication::screens().size() < 2)
            this->setWindowState(Qt::WindowNoState);
        //else this->setWindowState(Qt::WindowFullScreen);
}

void TulemuseAken::muudaKirjaFont()
{
        kirjaFont = QFontDialog::getFont(0, kirjaFont);
        this->setFont(kirjaFont);
        painter->setFont(kirjaFont);
        joonista();
}

void TulemuseAken::muudaPealkirjaFont()
{
        pealkirjaFont = QFontDialog::getFont(0, pealkirjaFont);
        //this->setFont(pealkirjaFont);
        painter->setFont(pealkirjaFont);
        joonista();
}

void TulemuseAken::muudaPaiseFont()
{
        paiseFont = QFontDialog::getFont(0, paiseFont);
        //this->setFont(kirjaFont);
        painter->setFont(paiseFont);
        joonista();
}

void TulemuseAken::joonista()
{
        //QPen pen(painter->pen());
        pilt->fill();
        //painter->setPen(QColor(Qt::red));
        painter->setFont(pisike);
        painter->drawText(1460, 1190, "Protokollitaja " + versioon);
        painter->drawText(1, 1190, "Autor: Ümeramees");
        painter->setFont(kirjaFont);
        painter->drawText(450, 1190, "Vaata tulemusi internetis: https://protokollitaja.eu");
        painter->setFont(pealkirjaFont);
        painter->drawText(60, 50, voistluseNimi);
        painter->setFont(paiseFont);
        painter->drawText(1345, 130, "Summa");
        if(ind){
                if(loplik){
                        painter->drawText(1450, 130, "Finaal");
                }else painter->drawText(1450, 130, "Keskm. l.");
                if(mitmeJarel == 4){
                        painter->drawText(1100, 130, "Seeriad");
                        painter->drawText(850, 130, tr("Põlv"));
                        painter->drawText(910, 130, "Lam.");
                }else painter->drawText(1000, 130, "Seeriad");
        }else{
                painter->drawText(1450, 130, "Keskm.");
        }
        painter->setFont(kirjaFont);
        painter->drawText(800, 50, 700, 50, Qt::AlignRight, aegKoht);
        painter->drawText(75, 125, pealKiri);
        for(int i = 0; i < ridadeArv; i++){
                painter->drawText(20, 175 + i * 41, read[i][0]);
                painter->drawText(70, 175 + i * 41, read[i][1]);
                painter->drawText(255, 175 + i * 41, read[i][2]);
                if(ind){
                    painter->drawText(495, 175 + i * 41, read[i][3]);
                    painter->drawText(575, 175 + i * 41, read[i][4]);
                    //painter->setLayoutDirection(Qt::RightToLeft);
                    painter->drawText(790, 175 + i * 41, read[i][5]);
                    if(mitmeJarel == 4) painter->setFont(summaF);
                    painter->drawText(850, 175 + i * 41, read[i][6]);

                    if(mitmeJarel == 0){
                        painter->drawText(910 - 50, 175 + i * 41, read[i][7]);
                        painter->drawText(970 - 30, 175 + i * 41, read[i][8]);
                        painter->drawText(1030 - 10, 175 + i * 41, read[i][9]);
                        painter->drawText(1090 + 10, 175 + i * 41, read[i][10]);
                        painter->drawText(1150 + 30, 175 + i * 41, read[i][11]);
                        painter->drawText(1210 + 50, 175 + i * 41, read[i][12]);
                        painter->setFont(summaF);
                        painter->drawText(1270 + 70, 175 + i * 41, read[i][13]);
                    }else{
                        if(mitmeJarel == 2) painter->setFont(summaF);
                        painter->drawText(910/* - 50*/, 175 + i * 41, read[i][7]);
                        if(mitmeJarel == 3) painter->setFont(summaF);
                        if(mitmeJarel == 2 || mitmeJarel == 4) painter->setFont(kirjaFont);
                        painter->drawText(970/* - 30*/, 175 + i * 41, read[i][8]);
                        if(mitmeJarel == 3) painter->setFont(kirjaFont);
                        painter->drawText(1030 /*- 10*/, 175 + i * 41, read[i][9]);
                        if(mitmeJarel == 2) painter->setFont(summaF);
                        painter->drawText(1090 /*+ 10*/, 175 + i * 41, read[i][10]);
                        if(mitmeJarel == 2) painter->setFont(kirjaFont);
                        painter->drawText(1150/* + 30*/, 175 + i * 41, read[i][11]);
                        if(mitmeJarel == 3) painter->setFont(summaF);
                        painter->drawText(1210 /*+ 50*/, 175 + i * 41, read[i][12]);
                        painter->setFont(summaF);
                        painter->drawText(1270 /*+ 70*/, 175 + i * 41, read[i][13]);
                    }
                    //painter->setLayoutDirection(Qt::LeftToRight);
                    painter->drawText(1355, 175 + i * 41, read[i][14]);
                    painter->setFont(kirjaFont);
                    if(loplik)
                        painter->drawText(1430, 175 + i * 41, read[i][15]);
                    else painter->drawText(1450, 175 + i * 41, read[i][15]);
                }else{
                        painter->drawText(550, 175 + i * 41, read[i][3]);
                        painter->drawText(650, 175 + i * 41, read[i][4]);
                        painter->drawText(750, 175 + i * 41, read[i][5]);
                        painter->drawText(850, 175 + i * 41, read[i][6]);
                        painter->drawText(950, 175 + i * 41, read[i][7]);
                        painter->drawText(1050, 175 + i * 41, read[i][8]);
                        painter->drawText(1150, 175 + i * 41, read[i][9]);
                        painter->drawText(1250, 175 + i * 41, read[i][10]);
                        painter->drawText(1355, 175 + i * 41, read[i][11]);
                        if(loplik)
                                painter->drawText(1430, 175 + i * 41, read[i][12]);
                        else painter->drawText(1450, 175 + i * 41, read[i][12]);
                }
        }
/*	painter->drawText(500, 200, *rad);
        painter->drawText(700, 200, *las);
        painter->drawText(950, 200, *see);
        painter->drawText(1170, 200, *kok);
        painter->drawText(1400, 200, *vah);
        painter->drawLine(20, 210, 1580, 210);
        for(int i = 0; i < 8; i++){
                int y = 270 + 120 * i;
                if(*las == "10. lask"){
                        if(i < 8)
                                if(*summa[i] == *summa[i+1] && *summa[i] != "0,0")
                                        painter->setPen(QColor(Qt::red));
                        if(i != 0)
                                if(*summa[i] == *summa[i-1] && *summa[i] != "0,0")
                                        painter->setPen(QColor(Qt::red));
                }
                painter->drawText(20, y, 60, 60, Qt::AlignCenter, *koht[i]);
                painter->drawText(90, y, 420, 60, Qt::AlignLeft | Qt::AlignVCenter, *nimi[i]);
                painter->drawText(500, y, 150, 60, Qt::AlignCenter, *rada[i]);
                painter->drawText(700, y, 200, 60, Qt::AlignCenter, *lask[i]);
                painter->drawText(950, y, 200, 60, Qt::AlignCenter, *seeria[i]);
                painter->drawText(1120, y, 300, 60, Qt::AlignCenter, *summa[i]);
                painter->drawText(1380, y, 200, 60, Qt::AlignCenter, *vahe[i]);
                painter->setPen(pen);
        }*/
        silt->setPixmap(/*QPixmap::fromImage(*/*pilt);
        mitmeJarel = 0;
}

int TulemuseAken::getRidadeArv()
{
        return ridadeArv;
}

void TulemuseAken::muudaRidadeArv(int r)
{
        if(ridadeArv < r){
                for(int i = read.count(); i < r; i++){
                        QStringList rida;
                        for(int j = 0; j < 12; j++)
                                rida << " ";
                        read << rida;
                }
        }
        ridadeArv = r;
}

#ifndef TULEMUSEAKEN_H_
#define TULEMUSEAKEN_H_

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QMenu>
#else
#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QSpacerItem>
#include <QtGui/QMenu>
#endif
#include <QShortcut>
#include <QFontDialog>
#include <QtCore/QVariant>
#include <QScreen>

#include "targettypes.h"

class DuelPair;
class DuelView;

class TulemuseAken : public QWidget
{

	Q_OBJECT

private:
	int ridadeArv;

public:
	bool ind;
	bool loplik;
	int mitmeJarel;
	QFont kirjaFont;
	QFont paiseFont;
	QFont pealkirjaFont;
	QFont pisike;
	QFont summaF;
	QPixmap *pilt;
	QPainter *painter;
	QLabel *silt;
	DuelView *duel;   //Duel match's view, shown instead of silt, when a duel match is shown
	QString voistluseNimi;
	QString aegKoht;
	QString pealKiri;
	QList<QStringList> read;
	QLocale *est;
	QMenu *popup;
	QTimer *timer;
	QAction *fontAct;
	QAction *pealkirjaFontAct;
	QAction *paiseFontAct;
	QAction *fullScreenAct;
	TulemuseAken(QWidget *parent = 0);
	virtual ~TulemuseAken();
	void resizeEvent(QResizeEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);
	int getRidadeArv();
	void  muudaRidadeArv(int);
	void showDuel(const QList<DuelPair*> &pairs, const TargetTypes::TargetType targetType);   //Shows the duel match's view
	void showResults();   //Shows the ordinary results view
public slots:
	void muudaKirjaFont();
	void muudaPealkirjaFont();
	void muudaPaiseFont();
    void fullScreen();
    void fullScreen(QScreen*);   //Kui projektor järelt ära võetakse, teha aken mitte fullscreeniks, et arvuti ekraan seda täis ei läheks
	void joonista();
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

signals:
	void naitaJargmist();
};

#endif /*TULEMUSEAKEN_H_*/

#ifndef LISALASKUDEAKEN_H
#define LISALASKUDEAKEN_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QDialog>
#else
#include <QtGui/QDialog>
#endif
#include <QInputDialog>
#include "ui_lisalaskudeaken.h"

class LisaLaskudeAken : public QDialog
{
    Q_OBJECT

public:
    LisaLaskudeAken(QWidget *parent = 0);
    ~LisaLaskudeAken();

    Ui::LisaLaskudeAkenClass ui;

public slots:
	void ules();
	void alla();
	void eemalda();
	void lisa();
};

#endif // LISALASKUDEAKEN_H

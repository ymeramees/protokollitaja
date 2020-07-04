#ifndef SEADEDKAST_H
#define SEADEDKAST_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QDialog>
#else
#include <QtGui/QDialog>
#endif
#include <QCloseEvent>
#include "ui_seadedkast.h"

class SeadedKast : public QDialog
{
    Q_OBJECT

    void closeEvent(QCloseEvent *event);

public:
//	int kirjutusAbi, autosave, aeg, sakiAsukoht;
//	QString voistluseNimi, aegKoht;
    SeadedKast(QString nim, QString aegK, QWidget *parent = 0);
    ~SeadedKast();
    Ui::SeadedKastClass ui;

public slots:
	void salvesta();

Q_SIGNALS:
	void salvestatud();
};

#endif // SEADEDKAST_H

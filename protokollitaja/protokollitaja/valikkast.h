#ifndef VALIKKAST_H
#define VALIKKAST_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QDialog>
#else
#include <QtGui/QDialog>
#endif
#include <QCloseEvent>
#include <QMessageBox>
#include "ui_valikkast.h"
#include "commonsettings.h"
#include "ageclasses.h"
#include "qualificationevents.h"
#include "targettypes.h"

class ValikKast : public QDialog
{
    Q_OBJECT

public:
    TargetTypes::TargetType m_targetType;   //defineeritud järgmiselt
    // enum Relv {Muu = 0, Ohupuss = 1, Ohupustol = 2, Sportpuss = 3, Spordipustol = 4, Puss = 5, Pustol = 6};
    QualificationEvents::EventType eventType() const;
    ValikKast(QWidget *parent = 0);
    ~ValikKast();
    Ui::ValikKastClass ui;

protected:

public slots:
	void muudaEkraaniNimi(int);
        void muudaHarjutus(int);
	void voimalda(int indeks);
	void sulge();

private:
    void changeEvent(QEvent *event);
    void updateUi();
};

#endif // VALIKKAST_H

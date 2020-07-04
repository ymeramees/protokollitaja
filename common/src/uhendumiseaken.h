#ifndef UHENDUMISEAKEN_H
#define UHENDUMISEAKEN_H

#include <QDialog>
#include "ui_uhendumiseaken.h"

class UhendumiseAken : public QDialog
{
    Q_OBJECT

public:
    explicit UhendumiseAken(QWidget *parent = 0);
    QString aadress;
    int port;
    ~UhendumiseAken();
    Ui::UhendumiseAken ui;
};

#endif // UHENDUMISEAKEN_H

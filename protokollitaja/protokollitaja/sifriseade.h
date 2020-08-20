#ifndef SIFRISEADE_H
#define SIFRISEADE_H

#include <QDialog>
#include "ui_sifriseade.h"

class SifriSeade : public QDialog
{
    Q_OBJECT

public:
    explicit SifriSeade(QWidget *parent = 0);
    ~SifriSeade();
    Ui::SifriSeade ui;
};

#endif // SIFRISEADE_H

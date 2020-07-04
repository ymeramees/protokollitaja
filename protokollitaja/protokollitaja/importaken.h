#ifndef IMPORTAKEN_H
#define IMPORTAKEN_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QDialog>
#else
#include <QtGui/QDialog>
#endif

#include <QFileDialog>
#include <QTextStream>
#include <QClipboard>
#include <QFile>

#include "ui_importaken.h"
#include "laskur.h"
#include "leht.h"

class ImportAken : public QDialog
{
    Q_OBJECT

private:
    bool kirjutusabi = false;

public:
    Leht* leht;
    ImportAken(QWidget *parent = 0);
    ~ImportAken();
    Ui::ImportAkenClass ui;

public slots:
	void fromFile();
        void clearSheet();
	void fromClipboard();
};

#endif // IMPORTAKEN_H

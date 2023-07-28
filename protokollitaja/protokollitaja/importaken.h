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
#include "tabwidgetwithsettings.h"
#include "qualificationevents.h"
#include "kllfilerw.h"
#include "laskur.h"
#include "leht.h"

class ImportAken : public QDialog
{
    Q_OBJECT

private:
    bool kirjutusabi = false;
    int m_currentCompetitorId;

public:
    Leht* leht = nullptr;
    ImportAken(QWidget *parent = 0);
    ~ImportAken();
    Ui::ImportAkenClass ui;
    int currentCompetitorId();
    void setCurrentCompetitorId(int newId);

public slots:
    void clearSheet();
    void fromClipboard();
    void fromFile();
    void fromKllFile(QString fileName);
};

#endif // IMPORTAKEN_H

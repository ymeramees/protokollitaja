#ifndef NEWTABDIALOG_H
#define NEWTABDIALOG_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QDialog>
#else
#include <QtGui/QDialog>
#endif
#include <QCloseEvent>
#include <QMessageBox>
#include "ui_newtabdialog.h"
#include "commonsettings.h"
#include "ageclasses.h"
#include "qualificationevents.h"
#include "targettypes.h"

class NewTabDialog : public QDialog
{
    Q_OBJECT

public:
    TargetTypes::TargetType m_targetType;
    QualificationEvents::EventType eventType() const;
    NewTabDialog(QWidget *parent = 0);
    ~NewTabDialog();
    bool isDuelMatch() const;
    bool isTeamEvent() const;
    QString leftTeamName() const;
    int pairsCount() const;
    QString rightTeamName() const;
    Ui::ValikKastClass ui;

protected:

private:
    void changeEvent(QEvent *event);
    void updateDuelFields();
    void updateUi();

private slots:
    void changeScreenName(int);
    void changeEventType(int);
    void changeTeamName(QString);
    void changeToTeam(int);
    void close();
};

#endif // NEWTABDIALOG_H

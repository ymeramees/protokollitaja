#ifndef SHOTEDIT_H
#define SHOTEDIT_H

#include <QContextMenuEvent>
#include <QLineEdit>
#include <QMenu>
#include "lask.h"

class ShotEdit : public QLineEdit
{
    Q_OBJECT
public:
    ShotEdit();
    ShotEdit(QJsonObject);
    bool ignored() const;
    void init();
    void setShot(Lask shot);
    void setSiusShot(QString siusRow);
    Lask shot();
    QJsonObject toJson() const;
    void updateGui();

public slots:
    void contextMenuEvent(QContextMenuEvent *event);
    void setIgnored(bool newIgnored);
    void setShotFromGui();

signals:
    void shotIgnored();
    void shotUnignored();
    void valueChanged();

private:
    Lask m_shot;
    bool m_ignored = false;
    QString m_originalShotValue;
    QPalette m_standardBackground;
    void updateBackground();
};

#endif // SHOTEDIT_H

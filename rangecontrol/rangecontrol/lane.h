#ifndef LANE_H
#define LANE_H

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTextStream>
#include <QWidget>
#include <QEvent>

#include "siusshotdata.h"
#include "qualificationevents.h"

class Lane : public QWidget
{
    Q_OBJECT
public:
//    enum TargetStatuses {MAIN, IP_MATCH_WAIT_SIGHTERS, IP_MATCH_SIGHTERS, IP_MATCH_WAIT_START, IP_MATCH, IP_MATCH_ENDED, HALT, PRACTISE, MATCH_SIGHTERS, MATCH, LOAD_TIMER};
//    static inline QStringList TargetStatusTexts = {"main", "ip_match_wait_sighters", "ip_match_sighters", "ip_match_wait_start", "ip_match", "ip_match_ended", "halt", "practise", "match_sighters", "match", "load_timer"};
    explicit Lane(QStringList disciplines, QWidget *parent = nullptr);
    explicit Lane(int laneNo, QStringList disciplines, QString ip = "", QWidget *parent = nullptr);
    void clear();
    QString id();
    QString ip();
    QString target();
    QString firstName();
    bool inCompetition();
//    int increaseAndGetCurrentShotIndex();
    QString lastName();
    QString club();
    QualificationEvents::QualificationEvent event();
    QString decimals();
    QString noOfShots();
    bool selected();
    void setCompetitionClass(QString newClass);
    void setSiusCompetitorRow(QString competitorRow);
    void setStartListCompetitorRow(QString competitorRow);
    void setCurrentShotIndex(int newIndex);
    void setDiscipline(QString newDiscipline);
    void setIp(QString newIp);
    void setLastShotLabel(QString newLabel);
    void setSelected(bool selected);
    void setShotsNumber(int newNumber);
    void setStatus(QString newStatus);
    void setTargetAndIp(int targetNo, QString ip);

public slots:
    void updateDecimals(int);
    void updateShots(int);

signals:
    void commandIssued(Lane *lane);

private:
    void changeEvent(QEvent *event);
    void init(QStringList disciplines);
    void updateUi();
//    QHBoxLayout hBox;
    bool m_inCompetition;
    int m_currentShotIndex = 0;
    QCheckBox m_selected;
    QLineEdit m_targetEdit;
    QLineEdit m_idEdit;
    QLineEdit m_firstNameEdit;
    QLineEdit m_lastNameEdit;
    QLineEdit m_clubEdit;
    QComboBox m_diciplineBox;
    QCheckBox m_decimals;
    QSpinBox m_shotsEdit;
    QLabel m_lastShotLabel;
    QLabel m_totalLabel;
    QPushButton m_commandButton;
    QLineEdit m_ipEdit;
    QLabel m_statusLabel;
};

#endif // LANE_H

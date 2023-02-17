#ifndef COMPETITOR_H
#define COMPETITOR_H

#include <QLabel>
#include <QWidget>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QHBoxLayout>
#include <QTextStream>
#include <QMessageBox>
#include <QInputDialog>
#include "shotedit.h"
#include "siusshotdata.h"

extern bool verbose;

class Competitor2022 : public QWidget
{
    Q_OBJECT
public:
    explicit Competitor2022(const int id, const QJsonArray configJson, const bool scoringWithPoints, QWidget *parent = nullptr);
    explicit Competitor2022(const QJsonObject &json, const bool scoringWithPoints, QWidget *parent = nullptr);
    ~Competitor2022();
    bool readSiusShot(SiusShotData shotData);
    int current10Sum() const;  //Current result x10 to avoid floating point arithmetic errors
    int id();
    bool isActive() const;
    std::optional<Lask> shotAt(int index);
    QString name();
    QString lastResult();
    int lastValidShotIndex() const;
    QString resultAt(int index) const;
    QString total();
    void setActive(bool active);
    void setId(int id);
    QJsonObject toJson() const;

signals:
    void modified() const;
    void newShot() const;
    void statusInfo(QString statusInfo);

public slots:
    void handleIgnoredShot();
    void handleUnignoredShot();
    void mouseDoubleClickEvent(QMouseEvent *event);
    QString previousSiusRow();
    void setDisplayName(QString newName);
    bool setPoints(int shotNo, int points);
    void setPreviousSiusRow(QString newSiusRow);
    void setQualificationResult(QString result);
    bool setShot(int shotNo, Lask newShot);
    void setShot(int shotNo, QString siusRow);
    void sum();

private:
    QCheckBox m_active;
    int m_id = 0;
    QLabel m_idLabel;
    QLineEdit m_nameEdit;
    QLineEdit m_resultEdit;
    bool m_scoringWithPoints;
    QSpinBox m_siusOffset;
    QString m_previousSiusRow;
    QVector<QLabel*> m_pointsLabels;
    QVector<QLabel*> m_totalLabels;
    QVector<QVector<ShotEdit*>*> m_series;
    QVector<ShotEdit*> m_shots;
    void createShotEditConnections(ShotEdit* shotEdit);
    void setupCompetitor(QHBoxLayout *layout, bool active, int id, QString name, QString result, const bool scoringWithPoints);
};

#endif // COMPETITOR_H

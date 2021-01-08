#ifndef COMPETITOR_H
#define COMPETITOR_H

#include <QLabel>
#include <QWidget>
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

class Competitor : public QWidget
{
    Q_OBJECT
public:
    explicit Competitor(const int id, const QJsonArray configJson, QWidget *parent = nullptr);
    explicit Competitor(const QJsonObject &json, QWidget *parent = nullptr);
    ~Competitor();
    bool readSiusShot(SiusShotData shotData);
    int current10Sum() const;  //Current result x10 to avoid floating point arithmetic errors
    int id();
    std::optional<Lask> shotAt(int index);
    QString name();
    QString lastResult();
    QString lastSum();
    QJsonObject toJson() const;

signals:
    void newShot() const;
    void statusInfo(QString statusInfo);

public slots:
    void handleIgnoredShot();
    void handleUnignoredShot();
    void mouseDoubleClickEvent(QMouseEvent *event);
    QString previousSiusRow();
    void setPreviousSiusRow(QString newSiusRow);
    bool setShot(int shotNo, Lask newShot);
    void setShot(int shotNo, QString siusRow);
    void sum();

private:
    int m_id;
    QLabel m_idLabel;
    QLineEdit m_nameEdit;
    QString m_previousSiusRow;
    QVector<QLabel*> m_sumLabels;
    QVector<QVector<ShotEdit*>*> m_series;
    QVector<ShotEdit*> m_shots;
    void createShotEditConnections(ShotEdit* shotEdit);
};

#endif // COMPETITOR_H

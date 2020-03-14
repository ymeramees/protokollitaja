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

extern bool verbose;

class Competitor : public QWidget
{
    Q_OBJECT
public:
    explicit Competitor(const QJsonArray configJson, QWidget *parent = nullptr);
    explicit Competitor(const QJsonObject &json, QWidget *parent = nullptr);
    ~Competitor();
    int id();
    QString name();
    QString lastResult();
    QString lastSum();
    void toJson(QJsonObject &json) const;

signals:
    void newShot() const;
    void statusInfo(QString statusInfo);

public slots:
    int current10Sum() const;  //Current result x10 to avoid floating point arithmetic errors
    void mouseDoubleClickEvent(QMouseEvent *event);
    QString previousSiusRow();
    void setPreviousSiusRow(QString newSiusRow);
    void setShot(int shotNo, QString shotValue);
    void sum();

private:
    int m_id;
    QLabel m_idLabel;
    QLineEdit m_nameEdit;
    QString m_previousSiusRow;
    QVector<QLabel*> m_sumLabels;
    QVector<QVector<QLineEdit*>*> m_series;
    QVector<QLineEdit*> m_shots;
};

#endif // COMPETITOR_H

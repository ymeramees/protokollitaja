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

extern bool verbose;

class Competitor : public QWidget
{
    Q_OBJECT
public:
    explicit Competitor(const QJsonArray configJson, QWidget *parent = nullptr);
    explicit Competitor(const QJsonObject &json, QWidget *parent = nullptr);
    ~Competitor();
    void toJson(QJsonObject &json) const;

signals:
    void newShot() const;

public slots:
    int current10Sum() const;  //Current result x10 to avoid floating point arithmetic errors

private:
    QLineEdit nameEdit;
    QVector<QLabel*> sumLabels;
    QVector<QVector<QLineEdit*>*> series;
    QVector<QLineEdit*> shots;

private slots:
    void sum();
};

#endif // COMPETITOR_H

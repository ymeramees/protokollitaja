#ifndef COMPETITOR_H
#define COMPETITOR_H

#include <QLabel>
#include <QWidget>
#include <QLineEdit>
#include <QJsonArray>
#include <QHBoxLayout>
#include <QTextStream>

extern bool verbose;

class Competitor : public QWidget
{
    Q_OBJECT
public:
    explicit Competitor(QJsonArray configJson, QWidget *parent = nullptr);
    ~Competitor();

signals:
    void newShot();

public slots:
    int current10Sum();  //Current result x10 to avoid floating point arithmetic errors

private:
    QLineEdit *nameEdit = nullptr;
    QVector<QLabel*> sumLabels;
    QVector<QVector<QLineEdit*>*> series;
    QVector<QLineEdit*> shots;

private slots:
    void sum();
};

#endif // COMPETITOR_H

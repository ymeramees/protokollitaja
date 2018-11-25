#ifndef PROTOFINAAL_H
#define PROTOFINAAL_H

#include <QJsonDocument>
#include <QMainWindow>
#include <QMessageBox>
#include <QTextStream>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QFile>

#include "team.h"

extern bool verbose;

class Protofinaal : public QMainWindow
{
    Q_OBJECT

public:
    Protofinaal(QWidget *parent = nullptr);
    ~Protofinaal();

private:
    void createLayout(QJsonObject &jsonObj);
    void initialize();
};

#endif // PROTOFINAAL_H

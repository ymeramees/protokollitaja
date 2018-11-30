#ifndef PROTOFINAAL_H
#define PROTOFINAAL_H

#include <QCoreApplication>
#include <QJsonDocument>
#include <QMainWindow>
#include <QMessageBox>
#include <QTextStream>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QStatusBar>
#include <QLineEdit>
#include <QMenuBar>
#include <QLabel>
#include <QTimer>
#include <QFile>

#include "team.h"
#include "initialdialog.h"

extern bool verbose;

class Protofinaal : public QMainWindow
{
    Q_OBJECT

public:
    Protofinaal(QWidget *parent = nullptr);
    ~Protofinaal();
    void open();
    void save();
    void toJson(QJsonObject &json) const;

private slots:
    void initialize();
    void loadFile(QString fileName);
    void updateInitialDialog();

private:
    InitialDialog *initialDialog = nullptr;
    QString competitionName;
    QString currentFile;
    QString eventName;
    QString timePlace;
    QVBoxLayout *vBox = nullptr;
    QVector<Team*> teams;
    void clear();
    void createLayout(QJsonObject &jsonObj);
    void createMenus();
    QJsonObject readFinalsFile(QString fileName, bool showErrors = true);
    void readSettings();
    void writeFinalsFile(QString fileName);
    void writeSettings();
};

#endif // PROTOFINAAL_H

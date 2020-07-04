#ifndef PROTOFINAAL_H
#define PROTOFINAAL_H

#include <QCoreApplication>
#include <QDesktopWidget>
#include <QJsonDocument>
#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>
#include <QTextStream>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QCloseEvent>
#include <QStatusBar>
#include <QLineEdit>
#include <QMenuBar>
#include <QLabel>
#include <QTimer>
#include <QFile>

#include "team.h"
#include "initialdialog.h"
#include "spectatorwindow.h"
#include "lask.h"
#include "siusdataconnections.h"

extern bool verbose;

class Protofinaal : public QMainWindow
{
    Q_OBJECT

public:
    Protofinaal(QWidget *parent = nullptr);
    ~Protofinaal();
    void open();
    void save();
    void showSpecatorWindowOnSecondScreen();
    QJsonObject toJson() const;

public slots:
    void connectToSiusData();
    void readSiusInfo(QStringList lines, int socketIndex);
    void statusBarInfoChanged(QString newStatusInfo);
    void sumAllTeams();

private slots:
    void connectionToSiusLost(int connectionIndex);
    void initialize();
    void loadFile(QString fileName);
    void updateInitialDialog();
    void updateSpectatorWindow();

private:
    bool competitionStarted = false;
    InitialDialog *initialDialog = nullptr;
    SiusDataConnections *siusDataConnections = nullptr;
    SpectatorWindow spectatorWindow;
    QFile *logFile = nullptr;
    QFile *siusLog = nullptr;
    QString competitionName;
    QString currentFile;
    QString eventName;
    QString timePlace;
    QTextStream logOut;
    QVBoxLayout *vBox = nullptr;
    QVector<Team*> teams;
    void clear();
    void closeEvent(QCloseEvent *event);
    void createLayout(QJsonObject &jsonObj);
    void createMenus();
    QJsonObject readFinalsFile(QString fileName, bool showErrors = true);
    void readSettings();
    void sortTeams();
    void writeFinalsFile(QString fileName);
    void writeSettings();
};

#endif // PROTOFINAAL_H

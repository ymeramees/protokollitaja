#ifndef PROTOFINAAL_H
#define PROTOFINAAL_H

#include <QCoreApplication>
#include <QJsonDocument>
#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>
#include <QTextStream>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QCloseEvent>
#include <QScrollArea>
#include <QStatusBar>
#include <QLineEdit>
#include <QMenuBar>
#include <QLabel>
#include <QTimer>
#include <QFile>

#include "team2022.h"
#include "teamstable2022.h"
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
    void statusBarInfoChanged(QString newStatusInfo);
    void updateSpectatorWindow();

private slots:
    void connectionToSiusLost(int connectionIndex);
    void importSiusStartList();
    void initialize();
    void loadFile(QString fileName);
    void updateInitialDialog();

private:
    bool competitionStarted = false;
    bool m_modifiedAfterSave = false;
    bool m_scoringWithPoints = false;
    CommonSettings m_settings;
    InitialDialog *m_initialDialog = nullptr;
    SiusDataConnections *siusDataConnections = nullptr;
    SpectatorWindow m_spectatorWindow;
    QFile *logFile = nullptr;
    QFile *siusLog = nullptr;
    QString competitionName;
    QString currentFile;
    QString eventName;
    QString eventType;
    QString timePlace;
    QTextStream logOut;
    void clear();
    void closeEvent(QCloseEvent *event);
    void createMenus();
    QJsonObject readFinalsFile(QString fileName, bool showErrors = true);
    void readSettings();
    QVBoxLayout vBox;
    QVector<TeamsTable2022*> m_teamsTables;
    void writeFinalsFile(QString fileName);
    void writeSettings();
};

#endif // PROTOFINAAL_H

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
#include <QTranslator>
#include <QStatusBar>
#include <QLineEdit>
#include <QMenuBar>
#include <QLabel>
#include <QTimer>
#include <QFile>

#include "team.h"
#include "version.h"
#include "teamstable.h"
#include "programname.h"
#include "initialdialog.h"
#include "spectatorwindow.h"
#include "lask.h"
#include "siusdataconnections.h"
#include "finalsformats.h"

extern bool verbose;

class Protofinaal : public QMainWindow
{
    Q_OBJECT

public:
    Protofinaal(QString fileName = "", QWidget *parent = nullptr);
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
    bool m_competitionStarted = false;
    bool m_modifiedAfterSave = false;
    bool m_scoringWithPoints = false;
    CommonSettings m_settings;
    InitialDialog *m_initialDialog = nullptr;
    SiusDataConnections *m_siusDataConnections = nullptr;
    SpectatorWindow m_spectatorWindow;
    QFile *m_logFile = nullptr;
    QFile *m_siusLog = nullptr;
    QString m_competitionName;
    QString m_currentFile;
    QString m_eventName;
    QString m_eventType;
    QString m_timePlace;
    QTextStream m_logOut;
    QTranslator m_translator;
    void changeLanguage(bool);
    void clear();
    void closeEvent(QCloseEvent *event);
    void createMenus();
    QJsonObject readFinalsFile(QString fileName, bool showErrors = true);
    void readSettings();
    QVBoxLayout m_vBox;
    QVector<TeamsTable*> m_teamsTables;
    void createLayoutFromConf(QJsonObject conf);
    void setupTranslator();
    void writeFinalsFile(QString fileName);
    void writeSettings();
};

#endif // PROTOFINAAL_H

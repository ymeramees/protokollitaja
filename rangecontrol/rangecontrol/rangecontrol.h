#ifndef PROTORANGECONTROL_H
#define PROTORANGECONTROL_H

#include <QAction>
#include <QFile>
#include <QApplication>
#include <QFileDialog>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QTcpSocket>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QTranslator>

#include "connectionserver.h"
#include "dataconnection.h"
#include "header.h"
#include "lane.h"
#include "lask.h"
#include "programname.h"
#include "version.h"
#include "qualificationevents.h"

class RangeControl : public QMainWindow
{
    Q_OBJECT

public:
    RangeControl(QString language = "", QWidget *parent = nullptr);
    ~RangeControl();

private:
    Lane* addLane(int targetNo, QString ip);
    void addShot(SiusShotData shotData);
    QStringList convertToSiusRows(SiusShotData shotData);
    void clearLanes();
    void createMenus();
    std::optional<Lane*> findLane(int laneNo);
    std::optional<Lane*> findLane(QString laneNo);
    void importStartList();
    void initialize();
    void loadStartList(QStringList startList);
    void publishAllShots(DataConnection *connection);
    void publishSelectedCompetitorShots();
    void publishShot(SiusShotData shotData);
    QJsonDocument readSettings();
    void saveSettings();
    void sendAck(Lane *lane);
    void sendInit(Lane *lane);
    void sendMessage(Lane *lane, QString message);
    void sendMessageToAllSelected(QString message);
    void setCheckedAll();
    void setNumberOfShots();
    void setTargetTypes();
    void showMessage(QString msg);
    void unsetCheckedAll();
    int m_inbandPort = 5451;
    int m_serverPort = 4000;
    ConnectionServer m_server;
    QMap<int, Lane*> m_lanes;
    QVector<SiusShotData> m_shots;
    QTcpSocket *tcpSocket = nullptr;
    QVBoxLayout *vBox = nullptr;
    QString m_language;
    QString m_message;
    QTextStream m_incomingLog;
    QTextStream *m_shotsLog = nullptr;
    QTranslator m_translator;

private slots:
    void allShotsDataReceived(int target, QString shotsData);
    void changeLanguage();
    void continueSendingMessage();
    void sendInitToAllSelected();
    void setupLanes(QJsonDocument fileJson);
    void updateStatus(int target, QString newStatus);
    void newShot(int target, SiusShotData shotData);
    void newTargetIp(int target, QString ip);
};
#endif // PROTORANGECONTROL_H

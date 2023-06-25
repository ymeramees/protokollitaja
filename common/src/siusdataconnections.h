#ifndef SIUSDATACONNECTIONS_H
#define SIUSDATACONNECTIONS_H

#include <QDialog>

#include "siusdataconnection.h"

namespace Ui {
class SiusDataConnections;
}

class SiusDataConnections : public QDialog
{
    Q_OBJECT

public:
    explicit SiusDataConnections(
            QFile *siusLog,
            QTextStream *log,
            CommonSettings *settings,
            QWidget *parent = 0
            );
    ~SiusDataConnections();
    void sendDataToRangeControl(QString data);

//public slots:
//    void closeConnections();

private:
    Ui::SiusDataConnections *ui;
    CommonSettings *m_settings = nullptr;
    QFile *siusLog = nullptr;
    QList<SiusDataConnection*> sockets;
    QTextStream *log = nullptr;
    QVBoxLayout *vBox = nullptr;

private slots:
    void connectToSiusData();
//    void reConnect();

signals:
    void disconnectedFromSius(int socketIndex);
    void errorInfo(QString errorInfo);
//    void linesRead(QStringList lines, int socketIndex);
    void shotRead(SiusShotData shotData);
    void statusInfo(QString statusInfo);
};

#endif // SIUSDATACONNECTIONS_H

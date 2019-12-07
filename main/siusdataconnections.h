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
    explicit SiusDataConnections(QFile *siusLog, QTextStream *log, QWidget *parent = 0);
    ~SiusDataConnections();

//public slots:
//    void closeConnections();

private:
    Ui::SiusDataConnections *ui;
    QFile *siusLog = nullptr;
    QList<SiusDataConnection*> sockets;
    QTextStream *log = nullptr;
    QVBoxLayout *vBox = nullptr;

private slots:
    void connectToSiusData();
//    void reConnect();

signals:
    void disconnectedFromSius(int socketIndex);
    void linesRead(QStringList lines, int socketIndex);
    void statusInfo(QString statusInfo);
};

#endif // SIUSDATACONNECTIONS_H

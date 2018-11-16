#ifndef SIUSDATACONNECTIONS_H
#define SIUSDATACONNECTIONS_H

#include <QDialog>

namespace Ui {
class SiusDataConnections;
}

class SiusDataConnections : public QDialog
{
    Q_OBJECT

public:
    explicit SiusDataConnections(QWidget *parent = 0);
    ~SiusDataConnections();

private:
    Ui::SiusDataConnections *ui;

private slots:
    void connectToSiusData();
    void reConnect();

signals:
    void linesRead(QStringList lines, int socketIndex);
};

#endif // SIUSDATACONNECTIONS_H

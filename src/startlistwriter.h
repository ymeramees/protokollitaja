#ifndef STARTLISTWRITER_H
#define STARTLISTWRITER_H

#include <QFile>
#include <QVector>
#include <QFileDialog>
#include <QStringList>
#include <QTextStream>

class StartListWriter : public QObject
{
    Q_OBJECT
public:
    explicit StartListWriter(QVector<QStringList> competitorsList, QString filePath, QObject *parent = nullptr);

signals:

public slots:

private:
    QString filePath;
    QVector<QStringList> competitorsList;
private slots:
    void writeStartListFile();
};

#endif // STARTLISTWRITER_H

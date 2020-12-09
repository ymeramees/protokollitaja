#ifndef DATAUPLOADER_H
#define DATAUPLOADER_H

#include <QObject>
#include <QtNetwork>

class DataUploader : public QObject
{
    Q_OBJECT
public:
    explicit DataUploader(bool verbose, QObject *parent = nullptr);
    ~DataUploader();
    void uploadResults(const QUrl url, const QString headerData, const QString webCompetitionId, const QJsonDocument data);

signals:
    void uploadFinished(bool success, QString reply, QString errorString = "");

private slots:
    void restClientFinished(QNetworkReply *reply);

private:
    bool m_verbose = false;
    QNetworkAccessManager *m_restClient = nullptr;

};

#endif // DATAUPLOADER_H

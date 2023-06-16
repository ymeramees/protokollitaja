#include "datauploader.h"

DataUploader::DataUploader(bool verbose, QObject *parent) : QObject(parent)
{
    m_verbose = verbose;
}

DataUploader::~DataUploader()
{
    if(m_restClient != nullptr)
        m_restClient->deleteLater();
}

void DataUploader::restClientFinished(QNetworkReply *reply)
{
    QString answer = reply->readAll();
    if(reply->error()){
        QTextStream(stdout) << "Error with upload: " << reply->errorString() << " " << answer << Qt::endl;

        emit uploadFinished(false, answer, reply->errorString());
    }else{
        QTextStream(stdout) << "Reply to upload: " << answer << Qt::endl;

        emit uploadFinished(true, answer);
    }
}

void DataUploader::uploadResults(const QUrl url, const QString headerData, const QString webCompetitionId, const QJsonDocument data)
{
    QNetworkRequest request;
    request.setUrl(url);

    QTextStream(stdout) << "Url: " << request.url().toString() << Qt::endl;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", headerData.toLocal8Bit());

#ifdef QT_DEBUG
    QSslConfiguration sslConf = request.sslConfiguration();
    sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(sslConf);
#endif

    if(m_restClient == nullptr)
        m_restClient = new QNetworkAccessManager(this);

    connect(m_restClient, &QNetworkAccessManager::finished, this, &DataUploader::restClientFinished);

    if(webCompetitionId.isEmpty()) {
        m_restClient->post(request, data.toJson());
    } else {
        if(m_verbose)
            QTextStream(stdout) << "PUT request, webCompetitionId: " << webCompetitionId << Qt::endl;
        m_restClient->put(request, data.toJson());
    }
}

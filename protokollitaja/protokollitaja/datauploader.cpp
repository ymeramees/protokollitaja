#include "datauploader.h"

DataUploader::DataUploader(bool verbose, QTextStream *log, QObject *parent) : QObject(parent)
{
    m_verbose = verbose;
    m_log = log;
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

        if (reply->errorString().contains("certificate", Qt::CaseInsensitive)) {
            const auto cert = reply->sslConfiguration().peerCertificate();
            *m_log << QTime::currentTime().toString("hh:mm:ss") << "#ERROR: SSL Certificate Error: Issued to " << cert.subjectInfo(QSslCertificate::CommonName).join(", ")
                       << ", issued by " << cert.issuerInfo(QSslCertificate::CommonName).join(", ")
                       << ", valid from " << cert.effectiveDate().toString(Qt::ISODate)
                       << " to " << cert.expiryDate().toString(Qt::ISODate) << Qt::endl;
            *m_log << " #SAN: " << cert.subjectAlternativeNames().values().join("; ") << Qt::endl;
            *m_log << " #URL host: " << reply->url().host() << Qt::endl;
        }

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

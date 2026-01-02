#include "filedownloader.h"

FileDownloader::FileDownloader(QUrl imageUrl, QObject *parent) :
    QObject(parent)
{
    connect(&m_WebCtrl, SIGNAL(finished(QNetworkReply*)),
                SLOT(fileDownloaded(QNetworkReply*)));
    connect(
        &m_WebCtrl,
        SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError>)),
        SLOT(printErrors(QNetworkReply*, const QList<QSslError>&))
    );

    QNetworkRequest request(imageUrl);
    m_WebCtrl.get(request);
}

FileDownloader::~FileDownloader()
{

}

void FileDownloader::fileDownloaded(QNetworkReply* pReply)
{
    m_DownloadedData = pReply->readAll();
    //emit a signal
    pReply->deleteLater();
    emit downloaded();
}

void FileDownloader::printErrors(QNetworkReply *pReply, const QList<QSslError> &errors)
{
    for (auto error: errors) {
        QTextStream(stdout) << "#ERROR: " << error.errorString() << Qt::endl;
    }

    QVariant statusCode = pReply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    if ( !statusCode.isValid() ) {
        QTextStream(stdout) << "Status code: unknown" << Qt::endl;
        return;
    }

    int status = statusCode.toInt();

    QTextStream(stdout) << "Status code: " << status << Qt::endl;

    if ( status != 200 )
    {
        QString reason = pReply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        QTextStream(stdout) << "Reason: " << reason << Qt::endl;
    }
}

QByteArray FileDownloader::downloadedData() const
{
    return m_DownloadedData;
}

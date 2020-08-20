#include "updatechecker.h"

UpdateChecker::UpdateChecker(QString currentVersion, QObject *parent) : QObject(parent)
{
    m_currentVersion = currentVersion;
}

UpdateChecker::~UpdateChecker()
{
    if (m_restClient != nullptr)
        m_restClient->deleteLater();
}

void UpdateChecker::checkVersionFromWeb(QString url)
{
    QUrl webPage(url);
    if(m_downloader != nullptr)    //Kui on korra juba tõmmatud, siis tuleb vana kustutada
        m_downloader->deleteLater();

    m_downloader = new FileDownloader(webPage, this);

    connect(m_downloader, &FileDownloader::downloaded, this, &UpdateChecker::readWebVersionInfo);
}

void UpdateChecker::getLatestVersionInfo(QString user, QString repo)
{
    QUrl url("https://api.github.com/repos/" + user +"/" + repo + "/tags");

    QNetworkRequest request;
    request.setUrl(url);

    QTextStream(stdout) << "Url: " << request.url().toString() << endl;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if(m_restClient == nullptr)
        m_restClient = new QNetworkAccessManager(this);

    connect(m_restClient, &QNetworkAccessManager::finished, this, &UpdateChecker::restClientFinished);

    m_restClient->get(request);
}

bool UpdateChecker::isCurrentVersionOld(QString currentVersion, QString newVersion)
{
    QStringList versionNo = currentVersion.split(".");
    QStringList newVersionNo = newVersion.split(".");

    // Compare versions starting from major numbers
    if(versionNo[0].toInt() < newVersionNo[0].toInt()
        || (versionNo[1].toInt() < newVersionNo[1].toInt() && versionNo[0].toInt() == newVersionNo[0].toInt())
        || (versionNo[2].toInt() < newVersionNo[2].toInt() && versionNo[1].toInt() == newVersionNo[1].toInt() && versionNo[0].toInt() == newVersionNo[0].toInt())){
        return true;
    }else{  // If none of the previous, then current version is newest
        return false;
    }
}

void UpdateChecker::readWebVersionInfo()
{
    static bool allAddressesChecked = false;
    QString info(m_downloader->downloadedData());
    if(!info.startsWith("<!--proto")){
        if(allAddressesChecked){
            emit versionInfoResponse(false, "#ERROR: Unable to find version info! Program needs to be updated manually!");
        }else{
            checkVersionFromWeb("https://ymeramees.no-ip.org/protokollitaja/inf20150118");  // Fallback address
            allAddressesChecked = true;
        }
    } else {
        QStringList versionsList = info.left(info.indexOf("\n")).split(";");    // Version no, together with list of files to be updated
        bool updateExists = isCurrentVersionOld(m_currentVersion, versionsList[1]);
        emit versionInfoResponse(updateExists, versionsList[1]);
    }
}

void UpdateChecker::restClientFinished(QNetworkReply *reply)
{
    QByteArray answer = reply->readAll();
    if(reply->error()){
        checkVersionFromWeb("https://webzone.ee/protokollitaja/inf20150118");   // Check version info from a webpage
    } else {
        QJsonDocument json = QJsonDocument::fromJson(answer);
        bool updateExists = isCurrentVersionOld(m_currentVersion, json[0]["name"].toString());
        emit versionInfoResponse(updateExists, json[0]["name"].toString());
    }
}

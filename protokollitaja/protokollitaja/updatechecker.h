#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTextStream>
#include <QJsonDocument>

#include "filedownloader.h"

class UpdateChecker : public QObject
{
    Q_OBJECT
public:
    explicit UpdateChecker(QString currentVersion, QTextStream *log, QObject *parent = nullptr);
    ~UpdateChecker();
    void getLatestVersionInfo(QString user, QString repo);

signals:
    void versionInfoResponse(bool, QString);

private slots:
    void readWebVersionInfo();
    void restClientFinished(QNetworkReply *reply);

private:
    QString m_currentVersion;
    QTextStream *m_log = nullptr;
    FileDownloader *m_downloader = nullptr;    // For downloading version info file
    QNetworkAccessManager *m_restClient = nullptr;
    void checkVersionFromWeb(QString url);
    bool isCurrentVersionOld(QString currentVersion, QString newVersion);

    friend class UpdateCheckerTest;
};

#endif // UPDATECHECKER_H

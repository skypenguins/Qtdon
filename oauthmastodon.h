#ifndef OAUTHMASTODON_H
#define OAUTHMASTODON_H

#include "oauth2.h"
#include <QObject>
#include <QTimer>
#include <QEventLoop>
#include <QMessageBox>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QJsonObject>
#include <QJsonDocument>

#include <QDebug>

/* 既にaccess_tokenがある場合、oauth_verifierでhostとaccess_tokenの組み合わせが有効か確認する処理を入れる */

class OAuthMastodon : public oauth2
{
    Q_OBJECT
public:
    explicit OAuthMastodon(QObject *parent = 0);
    explicit OAuthMastodon(const QString& mastodonHost, const QByteArray& accessToken, QObject *parent = 0);
    explicit OAuthMastodon(QNetworkAccessManager* networkManager, QObject *parent = 0);
    explicit OAuthMastodon(const QString& clientId, const QString& clientSecret, QObject *parent = 0); /* use once */
    void setNetworkAccessManager(QNetworkAccessManager* networkManager);
    void setAccessToken(const QByteArray& accessToken);
    QNetworkAccessManager* networkAccessManager() const;
    QByteArray accessToken() const;
    void requestAuthorization(const QString& mastodonHost);
    QString mastodonHost() const;

signals:
    void authorizationUrlGenerated();
    void authorizeAuthCodeFinished();

public slots:
    void requestAccessToken(const QString& authCode);
    void popUpError(QNetworkReply* errorReply);

private:
    QNetworkAccessManager* m_networkManager;
    QByteArray m_accessToken;
    QString m_mastodonHost;
};

#endif // OAUTHMASTODON_H

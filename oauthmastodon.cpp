#include "oauthmastodon.h"

#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>

OAuthMastodon::OAuthMastodon(QObject *parent)
    : OAuth2(parent)
{
}

OAuthMastodon::OAuthMastodon(const QString &mastodonHost,
                             const QByteArray &accessToken,
                             QObject *parent)
    : OAuth2(parent)
    , m_mastodonHost(mastodonHost)
    , m_accessToken(accessToken)
{
}

OAuthMastodon::OAuthMastodon(const QString &clientId,
                             const QString &clientSecret,
                             QObject *parent)
    : OAuth2(clientId, clientSecret, parent)
{
}

// ---------------------------------------------------------------------------
// Accessors
// ---------------------------------------------------------------------------

void OAuthMastodon::setNetworkAccessManager(QNetworkAccessManager *manager)
{
    m_networkManager = manager;
}

QNetworkAccessManager *OAuthMastodon::networkAccessManager() const
{
    return m_networkManager;
}

void OAuthMastodon::setAccessToken(const QByteArray &accessToken)
{
    m_accessToken = accessToken;
}

QByteArray OAuthMastodon::accessToken() const
{
    return m_accessToken;
}

QString OAuthMastodon::mastodonHost() const
{
    return m_mastodonHost;
}

// ---------------------------------------------------------------------------
// Authorization flow
// ---------------------------------------------------------------------------

void OAuthMastodon::requestAuthorization(const QString &mastodonHost)
{
    m_mastodonHost = mastodonHost;

    QUrl url(QStringLiteral("https://") + m_mastodonHost
             + QStringLiteral("/oauth/authorize"));

    url.setQuery(generateAuthQuery(
        QStringLiteral("urn:ietf:wg:oauth:2.0:oob"),
        QStringLiteral("code"),
        QStringLiteral("read write follow")));

    QDesktopServices::openUrl(url);
    emit authorizationUrlOpened();
}

void OAuthMastodon::requestAccessToken(const QString &authCode)
{
    if (!m_networkManager) {
        emit errorOccurred(tr("Network manager is not initialised."));
        return;
    }

    QUrl url(QStringLiteral("https://") + m_mastodonHost
             + QStringLiteral("/oauth/token"));

    const QByteArray postData = generateTokenRequestData(
        authCode, QStringLiteral("urn:ietf:wg:oauth:2.0:oob"));

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/x-www-form-urlencoded"));

    QNetworkReply *reply = m_networkManager->post(request, postData);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred(reply->errorString());
            return;
        }

        const auto response = reply->readAll();
        const auto json = QJsonDocument::fromJson(response);

        if (!json.isObject()
            || !json.object().contains(QStringLiteral("access_token"))) {
            emit errorOccurred(tr("Invalid token response from server."));
            return;
        }

        m_accessToken = json.object()
                            .value(QStringLiteral("access_token"))
                            .toString()
                            .toUtf8();

        qDebug() << "Authorization succeeded.";
        emit accessTokenReceived();
    });
}

#include "oauthmastodon.h"

#include <QDesktopServices>
#include <QUrl>

// ===========================================================================
// OAuthMastodon
// ===========================================================================

OAuthMastodon::OAuthMastodon(QObject *parent)
    : QObject(parent)
    , m_oauth2(new QOAuth2AuthorizationCodeFlow(this))
    , m_replyHandler(new QOAuthHttpServerReplyHandler(0, this))
{
    m_oauth2->setReplyHandler(m_replyHandler);

    connect(m_oauth2, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
            this, [this](const QUrl &url) {
                QDesktopServices::openUrl(url);
                emit authorizationUrlOpened();
            });

    connect(m_oauth2, &QOAuth2AuthorizationCodeFlow::granted,
            this, [this]() {
                m_accessToken = m_oauth2->token().toUtf8();
                qDebug() << "Authorization succeeded.";
                emit accessTokenReceived();
            });

    connect(m_oauth2, &QAbstractOAuth2::serverReportedErrorOccurred,
            this, [this](const QString &error,
                         const QString &errorDescription,
                         const QUrl & /*uri*/) {
                const auto msg = errorDescription.isEmpty()
                                     ? error
                                     : error + QStringLiteral(": ")
                                           + errorDescription;
                emit errorOccurred(msg);
            });
}

OAuthMastodon::OAuthMastodon(const QString &mastodonHost,
                             const QByteArray &accessToken,
                             QObject *parent)
    : OAuthMastodon(parent)
{
    m_mastodonHost = mastodonHost;
    m_accessToken  = accessToken;
    m_oauth2->setToken(QString::fromUtf8(accessToken));
}

// ---------------------------------------------------------------------------
// Accessors
// ---------------------------------------------------------------------------

void OAuthMastodon::setNetworkAccessManager(QNetworkAccessManager *manager)
{
    m_networkManager = manager;
    m_oauth2->setNetworkAccessManager(manager);
}

QNetworkAccessManager *OAuthMastodon::networkAccessManager() const
{
    return m_networkManager;
}

void OAuthMastodon::setClientCredentials(const QString &clientKey,
                                         const QString &clientSecret)
{
    m_oauth2->setClientIdentifier(clientKey);
    m_oauth2->setClientIdentifierSharedKey(clientSecret);
}

void OAuthMastodon::setAccessToken(const QByteArray &accessToken)
{
    m_accessToken = accessToken;
    m_oauth2->setToken(QString::fromUtf8(accessToken));
}

QByteArray OAuthMastodon::accessToken() const
{
    return m_accessToken;
}

QString OAuthMastodon::mastodonHost() const
{
    return m_mastodonHost;
}

void OAuthMastodon::setMastodonHost(const QString &host)
{
    m_mastodonHost = host;
}

// ---------------------------------------------------------------------------
// Authorization flow
// ---------------------------------------------------------------------------

void OAuthMastodon::requestAuthorization(const QString &mastodonHost)
{
    m_mastodonHost = mastodonHost;

    const QString baseUrl =
        QStringLiteral("https://") + mastodonHost;

    // Build the redirect URI from the local server's port.
    const auto redirectUri = QStringLiteral("http://127.0.0.1:%1/")
                                 .arg(m_replyHandler->port());

    m_oauth2->setAuthorizationUrl(
        QUrl(baseUrl + QStringLiteral("/oauth/authorize")));
    m_oauth2->setTokenUrl(
        QUrl(baseUrl + QStringLiteral("/oauth/token")));
    m_oauth2->setRequestedScopeTokens(
        {QByteArrayLiteral("read"), QByteArrayLiteral("write"),
         QByteArrayLiteral("follow")});

    m_oauth2->grant();
}

QByteArray OAuthMastodon::generateBearerHeader(const QByteArray &accessToken)
{
    return QByteArray("Bearer ") + accessToken;
}

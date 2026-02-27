#ifndef OAUTHMASTODON_H
#define OAUTHMASTODON_H

#include <QNetworkAccessManager>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QObject>

/**
 * @brief Mastodon-specific OAuth2 flow using QtNetworkAuth.
 *
 * Uses QOAuthHttpServerReplyHandler to run a temporary local HTTP server
 * that captures the authorization redirect automatically — no manual
 * code entry required.
 *
 * Usage:
 *   1. Call requestAuthorization(host) — browser opens, local server starts.
 *   2. User authorises in the browser.
 *   3. Browser redirects to localhost; QtNetworkAuth exchanges the code
 *      for an access token and emits accessTokenReceived().
 */
class OAuthMastodon : public QObject
{
    Q_OBJECT

public:
    explicit OAuthMastodon(QObject *parent = nullptr);
    explicit OAuthMastodon(const QString &mastodonHost,
                           const QByteArray &accessToken,
                           QObject *parent = nullptr);

    void setNetworkAccessManager(QNetworkAccessManager *manager);
    [[nodiscard]] QNetworkAccessManager *networkAccessManager() const;

    /** Set the client key and secret before starting the auth flow. */
    void setClientCredentials(const QString &clientKey,
                              const QString &clientSecret);

    void setAccessToken(const QByteArray &accessToken);
    [[nodiscard]] QByteArray accessToken() const;

    void setMastodonHost(const QString &host);
    [[nodiscard]] QString mastodonHost() const;

    /** Open the authorization URL in the default browser. */
    void requestAuthorization(const QString &mastodonHost);

    /** Return a "Bearer <token>" header value. */
    [[nodiscard]] static QByteArray generateBearerHeader(
        const QByteArray &accessToken);

signals:
    void authorizationUrlOpened();
    void accessTokenReceived();
    void errorOccurred(const QString &message);

private:
    QOAuth2AuthorizationCodeFlow *m_oauth2 = nullptr;
    QOAuthHttpServerReplyHandler *m_replyHandler = nullptr;
    QNetworkAccessManager *m_networkManager = nullptr;
    QByteArray m_accessToken;
    QString m_mastodonHost;
};

#endif // OAUTHMASTODON_H

#ifndef OAUTHMASTODON_H
#define OAUTHMASTODON_H

#include "oauth2.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

/**
 * @brief Mastodon-specific OAuth2 flow.
 *
 * 1. requestAuthorization()  — opens the browser for the user to authorise.
 * 2. requestAccessToken()    — exchanges the auth-code for a bearer token
 *                              (fully asynchronous, result via signal).
 */
class OAuthMastodon : public OAuth2
{
    Q_OBJECT

public:
    explicit OAuthMastodon(QObject *parent = nullptr);
    explicit OAuthMastodon(const QString &mastodonHost,
                           const QByteArray &accessToken,
                           QObject *parent = nullptr);
    explicit OAuthMastodon(const QString &clientId,
                           const QString &clientSecret,
                           QObject *parent = nullptr);

    void setNetworkAccessManager(QNetworkAccessManager *manager);
    [[nodiscard]] QNetworkAccessManager *networkAccessManager() const;

    void setAccessToken(const QByteArray &accessToken);
    [[nodiscard]] QByteArray accessToken() const;

    [[nodiscard]] QString mastodonHost() const;

    /** Open the authorization URL in the default browser. */
    void requestAuthorization(const QString &mastodonHost);

signals:
    void authorizationUrlOpened();
    void accessTokenReceived();
    void errorOccurred(const QString &message);

public slots:
    /** Exchange the user-provided auth code for an access token (async). */
    void requestAccessToken(const QString &authCode);

private:
    QNetworkAccessManager *m_networkManager = nullptr;
    QByteArray m_accessToken;
    QString m_mastodonHost;
};

#endif // OAUTHMASTODON_H

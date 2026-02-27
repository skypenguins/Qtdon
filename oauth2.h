#ifndef OAUTH2_H
#define OAUTH2_H

#include <QObject>
#include <QUrlQuery>

/**
 * @brief Generic OAuth2 helper — builds authorization queries, token request
 *        payloads, and Bearer headers.
 *
 * Sub-class this to add provider-specific behaviour (see OAuthMastodon).
 */
class OAuth2 : public QObject
{
    Q_OBJECT

public:
    explicit OAuth2(QObject *parent = nullptr);
    explicit OAuth2(const QString &clientId,
                    const QString &clientSecret,
                    QObject *parent = nullptr);

    /** Build the query string for the authorization URL. */
    [[nodiscard]] QUrlQuery generateAuthQuery(
        const QString &redirectUri,
        const QString &responseType,
        const QString &scope) const;

    /** Build the POST body for the token endpoint. */
    [[nodiscard]] QByteArray generateTokenRequestData(
        const QString &authCode,
        const QString &redirectUri,
        const QString &grantType = QStringLiteral("authorization_code")) const;

    /** Return a "Bearer <token>" header value. */
    [[nodiscard]] static QByteArray generateBearerHeader(const QByteArray &accessToken);

private:
    QString m_clientId;
    QString m_clientSecret;
};

#endif // OAUTH2_H

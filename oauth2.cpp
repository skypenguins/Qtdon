#include "oauth2.h"

namespace {
// TODO: Replace with dynamic client registration or external configuration.
constexpr auto CLIENT_ID     = "***";
constexpr auto CLIENT_SECRET = "***";
} // namespace

OAuth2::OAuth2(QObject *parent)
    : QObject(parent)
    , m_clientId(QLatin1String(CLIENT_ID))
    , m_clientSecret(QLatin1String(CLIENT_SECRET))
{
}

OAuth2::OAuth2(const QString &clientId, const QString &clientSecret, QObject *parent)
    : QObject(parent)
    , m_clientId(clientId)
    , m_clientSecret(clientSecret)
{
}

QUrlQuery OAuth2::generateAuthQuery(const QString &redirectUri,
                                    const QString &responseType,
                                    const QString &scope) const
{
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("redirect_uri"),  redirectUri);
    query.addQueryItem(QStringLiteral("response_type"), responseType);
    query.addQueryItem(QStringLiteral("client_id"),     m_clientId);
    query.addQueryItem(QStringLiteral("scope"),         scope);
    return query;
}

QByteArray OAuth2::generateTokenRequestData(const QString &authCode,
                                            const QString &redirectUri,
                                            const QString &grantType) const
{
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("client_id"),     m_clientId);
    query.addQueryItem(QStringLiteral("client_secret"), m_clientSecret);
    query.addQueryItem(QStringLiteral("code"),          authCode);
    query.addQueryItem(QStringLiteral("grant_type"),    grantType);
    query.addQueryItem(QStringLiteral("redirect_uri"),  redirectUri);
    return query.toString().toUtf8();
}

QByteArray OAuth2::generateBearerHeader(const QByteArray &accessToken)
{
    return QByteArray("Bearer ") + accessToken;
}

#include "oauth2.h"

/* Temporary */
#define CLIENT_ID "***"
#define CLIENT_SECRET "***"

oauth2::oauth2(QObject *parent)
    : QObject(parent), m_clientId(CLIENT_ID), m_clientSecret(CLIENT_SECRET)
{

}

oauth2::oauth2(const QString &clientId, const QString &clientSecret, QObject *parent)
    : QObject(parent), m_clientId(clientId), m_clientSecret(clientSecret)
{
}

QUrlQuery oauth2::generateAuthenticationQuery(QString redirectUri, QString responseType, QString scope)
{
    QMap<QString, QString> params;
    params["redirect_uri"]  = redirectUri;
    params["response_type"] = responseType;
    params["client_id"]     = m_clientId;
    params["scope"]         = scope;

    QUrlQuery requestQuery;
    QMapIterator<QString,QString> itr(params);
    while (itr.hasNext()) {
        itr.next();
        requestQuery.addQueryItem(itr.key(), itr.value());
    }

    return requestQuery;
}

QByteArray oauth2::generateAuthorizationPostData(QString authorizationCode, QString redirectUri, QString grantType)
{
    QMap<QString, QString> params;
    params["client_id"]     = m_clientId;
    params["client_secret"] = m_clientSecret;
    params["code"]          = authorizationCode;
    params["grant_type"]    = grantType;
    params["redirect_uri"]  = redirectUri;

    QUrlQuery requestQuery;
    QMapIterator<QString,QString> itr(params);
    while (itr.hasNext()) {
        itr.next();
        requestQuery.addQueryItem(itr.key(), itr.value());
    }

    return requestQuery.toString().toUtf8();
}

QByteArray oauth2::generateAuthorizationHeader(const QByteArray& accessToken)
{
    return QByteArray("Bearer " + accessToken);
}

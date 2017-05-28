#ifndef OAUTH_H
#define OAUTH_H

#include <QObject>
#include <QMap>
#include <QUrl>
#include <QUrlQuery>

/* This class contains minimal functions for Mastodon API.
 * Authorization Code is required.
 */

class oauth2 : public QObject
{
    Q_OBJECT
public:
    explicit oauth2(QObject *parent = 0);
    explicit oauth2(const QString& clientId, const QString& clientSecret, QObject *parent = 0); /* If end user uses an existing client_id and client_secret. (Advanced Option) */
    QUrlQuery generateAuthenticationQuery(QString redirectUri, QString responseType, QString scope);
    QByteArray generateAuthorizationPostData(QString authorizationCode, QString redirectUri, QString grantType = "authorization_code");
    QByteArray generateAuthorizationHeader(const QByteArray& accessToken);

signals:

private slots:

private:
    QString m_clientId;
    QString m_clientSecret;
};

#endif // OAUTH_H

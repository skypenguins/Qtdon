#ifndef QMASTODONNETBASE_H
#define QMASTODONNETBASE_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>

#include "oauthmastodon.h"

class QMastodonNetBase : public QObject
{
    Q_OBJECT
public:
    explicit QMastodonNetBase(QObject *parent = 0);
    explicit QMastodonNetBase(OAuthMastodon *oauthMastodon, QObject *parent = 0);

    enum ErrorCode {
        JsonParsingError = 1,
        UnknownError = 2,
        NotModified = 304,
        BadRequest = 400,
        Unauthorized = 401,
        Forbidden = 403,
        NotFound = 404,
        NotAcceptable = 406,
        EnhanceYourCalm = 420,
        InternalServerError = 500,
        BadGateway = 502,
        ServiceUnavailable = 503
    };

    void setOAuthMastodon(OAuthMastodon* oauthMastodon);
    OAuthMastodon* oauthMastodon() const;

    void setAuthenticationEnabled(bool enable);
    bool isAuthenticationEnabled() const;

    QByteArray response();
    QString lastErrorMessage() const;

signals:
    void finished(const QByteArray& response);
    void error(QMastodonNetBase::ErrorCode code, const QString& errorMsg);

protected slots:
    void reply();

protected:
    virtual void parseJsonFinished(const QJsonDocument& jsonDoc) = 0;
    void parseJson(const QByteArray& jsonData);
    void setLastErrorMessage(const QString& errMsg);

private:
    OAuthMastodon* m_oauthMastodon;
    QByteArray m_response;
    QString m_lastErrorMessage;
    bool m_authentication;
};

#endif // QMASTODONNETBASE_H

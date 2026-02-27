#ifndef QMASTODONNETBASE_H
#define QMASTODONNETBASE_H

#include <QObject>
#include <QJsonDocument>

#include "oauthmastodon.h"

/**
 * @brief Abstract base for Mastodon API requests.
 *
 * Handles network reply routing, HTTP-status-based error mapping and
 * JSON parsing dispatch.  Concrete sub-classes implement parseJsonFinished().
 */
class QMastodonNetBase : public QObject
{
    Q_OBJECT

public:
    enum class ErrorCode {
        JsonParsingError    = 1,
        UnknownError        = 2,
        NotModified         = 304,
        BadRequest          = 400,
        Unauthorized        = 401,
        Forbidden           = 403,
        NotFound            = 404,
        NotAcceptable       = 406,
        RateLimited         = 429,
        InternalServerError = 500,
        BadGateway          = 502,
        ServiceUnavailable  = 503
    };
    Q_ENUM(ErrorCode)

    explicit QMastodonNetBase(QObject *parent = nullptr);
    explicit QMastodonNetBase(OAuthMastodon *oauthMastodon,
                              QObject *parent = nullptr);

    void setOAuthMastodon(OAuthMastodon *oauthMastodon);
    [[nodiscard]] OAuthMastodon *oauthMastodon() const;

    void setAuthenticationEnabled(bool enable);
    [[nodiscard]] bool isAuthenticationEnabled() const;

    [[nodiscard]] QByteArray response() const;
    [[nodiscard]] QString lastErrorMessage() const;

signals:
    void finished(const QByteArray &response);
    void error(ErrorCode code, const QString &errorMsg);

protected slots:
    /** Connect QNetworkReply::finished to this slot. */
    void onReplyFinished();

protected:
    /** Called after successful JSON parsing — implement in sub-classes. */
    virtual void parseJsonFinished(const QJsonDocument &jsonDoc) = 0;
    void parseJson(const QByteArray &jsonData);
    void setLastErrorMessage(const QString &errMsg);

private:
    OAuthMastodon *m_oauthMastodon = nullptr;
    QByteArray m_response;
    QString m_lastErrorMessage;
    bool m_authenticationEnabled = true;
};

#endif // QMASTODONNETBASE_H

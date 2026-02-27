#include "qmastodonnetbase.h"

#include <QNetworkReply>
#include <QNetworkRequest>

QMastodonNetBase::QMastodonNetBase(QObject *parent)
    : QObject(parent)
{
}

QMastodonNetBase::QMastodonNetBase(OAuthMastodon *oauthMastodon, QObject *parent)
    : QObject(parent)
    , m_oauthMastodon(oauthMastodon)
{
}

// ---------------------------------------------------------------------------
// Accessors
// ---------------------------------------------------------------------------

void QMastodonNetBase::setOAuthMastodon(OAuthMastodon *oauthMastodon)
{
    m_oauthMastodon = oauthMastodon;
}

OAuthMastodon *QMastodonNetBase::oauthMastodon() const
{
    return m_oauthMastodon;
}

void QMastodonNetBase::setAuthenticationEnabled(bool enable)
{
    m_authenticationEnabled = enable;
}

bool QMastodonNetBase::isAuthenticationEnabled() const
{
    return m_authenticationEnabled;
}

QByteArray QMastodonNetBase::response() const
{
    return m_response;
}

QString QMastodonNetBase::lastErrorMessage() const
{
    return m_lastErrorMessage;
}

// ---------------------------------------------------------------------------
// JSON helpers
// ---------------------------------------------------------------------------

void QMastodonNetBase::parseJson(const QByteArray &jsonData)
{
    const auto jsonDoc = QJsonDocument::fromJson(jsonData);
    parseJsonFinished(jsonDoc);
}

// ---------------------------------------------------------------------------
// Reply handling
// ---------------------------------------------------------------------------

void QMastodonNetBase::onReplyFinished()
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply)
        return;

    reply->deleteLater();

    if (reply->error() == QNetworkReply::NoError) {
        m_response = reply->readAll();
        emit finished(m_response);
        parseJson(m_response);
        return;
    }

    m_response = reply->readAll();
    const int httpStatus =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    switch (httpStatus) {
    case static_cast<int>(ErrorCode::NotModified):
    case static_cast<int>(ErrorCode::BadRequest):
    case static_cast<int>(ErrorCode::Unauthorized):
    case static_cast<int>(ErrorCode::Forbidden):
    case static_cast<int>(ErrorCode::NotFound):
    case static_cast<int>(ErrorCode::NotAcceptable):
    case static_cast<int>(ErrorCode::RateLimited):
    case static_cast<int>(ErrorCode::InternalServerError):
    case static_cast<int>(ErrorCode::BadGateway):
    case static_cast<int>(ErrorCode::ServiceUnavailable):
        emit error(static_cast<ErrorCode>(httpStatus), reply->errorString());
        break;
    default:
        emit error(ErrorCode::UnknownError, reply->errorString());
        break;
    }
}

void QMastodonNetBase::setLastErrorMessage(const QString &errMsg)
{
    m_lastErrorMessage = errMsg;
}

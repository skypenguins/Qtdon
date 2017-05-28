#include "qmastodonnetbase.h"

QMastodonNetBase::QMastodonNetBase(QObject *parent)
    : QObject(parent), m_oauthMastodon(0)
{
}

QMastodonNetBase::QMastodonNetBase(OAuthMastodon *oauthMastodon, QObject *parent)
    : QObject(parent), m_oauthMastodon(oauthMastodon), m_authentication(true)
{
}

void QMastodonNetBase::setOAuthMastodon(OAuthMastodon *oauthMastodon)
{
    m_oauthMastodon = oauthMastodon;
}

OAuthMastodon* QMastodonNetBase::oauthMastodon() const
{
    return m_oauthMastodon;
}

QByteArray QMastodonNetBase::response()
{
    return m_response;
}

QString QMastodonNetBase::lastErrorMessage() const
{
    return m_lastErrorMessage;
}

void QMastodonNetBase::setAuthenticationEnabled(bool enable)
{
    m_authentication = enable;
}

bool QMastodonNetBase::isAuthenticationEnabled() const
{
    return m_authentication;
}

void QMastodonNetBase::parseJson(const QByteArray &jsonData)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);

    parseJsonFinished(jsonDoc);
}

void QMastodonNetBase::reply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << "QMastodonNetBase reply()";
    if(reply){
        if(reply->error() == QNetworkReply::NoError){
            m_response = reply->readAll();
            emit finished(m_response);

            QMessageBox resMsgBox;
            resMsgBox.setIcon(QMessageBox::Information);
            resMsgBox.setWindowTitle(tr("Response"));
            resMsgBox.setText(tr("Response Data"));
            resMsgBox.setInformativeText(m_response);
            resMsgBox.exec();

            // parseJson(m_response);
        } else {
            m_response = reply->readAll();

            int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

            switch(httpStatus){
            case NotModified:
            case BadRequest:
            case Unauthorized:
            case Forbidden:
            case NotFound:
            case NotAcceptable:
            case EnhanceYourCalm:
            case InternalServerError:
            case BadGateway:
            case ServiceUnavailable:
                emit error(static_cast<ErrorCode>(httpStatus), m_lastErrorMessage);
                break;
            default:
                emit error(UnknownError, m_lastErrorMessage);
            }
        }
        reply->deleteLater();
    }
}

void QMastodonNetBase::setLastErrorMessage(const QString &errMsg)
{
    m_lastErrorMessage = errMsg;
}

#include "oauthmastodon.h"

OAuthMastodon::OAuthMastodon(QObject *parent)
    : oauth2(parent)
{
}

OAuthMastodon::OAuthMastodon(const QString &mastodonHost, const QByteArray &accessToken, QObject *parent)
    : oauth2(parent), m_networkManager(0), m_mastodonHost(mastodonHost), m_accessToken(accessToken)
{
}

OAuthMastodon::OAuthMastodon(QNetworkAccessManager *networkManager, QObject *parent)
    : oauth2(parent), m_networkManager(networkManager)
{
}

OAuthMastodon::OAuthMastodon(const QString &clientId, const QString &clientSecret, QObject *parent)
    : oauth2(clientId, clientSecret, parent)
{
}

void OAuthMastodon::setNetworkAccessManager(QNetworkAccessManager *networkManager)
{
    m_networkManager = networkManager;
}

void OAuthMastodon::setAccessToken(const QByteArray &accessToken)
{
    m_accessToken = accessToken;
}

QNetworkAccessManager* OAuthMastodon::networkAccessManager() const
{
    return m_networkManager;
}

QByteArray OAuthMastodon::accessToken() const
{
    return m_accessToken;
}

void OAuthMastodon::requestAuthorization(const QString& mastodonHost)
{
    m_mastodonHost = mastodonHost;

    QUrl url("https://" + m_mastodonHost + "/oauth/authorize");

    QUrlQuery oauthQuery = generateAuthenticationQuery("urn:ietf:wg:oauth:2.0:oob", "code", "read write follow");

    url.setQuery(oauthQuery);

    emit authorizationUrlGenerated();

    QDesktopServices::openUrl(url);
}

QString OAuthMastodon::mastodonHost() const
{
    return m_mastodonHost;
}

void OAuthMastodon::requestAccessToken(const QString& authCode)
{
    QUrl url("https://" + m_mastodonHost + "/oauth/token");

    QByteArray postData = generateAuthorizationPostData(authCode, "urn:ietf:wg:oauth:2.0:oob", "authorization_code");

    QEventLoop q;
    QTimer t;
    t.setSingleShot(true);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply* reply = m_networkManager->post(request, postData);

    connect(reply, SIGNAL(finished()), &q, SLOT(quit()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(popUpError(reply)));
    connect(&t, SIGNAL(timeout()), &q, SLOT(quit()));

    t.start(5000);
    q.exec();

    if(t.isActive()){
        QByteArray response = reply->readAll();
        reply->deleteLater();

        auto resultJson = QJsonDocument::fromJson(response);
        m_accessToken = resultJson.object().value("access_token").toString().toUtf8();

        emit authorizeAuthCodeFinished();

        qDebug() << "Authorization is succeeded.";
    } else {
        QMessageBox errMegBox;
        errMegBox.setIcon(QMessageBox::Critical);
        errMegBox.setWindowTitle(tr("Timeout"));
        errMegBox.setText(tr("The authorization is timeout."));
        errMegBox.setInformativeText(tr("Please try again later."));
        errMegBox.exec();
    }
}

void OAuthMastodon::popUpError(QNetworkReply* errorReply)
{
    QMessageBox errMegBox2;
    errMegBox2.setIcon(QMessageBox::Critical);
    errMegBox2.setWindowTitle(tr("Error"));
    errMegBox2.setText("Error is occured.");
    errMegBox2.setInformativeText(errorReply->errorString());
    errMegBox2.exec();
}
